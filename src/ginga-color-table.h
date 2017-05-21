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

#ifndef GINGA_COLOR_TABLE_H
#define GINGA_COLOR_TABLE_H

#include "ginga.h"
#include <glib.h>
#include <glib/gprintf.h>

typedef struct _GingaColorTable
{
  const char *name;
  guchar r;
  guchar g;
  guchar b;
} GingaColorTable;

static const GingaColorTable ginga_color_table[] = {
  /* KEEP THIS SORTED ALPHABETICALLY */
  {"aliceBlue", 240, 248, 255},
  {"antiqueWhite", 250, 235, 215},
  {"aqua", 0, 255, 255},
  {"aquamarine", 127, 255, 212},
  {"azure", 240, 255, 255},
  {"beige", 245, 245, 220},
  {"bisque", 255, 228, 196},
  {"black", 0, 0, 0},
  {"blanchedAlmond", 255, 235, 205},
  {"blue", 0, 0, 255},
  {"blueViolet", 138, 43, 226},
  {"brown", 165, 42, 42},
  {"burlyWood", 222, 184, 135},
  {"cadetBlue", 95, 158, 160},
  {"chartreuse", 127, 255, 0},
  {"chocolate", 210, 105, 30},
  {"coral", 255, 127, 80},
  {"cornflowerBlue", 100, 149, 237},
  {"cornsilk", 255, 248, 220},
  {"crimson", 220, 20, 60},
  {"cyan", 0, 255, 255},
  {"darkBlue", 0, 0, 139},
  {"darkCyan", 0, 139, 139},
  {"darkGoldenRod", 184, 134, 11},
  {"darkGray", 169, 169, 169},
  {"darkGreen", 0, 100, 0},
  {"darkKhaki", 189, 183, 107},
  {"darkMagenta", 139, 0, 139},
  {"darkOliveGreen", 85, 107, 47},
  {"darkOrange", 255, 140, 0},
  {"darkOrchid", 153, 50, 204},
  {"darkRed", 139, 0, 0},
  {"darkSalmon", 233, 150, 122},
  {"darkSeaGreen", 143, 188, 143},
  {"darkSlateBlue", 72, 61, 139},
  {"darkSlateGray", 47, 79, 79},
  {"darkTurquoise", 0, 206, 209},
  {"darkViolet", 148, 0, 211},
  {"deepPink", 255, 20, 147},
  {"deepSkyBlue", 0, 191, 255},
  {"dimGray", 105, 105, 105},
  {"dodgerBlue", 30, 144, 255},
  {"fireBrick", 178, 34, 34},
  {"floralWhite", 255, 250, 240},
  {"forestGreen", 34, 139, 34},
  {"fuchsia", 255, 0, 255},
  {"gainsboro", 220, 220, 220},
  {"ghostWhite", 248, 248, 255},
  {"gold", 255, 215, 0},
  {"goldenRod", 218, 165, 32},
  {"gray", 128, 128, 128},
  {"green", 0, 128, 0},
  {"greenYellow", 173, 255, 47},
  {"honeyDew", 240, 255, 240},
  {"hotPink", 255, 105, 180},
  {"indianRed", 205, 92, 92},
  {"indigo", 75, 0, 130},
  {"ivory", 255, 255, 240},
  {"khaki", 240, 230, 140},
  {"lavender", 230, 230, 250},
  {"lavenderBlush", 255, 240, 245},
  {"lawnGreen", 124, 252, 0},
  {"lemonChiffon", 255, 250, 205},
  {"lightBlue", 173, 216, 230},
  {"lightCoral", 240, 128, 128},
  {"lightCyan", 224, 255, 255},
  {"lightGoldenRodYellow", 250, 250, 210},
  {"lightGray", 211, 211, 211},
  {"lightGreen", 144, 238, 144},
  {"lightPink", 255, 182, 193},
  {"lightSalmon", 255, 160, 122},
  {"lightSeaGreen", 32, 178, 170},
  {"lightSkyBlue", 135, 206, 250},
  {"lightSlateGray", 119, 136, 153},
  {"lightSteelBlue", 176, 196, 222},
  {"lightYellow", 255, 255, 224},
  {"lime", 0, 255, 0},
  {"limeGreen", 50, 205, 50},
  {"linen", 250, 240, 230},
  {"magenta", 255, 0, 255},
  {"maroon", 128, 0, 0},
  {"mediumAquaMarine", 102, 205, 170},
  {"mediumBlue", 0, 0, 205},
  {"mediumOrchid", 186, 85, 211},
  {"mediumPurple", 147, 112, 219},
  {"mediumSeaGreen", 60, 179, 113},
  {"mediumSlateBlue", 123, 104, 238},
  {"mediumSpringGreen", 0, 250, 154},
  {"mediumTurquoise", 72, 209, 204},
  {"mediumVioletRed", 199, 21, 133},
  {"midnightBlue", 25, 25, 112},
  {"mintCream", 245, 255, 250},
  {"mistyRose", 255, 228, 225},
  {"moccasin", 255, 228, 181},
  {"mavajoWhite", 255, 222, 173},
  {"navy", 0, 0, 128},
  {"oldLace", 253, 245, 230},
  {"olive", 128, 128, 0},
  {"oliveDrab", 107, 142, 35},
  {"orange", 255, 165, 0},
  {"orangeRed", 255, 69, 0},
  {"orchid", 218, 112, 214},
  {"paleGoldenRod", 238, 232, 170},
  {"paleGreen", 152, 251, 152},
  {"paleTurquoise", 175, 238, 238},
  {"paleVioletRed", 219, 112, 147},
  {"papayaWhip", 255, 239, 213},
  {"peachPuff", 255, 218, 185},
  {"peru", 205, 133, 63},
  {"pink", 255, 192, 203},
  {"plum", 221, 160, 221},
  {"powderBlue", 176, 224, 230},
  {"purple", 128, 0, 128},
  {"red", 255, 0, 0},
  {"rosyBrown", 188, 143, 143},
  {"royalBlue", 65, 105, 225},
  {"saddleBrown", 139, 69, 19},
  {"salmon", 250, 128, 114},
  {"sandyBrown", 244, 164, 96},
  {"seaGreen", 46, 139, 87},
  {"seaShell", 255, 245, 238},
  {"sienna", 160, 82, 45},
  {"silver", 192, 192, 192},
  {"skyBlue", 135, 206, 235},
  {"slateBlue", 106, 90, 205},
  {"slateGray", 112, 128, 144},
  {"snow", 255, 250, 250},
  {"springGreen", 0, 255, 127},
  {"steelBlue", 70, 130, 180},
  {"tan", 210, 180, 140},
  {"teal", 0, 128, 128},
  {"thistle", 216, 191, 216},
  {"tomato", 255, 99, 71},
  {"turquoise", 64, 224, 208},
  {"violet", 238, 130, 238},
  {"wheat", 245, 222, 179},
  {"white", 255, 255, 255},
  {"whiteSmoke", 245, 245, 245},
  {"yellow", 255, 255, 0},
  {"yellowGreen", 154, 205, 50},
};

static G_GNUC_PURE int
ginga_color_table_compar (const void *e1, const void *e2)
{
  const GingaColorTable *c1;
  const GingaColorTable *c2;

  c1 = (const GingaColorTable *) e1;
  c2 = (const GingaColorTable *) e2;

  return g_ascii_strcasecmp (c1->name, c2->name);
}

/* Gets the color value associated with the given name.  If NAME is in color
   table, stores its color components into *R, *G, *B, and returns true,
   otherwise returns false.  */
static inline gboolean
ginga_color_table_index (const char *name, guchar *r, guchar *g, guchar *b)
{
  GingaColorTable key;
  GingaColorTable *match;

  name = g_ascii_strdown(name,-1);

  key.name = name;
  match = (GingaColorTable *)
    bsearch (&key, ginga_color_table, G_N_ELEMENTS (ginga_color_table),
             sizeof (*ginga_color_table), ginga_color_table_compar);
  if (match == NULL)
    return FALSE;

  *r = match->r;
  *g = match->g;
  *b = match->b;


  return TRUE;
}

/* Formating of Hex Input to 8 bytes standard (#RRGGBBAA) */
static inline const char *
ginga_color_hex_formatter(string hex){
   //remove #
   if(hex[0]=='#') hex.erase(0, 1);
   // 16 colors GrayScale
   if(hex.length() == 1){
      hex.insert(1, string(1,hex[0]) );
      hex.insert(2, string(1,hex[0]) );
      hex.insert(3, string(1,hex[0]) );
      hex.insert(4, string(1,hex[0]) );
      hex.insert(5, string(1,hex[0]) );
      hex.append("FF");
   } // 256 colors GrayScale
    if(hex.length() == 2){
      hex.insert(2, string(1,hex[0]) );
      hex.insert(3, string(1,hex[1]) );
      hex.insert(4, string(1,hex[0]) );
      hex.insert(5, string(1,hex[1]) );
      hex.append("FF");
   } // RGB 16 colors per channel
   else if(hex.length() == 3){
      hex.insert(3, string(1,hex[2]) );
      hex.insert(2, string(1,hex[1]) );
      hex.insert(1, string(1,hex[0]) );
      hex.append("FF");
  } // RGB 16 colors per channel and A with 16 levels
   else if(hex.length() == 4){
      hex.insert(4, string(1,hex[3]) );
      hex.insert(3, string(1,hex[2]) );
      hex.insert(2, string(1,hex[1]) );
      hex.insert(1, string(1,hex[0]) );
   } //RGB 256 colors per channel and A with 16 levels
   else if(hex.length() == 5){
      hex.insert(5, string(1,hex[4]) );
      hex.append("FF");
   } //RGB 256 colors per channel
   else if(hex.length() == 6){
      hex.append("FF");
   } //RGB 256 colors per channel and A with 16 levels
   else if(hex.length() == 7){
       hex.insert(7, string(1,hex[6]) );
   } //RGB 256 colors per channel and A with 256 levels
   else if(hex.length() > 8){
      return  hex.substr (0,8).c_str(); //limit to 8 characters
   }
   return hex.c_str();
}

/* Gets the matching SDL_Color to the given RGB or RGBA input (255,255,255
or 255,255,255,255).  If the input is valid and returns true, otherwise
returns false.  */
static inline gboolean
ginga_rgba_to_sdl_color(const string value,  SDL_Color *color){
   gchar **pixels = g_strsplit(value.c_str(),",",-1 );
   if(g_strv_length(pixels) < 3 )
         return FALSE;

   color->r = xstrtouint8 (pixels[0], 10);
   color->g = xstrtouint8 (pixels[1], 10);
   color->b = xstrtouint8 (pixels[2], 10);
   if(pixels[3] != NULL)
     color->a = xstrtouint8 (pixels[3], 10);
   else
     color->a=255;

   g_strfreev(pixels);
   return TRUE;
}

/* Gets the matching SDL_Color to the given RGB or RGBA text input [rgb(255,255,255)
or rgba(255,255,255,255)]. If the input is valid and returns true, otherwise
returns false.  */
static inline gboolean
ginga_rgbatext_to_sdl_color(const string value, SDL_Color *color){
      gchar **pixels = g_strsplit( value.substr
                                   (value.find("(")+1,
                                    value.find(")")-value.find("(")-1).c_str()
                                   ,",",-1 );
   if(g_strv_length(pixels) < 3 )
         return FALSE;

   color->r = xstrtouint8 (pixels[0], 10);
   color->g = xstrtouint8 (pixels[1], 10);
   color->b = xstrtouint8 (pixels[2], 10);
   if(pixels[3] != NULL)
     color->a = xstrtouint8 (pixels[3], 10);
   else
     color->a=255;

   g_strfreev(pixels);
   return TRUE;
}

/* Gets the matching SDL_Color to the given HEX code input.  If the input
is valid returns true, otherwise returns false.  */
static inline gboolean
ginga_hex_to_sdl_color(const string hex, SDL_Color *color){
   const char *c = ginga_color_hex_formatter(hex);
   if (strlen(c) < 8)
     return FALSE;
   int hexValue=0;

   for(int i=0; i<8; i++, ++c){
         int  thisC = *c;
        thisC = toupper (thisC);
        hexValue <<= 4;
        if( thisC >= 48 &&  thisC <= 57 )
            hexValue += thisC - 48;
        else if( thisC >= 65 && thisC <= 70)
            hexValue += thisC - 65 + 10;
        else return FALSE;
    }

   color->r = (guint8)((hexValue >> 24) & 0xFF); // extract the RR byte
   color->g = (guint8)((hexValue >> 16) & 0xFF); // extract the GG byte
   color->b = (guint8)((hexValue >> 8) & 0xFF);  // extract the BB byte
   color->a = (guint8)((hexValue) & 0xFF);       // extract the AA byte

   return TRUE;
}

/* Convert byte color to percentage  */
static inline double
ginga_color_percent(guint c){
    if(c>0) return (double)c/255;
    else return 0;
}

/* Gets the matching SDL_Color to the given input.  If Value is a
 HEX (#FFF) the function calls 'ginga_hex_to_sdl_color'. If  Value is a RGB_TEXT,
 like 'rgb(255,255,255)', the function calls 'ginga_rgbatext_to_sdl_color'. If the
 value is a COLOR NAME the function calls 'ginga_color_table_index'. Return true if
 the input is valid, otherwise returns false. */
static inline gboolean
ginga_color_input_to_sdl_color(const string &value, SDL_Color *color){
   if(value[0] == '#') //by hex
         return ginga_hex_to_sdl_color(value, color);
   else if(value.substr (0,3)=="rgb") //by rgbatxt
         return ginga_rgbatext_to_sdl_color(value, color);
   else  if(ginga_color_table_index (value.c_str(), &color->r, &color->g, &color->b )){  //by name
         color->a=255;
         return TRUE;
   }
   else
         return ginga_rgba_to_sdl_color(value, color);
}

#endif /* GINGA_COLOR_TABLE_H */
