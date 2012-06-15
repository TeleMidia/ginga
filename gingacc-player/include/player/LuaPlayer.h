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

#ifndef LUAPLAYER_H
#define LUAPLAYER_H

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "util/functions.h"

#include <pthread.h>
#include "nclua.h"

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

#include "Player.h"

struct Field {
  string str;
  map<string, struct Field> table;
};

#define LUAPLAYER_TCP "luaplayer.TCP"

LUALIB_API int luaopen_canvas (lua_State* L);
LUALIB_API int lua_createcanvas (lua_State* L, ISurface* sfc, int collect);

#define LUAPLAYER_BEGIN_DECLS NAMESPACE_GINGA_CORE_PLAYER_BEGIN
#define LUAPLAYER_END_DECLS   NAMESPACE_GINGA_CORE_PLAYER_END

LUAPLAYER_BEGIN_DECLS

typedef struct _scopeinfo_t
{
  short type;
  string scopeId;
  double initTime;
  double endTime;
} scopeinfo_t;

class LuaPlayer:
public Player, public IInputEventListener
{

 private:
  nclua_t *nc;                /* the NCLua state */
  pthread_mutex_t mutex;      /* sync access to player */

  bool isHandler;             /* true if player has the focus */
  bool loaded;                /* true if script was loaded */
  bool played;                /* true if script was executed */
  bool running;               /* true if script is executing */

  string currentScope;
  map<string, scopeinfo_t *> *scopes;
  void load ();
  void dispatch ();
  void post (string action);

 public:

  LuaPlayer (GingaScreenID screenId, string mrl);
  virtual ~LuaPlayer ();

  // Player interface.
  void play ();
  void stop ();
  void abort ();
  void pause ();
  void resume ();
  virtual bool hasPresented ();
  virtual void setPropertyValue (string name, string value);
  bool setKeyHandler(bool isHandler);
  void setScope(string scope, short type=TYPE_PRESENTATION, double begin=-1, double end=-1, double outTransDur=-1);
  void setCurrentScope(string scopeId);

  // IInputEventListener.
  bool userEventReceived(IInputEvent* evt);

  // TODO: Make private.
  IInputManager* im;
  bool isRunning ();
  void doSetPropertyValue (string name, string value);
  GingaScreenID getScreenId ();
  ILocalScreenManager *getScreenManager ();
  void refreshContent();
};

LUAPLAYER_END_DECLS

#endif /* LUAPLAYER_H */
