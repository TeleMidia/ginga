/* LuaCanvas.cpp -- The NCLua Canvas API.
   Copyright (C) 2006-2012 PUC-Rio/Laboratorio TeleMidia

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

extern "C"
{
#include <assert.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include "nclua.h"
#include "nclua-private.h"
}

#include "player/LuaPlayer.h"
#include "player/ImagePlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "player/PlayersComponentSupport.h"

#include "mb/interface/IFontProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#define LUAPLAYER_CANVAS  "luaplayer.Canvas"
#define REFFILL           (-3)
#define REFFRAME          (-4)
#define CHECKCANVAS(L) ((Canvas*) luaL_checkudata (L, 1, LUAPLAYER_CANVAS))

static inline LuaPlayer *
GETPLAYER (lua_State *L)
{
  nclua_t *nc;
  LuaPlayer *player;
  nc = nclua_get_nclua_state (L);
  player = (LuaPlayer *) nclua_get_user_data (nc, NULL);
  return player;
}

typedef struct
{
  ISurface *sfc;
  int    collect;
  Color *color;
  struct { int x; int y; int w; int h; int inUse; } crop;
  struct { int x; int y; int w; int h; } clip;
  struct { int w; int h; int inUse;} scale;
  struct { char face[20]; int size; char style[20]; } font;
} Canvas;


/* Function prototypes:  */
int lua_createcanvas (lua_State *L, ISurface *sfc, int collect);

static int
l_new (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);
  LuaPlayer *player = GETPLAYER (L);
  GingaScreenID id = player->getScreenId ();
  ILocalScreenManager *sm = player->getScreenManager ();
  ISurface *sfc;

  switch (lua_type (L, 2))
    {
    case LUA_TSTRING:
      {
        const char *path = luaL_checkstring (L, 2);
        sfc = sm->createRenderedSurfaceFromImageFile (id, path);
        if (sfc == NULL)
          {
            lua_pushnil (L);
            lua_pushfstring (L, "no such file: %s", path);
            return 2;
          }
        break;
      }
    case LUA_TNUMBER:
      {
        int width = luaL_checkint (L, 2);
        int height = luaL_checkint (L, 3);
        sfc = sm->createSurface (id, width, height);
        sfc->clearContent ();
        break;
      }
    default:
      luaL_argerror (L, 2, NULL);
    }

  return lua_createcanvas (L, sfc, 1);
}

/* canvas:attrSize () -> width, height: number
 */

static int
l_attrSize (lua_State * L)
{
  Canvas *canvas = CHECKCANVAS (L);

  if (lua_gettop (L) == 1)
    {
      int width;
      int height;
      canvas->sfc->getSize (&width, &height);

      lua_pushnumber (L, width);
      lua_pushnumber (L, height);
      return 2;
    }

  return luaL_error (L, "not supported"); /* set */
}

/* canvas:attrColor (R, G, B, A: number)
 * canvas:attrColor (clr_name: string)
 * canvas:attrColor () -> R, G, B, A: number
 */

static int
l_attrColor (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);
  int r;
  int g;
  int b;
  int a;

  if (lua_gettop (L) == 1)      /* get */
    {
      lua_pushinteger (L, canvas->color->getR ());
      lua_pushinteger (L, canvas->color->getG ());
      lua_pushinteger (L, canvas->color->getB ());
      lua_pushinteger (L, canvas->color->getAlpha ());
      return 4;
    }

  /* set */

  if (canvas->color != NULL)
    {
      delete canvas->color;
      canvas->color = NULL;
    }

  if (lua_type (L, -1) == LUA_TNUMBER)
    {
#     define tocolor(n) max (min (n, 255), 0)
      r = tocolor (luaL_checkint (L, 2));
      g = tocolor (luaL_checkint (L, 3));
      b = tocolor (luaL_checkint (L, 4));
      a = tocolor (luaL_checkint (L, 5));
      canvas->color = new Color (r, g, b, a);
    }
  else
    {
      const char *name = luaL_checkstring (L, -1);
      canvas->color = new Color (name);
    }

  r = canvas->color->getR ();
  g = canvas->color->getG ();
  b = canvas->color->getB ();
  a = canvas->color->getAlpha ();
  canvas->sfc->setColor (r, g, b, a);

  return 0;
}

static int
l_attrFont (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);

  if (lua_gettop (L) == 1)      /* get */
    {
      lua_pushstring (L, canvas->font.face);
      lua_pushinteger (L, canvas->font.size);
      lua_pushstring (L, canvas->font.style);
      return 3;
    }

  /* set */
  strncpy (canvas->font.face, luaL_checkstring (L,2),
           nelementsof (canvas->font.face));

  canvas->font.size = luaL_checkint (L, 3);

  strncpy (canvas->font.style, luaL_optstring (L, 4, "normal"),
           nelementsof (canvas->font.style));

  string path = SystemCompat::appendGingaFilesPrefix ("font")
    + SystemCompat::getIUriD ();

  path.append (canvas->font.face, strlen (canvas->font.face));

  if (path.length () < 4 || path.substr (path.length () - 4, 4) != ".ttf")
    {
      path = path + ".ttf";
    }

  IFontProvider *font = NULL;
  LuaPlayer *player = GETPLAYER (L);
  GingaScreenID id = player->getScreenId ();
  ILocalScreenManager *sm = player->getScreenManager ();
  font = sm->createFontProvider (id, path.c_str (), canvas->font.size);

  if (font == NULL)
    {
      luaL_error (L, "invalid font: %s", path.c_str ());
    }

  if (canvas->sfc == NULL)
    {
      luaL_error (L, "invalid canvas");
    }

  canvas->sfc->setSurfaceFont ((void *) font);
  return 0;
}

static int
l_attrCrop (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);

  if (lua_gettop (L) == 1)      /* get */
    {
      lua_pushinteger (L, canvas->crop.x);
      lua_pushinteger (L, canvas->crop.y);
      lua_pushinteger (L, canvas->crop.w);
      lua_pushinteger (L, canvas->crop.h);
      return 4;
    }

  /* set */

  canvas->crop.inUse = 1;
  canvas->crop.x = luaL_checkint (L, 2);
  canvas->crop.y = luaL_checkint (L, 3);
  canvas->crop.w = luaL_checkint (L, 4);
  canvas->crop.h = luaL_checkint (L, 5);
  return 0;
}

static int
l_attrClip (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);

  if (lua_gettop (L) == 1)       /* get */
    {
      lua_pushinteger (L, canvas->clip.x);
      lua_pushinteger (L, canvas->clip.y);
      lua_pushinteger (L, canvas->clip.w);
      lua_pushinteger (L, canvas->clip.h);
      return 4;
    }

  /* set */

  canvas->clip.x = luaL_checkint (L, 2);
  canvas->clip.y = luaL_checkint (L, 3);
  canvas->clip.w = luaL_checkint (L, 4);
  canvas->clip.h = luaL_checkint (L, 5);

  canvas->sfc->setClip (canvas->clip.x, canvas->clip.y,
                       canvas->clip.w, canvas->clip.h);
  return 0;
}

/* canvas:attrScale (w,h: number|boolean)
 * canvas:attrScale () -> w, h: number
 */
static int
l_attrScale (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);

  if (lua_gettop (L) == 1)       /* get */
    {
      lua_pushnumber (L, canvas->scale.w);
      lua_pushnumber (L, canvas->scale.h);
      return 2;
    }

  /* set */
  int width;
  int height;
  canvas->sfc->getSize (&width, &height);

  // For some reason DFB is using not the original size of the canvas
  // but the scaled size, if a scale was applied before.
  int scalew = width;
  int scaleh = height;

  if (canvas->scale.inUse)
    {
      scalew = canvas->scale.w;
      scaleh = canvas->scale.h;
    }

  if (lua_isboolean (L, 2))
    {
      int new_height = luaL_checkint (L, 3);
      height = new_height;
      width = ((double) new_height / height) * width;

    }
  else if (lua_isboolean (L, 3))
    {
      int new_width = luaL_checkint (L, 2);
      height = ((double) new_width / width) * height;
      width = new_width;
    }
  else
    {
      width = luaL_checkint (L, 2);
      height = luaL_checkint (L, 3);
    }

  double x = ((double) width / scalew);
  double y = ((double) height / scaleh);

  canvas->scale.w = width;
  canvas->scale.h = height;

  canvas->sfc->scale (x, y);
  canvas->scale.inUse = 1;

  return 0;
}

static int
l_drawLine (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);
  int x1 = luaL_checkint (L, 2);
  int y1 = luaL_checkint (L, 3);
  int x2 = luaL_checkint (L, 4);
  int y2 = luaL_checkint (L, 5);

  canvas->sfc->drawLine (x1, y1, x2, y2);
  return 0;
}

static int
l_drawRect (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);
  int x = luaL_checkint (L, 3);
  int y = luaL_checkint (L, 4);
  int w = luaL_checkint (L, 5);
  int h = luaL_checkint (L, 6);

  lua_settop (L, 6);
  lua_rawgeti (L, LUA_ENVIRONINDEX, REFFILL);
  if (lua_equal (L, 2, -1))     /* fill */
    {
      canvas->sfc->fillRectangle (x, y, w, h);
      return 0;
    }

  lua_rawgeti (L, LUA_ENVIRONINDEX, REFFRAME);
  if (lua_equal (L, 2, -1))     /* frame */
    {
      canvas->sfc->drawRectangle (x, y, w, h);
      return 0;
    }

  luaL_argerror (L, 2, "invalid mode");
  return 0;
}

static int
l_drawText (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);
  int x = luaL_checkint (L,2);
  int y = luaL_checkint (L,3);
  const char *text = luaL_checkstring (L, 4);
  canvas->sfc->drawString (x, y, text);
  return 0;
}

static int
l_flush (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);
  canvas->sfc->flip ();
  GETPLAYER (L)->refreshContent ();
  return 0;
}

static int
l_clear (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);
  canvas->sfc->clearContent ();
  return 0;
}

static int
l_compose (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);
  Canvas *src = (Canvas *) luaL_checkudata (L, 4, LUAPLAYER_CANVAS);
  int x = luaL_checkint (L, 2);
  int y = luaL_checkint (L, 3);

  if (src->crop.inUse == 1)
    {
      canvas->sfc->blit (x, y,src->sfc, src->crop.x, src->crop.y,
                         src->crop.w, src->crop.h);
    }
  else
    {
      canvas->sfc->blit (x, y, src->sfc);
    }
  return 0;
}

static int
l_measureText (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);
  int w;
  int h;
  canvas->sfc->getStringExtents (luaL_checkstring (L,2), &w, &h);
  lua_pushnumber (L, w);
  lua_pushnumber (L, h);
  return 2;
}

static int
l_gc (lua_State *L)
{
  Canvas *canvas = CHECKCANVAS (L);
  delete canvas->color;

  if (canvas->collect)
    {
      delete canvas->sfc;
      canvas->sfc = NULL;
    }
  return 0;
}


static const struct luaL_Reg meths[] = {
  { "new",         l_new         },
  { "attrSize",    l_attrSize    },
  { "attrColor",   l_attrColor   },
  { "attrFont",    l_attrFont    },
  { "attrCrop",    l_attrCrop    },
  { "attrClip",    l_attrClip    },
  { "attrScale",   l_attrScale   },
  { "drawLine",    l_drawLine    },
  { "drawRect",    l_drawRect    },
  { "drawText",    l_drawText    },
  { "flush",       l_flush       },
  { "clear",       l_clear       },
  { "compose",     l_compose     },
  { "measureText", l_measureText },
  { "__gc",        l_gc          },
  { NULL,          NULL          }
};

int
luaopen_canvas (lua_State *L)
{
  lua_pushstring (L, "fill");
  lua_pushstring (L, "frame");
  lua_rawseti (L, LUA_ENVIRONINDEX, REFFRAME);
  lua_rawseti (L, LUA_ENVIRONINDEX, REFFILL);

  /* meta = { __index=meta } */
  luaL_newmetatable (L, LUAPLAYER_CANVAS);
  lua_pushvalue (L, -1);
  lua_setfield (L, -2, "__index");
  luaL_register (L, NULL, meths);
  lua_pop (L, 1);
  lua_pushnil (L);

  return 1;
}

int
lua_createcanvas (lua_State *L, ISurface *sfc, int collect)
{
  Canvas *canvas = (Canvas*) lua_newuserdata (L, sizeof (Canvas));
  canvas->sfc = sfc;
  canvas->collect = collect;
  luaL_getmetatable (L, LUAPLAYER_CANVAS);
  lua_setmetatable (L, -2);

  // default crop: none
  canvas->crop.inUse = 0;

  // default color: black
  canvas->color = new Color ("black");
  int r = canvas->color->getR ();
  int g = canvas->color->getG ();
  int b = canvas->color->getB ();
  int a = canvas->color->getAlpha ();
  canvas->sfc->setColor (r, g, b, a);

  return 1;
}
