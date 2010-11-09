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

/*******************************************************************************
 * CANVAS
 * A estrutura abaixo mantem um ponteiro para a superficie do DFB e atributos
 * configurados atraves de chamadas a `attr*()`.
 *
 * Os metodos com prefixo `attr` sao usado para ler e alterar atributos,
 * portanto sao sempre divididos em duas partes (GET e SET).
 * A parte GET acessa a estrutura e retorna os valores correntes.
 * A parte SET altera a estrutura e chama a funcao DFB correspondente.
 *
 * As funcoes de `draw*()` (primitivas graficas) e miscelanea apenas chamam as
 * funcoes DFB correspondentes.
 ******************************************************************************/

#include "player/LuaPlayer.h"
#include "player/ImagePlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "player/PlayersComponentSupport.h"

#include "system/io/interface/content/text/IFontProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#define LUAPLAYER_CANVAS  "luaplayer.Canvas"
#define REFFILL           (-3)
#define REFFRAME          (-4)
#define FONTDIR   "/usr/local/etc/ginga/files/font/"
#define CHECKCANVAS(L) ((Canvas*) luaL_checkudata(L, 1, LUAPLAYER_CANVAS))

typedef struct Canvas {
	ISurface* sfc;
	int    collect;
	Color* color;
	struct { int x; int y; int w; int h; int inUse; }
           crop;
	struct { int x; int y; int w; int h; }
           clip;
	struct { int w; int h; int inUse;}
	   scale;
	struct { char face[20]; int size; char style[20]; }
           font;
} Canvas;

/*******************************************************************************
 * canvas:new()
 * - Imagem: retorna um novo canvas com a imagem renderizada.
 * - Dimensoes: retorna um novo canvas com as dimensoes passadas.
 ******************************************************************************/

LUALIB_API int lua_createcanvas (lua_State* L, ISurface* sfc,
                                 int collect);
static int l_new (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);
	ISurface* sfc = NULL;
	int type = lua_type(L, 2);

	switch (type)
	{
		// IMAGE
		// [ canvas | img_path ]
		case LUA_TSTRING: {
			sfc = ImagePlayer::renderImage(
					(char*)luaL_checkstring(L, 2));

			break;
		}

		// NEW { w, h }
		// [ canvas | w | h ]
		case LUA_TNUMBER: {
#if HAVE_COMPSUPPORT
			sfc = ((SurfaceCreator*)(
					cm->getObject("Surface")))(
							NULL,
							luaL_checkint(L, 2),
							luaL_checkint(L, 3));

			sfc->setBgColor(canvas->color);
			sfc->clearContent();
#else
			sfc = new DFBSurface(luaL_checkint(L, 2), luaL_checkint(L, 3));
			sfc->setBgColor(canvas->color);
			sfc->clearContent();
#endif
			break;
		}

		default:
			return luaL_argerror(L, 2, NULL);
	}

	return lua_createcanvas(L, sfc, 1);  // [ ... | canvas ] -> canvas
}

/*******************************************************************************
 * ATRIBUTOS
 * - canvas:attrFont()
 * - canvas:attrCrop()
 * - canvas:attrClip()
 * - canvas:attrScale() 
 ******************************************************************************/

/*******************************************************************************
 * canvas:attrSize () -> width, height: number 
 ******************************************************************************/

static int l_attrSize (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);

	// GET
	// [ canvas ]
	if (lua_gettop(L) == 1) {
		int width, height;
		canvas->sfc->getSize(&width, &height);
		lua_pushnumber(L, width);   // [ canvas | width ]
		lua_pushnumber(L, height);  // [ canvas | width | height ]
		return 2;                   // -> width, height
	}

	// SET
	return luaL_error(L, "not supported");
}

/*******************************************************************************
 * canvas:attrColor (R, G, B, A: number)
 * canvas:attrColor (clr_name: string)
 * canvas:attrColor () -> R, G, B, A: number
 ******************************************************************************/
static int l_attrColor (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);

	// GET
	// [ canvas ]
	if (lua_gettop(L) == 1) {
		lua_pushnumber(L, canvas->color->getR());     // [ canvas|R ]
		lua_pushnumber(L, canvas->color->getG());     // [ canvas|R|G ]
		lua_pushnumber(L, canvas->color->getB());     // [ canvas|R|G|B ]
		lua_pushnumber(L, canvas->color->getAlpha()); // [ canvas|R|G|B|A ]
		return 4;                                     // -> R, G, B, A
	}

	// SET

	if (canvas->color != NULL) {
		delete canvas->color;
		canvas->color = NULL;
	}

	// [ canvas | R | G | B | A ]
	if (lua_type(L, -1) == LUA_TNUMBER)
	{
		// TODO: teste limites de r,g,b,a
		canvas->color = new Color(luaL_checkint(L, 2),
		                          luaL_checkint(L, 3),
								  luaL_checkint(L, 4),
								  luaL_checkint(L, 5));
	}
	// [ canvas | 'color' ]
	else
	{
		canvas->color = new Color(luaL_checkstring(L, -1));
	}
	canvas->sfc->setColor(canvas->color);
	return 0;
}

static int l_attrFont (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);

	// GET
	// [ canvas ]
	if (lua_gettop(L) == 1) {
		lua_pushstring(L, canvas->font.face);   // [ canvas|face ]
		lua_pushnumber(L, canvas->font.size);   // [ canvas|face|size ]
		lua_pushstring(L, canvas->font.style);  // [ canvas|face|size|style ]
		return 3;                               // -> face, size, style
	}

	// SET
	// [ canvas | face | size | style ]
	strncpy(canvas->font.face, luaL_checkstring(L,2), 20);
	canvas->font.size = luaL_checkint(L, 3);
	strncpy(canvas->font.style, luaL_optstring(L,4,"normal"), 20);

	char path[255]; path[0] = '\0';
	strncat(path, FONTDIR, 100);
	strncat(path, canvas->font.face, 20);
    //strncat(path, "-", 2);
    //strncat(path, canvas->font.style, 20);
	strncat(path, ".ttf", 5);
    //cout << "PATH: " << path << endl;

	IFontProvider* font = NULL;
#if HAVE_COMPSUPPORT
	font = ((FontProviderCreator*)(cm->getObject("FontProvider")))(
			path, canvas->font.size);
#else
	font = new DFBFontProvider(path, canvas->font.size);
#endif

	if (font == NULL) luaL_error(L, "invalid font: %s", path);
	canvas->sfc->setFont((void*)font);
	delete font;
	return 0;
}

static int l_attrCrop (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);

	// GET
	// [ canvas ]
	if (lua_gettop(L) == 1) {
		lua_pushnumber(L, canvas->crop.x);  // [ canvas | x ]
		lua_pushnumber(L, canvas->crop.y);  // [ canvas | x | y ]
		lua_pushnumber(L, canvas->crop.w);  // [ canvas | x | y | w ]
		lua_pushnumber(L, canvas->crop.h);  // [ canvas | x | y | w | h ]
		return 4;                           // -> x, y, w, h
	}

	// SET
	// [ canvas | x | y | w | h ]
    canvas->crop.inUse = 1;
	canvas->crop.x = luaL_checkint(L, 2);
	canvas->crop.y = luaL_checkint(L, 3);
	canvas->crop.w = luaL_checkint(L, 4);
	canvas->crop.h = luaL_checkint(L, 5);
	return 0;
}

static int l_attrClip (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);

	// GET
	// [ canvas ]
	if (lua_gettop(L) == 1) {
		lua_pushnumber(L, canvas->clip.x);  // [ canvas | x ]
		lua_pushnumber(L, canvas->clip.y);  // [ canvas | x | y ]
		lua_pushnumber(L, canvas->clip.w);  // [ canvas | x | y | w ]
		lua_pushnumber(L, canvas->clip.h);  // [ canvas | x | y | w | h ]
		return 4;                           // -> x, y, w, h
	}

	// SET
	// [ canvas | x | y | w | h ]
	canvas->clip.x = luaL_checkint(L, 2);
	canvas->clip.y = luaL_checkint(L, 3);
	canvas->clip.w = luaL_checkint(L, 4);
	canvas->clip.h = luaL_checkint(L, 5);

	canvas->sfc->setClip(
			canvas->clip.x, canvas->clip.y, canvas->clip.w, canvas->clip.h);

	return 0;
}

/*******************************************************************************
 * canvas:attrScale (w,h: number|boolean)
 * canvas:attrScale () -> w, h: number
 ******************************************************************************/
static int l_attrScale (lua_State* L)
{
	Canvas* canvas = CHECKCANVAS(L);

	// GET
	// [ canvas ]
	if (lua_gettop(L) == 1) {
		lua_pushnumber(L, canvas->scale.w);
		lua_pushnumber(L, canvas->scale.h);
		return 2;                          
	}

	//SET
	int width, height;
	canvas->sfc->getSize(&width, &height);
	
	// For some reason DFB is using not the original size of the canvas
	// but the scaled size, if a scale was applied before.
	int scalew = width;
	int scaleh = height;
	if(canvas->scale.inUse) {
		scalew = canvas->scale.w;
		scaleh = canvas->scale.h;
	}

	if(lua_isboolean(L, 2)) {
		int new_height = luaL_checkint(L, 3);		
		width = ((double)new_height/height) * width;
		height = new_height;
	
	} else if (lua_isboolean(L, 3)) {
		int new_width = luaL_checkint(L, 2);
		height = ((double)new_width/width) * height;
		width = new_width;
	} else {
		width = luaL_checkint(L, 2);
		height = luaL_checkint(L, 3);
	}

	double x = ((double)width/scalew);
	double y = ((double)height/scaleh);

	canvas->scale.w = width;
	canvas->scale.h = height;

	canvas->sfc->scale(x, y);
	canvas->scale.inUse = 1;

	return 0;
}

/******************************************************************************
 * PRIMITIVAS GRAFICAS:
 * - canvas:drawLine()
 * - canvas:drawRect()
 * - canvas:drawText()
 ******************************************************************************/

static int l_drawLine (lua_State* L)
{
	// [ canvas | x1 | y1 | x2 | y2 ]
	Canvas* canvas = CHECKCANVAS(L);
	canvas->sfc->drawLine(
			luaL_checkint(L, 2),
			luaL_checkint(L, 3),
			luaL_checkint(L, 4),
			luaL_checkint(L, 5));

	return 0;
}

static int l_drawRect (lua_State* L)
{
	// [ canvas | mode | x | y | w | h ]
	Canvas* canvas = CHECKCANVAS(L);
	lua_settop(L, 6);

	// FILL
    // [ canvas | mode | x | y | w | h | "fill" ]
	lua_rawgeti(L, LUA_ENVIRONINDEX, REFFILL);
	if (lua_equal(L, 2, -1)) {
		canvas->sfc->fillRectangle(luaL_checkint(L, 3),
                                             luaL_checkint(L, 4),
                                             luaL_checkint(L, 5),
                                             luaL_checkint(L, 6));
		return 0;
	}

	// FRAME
    // [ canvas | mode | x | y | w | h | "fill" | "frame" ]
	lua_rawgeti(L, LUA_ENVIRONINDEX, REFFRAME);
	if (lua_equal(L, 2, -1)) {
		canvas->sfc->drawRectangle(luaL_checkint(L, 3),
                                             luaL_checkint(L, 4),
                                             luaL_checkint(L, 5),
                                             luaL_checkint(L, 6));
		return 0;
	}

	return luaL_argerror(L, 2, "invalid mode");
}

static int l_drawText (lua_State* L)
{
    // [ canvas | x | y | text ]
	Canvas* canvas = CHECKCANVAS(L);
	canvas->sfc->drawString(luaL_checkint(L,2),
                            luaL_checkint(L,3), luaL_checkstring(L, 4));

	return 0;
}

/*******************************************************************************
 * MISCELANEA:
 * - canvas:flush()
 * - canvas:compose()
 * - canvas:measureText()
 ******************************************************************************/

static int l_flush (lua_State* L)
{ new Color("black");
	Canvas* canvas = CHECKCANVAS(L);
	canvas->sfc->flip();
	GETPLAYER(L)->refreshContent();
	return 0;
}

static int l_clear (lua_State* L)
{
    // [ ]
	Canvas* canvas = CHECKCANVAS(L);
    canvas->sfc->setBgColor(canvas->color);
    return 0;
}

static int l_compose (lua_State* L)
{
    // [ canvas | x | y | src ]
	Canvas* canvas = CHECKCANVAS(L);
	Canvas* src    = (Canvas*) luaL_checkudata(L, 4, LUAPLAYER_CANVAS);
	int x = luaL_checkint(L, 2);
	int y = luaL_checkint(L, 3);

	if (src->crop.inUse == 1) {
		canvas->sfc->blit(
				x, y,
				src->sfc, src->crop.x, src->crop.y, src->crop.w, src->crop.h);

	} else {
		canvas->sfc->blit(x, y, src->sfc);
	}

	return 0;
}

static int l_measureText (lua_State* L)
{
	// [ canvas | text ]
	int w, h;
	Canvas* canvas = CHECKCANVAS(L);

	canvas->sfc->getStringExtents(luaL_checkstring(L,2), &w, &h);

	lua_pushnumber(L, w);  // [ canvas | text | w ]
	lua_pushnumber(L, h);  // [ canvas | text | w | h ]
	return 2;
}

static int l_gc (lua_State* L)
{
	// [ canvas ]
	Canvas* canvas = CHECKCANVAS(L);
	delete canvas->color;

	if (canvas->collect) {
		delete canvas->sfc;
		canvas->sfc = NULL;
	}

	return 0;
}

/*********************************************************************
 * Funcoes exportadas pelo modulo.
 ********************************************************************/

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

/*********************************************************************
 * Funcao que carrega o modulo.
 * - cria as variaveis locais ao modulo
 ********************************************************************/

LUALIB_API int luaopen_canvas (lua_State* L)
{
	// [ ... ]
	lua_pushstring(L, "fill");                       // [ ... | 1 ]
	lua_pushstring(L, "frame");                      // [ ... | 2 ]
	lua_rawseti(L, LUA_ENVIRONINDEX, REFFRAME);      // [ ... | 1 ]
	lua_rawseti(L, LUA_ENVIRONINDEX, REFFILL);       // [ ... ]

	// meta = { __index=meta }
	luaL_newmetatable(L, LUAPLAYER_CANVAS);          // [ ... | meta ]
	lua_pushvalue(L, -1);                            // [ ... | meta | meta ]
	lua_setfield(L, -2, "__index");                  // [ ... | meta ]

	luaL_register(L, NULL, meths);                   // [ ... | meta ]
	lua_pop(L, 1);                                   // [ ... ]
	lua_pushnil(L);                                  // [ ... | nil ]
	return 1;
}

/*********************************************************************
 * FUNCOES INTERNAS
 ********************************************************************/

LUALIB_API int lua_createcanvas (lua_State* L, ISurface* sfc, int collect)
{
	// [ ... ]
	Canvas* canvas = (Canvas*) lua_newuserdata(L, sizeof(Canvas));
                                               // [ ... | canvas ]
	canvas->sfc = sfc;
	canvas->collect = collect;
	luaL_getmetatable(L, LUAPLAYER_CANVAS);    // [ ... | canvas | meta ]
	lua_setmetatable(L, -2);                   // [ ... | canvas ]

    // default crop: none
    canvas->crop.inUse = 0;

	// default color: black
	canvas->color = new Color("black");
	canvas->sfc->setColor(canvas->color);

	return 1;
}
