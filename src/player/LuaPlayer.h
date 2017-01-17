/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef LUAPLAYER_H
#define LUAPLAYER_H

#include "Player.h"
#include "mb/SDLWindow.h"
#include "mb/IFontProvider.h"
#include "mb/InputManager.h"
using namespace::br::pucrio::telemidia::ginga::core::mb;

#include "system/SystemCompat.h"
using namespace::br::pucrio::telemidia::ginga::core::system::compat;

BR_PUCRIO_TELEMIDIA_GINGA_CORE_PLAYER_BEGIN

class LuaPlayer : public Player, public IInputEventListener
{
private:

     ncluaw_t *nw;              // the NCLua state
     bool hasExecuted;          // true if script was executed
     bool isKeyHandler;         // true if player has the focus
     string scope;              // the label of the active anchor
     pthread_mutex_t mutex;     // sync access to player
     InputManager *im;

     // Update thread.
     static list <LuaPlayer *> *nw_update_list;
     static pthread_mutex_t nw_update_mutex;
     static pthread_t nw_update_tid;
     static void *nw_update_thread (void *data);
     static void nw_update_insert (LuaPlayer *player);

public:
     static void nw_update_remove (LuaPlayer *player);

public:
     LuaPlayer (GingaScreenID screenId, string mrl);
     virtual ~LuaPlayer (void);

     // TODO: Make private.
     void lock (void);
     void unlock (void);
     bool doPlay  (void);
     void doStop (void);

     // Player interface.
     void abort (void);
     void pause (void);
     bool play (void);
     void resume (void);
     void stop (void);
     virtual bool hasPresented (void);
     void setCurrentScope (string scopeId);
     bool setKeyHandler (bool isHandler);
     virtual void setPropertyValue (string name, string value);

     // Input event callback.
     bool userEventReceived (SDLInputEvent * evt);
};

BR_PUCRIO_TELEMIDIA_GINGA_CORE_PLAYER_END

#endif // LUAPLAYER_H

// Local variables:
// mode: c++
// c-file-style: "k&r"
// End:
