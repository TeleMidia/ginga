/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include <assert.h>

#include "nclua.h"

#include "player/LuaPlayer.h"

#include "player/PlayersComponentSupport.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "mb/IInputManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {

LuaPlayer::LuaPlayer (GingaScreenID screenId, string mrl)
  : Player(screenId, mrl), Thread()
{
  chdir (SystemCompat::getPath (mrl).c_str ());
  this->im = dm->getInputManager (myScreen);
  this->im->addApplicationInputEventListener (this);

  this->surface = dm->createSurface (myScreen);
  this->surface->setCaps(this->surface->getCap("ALPHACHANNEL"));

  this->currentScope = "";        /* whole content anchor */
  this->scopes = new map<string, struct scopeInfo*>;
  addScope("", TYPE_PRESENTATION, -1, -1);

  this->nc = nclua_create ();
  assert (this->nc != NULL);
  nclua_set_user_data (this->nc, (void *) this, NULL);

  /* FIXME: Cleanup this mess.  */
  this->L = (lua_State *) nclua_get_lua_state (nc);

  lua_pushcfunction (this->L, luaopen_event);
  if (lua_pcall (this->L, 0, 0, 0) != 0)
    {
      cout << "LUAPLAYER ERROR\t" << lua_tostring (this->L, -1) << endl;
      assert (0);
    }

  this->lock();
  lua_pushcfunction(this->L, luaopen_canvas);
  if (lua_pcall(this->L, 0, 0, 0) != 0)
    {
      cout << "LUAPLAYER ERROR\t" << lua_tostring (this->L, -1) << endl;
      assert (0);
    }
  this->unlock();

  this->tcp_running = false;
  this->running = true;
  this->played = false;
  this->loaded = false;
  this->isHandler = false;
}

LuaPlayer::~LuaPlayer ()
{
  this->lock();
  this->running = false;
  nclua_destroy (this->nc);
  this->L = NULL;
  this->unlock();
}

GingaScreenID LuaPlayer::getScreenId() {
  return myScreen;
}

ILocalScreenManager* LuaPlayer::getScreenManager() {
  return dm;
}

LuaPlayer* LuaPlayer::getPlayer (lua_State *L) {
  nclua_t *nc;
  nc = nclua_get_nclua_state ((void *) L);
  return (LuaPlayer *) nclua_get_user_data (nc);
}

void LuaPlayer::load ()
{
    if (this->loaded)
      return;

    this->loaded = true;

    ISurface *s = this->getSurface();
    if (s != NULL) {
      lua_createcanvas (this->L, s, 0);
      lua_setglobal (this->L, "canvas");
    }

    if(luaL_loadfile(this->L, this->mrl.c_str()) ) {
      this->notifyPlayerListeners(PL_NOTIFY_ABORT, "");
      lua_error(this->L);
      return;
    }
    lua_call(this->L, 0, 0);
}

void LuaPlayer::run ()
{
    this->lock();
    this->load();
    this->unlock();
}

void LuaPlayer::post (string action)
{
    struct scopeInfo* scope = (*this->scopes)[this->currentScope];

    // PRESENTATION
    if (scope->type == TYPE_PRESENTATION)
      {
        map<string,string> evt;
        evt["class"]     = "ncl";
        evt["type"]      = "presentation";
        evt["action"]    = action;

        //TODO: Is an interval anchor id a label?
        evt["label"]     = currentScope;

        if (scope->initTime != -1) {
          evt["begin"] = scope->initTime; //TODO: tostring?
          evt["end"]   = scope->endTime; //TODO: tostring?
        }
        ext_postHash(this->L, evt);
      }
    else
      {
      }
}

void LuaPlayer::play()
{
  this->lock();
  Player::play();
  this->load();
  this->post("start");
  this->unlock();
}

void LuaPlayer::stop ()
{
  this->lock();
  this->post("stop");

  if (this->currentScope == "")
    {
      this->im->removeApplicationInputEventListener(this);
      this->forcedNaturalEnd = true;
      this->played = true;
    }

  Player::stop();
  this->unlock();
}

void LuaPlayer::pause()
{
    this->lock();
    this->post("pause");
    Player::pause();
    this->unlock();
}

void LuaPlayer::resume ()
{
    this->lock();
    Player::resume();
    this->post("resume");
    this->unlock();
}

void LuaPlayer::abort()
{
    this->lock();
    this->post("abort");
    this->played = true;
    this->unlock();
    Player::abort();
}

void LuaPlayer::unprotectedSetPropertyValue(string name, string value) {
  Player::setPropertyValue(name, value);
}

void LuaPlayer::setPropertyValue(string name, string value) {

  this->lock();
  Player::setPropertyValue(name, value);

  map<string,string> evt;
  evt["class"]  = "ncl";
  evt["type"]   = "attribution";
  evt["action"] = "start";
  evt["name"]   = name;
  evt["value"]  = value;

  ext_postHash(this->L, evt);
  this->unlock();
}

bool LuaPlayer::userEventReceived (IInputEvent* evt)
{
  this->lock();

  if (evt->isApplicationType()) {
    lua_State* srcL = (lua_State*) evt->getApplicationData();
    if (srcL == L) {
      ext_postRef(this->L, evt->getType());
    }

  } else if (evt->isKeyType() && this->isHandler) {
    string key_str = CodeMap::getInstance()->getValue(evt->getKeyCode(myScreen));
    map<string,string> lua_evt;
    lua_evt["class"] = "key";
    lua_evt["type"]  = evt->isPressedType() ? "press" : "release";
    lua_evt["key"]   = key_str;
    ext_postHash(this->L, lua_evt);
  }
  this->unlock();
  return true;
}

void LuaPlayer::refreshContent() {
  if (notifyContentUpdate) {
    notifyPlayerListeners(PL_NOTIFY_UPDATECONTENT, "", TYPE_PASSIVEDEVICE, "");
  }
}

bool LuaPlayer::hasPresented() {
  return played;
}

bool LuaPlayer::setKeyHandler(bool isHandler) {
    this->isHandler = isHandler;
    return this->isHandler;
}

void LuaPlayer::setScope(string scopeId, short type,
                         double begin, double end, double outTransDur)
{
  addScope (scopeId, type, begin, end);
}

void LuaPlayer::addScope(string scopeId, short type, double begin, double end)
{
  struct scopeInfo* newScope;

  if (this->scopes->count(scopeId) != 0)
    return;

  newScope = new struct scopeInfo;
  (*this->scopes)[scopeId] = newScope;
  newScope->type     = type;
  newScope->scopeId  = scopeId;
  newScope->initTime = begin;
  newScope->endTime  = end;
}

void LuaPlayer::setCurrentScope(string scopeId) {
  this->currentScope = scopeId;
}

} } } } } }

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
createLuaPlayer(GingaScreenID screenId, const char* mrl, bool hasVisual) {
  return new ::br::pucrio::telemidia::ginga::core::player::LuaPlayer(screenId, (string)mrl);
}

extern "C" void destroyLuaPlayer(::br::pucrio::telemidia::ginga::core::player::IPlayer* p)
{
  delete p;
}
