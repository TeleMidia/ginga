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

#include "../../../../include/LuaPlayer.h"

#include "../../../../include/PlayersComponentSupport.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/io/IInputManager.h"

void l_dump (lua_State* L, char* point)
{
	int i;
	int top = lua_gettop(L);
	printf("Ponto %s: ", point);
	for (i=1; i<=top; i++)
	{
		int t = lua_type(L, i);
		switch (t)
		{
			case LUA_TSTRING:
				printf("'%s'", lua_tostring(L, i));
				break;
			case LUA_TBOOLEAN:
				printf(lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				printf("%g", lua_tonumber(L, i));
				break;
			default:
				printf("%s (%p)", lua_typename(L, t), lua_topointer(L, i));
				break;
		}
		printf(" ");
	}
	printf("\n");
}

void t_dump (lua_State* L, int idx)
{
	lua_pushnil(L);
	while (lua_next(L, idx) != 0)
	{
		printf("%s - %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {

// MACROS ASSERTIVAS

/*
 * Construtor:
 * - cria o estado Lua
 * - cria superficie grafica
 * - inicializa estruturas internas de apoio
 * - inicializa bibliotecas disponiveis ao NCLua
 * - associa Player ao seu estado Lua
 */
LuaPlayer::LuaPlayer (string mrl) : Player(mrl), Thread()
{
    chdir(getPath(mrl).c_str());    // execucao a partir do diretorio fonte
    this->L = luaL_newstate();      // estado Lua

#if HAVE_COMPSUPPORT
	this->im = ((InputManagerCreator*)(cm->getObject("InputManager")))();
    this->surface = ((SurfaceCreator*)(cm->getObject("Surface")))(NULL, 0, 0);
#else
    this->im = InputManager::getInstance();
    this->surface = new DFBSurface();
#endif

    this->surface->setCaps(this->surface->getCap("ALPHACHANNEL"));
	this->im->addApplicationInputEventListener(this);

    // ESCOPO
    this->currentScope = "";        // escopo global
    this->scopes = new map<string, struct scopeInfo*>;
    addScope("", TYPE_PRESENTATION, -1, -1);

    // a partir de um estado Lua eh possivel chegar ao seu Player
	lua_pushlightuserdata(this->L, this);         // [ LuaPlayer* ]
	lua_setfield(this->L, LUA_REGISTRYINDEX, LUAPLAYER_PLAYER);    // [ ]

    // INNICIALIZACAO DE BIBLIOTECAS DISPONIVEIS AO NCLUA

    // biblioteca padrao
	luaL_openlibs(this->L);

    // modulo 'event'
	lua_pushcfunction(this->L, luaopen_event);    // [ l_event ]
	lua_call(this->L, 0, 0);                      // [ ]
    this->tcp_running = false;

    // modulo 'canvas'
    this->lock();
	lua_pushcfunction(this->L, luaopen_canvas);   // [ l_canvas ]
	lua_call(this->L, 0, 0);                      // [ ]
    this->unlock();

    // estado inicial
	this->running = true;
	this->played = false;
	this->loaded = false;
	this->isHandler = false;
}

/*
 * DESTRUTOR:
 * - para de receber eventos de teclado
 * - fecha o estado Lua
 * - sinaliza fim da execucao
 */
LuaPlayer::~LuaPlayer ()
{
    this->lock();
	this->running = false;
	lua_pushcfunction(this->L, luaclose_event);   // [ l_event ]
	//lua_call(this->L, 0, 0);                      // [ ]
	lua_close(this->L);
	this->L = NULL;
    this->unlock();
}

// Retorna Player associado ao estado Lua.
LuaPlayer* LuaPlayer::getPlayer (lua_State* L)
{
	// [ ... ]
	lua_getfield(L, LUA_REGISTRYINDEX, LUAPLAYER_PLAYER);
                                                      // [ ... | LuaPlayer* ]
	LuaPlayer* player = (LuaPlayer*) lua_touserdata(L, -1);
	lua_pop(L, 1);                                    // [ ... ]
	return player;
}

void LuaPlayer::load ()
{
    if (this->loaded) return;
    this->loaded = true;

    // disponibiliza a superficie do NCLua atraves do modulo canvas
	ISurface* s = this->getSurface();
	if (s) {
		lua_createcanvas(this->L, s, 0);              // [ canvas ]
		lua_setglobal(this->L, "canvas");             // [ ]
	}

    // carrega o NCLua e checa se houve erros
	if( luaL_loadfile(this->L, this->mrl.c_str()) ) { // [ func ]
		this->notifyListeners(PL_NOTIFY_ABORT, "");
		lua_error(this->L);
		return;
	}
    lua_call(this->L, 0, 0);                          // [ ]
}

void LuaPlayer::run ()
{
    this->lock();
    this->load();
    this->unlock();
}

// METODOS DE COMUNICACAO COM O FORMATADOR: play, stop, abort, pause e resume

void LuaPlayer::post (string action)
{
    struct scopeInfo* scope = (*this->scopes)[this->currentScope];

    // PRESENTATION
	if (scope->type == TYPE_PRESENTATION)
    {
        map<string,string> evt;
            evt["class"]    = "ncl";
            evt["type"]     = "presentation";
            evt["action"]   = action;
            if (scope->initTime == -1)
                evt["label"] = currentScope;
            else {
                evt["begin"] = scope->initTime; //TODO: tostring?
                evt["end"]   = scope->endTime; //TODO: tostring?
            }
        ext_postHash(this->L, evt);
	}
    else
    {
        // TODO: attribution, ...
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

void LuaPlayer::setPropertyValue2 (string name, string value)
{
	Player::setPropertyValue(name, value, 0, 0);
}

void LuaPlayer::setPropertyValue (string name, string value,
                                  double duration, double by)
{
    //cout << "SET == " << name << " = " << value << endl;
	this->lock();
	Player::setPropertyValue(name, value, duration, by);

    map<string,string> evt;
        evt["class"]  = "ncl";
        evt["type"]   = "attribution";
        evt["action"] = "start";
        evt["name"]   = name;
        evt["value"]  = value;

    ext_postHash(this->L, evt);
	this->unlock();
}

// TECLADO

bool LuaPlayer::userEventReceived (IInputEvent* evt)
{
	this->lock();

	if (evt->isUserClass()) {
//cout << ">recv " << L << " ref " << evt->getType() << endl;
        lua_State* srcL = (lua_State*) evt->getData();
        if (srcL == L) {
//cout << "<recv " << L << endl;
		    ext_postRef(this->L, evt->getType());
        }

	} else if (evt->isKeyType() && this->isHandler) {
		string key_str = CodeMap::getInstance()->getValue(evt->getKeyCode());
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
		notifyListeners(PL_NOTIFY_UPDATECONTENT, "", TYPE_PASSIVEDEVICE);
	}
}

/*
void LuaPlayer::pushEPGEvent (map<string, string> event)
{
	cout << "LuaPlayer::pushEPGEvent in '" << mrl << "'" << endl;
    *
	if (event->count("id") != 0) {
		cout << (*event)["id"] << " ";
	}

	if (event->count("startTime") != 0) {
		cout << (*event)["startTime"] << " ";
	}

	if (event->count("duration") != 0) {
		cout << (*event)["duration"] << " ";
	}

	if (event->count("isRunning") != 0) {
		cout << (*event)["isRunning"] << " ";
	}

	if (event->count("language") != 0) {
		cout << (*event)["language"] << " ";
	}

	if (event->count("name") != 0) {
		cout << (*event)["name"] << " ";
	}

	if (event->count("description") != 0) {
		cout << (*event)["description"] << " ";
	}
    *

	this->lock();
	lua_getfield(this->L, LUA_EVENTINDEX, "post"); // [ event.post ]
	lua_pushstring(this->L, "in");                 // [ event.post | "in" ]
	ext_postHash(this->L, event);                 // [ event.post | "in" | evt ]
	lua_call(this->L, 2, 0);                       // [ ]
	this->unlock();
}
*/

/*
string LuaPlayer::getPropertyValue (string name)
{
	this->lock();
	CHECKRET("getPropertyValue()", "")
	cout << "[LUA] getPropertyValue('" << name.c_str() << "')";

	lua_getglobal(this->L, name.c_str());         // [ var ]
	string ret = luaL_optstring(L, -1, "");
	lua_pop(this->L, 1);                          // [ ]

	cout << " -> '" << ret.c_str() << "'" << endl;
	this->unlock();
	return ret;
}
*/

// MÉTODOS AUXILIARES

bool LuaPlayer::hasPresented() {
    //cout << "LuaPlayer::hasPresented '" << played << "'" << endl;
	return played;
}

bool LuaPlayer::setKeyHandler(bool isHandler) {
    this->isHandler = isHandler;
    return this->isHandler;
}

void LuaPlayer::setScope(string scopeId, short type,
                         double begin, double end)
{
	addScope(scopeId, type, begin, end);
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
		createLuaPlayer(const char* mrl, bool hasVisual) {

	return new ::br::pucrio::telemidia::ginga::core::player::LuaPlayer(
			(string)mrl);
}

extern "C" void destroyLuaPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
