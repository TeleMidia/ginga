/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 2006-2012 PUC-Rio/Laboratorio TeleMidia

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

Copyright: 2006-2012 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

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

extern "C"
{
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "nclua.h"
}

#include "player/PlayersComponentSupport.h"
#include "mb/IInputManager.h"
#include "util/functions.h"
using namespace::br::pucrio::telemidia::util;

#include "player/LuaPlayer.h"

extern "C"
{
#include "Event.h"
}

int luaopen_canvas (lua_State *);
int lua_createcanvas (lua_State *, ISurface *, int);

LUAPLAYER_BEGIN_DECLS

// Initialize mutex m with recursive attribute.
#define INIT_MUTEX(m)                                                   \
     do {                                                               \
          pthread_mutexattr_t attr;                                     \
          pthread_mutexattr_init (&attr);                               \
          pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);   \
          pthread_mutex_init (m, &attr);                                \
          pthread_mutexattr_destroy (&attr);                            \
     } while (0)

// Locks/unlocks player mutex.
#define LOCK()   assert (pthread_mutex_lock (&this->mutex) == 0)
#define UNLOCK() assert (pthread_mutex_unlock (&this->mutex) == 0)

// Execution trace.
#ifdef ENABLE_TRACE
extern "C" {
#include <stdio.h>
}
# ifdef TRACE_TO_STDERR
#  define __trace(format, ...)                          \
     do {                                               \
          fflush (stdout);                              \
          fprintf (stderr, format"\n", ## __VA_ARGS__); \
          fflush (stderr);                              \
     } while (0)
# else
#  ifdef _MSC_VER
#   define snprintf _snprintf
#  endif
#  define __trace(format, ...)                                  \
     do {                                                       \
          char buf[1024];                                       \
          fflush (NULL);                                        \
          snprintf (buf, sizeof (buf), format, ## __VA_ARGS__); \
          clog << buf << endl;                                  \
          clog.flush ();                                        \
     } while (0)
# endif
// Print execution trace.
# define __where    __FILE__, __LINE__, __FUNCTION__
# define trace0()      __trace ("%s:%d:%s ()", __where)
# define trace(f, ...) __trace ("%s:%d:%s ("f")", __where, ## __VA_ARGS__)
#else
# define trace0()
# define trace(f, ...)
#endif // ENABLE_TRACE


// Private methods -- these should not call LOCK/UNLOCK.

// Schedules a new NCLua "cycle".

void LuaPlayer::scheduleUpdate (void)
{
     IInputEvent *evt;
     void *data = (void *) this->nc;
     evt = this->dm->createApplicationEvent (this->myScreen, 0, data);
     this->im->postInputEvent (evt);
}

// Destroys the associated NCLua engine and stops the player.

void LuaPlayer::doStop (void)
{
     lua_State *L;
     L = nclua_get_lua_state (this->nc);
     nclua_destroy (this->nc);
     lua_close (L);
     this->nc = NULL;
     this->im->removeApplicationInputEventListener (this);
     this->forcedNaturalEnd = true;
     this->hasExecuted = true;
}


// Constructor and destructor.

LuaPlayer::LuaPlayer (GingaScreenID id, string mrl) : Player (id, mrl)
{
     trace ("id=%d, mrl='%s'", id, mrl.c_str ());

     // FIXME: This is *WRONG*: the chdir() call changes the working
     // directory of the whole process.
     chdir (SystemCompat::getPath (mrl).c_str ());

     this->im = dm->getInputManager (this->myScreen);
     this->surface = dm->createSurface (myScreen);
     this->surface->setCaps (this->surface->getCap ("ALPHACHANNEL"));

     this->nc = NULL;           // created by start()
     INIT_MUTEX (&this->mutex);
     this->hasExecuted = false;
     this->isKeyHandler = false;
     this->scope = "";
}

LuaPlayer::~LuaPlayer (void)
{
     LOCK ();
     trace0 ();

     if (this->nc != NULL)
          this->doStop ();

     UNLOCK ();
     pthread_mutex_destroy (&this->mutex);
}


// Inherited from Player.

void LuaPlayer::abort (void)
{
     LOCK ();
     trace0 ();

     event_send_ncl_presentation_event (this->nc, Player::PL_NOTIFY_ABORT,
                                        this->scope.c_str ());
     this->stop ();

     UNLOCK ();
}

void LuaPlayer::pause (void)
{
     LOCK ();
     trace0 ();

     event_send_ncl_presentation_event (this->nc, Player::PL_NOTIFY_PAUSE,
                                        this->scope.c_str ());
     Player::pause ();

     UNLOCK ();
}

bool LuaPlayer::play (void)
{
     bool status = true;

     LOCK ();
     trace0 ();

     if (this->scope == "" && this->status == STOP)
     {
          lua_State *L;
          ISurface *surface;

          assert (this->nc == NULL);

          // Create Lua state and open extra libraries.

          L = luaL_newstate ();
          assert (L != NULL);

          luaL_openlibs (L);

          surface = this->getSurface ();
          surface->clearContent ();
          lua_pushcfunction (L, luaopen_canvas);
          lua_call (L, 0, 0);
          lua_createcanvas (L, surface, 0);
          lua_setglobal (L, "canvas");

          // Create the associated NCLua state.

          this->nc = nclua_create_for_lua_state (L);
          assert (nclua_status (this->nc) == NCLUA_STATUS_SUCCESS);
          nclua_set_user_data (this->nc, NULL, (void *) this, NULL);
          nclua_reset_uptime (this->nc);

          // Run script.

          if (luaL_dofile (L, this->mrl.c_str ()) != 0)
          {
               this->doStop ();
               Player::abort ();
               this->notifyPlayerListeners (Player::PL_NOTIFY_ABORT, "");
               status = false;

               fprintf (stderr, "%s\n", lua_tostring (L, -1));

               goto tail;
          }

          this->im->addApplicationInputEventListener (this);
          this->scheduleUpdate ();
     }

     // TODO: Should we post also the start of the whole content anchor?

     event_send_ncl_presentation_event (this->nc, Player::PL_NOTIFY_START,
                                        this->scope.c_str ());
     Player::play ();

tail:
     UNLOCK ();
     return status;
}

void LuaPlayer::resume (void)
{
     LOCK ();
     trace0 ();

     event_send_ncl_presentation_event (this->nc, Player::PL_NOTIFY_RESUME,
                                        this->scope.c_str ());
     Player::resume ();

     UNLOCK ();
}

void LuaPlayer::stop (void)
{
     LOCK ();
     trace0 ();

     // FIXME: stop() gets called even if the player is not running.
     if (this->nc != NULL)
     {
          event_send_ncl_presentation_event (this->nc,
                                             Player::PL_NOTIFY_STOP,
                                             this->scope.c_str ());
          this->doStop ();
          Player::stop ();
     }

     UNLOCK ();
}

bool LuaPlayer::hasPresented (void)
{
     bool hasExecuted;

     LOCK ();
     trace0 ();

     hasExecuted = this->hasExecuted;

     UNLOCK ();

     return hasExecuted;
}

void LuaPlayer::setCurrentScope (string name)
{
     LOCK ();
     trace ("name='%s'", name.c_str ());

     this->scope = name;
     UNLOCK ();
}

bool LuaPlayer::setKeyHandler (bool b)
{
     LOCK ();
     trace ("b=%s", b ? "true" : "false");

     this->isKeyHandler = b;

     UNLOCK ();

     return b;
}

void LuaPlayer::setPropertyValue (string name, string value)
{
     LOCK ();
     trace ("name='%s', value='%s'", name.c_str (), value.c_str ());


     // FIXME: Before calling play(), FormatterPlayerAdapter calls
     // setPropertyValue() to initialize the object's properties.  We
     // need to work around this bogus behavior, since it is the play()
     // call that creates the NCLua engine.  By the way, this workaround
     // is far from perfect.

     if (this->nc != NULL && this->status == PLAY)
       {
            const char *cname = name.c_str ();
            const char *cvalue = value.c_str ();

            event_send_ncl_attribution_event
                 (this->nc, Player::PL_NOTIFY_START, cname, cvalue);
            event_send_ncl_attribution_event
                 (this->nc, Player::PL_NOTIFY_STOP, cname, cvalue);
       }

     Player::setPropertyValue (name, value);

     UNLOCK ();
}


// Inherited from IInputEventListener.

bool LuaPlayer::userEventReceived (IInputEvent *evt)
{
     lua_State *L;

     LOCK ();
     // trace ("evt=%p", (void *) evt);

     if (this->nc == NULL)
     {
          goto tail;            // nothing to do
     }

     L = nclua_get_lua_state (this->nc);

     // "Cycle" event.

     if (evt->isApplicationType ()
         && ((nclua_t *) evt->getApplicationData ()) == this->nc)
     {
          nclua_cycle (this->nc);

          nclua_receive (this->nc, L);
          while (!lua_isnil (L, -1))
          {
               lua_pushcfunction (L, event_receive_event);
               lua_insert (L, -2);
               assert (lua_pcall (L, 1, LUA_MULTRET, 0) == 0);
               if (lua_isstring (L, -1))
               {
                    fprintf (stderr, "Warning: %s", lua_tostring (L, -1));
                    lua_pop (L, 1);
               }
               lua_pop (L, 1);

               nclua_receive (this->nc, L);
          }
          lua_pop (L, 1);
          this->scheduleUpdate ();
     }

     // Key event.

     else if (evt->isKeyType () && this->isKeyHandler)
     {
          string key;
          int press;

          key = CodeMap::getInstance ()
               ->getValue (evt->getKeyCode (myScreen));
          press = evt->isPressedType ();

          event_send_key_event (this->nc, key.c_str (), press);
     }

tail:
     UNLOCK ();

     return true;
}


// Extra public stuff (required by LuaCanvas).

GingaScreenID LuaPlayer::getScreenId (void)
{
     GingaScreenID screen;

     LOCK ();
     trace0 ();

     screen = this->myScreen;

     UNLOCK ();

     return screen;
}

ILocalScreenManager *LuaPlayer::getScreenManager (void)
{
     ILocalScreenManager *dm;

     LOCK ();
     trace0 ();

     dm = this->dm;

     UNLOCK ();

     return dm;
}

void LuaPlayer::refreshContent (void)
{
     LOCK ();
     trace0 ();

     if (!this->notifyContentUpdate)
     {
          goto tail;
     }

     notifyPlayerListeners (PL_NOTIFY_UPDATECONTENT, "",
                            TYPE_PASSIVEDEVICE, "");
tail:
     UNLOCK ();
}


// Component manager interface.

extern "C"
{

IPlayer *createLuaPlayer (GingaScreenID id, const char *mrl, bool b)
{
     return new LuaPlayer (id, string (mrl));
}

void destroyLuaPlayer (IPlayer * player)
{
     delete player;
}

} // extern "C"

LUAPLAYER_END_DECLS

// Local variables:
// mode: c++
// c-file-style: "k&r"
// End:
