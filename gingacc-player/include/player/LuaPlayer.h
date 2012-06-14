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

/*
 * O player Lua é responsável por intermediar a comunicação entre o ambiente
 * externo e o script NCLua do usuário.
 *
 * O ambiente externo é caracterizado por:
 * - Formatador:
 *   A comunicação se faz através de elos no documento NCL que relacionam áreas
 *   e propriedades do NCLua.
 *   Áreas são disparadas pelos métodos: play(), stop(), abort(), pause() e
 *   resume().
 *   Propriedades são alteradas pelo método: setProperty().
 *   O NCLua também pode notificar mudanças em suas áreas e propriedades
 *   através do método notifyPlayerListeners().
 * - Controle Remoto:
 *   Eventos de controle remoto chegam através do método userEventReceived().
 *
 * Arquivos auxiliares:
 * - LuaEvent.cpp: comunicação com o ambiente
 * - LuaCanvas.cpp: acesso à superfície gráfica do NCLua
 *
 * Também é responsabilidade do Player:
 * - Manter o lua_State associado ao NCLua.
 * - Manter a região do documento NCL associado ao NCLua.
 * - Inicializar as bibliotecas disponíveis ao NCLua:
 *   - biblioteca padrão
 *   - módulo event      (ver LuaEvent.cpp)
 *   - módulo canvas     (ver LuaCanvas.cpp)
 *   - módulo settings   (não implementado)
 *   - módulo persistent (não implementado)
 *
 * O Player é executado em uma thread separada, o sincronismo com o sistema é
 * feito através de chamdas ao método this->lock().
 *
 * ESCOPO: controle de qual propriedade ou área está participando de um elo no
 * NCL.
 *
 * PILHA DE LUA:
 * Funções que mexem na pilha possuem um comentário subsequente indicando o
 * estado da pilha de Lua, por exemplo:
 * lua_pushnil(L);       // [ nil ]
 * lua_pushnumber(L, 1); // [ nil | 1 ]
 * lua_pop(L, 2);        // [ ]
 *
 */

/*
 * TODO:
 * - remover strcmp dos módulos
 *
 * LuaEvent:
 * - eliminar REFNCLMAP
 * - funcao para timer
 * - problemas libasync
 *
 * LuaPlayer:
 * - testar sucesso de atribuicao nos this->*
 */

#ifndef LUAPLAYER_H_
#define LUAPLAYER_H_

#include "config.h"

extern "C" {
	//#include <ctrace.h>
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include <map>
#include <string>
#include <iostream>
using namespace std;

#include "mb/interface/IWindow.h"
#include "mb/interface/IFontProvider.h"
#include "mb/IInputManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#if HAVE_DATAPROC
#include "dataprocessing/IEPGProcessor.h"
#include "dataprocessing/IEPGListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::epg;
#endif //HAVE_DATAPROC

#include "Player.h"

struct Field {
	string str;
	map<string, struct Field> table;
};

#define LUAPLAYER_PLAYER    "luaplayer.Player"
#define LUAPLAYER_TCP       "luaplayer.TCP"

string cmd_to_str(lua_State* L);
void l_dump (lua_State* L, char* point);
void t_dump (lua_State* L, int idx);

// FUNCOES C AUXILIARES

// Converte uma HashTable C++ para uma tabela Lua.
LUALIB_API int ext_postHashRec (
		lua_State* L, map<string, struct Field> evt, bool dispatch);

LUALIB_API int ext_postHash (lua_State* L, map<string,string>evt);
LUALIB_API int ext_postRef (lua_State* L, int ref);

LUALIB_API int luaopen_event (lua_State* L);
LUALIB_API int luaclose_event (lua_State* L);
LUALIB_API int luaopen_canvas (lua_State* L);
LUALIB_API int lua_createcanvas (lua_State* L, ISurface* sfc, int collect);

#define GETPLAYER(L) (LuaPlayer::getPlayer(L))


// ESCOPO
struct scopeInfo {
    short type;
	string scopeId;
	double initTime;
	double endTime;
};

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {

#if HAVE_DATAPROC
  class LuaPlayer :
	  public Player, public Thread, public IInputEventListener,
	  	  public IEPGListener {
#else
  class LuaPlayer :
	  public Player, public Thread, public IInputEventListener {
#endif //HAVE_DATAPROC

	private:
		bool loaded;     // se o script ja foi carregado (executado)
		bool played;     // se o player ja foi executado
		bool running;    // se o player esta em execucao

        // ESCOPO
		string currentScope;
		map<string, struct scopeInfo*>* scopes;
		void addScope(string scope, short type, double begin=-1, double end=-1);
		void load ();
		void dispatch ();
        void post (string action);

	public:
		lua_State* L;    // estado Lua
        pthread_t tcp_thread_id;
        bool tcp_running;

	    IInputManager* im;
        bool isHandler;  // se o player esta com o foco

#if HAVE_DATAPROC
		IEPGProcessor* epgProc;
#endif //HAVE_DATAPROC

		LuaPlayer (GingaScreenID screenId, string mrl);
		virtual ~LuaPlayer ();

		GingaScreenID getScreenId();
		ILocalScreenManager* getScreenManager();
		void run();
		virtual bool hasPresented();

        // A partir de um estado Lua, eh possivel chegar ao Player associado.
		static LuaPlayer* getPlayer (lua_State* L);

		bool isRunning() {
			return running;
		}
        // COMUNICACAO COM O FORMATADOR
		void play();
		void stop();
		void abort();
		void pause();
		void resume();
        //string getPropertyValue (string name);

		void unprotectedSetPropertyValue(string name, string value);
		virtual void setPropertyValue(string name, string value);

#if HAVE_DATAPROC
		void pushSIEvent(map<string, struct SIField> event, unsigned char type);
		void addAsSIListener(unsigned char type);
#endif

        // TECLADO
		bool userEventReceived(IInputEvent* evt);
		void refreshContent();

        // ESCOPO
		bool setKeyHandler(bool isHandler);
		void setScope(
				string scope,
				short type=TYPE_PRESENTATION,
				double begin=-1, double end=-1, double outTransDur=-1);

		void setCurrentScope(string scopeId);
  };
} } } } } }

#endif /* LUAPLAYER_H_ */
