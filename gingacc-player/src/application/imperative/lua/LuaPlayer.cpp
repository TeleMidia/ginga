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

LUAPLAYER_BEGIN_DECLS

#define LOCK()   assert (pthread_mutex_lock (&this->mutex) == 0)
#define UNLOCK() assert (pthread_mutex_unlock (&this->mutex) == 0)

#define mutex_init(m)                                                   \
  do                                                                    \
    {                                                                   \
      pthread_mutexattr_t attr;                                         \
      pthread_mutexattr_init (&attr);                                   \
      pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);       \
      pthread_mutex_init (m, &attr);                                    \
      pthread_mutexattr_destroy (&attr);                                \
    }                                                                   \
  while (0)

LuaPlayer::LuaPlayer (GingaScreenID screenId, string mrl) : Player (screenId, mrl)
{
  lua_State *L;

  chdir (SystemCompat::getPath (mrl).c_str ());
  this->im = dm->getInputManager (myScreen);
  this->im->addApplicationInputEventListener (this);

  this->surface = dm->createSurface (myScreen);
  this->surface->setCaps(this->surface->getCap("ALPHACHANNEL"));

  this->currentScope = "";        /* whole content anchor */
  this->scopes = new map<string, scopeinfo_t *>;

  mutex_init (&this->mutex);

  this->nc = nclua_create ();
  assert (this->nc != NULL);
  nclua_set_user_data (this->nc, (void *) this, NULL);

  /* FIXME: Cleanup this mess.  */
  L = (lua_State *) nclua_get_lua_state (nc);

  lua_pushcfunction(L, luaopen_canvas);
  lua_call(L, 0, 0);

  this->running = true;
  this->played = false;
  this->loaded = false;
  this->isHandler = false;

  /* Initial scope is the whole content anchor.  */
  this->setScope ("", TYPE_PRESENTATION, -1, -1);
}

LuaPlayer::~LuaPlayer ()
{
  LOCK();

  this->running = false;
  nclua_destroy (this->nc);
  pthread_mutex_destroy (&this->mutex);

  UNLOCK();
}

GingaScreenID LuaPlayer::getScreenId ()
{
  GingaScreenID screen;

  LOCK ();
  screen = this->myScreen;
  UNLOCK ();

  return screen;
}

ILocalScreenManager *LuaPlayer::getScreenManager()
{
  ILocalScreenManager *dm;

  LOCK ();
  dm = this->dm;
  UNLOCK ();

  return dm;
}

bool
LuaPlayer::isRunning ()
{
  bool running;

  LOCK ();
  running = this->running;
  UNLOCK ();

  return running;
}

void LuaPlayer::load ()
{
  lua_State *L;
  ISurface *surface;

  LOCK ();

  if (this->loaded)
    goto tail;

  L = (lua_State *) nclua_get_lua_state (this->nc);

  this->loaded = true;

  surface = this->getSurface ();
  if (surface != NULL)
    {
      lua_createcanvas (L, surface, 0);
      lua_setglobal (L, "canvas");
    }

  if(luaL_loadfile (L, this->mrl.c_str ()))
    {
      this->notifyPlayerListeners(PL_NOTIFY_ABORT, "");
      goto tail;
    }
  lua_call(L, 0, 0);

 tail:
  UNLOCK ();
}

void LuaPlayer::post (string action)
{
  lua_State *L;
  scopeinfo_t *scope;

  LOCK ();

  L  = (lua_State *) nclua_get_lua_state (this->nc);
  scope = (*this->scopes)[this->currentScope];
  if (scope->type == TYPE_PRESENTATION)
    {
      lua_newtable (L);

      lua_pushstring (L, "ncl");
      lua_setfield (L, -2, "class");

      lua_pushstring (L, "presentation");
      lua_setfield (L, -2, "type");

      lua_pushstring (L, action.c_str ());
      lua_setfield (L, -2, "action");

      lua_pushstring (L, currentScope.c_str ());
      lua_setfield (L, -2, "label");
      nclua_send (nc, -1);
    }
  else
    {
    }

  UNLOCK ();
}

void LuaPlayer::play()
{
  LOCK ();

  this->load();
  this->post("start");

  Player::play();
  UNLOCK ();
}

void LuaPlayer::stop ()
{
  LOCK ();

  this->post("stop");

  if (this->currentScope == "")
    {
      this->im->removeApplicationInputEventListener(this);
      this->forcedNaturalEnd = true;
      this->played = true;
    }

  Player::stop();

  UNLOCK ();
}

void
LuaPlayer::pause()
{
  LOCK ();

  this->post("pause");
  Player::pause();

  UNLOCK ();
}

void
LuaPlayer::resume ()
{
  LOCK ();

  Player::resume();
  this->post("resume");

  UNLOCK ();
}

void
LuaPlayer::abort()
{
  LOCK ();
  this->post("abort");
  this->played = true;
  UNLOCK ();

  Player::abort();
}

void
LuaPlayer::doSetPropertyValue(string name, string value)
{
  LOCK ();
  Player::setPropertyValue (name, value);
  UNLOCK ();
}

void
LuaPlayer::setPropertyValue(string name, string value)
{
  lua_State *L;

  LOCK ();

  L = (lua_State *) nclua_get_lua_state (this->nc);
  this->doSetPropertyValue (name, value);

  lua_newtable (L);

  lua_pushstring (L, "ncl");
  lua_setfield (L, -2, "class");

  lua_pushstring (L, "presentation");
  lua_setfield (L, -2, "type");

  lua_pushstring (L, "start");
  lua_setfield (L, -2, "action");

  lua_pushstring (L, name.c_str ());
  lua_setfield (L, -2, "name");

  lua_pushstring (L, value.c_str ());
  lua_setfield (L, -2, "value");

  nclua_send (nc, -1);
  UNLOCK ();
}

bool
LuaPlayer::userEventReceived (IInputEvent* evt)
{
  lua_State *L;

  LOCK ();

  L = (lua_State *) nclua_get_lua_state (this->nc);

  if (evt->isApplicationType ())
    {
      lua_State* srcL = (lua_State*) evt->getApplicationData();
      if (srcL == L)
        {
          int ref = evt->getType ();
          lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
          luaL_unref(L, LUA_REGISTRYINDEX, ref);
          nclua_send (nc, -1);
        }
    }
  else if (evt->isKeyType() && this->isHandler)
    {
      string key_str = CodeMap::getInstance()->getValue(evt->getKeyCode(myScreen));
      lua_newtable (L);

      lua_pushstring (L, "key");
      lua_setfield (L, -2, "class");

      lua_pushstring (L, evt->isPressedType() ? "press" : "release");
      lua_setfield (L, -2, "type");

      lua_pushstring (L, key_str.c_str ());
      lua_setfield (L, -2, "key");
      nclua_send (nc, -1);
    }

  UNLOCK ();

  return true;
}

void LuaPlayer::refreshContent()
{
  LOCK ();
  if (notifyContentUpdate)
    notifyPlayerListeners(PL_NOTIFY_UPDATECONTENT, "", TYPE_PASSIVEDEVICE, "");
  UNLOCK ();
}

bool LuaPlayer::hasPresented()
{
  bool played;

  LOCK ();
  played = this->played;
  UNLOCK ();

  return played;
}

bool
LuaPlayer::setKeyHandler (bool isHandler)
{
  LOCK ();
  this->isHandler = isHandler;
  UNLOCK ();

  return isHandler;
}

void LuaPlayer::setScope(string scopeId, short type,
                         double begin, double end, double outTransDur)
{
  scopeinfo_t *scope;

  LOCK ();

  if (this->scopes->count(scopeId) != 0)
    goto tail;

  scope = new scopeinfo_t;
  scope->type     = type;
  scope->scopeId  = scopeId;
  scope->initTime = begin;
  scope->endTime  = end;

  (*this->scopes)[scopeId] = scope;

 tail:
  UNLOCK ();
}

void LuaPlayer::setCurrentScope (string scopeId)
{
  LOCK ();
  this->currentScope = scopeId;
  UNLOCK ();
}

// Component manager interface.

extern "C"
{
  IPlayer *
  createLuaPlayer(GingaScreenID screenId, const char* mrl, bool hasVisual)
  {
    string s = mrl;
    return new LuaPlayer (screenId, s);
  }

  void
  destroyLuaPlayer (IPlayer *p)
  {
    delete p;
  }
}

LUAPLAYER_END_DECLS
