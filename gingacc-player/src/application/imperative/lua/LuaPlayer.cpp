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

extern "C"
{
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include "nclua.h"
}

#include "player/PlayersComponentSupport.h"
#include "mb/IInputManager.h"
#include "util/functions.h"
using namespace::br::pucrio::telemidia::util;

#include "player/LuaPlayer.h"

int luaopen_canvas (lua_State *);
int lua_createcanvas (lua_State *, ISurface *, int);

LUAPLAYER_BEGIN_DECLS

// Uncomment the following line to enable module-level debugging messages.
// #define ENABLE_DEBUG

#if defined (ENABLE_DEBUG) && (defined (__GNUC__) || defined (_MSC_VER))

/* For fprintf, fputs, etc. */
#include <stdio.h>

/* Define __func__ if it is not already defined.  */
# ifdef _MSC_VER
#  define __func__ __FUNCTION__
# endif

/* The stream to which messages will be written to.  */
# define __debug_stream stderr

/* Prints the current location in source-file.  */
# define __print_location()                             \
     do {                                               \
          fprintf (__debug_stream, "%s:%d:%s()\n",      \
                   __FILE__, __LINE__, __func__);       \
     } while (0)

/* Prints execution trace message.  */
extern "C" { static int __trace_counter = 0; }
# define TRACE()                                                \
     do {                                                       \
          __trace_counter++;                                    \
          fflush (stdout);                                      \
          fprintf (__debug_stream, "#%d\t", __trace_counter);   \
          __print_location ();                                  \
          fflush (stderr);                                      \
     } while (0)

/* Prints debug message.  */
# define DEBUG(fmt, ...)                                        \
     do {                                                       \
          fflush (stdout);                                      \
          __print_location ();                                  \
          fputc (':', __debug_stream);                          \
          fprintf (__debug_stream, fmt, ## __VA_ARGS__);        \
          fputc ('\n', __debug_stream);                         \
          fflush (stderr);                                      \
     } while (0)
#else
# define TRACE()
# define DEBUG(fmt, ...)
#endif

#define INIT_MUTEX(m)                                                   \
     do {                                                               \
          pthread_mutexattr_t attr;                                     \
          pthread_mutexattr_init (&attr);                               \
          pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);   \
          pthread_mutex_init (m, &attr);                                \
          pthread_mutexattr_destroy (&attr);                            \
     } while (0)

#define LOCK()                                                  \
     do {                                                       \
          TRACE ();                                             \
          assert (pthread_mutex_lock (&this->mutex) == 0);      \
     } while (0)

#define UNLOCK()                                                \
     do {                                                       \
          assert (pthread_mutex_unlock (&this->mutex) == 0);    \
     } while (0)

#define ASSERT_NOT_REACHED assert (!"reached")


// Private methods -- these should not call LOCK/UNLOCK.

void LuaPlayer::send_ncl_presentation_event (string action, string label)
{
     lua_State *L = nclua_get_lua_state (this->nc);
     lua_newtable (L);

     lua_pushstring (L, "ncl");
     lua_setfield (L, -2, "class");

     lua_pushstring (L, "presentation");
     lua_setfield (L, -2, "type");

     lua_pushstring (L, action.c_str ());
     lua_setfield (L, -2, "action");

     lua_pushstring (L, label.c_str ());
     lua_setfield (L, -2, "label");

     lua_pushvalue (L, -1);
     nclua_sendx (nc);
     nclua_send (nc, L);
}

void LuaPlayer::send_ncl_attribution_event (string action, string name,
                                            string value)
{
     lua_State *L = nclua_get_lua_state (this->nc);
     lua_newtable (L);

     lua_pushstring (L, "ncl");
     lua_setfield (L, -2, "class");

     lua_pushstring (L, "attribution");
     lua_setfield (L, -2, "type");

     lua_pushstring (L, action.c_str ());
     lua_setfield (L, -2, "action");

     lua_pushstring (L, name.c_str ());
     lua_setfield (L, -2, "name");

     lua_pushstring (L, value.c_str ());
     lua_setfield (L, -2, "value");

     lua_pushvalue (L, -1);
     nclua_sendx (nc);
     nclua_send (nc, L);
}


// Constructor and destructor.

LuaPlayer::LuaPlayer (GingaScreenID id, string mrl):Player (id, mrl)
{
     TRACE ();

     // FIXME: This is *WRONG*; chdir() changes the working directory
     // of the whole process.
     chdir (SystemCompat::getPath (mrl).c_str ());

     this->im = dm->getInputManager (this->myScreen);
     this->surface = dm->createSurface (myScreen);
     this->surface->setCaps (this->surface->getCap ("ALPHACHANNEL"));

     this->nc = NULL;           // created by start()
     INIT_MUTEX (&this->mutex);
     this->has_presented = false;
     this->is_key_handler = false;
     this->scope = "";
}

LuaPlayer::~LuaPlayer ()
{
     LOCK ();
     nclua_destroy (this->nc);
     UNLOCK ();
     pthread_mutex_destroy (&this->mutex);
}


// Helper public methods.

void LuaPlayer::exec (int type, int action, string name, string value)
{
     LOCK ();

     assert (action == PL_NOTIFY_ABORT
             || action == PL_NOTIFY_PAUSE
             || action == PL_NOTIFY_RESUME
             || action == PL_NOTIFY_START
             || action == PL_NOTIFY_STOP);

     clog << "LuaPlayer::exec ";
     clog << "type = '"   << type   << "' ";
     clog << "action = '" << action << "' ";
     clog << "name = '"   << name   << "' ";
     clog << "value = '"  << value  << "' ";
     clog << endl;

     switch (type)
     {
     case TYPE_ATTRIBUTION:
          this->notifyPlayerListeners (action, name, type, value);
          break;

     case TYPE_PRESENTATION:
          this->notifyPlayerListeners (action, name);
          break;

     case TYPE_SELECTION:
          // TODO: Not implemented.
          break;

     default:
          ASSERT_NOT_REACHED;
     }

     UNLOCK ();
}


// Inherited from Player.

void LuaPlayer::abort ()
{
     LOCK ();
     this->send_ncl_presentation_event ("abort", this->scope);
     this->stop ();
     UNLOCK ();
}

void LuaPlayer::pause ()
{
     LOCK ();
     this->send_ncl_presentation_event ("pause", this->scope);
     Player::pause ();
     UNLOCK ();
}

bool LuaPlayer::play ()
{
     lua_State *L;
     ISurface *surface;
     bool status = true;

     LOCK ();

     if (this->scope == "" && this->status == STOP)
     {
          this->im->addApplicationInputEventListener (this);

          assert (this->nc == NULL);
          this->nc = nclua_create ();
          assert (nclua_status (this->nc) == NCLUA_STATUS_SUCCESS);
          nclua_set_user_data (this->nc, NULL, (void *) this, NULL);

          L = nclua_get_lua_state (this->nc);
          lua_pushcfunction (L, luaopen_canvas);
          lua_call (L, 0, 0);

          surface = this->getSurface ();
          surface->clearContent ();

          lua_createcanvas (L, surface, 0);
          lua_setglobal (L, "canvas");

          luaL_loadfile (L, this->mrl.c_str ());

          if (luaL_loadfile (L, this->mrl.c_str ()) != 0
              || lua_pcall (L, 0, 0, 0) != 0)
          {
               DEBUG ("%s", lua_tostring (L, -1));
               status = false;
               goto tail;
          }

          // Cycle once to initialize clock.
          // FIXME: This is temporary.
          nclua_cycle (nc);
     }

     // TODO: Should we post also the start of
     // the whole content anchor?
     this->send_ncl_presentation_event ("start", this->scope);
     Player::play ();

tail:
     UNLOCK ();
     return status;
}

void LuaPlayer::resume ()
{
     LOCK ();
     this->send_ncl_presentation_event ("resume", this->scope);
     Player::resume ();
     UNLOCK ();
}

void LuaPlayer::stop ()
{
     LOCK ();

     // FIXME: stop() gets called even if the player is not running.
     if (this->nc == NULL)
     {
          goto tail;
     }

     this->send_ncl_presentation_event ("stop", this->scope);
     nclua_destroy (this->nc);
     this->nc = NULL;
     this->im->removeApplicationInputEventListener (this);
     this->forcedNaturalEnd = true;
     this->has_presented = true;
     Player::stop ();

tail:
     UNLOCK ();
}

bool LuaPlayer::hasPresented (void)
{
     bool has_presented;

     LOCK ();
     has_presented = this->has_presented;
     UNLOCK ();

     return has_presented;
}

void LuaPlayer::setCurrentScope (string name)
{
     LOCK ();
     this->scope = name;
     UNLOCK ();
}

bool LuaPlayer::setKeyHandler (bool b)
{
     LOCK ();
     this->is_key_handler = b;
     UNLOCK ();

     return b;
}

void LuaPlayer::setPropertyValue (string name, string value)
{
     LOCK ();

     if (this->status != PLAY)  // initialization "set"
     {
          Player::setPropertyValue (name, value);
          goto tail;
     }

     if (this->nc != NULL)
       {
         // FIXME: Before calling play(), FormatterPlayerAdapter calls
         // setPropertyValue() to initialize the object's properties.  We
         // need to work around this bogus behavior, since it is the play()
         // call that creates the NCLua engine.  By the way, this workaround
         // is far from perfect.
         this->send_ncl_attribution_event ("start", name, value);
         this->send_ncl_attribution_event ("stop", name, value);
       }

     Player::setPropertyValue (name, value);

tail:
     UNLOCK ();
}


// Inherited from IInputEventListener.

bool LuaPlayer::userEventReceived (IInputEvent *evt)
{
     lua_State *L;

     LOCK ();

     if (this->nc == NULL)
     {
          goto tail;            // nothing to do
     }

     L = nclua_get_lua_state (this->nc);

     if (evt->isApplicationType ()
         && ((lua_State *) evt->getApplicationData ()) == L)
     {
          int ref = evt->getType ();
          lua_rawgeti (L, LUA_REGISTRYINDEX, ref);
          luaL_unref (L, LUA_REGISTRYINDEX, ref);
          lua_pushvalue (L, -1);
          nclua_sendx (nc);
          nclua_send (nc, L);
     }

#if 0
     // Cycle event.
     if (evt->isApplicationType ()
         && ((nclua_t *) evt->getApplicationData ()) != this->nc)
     {

          int n;

          nclua_cycle (this->nc);

          nclua_receive (this->nc, L);
          while (!lua_isnil (L, -1))
          {
               // Handle the received event.
               lua_pop (L, 1);
               nclua_receive (this->nc, L);
          }
          lua_pop (L, 1);

          // Schedule the next update.
          player->im->postInputEvent (evt);
     }
#endif

     // Selection event.
     else if (evt->isKeyType () && this->is_key_handler)
     {
          string key_str;

          key_str = CodeMap::getInstance ()
               ->getValue (evt->getKeyCode (myScreen));
          lua_newtable (L);

          lua_pushstring (L, "key");
          lua_setfield (L, -2, "class");

          lua_pushstring (L, evt->isPressedType () ? "press" : "release");
          lua_setfield (L, -2, "type");

          lua_pushstring (L, key_str.c_str ());
          lua_setfield (L, -2, "key");

          lua_pushvalue (L, -1);
          nclua_sendx (nc);
          nclua_send (nc, L);
     }

tail:
     UNLOCK ();

     return true;
}


// Extra public stuff.

GingaScreenID LuaPlayer::getScreenId ()
{
     GingaScreenID screen;

     LOCK ();
     screen = this->myScreen;
     UNLOCK ();

     return screen;
}

ILocalScreenManager *LuaPlayer::getScreenManager ()
{
     ILocalScreenManager *dm;

     LOCK ();
     dm = this->dm;
     UNLOCK ();

     return dm;
}

void LuaPlayer::refreshContent (void)
{
     LOCK ();

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
     IPlayer *createLuaPlayer (GingaScreenID id, const char *mrl,
                               bool has_visual)
     {
          return new LuaPlayer (id, string (mrl));
     }

     void destroyLuaPlayer (IPlayer * player)
     {
          delete player;
     }
}

LUAPLAYER_END_DECLS

// Local variables:
// mode: c++
// c-file-style: "k&r"
// End:
