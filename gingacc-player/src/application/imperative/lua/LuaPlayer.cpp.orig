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

#include <list>
#include <string>

extern "C"
{
#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nclua.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <pthread.h>
}

#include "player/PlayersComponentSupport.h"
#include "mb/IInputManager.h"
#include "mb/LocalScreenManager.h"
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

// Error logging.
#define __errprint(format, ...)                         \
     do {                                               \
          fflush (stdout);                              \
          fprintf (stderr, format"\n", ## __VA_ARGS__); \
          fflush (stderr);                              \
     } while (0)
#define error(f, ...)   __errprint ("LuaPlayer ERROR: "f, ## __VA_ARGS__)
#define warning(f, ...) __errprint ("LuaPlayer Warning: "f, ## __VA_ARGS__)

// Execution trace.
#ifdef LUAPLAYER_ENABLE_TRACE
# ifdef LUAPLAYER_TRACE_TO_STDERR
#  define __trace __errprint
# else
#  ifdef _MSC_VER
#   define snprintf _snprintf
#  endif
#  define __trace(format, ...)                                  \
     do {                                                       \
          char buf[1024];                                       \
          fflush (stdout);                                      \
          fflush (stderr);                                      \
          snprintf (buf, sizeof (buf), format, ## __VA_ARGS__); \
          clog << buf << endl;                                  \
          clog.flush ();                                        \
     } while (0)
# endif
// Print execution trace.
# define __where       __FILE__, __LINE__, __FUNCTION__
# define TRACE0()      __trace ("%s:%d:%s ()", __where)
# define TRACE(f, ...) __trace ("%s:%d:%s ("f")", __where, ## __VA_ARGS__)
#else
# define TRACE0()
# define TRACE(f, ...)
#endif // ENABLE_TRACE

// Mutex handling macros.
#define MUTEX_INIT(m)                                           \
     do {                                                       \
          pthread_mutexattr_t attr;                             \
          assert (pthread_mutexattr_init (&attr) == 0);         \
          assert (pthread_mutexattr_settype                     \
                  (&attr, PTHREAD_MUTEX_RECURSIVE) == 0);       \
          assert (pthread_mutex_init (m, &attr) == 0);          \
          assert (pthread_mutexattr_destroy (&attr) == 0);      \
     } while (0)
#define MUTEX_FINI(m)   assert (pthread_mutex_destroy (m) == 0)
#define MUTEX_LOCK(m)   assert (pthread_mutex_lock (m) == 0)
#define MUTEX_UNLOCK(m) assert (pthread_mutex_unlock (m) == 0)


// Update thread.

// List of active players.
list <LuaPlayer *> *LuaPlayer::nc_update_list = NULL;

// Synchronize access to nc_update_list.
pthread_mutex_t LuaPlayer::nc_update_mutex;

// Id of the update thread.
pthread_t LuaPlayer::nc_update_tid;

// Time delay (in milliseconds) between updates.
#define NC_UPDATE_DELAY 10

// Traverses the list of active players cycling their NCLua states.  This
// thread is created when the first NCLua state is allocated, and it is
// destroyed whenever the last NCLua state is destroyed.

void *LuaPlayer::nc_update_thread (void *data)
{
     while (true)
     {
          SystemCompat::uSleep ((NC_UPDATE_DELAY) * 1000);

          MUTEX_LOCK (&nc_update_mutex);

          if (nc_update_list == NULL) // end of cycle process
          {
               MUTEX_UNLOCK (&nc_update_mutex);
               return NULL;
          }

          list <LuaPlayer *> :: iterator i;
          list <LuaPlayer *> lst = *nc_update_list;

          for (i = lst.begin (); i != lst.end (); i++)
          {
               LuaPlayer *player;
               nclua_t *nc;
               lua_State *L;

               player = *i;
               player->lock ();

               nc = player->nc;
               L = nclua_get_lua_state (nc);

               nclua_cycle (nc);
               ev_tcp_cycle (L);

               nclua_receive (nc, L);
               while (!lua_isnil (L, -1))
               {
                    lua_pushcfunction (L, ev_receive_event);
                    lua_insert (L, -2);
                    assert (lua_pcall (L, 1, LUA_MULTRET, 0) == 0);

                    // Check if NCLua state was destroyed by doStop().
                    if (player->nc == NULL)
                    {
                         break;
                    }

                    nclua_receive (nc, L);
               }
               lua_pop (L, 1);
               player->unlock ();
          }

          MUTEX_UNLOCK (&nc_update_mutex);
     }

     return NULL;
}

// Inserts player PLAYER into update list.
// If this is the first player, creates both the list and the update thread.

void LuaPlayer::nc_update_insert (LuaPlayer *player)
{
     if (nc_update_list == NULL)
     {
          MUTEX_INIT (&nc_update_mutex);
          MUTEX_LOCK (&nc_update_mutex);
          nc_update_list = new list <LuaPlayer *> ();
          assert (pthread_create (&nc_update_tid, 0,
                                  nc_update_thread, NULL) == 0);
     }
     else
     {
          MUTEX_LOCK (&nc_update_mutex);
     }

     nc_update_list->push_back (player);
     MUTEX_UNLOCK (&nc_update_mutex);
}

// Removes player PLAYER from update list.
// If this is the last player, destroys both the list and the update thread.

void LuaPlayer::nc_update_remove (LuaPlayer *player)
{
     MUTEX_LOCK (&nc_update_mutex);

     assert (nc_update_list != NULL);
     nc_update_list->remove (player);

     // FIXME (The "Stop" Mess - Part II): If a "stop" was posted by the
     // NCLua script, this function will be called in the update thread.  At
     // this point, the player mutex is locked and the NCLua state is about
     // to be destroyed.  To avoid corruption, we postpone the destruction
     // of nc_update_list until the player's destructor is called.

     if (nc_update_list->empty () && !pthread_equal(pthread_self (), nc_update_tid))
     {
          delete nc_update_list;
          nc_update_list = NULL; // signal end of cycle process

          MUTEX_UNLOCK (&nc_update_mutex);
          assert (pthread_join (nc_update_tid, NULL) == 0);

          MUTEX_FINI (&nc_update_mutex);
     }
     else
     {
          MUTEX_UNLOCK (&nc_update_mutex);
     }
}


// Private (or almost private) methods -- these should not call LOCK/UNLOCK.

// Locks player.

void LuaPlayer::lock (void)
{
     MUTEX_LOCK (&this->mutex);
}

// Unlocks player.

void LuaPlayer::unlock (void)
{
     MUTEX_UNLOCK (&this->mutex);
}

// Creates and loads the associated NCLua engine.
// Returns true if successful, otherwise returns false.

bool LuaPlayer::doPlay (void)
{
     lua_State *L;
     ISurface *surface;

     TRACE0 ();

     assert (this->nc == NULL);

     // Create the Lua state and open extra libraries.

     L = luaL_newstate ();
     assert (L != NULL);

     luaL_openlibs (L);
     ev_tcp_open (L);

     surface = this->getSurface ();
     surface->clearContent ();
     lua_pushcfunction (L, luaopen_canvas);
     lua_call (L, 0, 0);
     lua_createcanvas (L, surface, 0);
     lua_setglobal (L, "canvas");

     // Create the associated NCLua state.

     this->nc = nclua_create_for_lua_state (L);
     assert (nclua_status (this->nc) == NCLUA_STATUS_SUCCESS);
     ev_install_wrappers (L);
     nclua_set_user_data (this->nc, NULL, (void *) this, NULL);
     nclua_reset_uptime (this->nc);

     // Run script.

     if (luaL_dofile (L, this->mrl.c_str ()) != 0)
     {
          error ("%s", lua_tostring (L, -1));
          this->doStop ();
          this->notifyPlayerListeners (Player::PL_NOTIFY_ABORT, "");
          Player::abort ();
          return false;
     }

     this->im->addApplicationInputEventListener (this);

     return true;
}

// Destroys the associated NCLua engine and stops the player.

void LuaPlayer::doStop (void)
{
     lua_State *L;

     TRACE0 ();

     assert (this->nc != NULL);

     // Destroy both states, Lua and NCLua.

     L = nclua_get_lua_state (this->nc);
     ev_uninstall_wrappers (L);
     ev_tcp_close (L);
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
     TRACE ("id=%d, mrl='%s'", id, mrl.c_str ());

     // FIXME: This is *WRONG*: the chdir() call changes the working
     // directory of the whole process.

     string cwd = SystemCompat::getPath (mrl);
     if (SystemCompat::changeDir (cwd.c_str ()) < 0)
     {
          char buf[1024];
          SystemCompat::strError (errno, buf, sizeof (buf));
          warning ("%s: %s", buf, cwd.c_str ());
     }

     LocalScreenManager::addIEListenerInstance(this);
     this->im = dm->getInputManager (this->myScreen);
     this->surface = dm->createSurface (myScreen);
     this->surface->setCaps (this->surface->getCap ("ALPHACHANNEL"));

     this->nc = NULL;           // created by start()
     MUTEX_INIT (&this->mutex);
     this->hasExecuted = false;
     this->isKeyHandler = false;
     this->scope = "";
}

LuaPlayer::~LuaPlayer (void)
{
     this->lock ();
     LocalScreenManager::removeIEListenerInstance(this);
     TRACE0 ();

     if (nc_update_list != NULL && nc_update_list->empty ())
     {
          // FIXME (The "Stop" Mess - Part III): If this->nc is NULL but
          // nc_update_list is non-NULL, then the '"Stop" Mess' is in
          // course.  Thus, we must call nc_update_remove to eventually
          // destroy the nc_update_list -- which, at this point, should be
          // empty.

          LuaPlayer::nc_update_remove (this);
     }

     if (this->nc != NULL)
     {
          this->doStop ();
     }

     this->unlock ();
     MUTEX_FINI (&this->mutex);
}


// Inherited from Player.

void LuaPlayer::abort (void)
{
     this->lock ();
     TRACE0 ();

     ev_send_ncl_presentation_event (this->nc, Player::PL_NOTIFY_ABORT,
                                     this->scope.c_str ());
     this->stop ();

     this->unlock ();
}

void LuaPlayer::pause (void)
{
     this->lock ();
     TRACE0 ();

     ev_send_ncl_presentation_event (this->nc, Player::PL_NOTIFY_PAUSE,
                                     this->scope.c_str ());
     Player::pause ();

     this->unlock ();
}

bool LuaPlayer::play (void)
{
     bool status;

     this->lock ();
     TRACE0 ();

     status = true;
     if (this->nc == NULL)
     {
          if (!this->doPlay ())
          {
               status = false;
               goto error;
          }

          nc_update_insert (this);
     }

     ev_send_ncl_presentation_event (this->nc, Player::PL_NOTIFY_START,
                                     this->scope.c_str ());
     Player::play ();

error:
     this->unlock ();

     return status;
}

void LuaPlayer::resume (void)
{
     this->lock ();
     TRACE0 ();

     ev_send_ncl_presentation_event (this->nc, Player::PL_NOTIFY_RESUME,
                                     this->scope.c_str ());
     Player::resume ();

     this->unlock ();
}

void LuaPlayer::stop (void)
{
     this->lock ();
     TRACE0 ();

     if (this->nc != NULL)
     {
          nc_update_remove (this);
          ev_send_ncl_presentation_event (this->nc, Player::PL_NOTIFY_STOP,
                                          this->scope.c_str ());
          nclua_cycle (this->nc);
          this->doStop ();
     }

     Player::stop ();

     this->unlock ();
}

bool LuaPlayer::hasPresented (void)
{
     bool hasExecuted;

     this->lock ();
     TRACE0 ();

     hasExecuted = this->hasExecuted;

     this->unlock ();

     return hasExecuted;
}

void LuaPlayer::setCurrentScope (string name)
{
     this->lock ();
     TRACE ("name='%s'", name.c_str ());

     this->scope = name;

     this->unlock ();
}

bool LuaPlayer::setKeyHandler (bool b)
{
     this->lock ();
     TRACE ("b=%s", b ? "true" : "false");

     this->isKeyHandler = b;

     this->unlock ();
     return b;
}

void LuaPlayer::setPropertyValue (string name, string value)
{
     this->lock ();
     TRACE ("name='%s', value='%s'", name.c_str (), value.c_str ());

     // FIXME: Before calling play(), FormatterPlayerAdapter calls
     // setPropertyValue() to initialize the object's properties.  We
     // need to work around this bogus behavior, since it is the play()
     // call that creates the NCLua engine.

     if (this->nc != NULL && this->status == PLAY)
       {
            const char *cname = name.c_str ();
            const char *cvalue = value.c_str ();

            ev_send_ncl_attribution_event
                 (this->nc, Player::PL_NOTIFY_START, cname, cvalue);
            ev_send_ncl_attribution_event
                 (this->nc, Player::PL_NOTIFY_STOP, cname, cvalue);
       }

     Player::setPropertyValue (name, value);

     this->unlock ();
}


// Inherited from IInputEventListener.

bool LuaPlayer::userEventReceived (IInputEvent *evt)
{
     this->lock ();
     TRACE0 ();

     if (this->nc == NULL)
     {
          goto tail;
     }

     if (evt->isKeyType () && this->isKeyHandler)
     {
          string key;
          int press;

          key = CodeMap::getInstance ()
               ->getValue (evt->getKeyCode (myScreen));
          press = evt->isPressedType ();

          ev_send_key_event (this->nc, key.c_str (), press);
     }

tail:
     this->unlock ();
     return true;
}


// Extra public stuff (required by LuaCanvas).

GingaScreenID LuaPlayer::getScreenId (void)
{
     GingaScreenID screen;

     this->lock ();

     screen = this->myScreen;

     this->unlock ();

     return screen;
}

ILocalScreenManager *LuaPlayer::getScreenManager (void)
{
     ILocalScreenManager *dm;

     this->lock ();

     dm = this->dm;

     this->unlock ();

     return dm;
}

void LuaPlayer::refreshContent (void)
{
     this->lock ();

     if (this->notifyContentUpdate)
     {
          notifyPlayerListeners
               (PL_NOTIFY_UPDATECONTENT, "", TYPE_PASSIVEDEVICE, "");
     }

     this->unlock ();
}


// Component manager interface.

extern "C"
{

IPlayer *createLuaPlayer (GingaScreenID id, const char *mrl, bool b)
{
     (void) b;
     return new LuaPlayer (id, string (mrl));
}

void destroyLuaPlayer (IPlayer *player)
{
     delete player;
}

} // extern "C"

LUAPLAYER_END_DECLS

// Local variables:
// mode: c++
// c-file-style: "k&r"
// End:
