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

#include "ginga.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wimplicit-fallthrough)

static map<string, SDL_Color> colortab =
{
  {"aliceBlue", {240, 248, 255, 255}},
  {"antiqueWhite", {250, 235, 215, 255}},
  {"aqua", {0, 255, 255, 255}},
  {"aquamarine", {127, 255, 212, 255}},
  {"azure", {240, 255, 255, 255}},
  {"beige", {245, 245, 220, 255}},
  {"bisque", {255, 228, 196, 255}},
  {"black", {0, 0, 0, 255}},
  {"blanchedAlmond", {255, 235, 205, 255}},
  {"blue", {0, 0, 255, 255}},
  {"blueViolet", {138, 43, 226, 255}},
  {"brown", {165, 42, 42, 255}},
  {"burlyWood", {222, 184, 135, 255}},
  {"cadetBlue", {95, 158, 160, 255}},
  {"chartreuse", {127, 255, 0, 255}},
  {"chocolate", {210, 105, 30, 255}},
  {"coral", {255, 127, 80, 255}},
  {"cornflowerBlue", {100, 149, 237, 255}},
  {"cornsilk", {255, 248, 220, 255}},
  {"crimson", {220, 20, 60, 255}},
  {"cyan", {0, 255, 255, 255}},
  {"darkBlue", {0, 0, 139, 255}},
  {"darkCyan", {0, 139, 139, 255}},
  {"darkGoldenRod", {184, 134, 11, 255}},
  {"darkGray", {169, 169, 169, 255}},
  {"darkGreen", {0, 100, 0, 255}},
  {"darkKhaki", {189, 183, 107, 255}},
  {"darkMagenta", {139, 0, 139, 255}},
  {"darkOliveGreen", {85, 107, 47, 255}},
  {"darkOrange", {255, 140, 0, 255}},
  {"darkOrchid", {153, 50, 204, 255}},
  {"darkRed", {139, 0, 0, 255}},
  {"darkSalmon", {233, 150, 122, 255}},
  {"darkSeaGreen", {143, 188, 143, 255}},
  {"darkSlateBlue", {72, 61, 139, 255}},
  {"darkSlateGray", {47, 79, 79, 255}},
  {"darkTurquoise", {0, 206, 209, 255}},
  {"darkViolet", {148, 0, 211, 255}},
  {"deepPink", {255, 20, 147, 255}},
  {"deepSkyBlue", {0, 191, 255, 255}},
  {"dimGray", {105, 105, 105, 255}},
  {"dodgerBlue", {30, 144, 255, 255}},
  {"fireBrick", {178, 34, 34, 255}},
  {"floralWhite", {255, 250, 240, 255}},
  {"forestGreen", {34, 139, 34, 255}},
  {"fuchsia", {255, 0, 255, 255}},
  {"gainsboro", {220, 220, 220, 255}},
  {"ghostWhite", {248, 248, 255, 255}},
  {"gold", {255, 215, 0, 255}},
  {"goldenRod", {218, 165, 32, 255}},
  {"gray", {128, 128, 128, 255}},
  {"green", {0, 128, 0, 255}},
  {"greenYellow", {173, 255, 47, 255}},
  {"honeyDew", {240, 255, 240, 255}},
  {"hotPink", {255, 105, 180, 255}},
  {"indianRed", {205, 92, 92, 255}},
  {"indigo", {75, 0, 130, 255}},
  {"ivory", {255, 255, 240, 255}},
  {"khaki", {240, 230, 140, 255}},
  {"lavender", {230, 230, 250, 255}},
  {"lavenderBlush", {255, 240, 245, 255}},
  {"lawnGreen", {124, 252, 0, 255}},
  {"lemonChiffon", {255, 250, 205, 255}},
  {"lightBlue", {173, 216, 230, 255}},
  {"lightCoral", {240, 128, 128, 255}},
  {"lightCyan", {224, 255, 255, 255}},
  {"lightGoldenRodYellow", {250, 250, 210, 255}},
  {"lightGray", {211, 211, 211, 255}},
  {"lightGreen", {144, 238, 144, 255}},
  {"lightPink", {255, 182, 193, 255}},
  {"lightSalmon", {255, 160, 122, 255}},
  {"lightSeaGreen", {32, 178, 170, 255}},
  {"lightSkyBlue", {135, 206, 250, 255}},
  {"lightSlateGray", {119, 136, 153, 255}},
  {"lightSteelBlue", {176, 196, 222, 255}},
  {"lightYellow", {255, 255, 224, 255}},
  {"lime", {0, 255, 0, 255}},
  {"limeGreen", {50, 205, 50, 255}},
  {"linen", {250, 240, 230, 255}},
  {"magenta", {255, 0, 255, 255}},
  {"maroon", {128, 0, 0, 255}},
  {"mediumAquaMarine", {102, 205, 170, 255}},
  {"mediumBlue", {0, 0, 205, 255}},
  {"mediumOrchid", {186, 85, 211, 255}},
  {"mediumPurple", {147, 112, 219, 255}},
  {"mediumSeaGreen", {60, 179, 113, 255}},
  {"mediumSlateBlue", {123, 104, 238, 255}},
  {"mediumSpringGreen", {0, 250, 154, 255}},
  {"mediumTurquoise", {72, 209, 204, 255}},
  {"mediumVioletRed", {199, 21, 133, 255}},
  {"midnightBlue", {25, 25, 112, 255}},
  {"mintCream", {245, 255, 250, 255}},
  {"mistyRose", {255, 228, 225, 255}},
  {"moccasin", {255, 228, 181, 255}},
  {"mavajoWhite", {255, 222, 173, 255}},
  {"navy", {0, 0, 128, 255}},
  {"oldLace", {253, 245, 230, 255}},
  {"olive", {128, 128, 0, 255}},
  {"oliveDrab", {107, 142, 35, 255}},
  {"orange", {255, 165, 0, 255}},
  {"orangeRed", {255, 69, 0, 255}},
  {"orchid", {218, 112, 214, 255}},
  {"paleGoldenRod", {238, 232, 170, 255}},
  {"paleGreen", {152, 251, 152, 255}},
  {"paleTurquoise", {175, 238, 238, 255}},
  {"paleVioletRed", {219, 112, 147, 255}},
  {"papayaWhip", {255, 239, 213, 255}},
  {"peachPuff", {255, 218, 185, 255}},
  {"peru", {205, 133, 63, 255}},
  {"pink", {255, 192, 203, 255}},
  {"plum", {221, 160, 221, 255}},
  {"powderBlue", {176, 224, 230, 255}},
  {"purple", {128, 0, 128, 255}},
  {"red", {255, 0, 0, 255}},
  {"rosyBrown", {188, 143, 143, 255}},
  {"royalBlue", {65, 105, 225, 255}},
  {"saddleBrown", {139, 69, 19, 255}},
  {"salmon", {250, 128, 114, 255}},
  {"sandyBrown", {244, 164, 96, 255}},
  {"seaGreen", {46, 139, 87, 255}},
  {"seaShell", {255, 245, 238, 255}},
  {"sienna", {160, 82, 45, 255}},
  {"silver", {192, 192, 192, 255}},
  {"skyBlue", {135, 206, 235, 255}},
  {"slateBlue", {106, 90, 205, 255}},
  {"slateGray", {112, 128, 144, 255}},
  {"snow", {255, 250, 250, 255}},
  {"springGreen", {0, 255, 127, 255}},
  {"steelBlue", {70, 130, 180, 255}},
  {"tan", {210, 180, 140, 255}},
  {"teal", {0, 128, 128, 255}},
  {"thistle", {216, 191, 216, 255}},
  {"tomato", {255, 99, 71, 255}},
  {"turquoise", {64, 224, 208, 255}},
  {"violet", {238, 130, 238, 255}},
  {"wheat", {245, 222, 179, 255}},
  {"white", {255, 255, 255, 255}},
  {"whiteSmoke", {245, 245, 245, 255}},
  {"yellow", {255, 255, 0, 255}},
  {"yellowGreen", {154, 205, 50, 255}},
};

bool
ginga_color_table_index (const string &color, SDL_Color *result)
{
  map<string, SDL_Color>::iterator it;
  if ((it = colortab.find (color)) == colortab.end ())
    return false;
  set_if_nonnull (result, it->second);
  return true;
}

static string
ginga_color_hex_normalize (string hex)
{
  if (hex[0] == '#')
    hex.erase (0, 1);

  switch (hex.length ())
    {
    case 1:
      // 16 colors gray-scale.
      hex.insert (1, string (1, hex[0]));
      hex.insert (2, string (1, hex[0]));
      hex.insert (3, string (1, hex[0]));
      hex.insert (4, string (1, hex[0]));
      hex.insert (5, string (1, hex[0]));
      hex.append("FF");
      break;
    case 2:
      // 256 colors gray scale.
      hex.insert (2, string (1, hex[0]));
      hex.insert (3, string (1, hex[1]));
      hex.insert (4, string (1, hex[0]));
      hex.insert (5, string (1, hex[1]));
      hex.append("FF");
      break;
    case 3:
      // RGB 16 colors per channel.
      hex.insert(3, string (1, hex[2]));
      hex.insert(2, string (1, hex[1]));
      hex.insert(1, string (1, hex[0]));
      hex.append("FF");
      break;
    case 4:
      // RGB 16 colors per channel and A with 16 levels.
      hex.insert (4, string (1, hex[3]));
      hex.insert (3, string (1, hex[2]));
      hex.insert (2, string (1, hex[1]));
      hex.insert (1, string (1, hex[0]));
      break;
    case 5:
      // RGB 256 colors per channel and A with 16 levels.
      hex.insert (5, string (1, hex[4]));
      hex.append("FF");
      break;
    case 6:
      // RGB 256 colors per channel.
      hex.append("FF");
      break;
    case 7:
      // RGB 256 colors per channel and A with 16 levels.
      hex.insert (7, string (1, hex[6]));
      break;
    case 8:
    default:
      // RGBA 256.
      break;
   }
  hex.assign (hex.substr (0,8));
  return hex;
}

bool
ginga_color_parse (const string &color, SDL_Color *result)
{
  SDL_Color clr = {0, 0, 0, 255}; // black
  string str;

  str = xstrchomp (color);
  if (str[0] == '#')
    {
      str = ginga_color_hex_normalize (str);
      clr.r = xstrtouint8 (str.substr (0,2), 16);
      clr.g = xstrtouint8 (str.substr (2,2), 16);
      clr.b = xstrtouint8 (str.substr (4,2), 16);
      clr.a = xstrtouint8 (str.substr (6,2), 16);
      goto success;
    }

  if (str.substr (0,3) == "rgb")
    {
      vector<string> v;
      size_t first, last;

      first = str.find ("(");
      if (first == string::npos)
        goto failure;

      last = str.find (")");
      if (last == string::npos)
        goto failure;

      v = xstrsplit (str.substr (first, last - first), ',');
      switch (v.size ())
        {
        case 4:
          clr.a = xstrtouint8 (v[3], 10);
          // fall-through
        case 3:
          clr.r = xstrtouint8 (v[0], 10);
          clr.g = xstrtouint8 (v[1], 10);
          clr.b = xstrtouint8 (v[2], 10);
          goto success;
        default:
          goto failure;
        }
    }

  if (!ginga_color_table_index (color, &clr))
    goto failure;

 success:
  set_if_nonnull (result, clr);
  return true;

 failure:
  return false;
}
