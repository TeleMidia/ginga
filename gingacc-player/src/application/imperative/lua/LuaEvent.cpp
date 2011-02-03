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

//#include <async.h>

#include <string.h>

#include "player/LuaPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "player/PlayersComponentSupport.h"

// TODO: alter type verification of other modules
// or try a more generic approach for protection

// Indices para variaveis locais ao modulo.
// Deve iniciar de 3 (REFNIL=-1/NOREF=-2)
#define LUAPLAYER_EVENT "luaplayer.Event"
enum {
	  REFLISTENERS = 3 // listeners table
	, REFNEWLISTENERS  // pending listeners table
	, REFNCLMAP        // table for ("action"->int | int->"action") mapping
    , REFTCPIN
    , REFTCPOUT
};

// Eventos TCP
static void* tcp_thread (void* data);

/*******************************************************************************
 * FUNCOES DO MODULO
 ******************************************************************************/

/*******************************************************************************
 * event.post
 * - dst: in/out, default=out
 * - evt: tabela com o  evento
 *
 *   Destino 'in': o evento eh postado na fila interna e o Player eh acordado
 *   (unlockConditionSatisfied()). No caso de um evento de teclado, este deve
 *   ser convertido para Lua (evt_key()).
 *
 *   Destino 'out': cada caso eh tratado de forma especial (tcp, ncl, etc).
 *
 ******************************************************************************/

/*********************************************************************
 * event.dispatch
 * - PILHA: [ -1/+0 ]
 * - executa os tratadores registrados passando o evento recebido
 *********************************************************************/

static void int_dispatch (lua_State* L)
{
    // [ ... | evt ]

    lua_getfield(L, LUA_REGISTRYINDEX, LUAPLAYER_EVENT);

    // [ ... | evt | env ]

    lua_rawgeti(L, -1, -REFNEWLISTENERS);  // [ ... | evt | env | newlst ]
    if (!lua_isnil(L, -1)) {
        lua_pushvalue(L, -1);              // [ ... | evt | env | newlst | newlst ]
        lua_rawseti(L, -3, -REFLISTENERS); // [ ... | evt | env | newlst ]
        lua_pushnil(L);                    // [ ... | evt | env | newlst | nil ]
        lua_rawseti(L, -3, -REFNEWLISTENERS); // [  | evt | env | newlst ]
    }
    else {
        lua_pop(L, 1);                     // [ ... | evt | env ]
	    lua_rawgeti(L, -1, -REFLISTENERS); // [ ... | evt | env | lst ]
    }

    // [ ... | evt | env | lst ]
    // iterate over all listeners and call each of them
	int len = lua_objlen(L, -1);
	for (int i=1; i<=len; i++)
    {
		lua_rawgeti(L, -1, i);         // [ ... | evt | env | lst | t ]
        lua_getfield(L, -1, "__func"); // [ ... | evt | env | lst | t | func ]
		lua_pushvalue(L, -5);          // [ ... | evt | env | lst | t | func | evt]

		if (lua_pcall(L, 1, 1, 0) != 0) { // [ ... | evt | env | lst | t | ret]
			cout << "LUAEVENT int_dispatch ERROR:: ";
			cout << lua_tostring(L, -1) << endl;
		}

		if (lua_toboolean(L, -1)) {
            lua_pop(L, 2);             // [ ... | evt | env | lst ]
            break;
        }
        lua_pop(L, 2);                 // [ ... | evt | env | lst ]
	}

    // [ ... | evt | env | lst ]
    lua_pop(L, 3);                     // [ ... ]
}

LUALIB_API int ext_postRef (lua_State* L, int ref)
{
    // [ ... ]
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);  // [ ... | evt ]
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    int_dispatch(L);                         // [ ... ]
    return 0;
}

LUALIB_API int ext_postHash (lua_State* L, map<string,string>evt)
{
    // [ ... ]
    map<string,string>::iterator it;
	lua_newtable(L);                            // [ ... | evt ]
    for (it=evt.begin(); it!=evt.end(); it++) {
		lua_pushstring(L, it->second.c_str());  // [ ... | evt | value ]
		lua_setfield(L, -2, it->first.c_str()); // [ ... | evt ]
    }
    // [ ... | evt ]
    int_dispatch(L);  // [ ...]
    return 0;
}

LUALIB_API int ext_postHashRec (lua_State* L, map<string, struct Field> evt,
		bool dispatch)
{
	string str;
	map<string, struct Field>::iterator it;

	// [ ... ]
	lua_newtable(L);    // [ ... | evt ]

	//cout << "LuaEvent::ext_postHashRec TABLE = {" << endl;

	for (it=evt.begin(); it!=evt.end(); ++it) {
    	//cout << "	LuaEvent::ext_postHashRec: " << it->first.c_str();

    	 if ( it->second.table.empty() == false){ //table inside table
    		//cout << " (table) = " << endl;

    		ext_postHashRec(L, it->second.table, false);// [ ... | evt | evt ]
			lua_setfield(L, -2, it->first.c_str()); // [ ... | evt ]
    	}
    	 else if (it->second.str.empty() == false) { // field is a string
			//cout << " = " << it->second.str.c_str() << endl;

			lua_pushstring(L, it->second.str.c_str());  // [ ... | evt | value ]
    		lua_setfield(L, -2, it->first.c_str()); // [ ... | evt ]
    	}
    }
    //cout << "LuaEvent::ext_postHashRec }" << endl;
    // [ ... | evt ]
    if (dispatch == true){
    	//cout << "LuaEvent::ext_postHashRec going to dispatch!" << endl;
    	int_dispatch(L);  // [ ... ]
	}
    return 0;
}

static int l_post (lua_State* L)
{
    // [ [dst] | evt ]
    	const char* dst = NULL;
	if (lua_gettop(L) == 1) {
		lua_pushstring(L, "out");                 // [ evt | "out" ]
		lua_insert(L, 1);                         // [ "out" | evt ]
	}
	if (!lua_isstring(L, 1)) {
		fprintf(stderr,"Lua conformity error: event module\n");
		fprintf(stderr,"Call to 'post' does not follow Ginga-NCL standard: bad argument #1 (string expected)\n");
		fprintf(stderr,"Chamada a 'post' nao segue padrao Ginga-NCL: argumento #1 (string esperado)\n");
		return 0;
	}
	else {
		dst = luaL_checkstring(L, 1);
	}
    // [ dst | evt ]

	// dst == "in"
	if ( !strcmp(dst, "in") )
	{
		// [ dst | evt ]
        //luaL_checktype(L, 2, LUA_TTABLE);
	if(!lua_istable(L, 2)){

			fprintf(stderr,"Lua conformity error: event module\n");
			fprintf(stderr,"Call to 'post' does not follow Ginga-NCL standard: bad argument #2 (table expected)\n");
			fprintf(stderr,"Chamada a 'post' nao segue padrao Ginga-NCL: argumento #2 (table esperado)\n");
	
			return 0;
		}
        int ref = luaL_ref(L, LUA_REGISTRYINDEX); // [ dst ]

//cout << ">send " << L << " ref " << ref << endl;
#if HAVE_COMPSUPPORT
        GETPLAYER(L)->im->postEvent(
	        ((UserEventCreator*)(cm->getObject("UserEvent")))(ref, L));
#else
#ifndef _WIN32
        GETPLAYER(L)->im->postEvent(new DFBGInputEvent(ref, (void*)L));
#else
		GETPLAYER(L)->im->postEvent(new DXInputEvent((void*)L, ref));
#endif
#endif
	}

	// dst == "out"
	else if ( !strcmp(dst, "out") )
	{
		//luaL_checktype(L, 2, LUA_TTABLE);
		if(!lua_istable(L, 2)){
			
			fprintf(stderr,"Lua conformity error: event module\n");
			fprintf(stderr,"Call to 'post' does not follow Ginga-NCL standard: bad argument #2 (table expected)\n");
			fprintf(stderr,"Chamada a 'post' nao segue padrao Ginga-NCL: argumento #2 (table esperado)\n");
			
			return 0;
		}
		lua_getfield(L, 2, "class");         // [ dst | evt | class ]
		const char* clazz = luaL_checkstring(L, -1);

		// TCP event
		if ( !strcmp(clazz, "tcp") )
		{
            LuaPlayer* player = GETPLAYER(L);
            if (!player->tcp_running) {
                pthread_create(&player->tcp_thread_id, 0, tcp_thread, player);
                pthread_detach(player->tcp_thread_id);
            }
            lua_rawgeti(L, LUA_ENVIRONINDEX, -REFTCPOUT); // [ dst | evt | class | f_out ]
            lua_pushvalue(L, 2);                          // [ dst | evt | class | f_out | evt ]

            if (lua_pcall(L, 1, 0, 0) != 0) {             // [ dst | evt | class ]
            	cout << "LUAEVENT l_post ERROR:: ";
            	cout << lua_tostring(L, -1) << endl;
            }
        }

        // NCL event
		else if ( !strcmp(clazz, "ncl") )
		{
			lua_getfield(L, 2, "type");
                    // [ dst | evt | class | type ]
			const char* type = luaL_checkstring(L, -1);

			// PRESENTATION event
			if ( !strcmp(type, "presentation") )
			{
				lua_rawgeti(L, LUA_ENVIRONINDEX, -REFNCLMAP);
                    // [ dst | evt | class | type | ttrans ]
				lua_getfield(L, 2, "action");
                    // [ dst | evt | class | type | ttrans | trans ]
				lua_gettable(L, -2);
                    // [ dst | evt | class | type | ttrans | TRANS ]
				lua_getfield(L, 2, "label");
                    // [ dst | evt | class | type | ttrans | TRANS | label ]
				if (lua_isnil(L, -1)) {
					lua_pop(L, 1);
                    // [ dst | evt | class | type | ttrans | TRANS ]
					lua_pushstring(L, "");
                    // [ dst | evt | class | type | ttrans | TRANS | "" ]
				}

                if ((lua_tointeger(L, -2) == Player::PL_NOTIFY_STOP) &&
                        (!strcmp(luaL_checkstring(L, -1),""))) {

                	GETPLAYER(L)->im->removeApplicationInputEventListener(
                			GETPLAYER(L));
                }

				GETPLAYER(L)->notifyListeners(lua_tointeger(L, -2),
                                              luaL_checkstring(L, -1));
			}
			// ATTRIBUTION event
			else if ( !strcmp(type, "attribution") )
			{
				lua_rawgeti(L, LUA_ENVIRONINDEX, -REFNCLMAP);
                    // [ dst | evt | class | type | ttrans ]
				lua_getfield(L, 2, "action");
                    // [ dst | evt | class | type | ttrans | trans ]
				lua_gettable(L, -2);
                    // [ dst | evt | class | type | ttrans | TRANS ]
				lua_getfield(L, 2, "name");
                    // [ dst | evt | class | type | ttrans | TRANS | prop ]
				lua_getfield(L, 2, "value");
                    // [ dst | evt | class | type | ttrans | TRANS | prop | v ]
				GETPLAYER(L)->unprotectedSetPropertyValue(
						luaL_checkstring(L,-2), luaL_optstring(L,-1,""));

				GETPLAYER(L)->notifyListeners(lua_tointeger(L, -3),
                                              luaL_checkstring(L, -2),
                                              Player::TYPE_ATTRIBUTION);
			}

	    // edit
		} else if ( !strcmp(clazz, "edit") ) {
			string strCmd;

			// [ dst | evt | class | type | command ]
			strCmd = cmd_to_str(L);
			GETPLAYER(L)->notifyListeners(Player::PL_NOTIFY_NCLEDIT, strCmd);

		// SI event
		} else if ( !strcmp(clazz, "si") ){
			cout << "LuaEvent::l_post SI EVENT";
			
			lua_getfield(L, 2, "type");
			// [ dst | evt | class | type ]
			const char* type = luaL_checkstring(L, -1);
			cout << " with type:" << type << endl;

			//TODO: handle epg request data table properly.
			if (!strcmp(type, "si")) {
				GETPLAYER(L)->addAsSIListener(1);

			} else if (!strcmp(type, "epg")) {
				GETPLAYER(L)->addAsSIListener(2);
			
			} else if (!strcmp(type, "mosaic")) {

				GETPLAYER(L)->addAsSIListener(3);
			} else if (!strcmp(type, "time")) {

				GETPLAYER(L)->addAsSIListener(4);
			}

		} else {
			return luaL_error(L, "invalid event class");
		}

	} else {
		return luaL_argerror(L, 1, "possible values are: 'in', 'out'");
	}

	return 0;
}

/*********************************************************************
 * event.timer
 * Cria uma thread para dormir pelo tempo especificado.
 * A thread recebe uma estrutura com o tempo, Player e estado Lua.
 * Essa estrutura eh associada aa callback a ser executada na expiracao do
 * timer, assim a thread pode ter acesso a ela apos expirar.
 *********************************************************************/

struct t_timer {
	LuaPlayer* player;
    lua_State* L;
	int time;
};

static void* sleep_thread (void* data)
{
	struct t_timer* t = (struct t_timer *)data;
#ifndef _WIN32
	usleep(t->time*1000);
#else
	Sleep(t->time);
#endif

	t->player->lock();
	lua_pushlightuserdata(t->L, t);         // [ ... | t* ]
	lua_gettable(t->L, LUA_REGISTRYINDEX);  // [ ... | func ]
	if (!lua_isnil(t->L, -1)) {
		if (lua_pcall(t->L, 0, 0, 0) != 0) {// [ ... ]
			cout << "LUAEVENT sleep_thread ERROR:: ";
			cout << lua_tostring(t->L, -1) << endl;
		}

	} else {
		lua_pop(t->L, 1);                   // [ ... ]
	}
	t->player->unlock();

	delete t;
	return NULL;
}

static int l_cancel (lua_State* L)
{
	// [ ]
	lua_pushvalue(L, lua_upvalueindex(2));  // [ t* ]
	lua_pushvalue(L, -1);                   // [ t* | t* ]
	lua_gettable(L, LUA_REGISTRYINDEX);     // [ t* | funcR ]
	lua_pushvalue(L, lua_upvalueindex(1));  // [ t* | funcR | funcU ]

	if (lua_equal(L, -1, -2)) {
		lua_pop(L, 2);                      // [ t* ]
		lua_pushnil(L);                     // [ t* | nil ]
		lua_settable(L, LUA_REGISTRYINDEX); // [ ]
	} else
		lua_pop(L, 3);                      // [ ]

	return 0;
}

static int l_timer (lua_State* L)
{
	// [ msec, func ]
	struct t_timer* t = new struct t_timer;
	t->player = GETPLAYER(L);
    t->L      = L;
	t->time   = luaL_checkinteger(L, 1);

	lua_pushlightuserdata(L, t);            // [ msec | func | t* ]
	lua_pushvalue(L, -1);                   // [ msec | func | t* | t* ]
	lua_pushvalue(L, -3);                   // [ msec | func | t* | t* | func ]

	// REGISTRY[t] = func
	//luaL_checktype(L, 2, LUA_TFUNCTION);
	if(!lua_isfunction(L, 2)){

		fprintf(stderr,"Lua conformity error: event module\n");
		fprintf(stderr,"Call to 'timer' does not follow Ginga-NCL standard: bad argument #2 (function expected)\n");
		fprintf(stderr,"Chamada a 'timer' nao segue padrao Ginga-NCL: argumento #2 (function esperado)\n");
				
		return 0;
	}
	lua_settable(L, LUA_REGISTRYINDEX);     // [ msec | func | t* ]

	// returns `cancel` function
	lua_pushcclosure(L, l_cancel, 2);       // [ msec | fcancel ]

	// sleep msec
	pthread_t id;
	pthread_create(&id, 0, sleep_thread, (void*)t);
	pthread_detach(id);

	return 1;
}

/*********************************************************************
 * event.uptime
 * Retorna o tempo de execucao do Player.
 *********************************************************************/

static int l_uptime (lua_State* L)
{
	lua_pushnumber(L, (GETPLAYER(L)->getMediaTime()) * 1000);  // [ msec ]
	return 1;
}

/*********************************************************************
 * event.register
 *********************************************************************/

static void int_newlisteners (lua_State* L, int lst)
{
    // [ ... ]
    lua_newtable(L);               // [ ... | newlst ]
    int len = lua_objlen(L, lst);
    for (int i=1; i<=len; i++)
    {
        lua_rawgeti(L, lst, i);    // [ ... | newlst | hdlr ]
        lua_rawseti(L, -2, i);     // [ ... | newlst ]
    }
    // [ ... | newlst ]
}

// registra duas iguais
static int l_register (lua_State* L)
{
    // [ [i] | func | [filter] ]

    // [i] -> i
    if (lua_type(L, 1) == LUA_TFUNCTION) {
	    lua_rawgeti(L, LUA_ENVIRONINDEX, -REFLISTENERS);
            // [ func | [filter] | listeners ]
        lua_pushnumber(L, lua_objlen(L, -1)+1);
            // [ func | [filter] | listeners | #listeners+1 ]
        lua_insert(L, 1);
            // [ #listeners+1 | func | [filter] | listeners ]
        lua_pop(L, 1);
            // [ #listeners+1 | func | [filter] ]
    }

    // [ i | func | [filter] ]

    // [filter] -> filter
    if (lua_gettop(L) == 2)
        lua_newtable(L);    // [ i | func | filter ]

    // [ i | func | filter ]
	//luaL_checktype(L, 1, LUA_TNUMBER);
	//luaL_checktype(L, 2, LUA_TFUNCTION);
	//luaL_checktype(L, 3, LUA_TTABLE);
    if(!lua_isnumber(L, 1)){

		fprintf(stderr,"Lua conformity error: event module\n");
		fprintf(stderr,"Call to 'register' does not follow Ginga-NCL standard: bad argument #1 (number expected)\n");
		fprintf(stderr,"Chamada a 'register' nao segue padrao Ginga-NCL: argumento #1 (number esperado)\n");
	    
	    	return 0;
    }
    if(!lua_isfunction(L,2)){

		fprintf(stderr,"Lua conformity error: event module\n");
		fprintf(stderr,"Call to 'register' does not follow Ginga-NCL standard: bad argument #2 (function expected)\n");
		fprintf(stderr,"Chamada a 'register' nao segue padrao Ginga-NCL: argumento #2 (function esperado)\n");
	    
	    	return 0;
    }
    if(!lua_istable(L,3)){

		fprintf(stderr,"Lua conformity error: event module\n");
		fprintf(stderr,"Call to 'register' does not follow Ginga-NCL standard: bad argument #3 (table expected)\n");
		fprintf(stderr,"Chamada a 'register' nao segue padrao Ginga-NCL: argumento #3 (table esperado)\n");
	        return 0;
    }

    // filter.__func = func
    lua_pushvalue(L, 2);           // [ i | func | filter | func ]
    lua_setfield(L, -2, "__func"); // [ i | func | filter ]

    // creates newlisteners
    lua_rawgeti(L, LUA_ENVIRONINDEX, -REFLISTENERS);    // [ i | func | filter | lst ]
    lua_rawgeti(L, LUA_ENVIRONINDEX, -REFNEWLISTENERS); // [ i | func | filter | lst | ? ]
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);           // [ i | func | filter | lst ]
        int_newlisteners(L, 4);  // [ i | func | filter | lst | newlst ]
        lua_pushvalue(L, -1);    // [ i | func | filter | lst | newlst | newlst ]
        lua_rawseti(L, LUA_ENVIRONINDEX, -REFNEWLISTENERS); // [ ewlst ]
    }

    // [ i | func | filter | lst | newlst ]
    lua_getglobal(L, "table");     // [ -> | table ]
    lua_getfield(L, -1, "insert"); // [ -> | table | tinsert ]
    lua_pushvalue(L, 5);           // [ -> | table | tinsert | newlst ]
    lua_pushvalue(L, 1);           // [ -> | table | tinsert | newlst | i ]
    lua_pushvalue(L, 3);           // [ -> | table | tinsert | newlst | i | filter ]

    if (lua_pcall(L, 3, 0, 0) != 0) {// [ -> | table ]
    	cout << "LUAEVENT l_register ERROR:: ";
    	cout << lua_tostring(L, -1) << endl;
    }

    // [ i | func | filter | lst | newlst | table ]
    return 0;
}

/*********************************************************************
 * event.unregister
 *********************************************************************/

static int l_unregister (lua_State* L)
{
	// [ func ]
	// luaL_checktype(L, 1, LUA_TFUNCTION);
	if(!lua_isfunction(L, 1)){

		fprintf(stderr,"Lua conformity error: event module\n");
		fprintf(stderr,"Call to 'unregister' does not follow Ginga-NCL standard: bad argument #1 (function expected)\n");
		fprintf(stderr,"Chamada a 'unregister' nao segue padrao Ginga-NCL: argumento #1 (function esperado)\n");
			        
		return 0;
	}
    // creates newlisteners
    lua_rawgeti(L, LUA_ENVIRONINDEX, -REFLISTENERS);    // [ func | lst ]
    lua_rawgeti(L, LUA_ENVIRONINDEX, -REFNEWLISTENERS); // [ func | lst | ? ]
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);                        // [ func | lst ]
        int_newlisteners(L, 4);               // [ func | lst | newlst ]
        lua_pushvalue(L, -1);                 // [ func | lst | newlst | newlst ]
        lua_rawseti(L, LUA_ENVIRONINDEX, -REFNEWLISTENERS); // [ ewlst ]
    }

    // [ func | lst | newlst ]

	int len = lua_objlen(L, 3);
    for (int i=len; i>=1; i--)
    {
	    lua_rawgeti(L, -1, i);         // [ func | lst | newlst | t ]
        lua_getfield(L, -1, "__func"); // [ func | lst | newlst | t | f' ]
	    if (!lua_equal(L, -3, -1)) {
            lua_getglobal(L, "table");     // [ -> | f' | table ]
            lua_getfield(L, -1, "remove"); // [ -> | f' | table | remove ]
            lua_pushvalue(L, 3);           // [ -> | f' | table | remove | newlst ]
            lua_pushnumber(L, i);          // [ -> | f' | table | remove | newlst | i ]

            if (lua_pcall(L, 2, 0, 0) != 0) {// [ -> | f' | table ]
            	cout << "LUAEVENT l_unregister ERROR:: ";
            	cout << lua_tostring(L, -1) << endl;
            }

            lua_pop(L, 1);                 // [ -> | f' ]
	    }
	    lua_pop(L, 2);                 // [ func | lst | newlst ]
	}

    // [ func | lst | newlst ]
    return 0;
}

/*********************************************************************
 * Funcoes exportadas pelo modulo.
 ********************************************************************/

static const struct luaL_Reg funcs[] = {
	{ "post",       l_post       },
	{ "timer",      l_timer      },
	{ "uptime",     l_uptime     },
	{ "register",   l_register   },
	{ "unregister", l_unregister },
	{ NULL,         NULL         }
};

/*********************************************************************
 * Funcao que carrega o modulo.
 * - cria as variaveis locais ao modulo
 ********************************************************************/

LUALIB_API int luaclose_event (lua_State* L)
{
	pthread_cancel(GETPLAYER(L)->tcp_thread_id);
	return 1;
}

LUALIB_API int luaopen_event (lua_State* L)
{
	// env = {}
	lua_newtable(L);                                  // [ env ]
    lua_pushvalue(L, -1);                             // [ env | env ]
    lua_setfield(L, LUA_REGISTRYINDEX, LUAPLAYER_EVENT); // [ env ]
	lua_replace(L, LUA_ENVIRONINDEX);                 // [ ]

    // env[LISTENERS] = {}
	lua_newtable(L);                                  // [ listeners ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFLISTENERS);  // [ ]

    // env[NCLMAP] = {
    //     start  = PL_NOTIFY_START,
    //     stop   = PL_NOTIFY_STOP,
    //     pause  = PL_NOTIFY_PAUSE,
    //     resume = PL_NOTIFY_RESUME,
    //     abort  = PL_NOTIFY_ABORT,
	// }
	lua_newtable(L);                              // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_START);   // [ nclmap | START ]
	lua_setfield(L, -2, "start");                 // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_STOP);    // [ nclmap | STOP ]
	lua_setfield(L, -2, "stop");                  // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_PAUSE);   // [ nclmap | PAUSE ]
	lua_setfield(L, -2, "pause");                 // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_RESUME);  // [ nclmap | RESUME ]
	lua_setfield(L, -2, "resume");                // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_ABORT);   // [ nclmap | ABORT ]
	lua_setfield(L, -2, "abort");                 // [ nclmap ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFNCLMAP); // [ ]

    // trigger tcp thread
    lua_getglobal(L, "require");                  // [ require ]
    lua_pushstring(L, "tcp_event");               // [ require | "tcp_event" ]

    if (lua_pcall(L, 1, 1, 0) != 0) {             // [ {f_in,f_out} ]
		cout << "LUAEVENT luaopen_event ERROR:: ";
		cout << lua_tostring(L, -1) << endl;
    }

    lua_rawgeti(L, -1, 1);                        // [ {f_in,f_out} | f_in ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFTCPIN);  // [ {f_in,f_out} ]
    lua_rawgeti(L, -1, 2);                        // [ {f_in,f_out} | f_out ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFTCPOUT); // [ {f_in,f_out} ]
    lua_pop(L, 1);                                // [ ]

	luaL_register(L, "event", funcs);             // [ event ]

	return 1;
}

static void* tcp_thread (void* data)
{
    LuaPlayer* player = (LuaPlayer*) data;
    player->tcp_running = true;
//cout << "TCP STARTED\n";
    while (1) {
#ifndef _WIN32
	    usleep(500000);
#else
		Sleep(500);
#endif
        player->lock();
        lua_getfield(player->L, LUA_REGISTRYINDEX, LUAPLAYER_EVENT);  // [ ... | env ]
        lua_rawgeti(player->L, -1, -REFTCPIN);       // [ ... | env | f_in ]

        if (lua_pcall(player->L, 0, 1, 0) != 0) {          // [ ... | env | count ]
        	cout << "LUAEVENT tcp_thread ERROR:: ";
        	cout << lua_tostring(player->L, -1) << endl;
        }

        int count = luaL_checknumber(player->L, -1); // [ ... | env | count ]
        lua_pop(player->L, 2);                       // [ ... ]
        player->unlock();
        if (count == 0) break;
    }
//cout << "TCP FINISHED\n";
    player->tcp_running = false;
    return NULL;
}
