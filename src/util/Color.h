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

#ifndef COLOR_H
#define COLOR_H

#include "ginga.h"

GINGA_UTIL_BEGIN

class Color
{
private:
  int r, g, b, alpha;

protected:
  void setColorToI (string color);

public:
  static const int white = 0xFFFFFF;
  static const int yellow = 0xFFFF00;
  static const int red = 0xFF0000;
  static const int pink = 0xFFC0CB;
  static const int orange = 0xFFA500;
  static const int magenta = 0xFF00FF;
  static const int green = 0x008000;
  static const int cyan = 0x00FFFF;
  static const int blue = 0x0000FF;
  static const int lightGray = 0xD3D3D3;
  static const int gray = 0x808080;
  static const int darkGray = 0xA9A9A9;
  static const int black = 0x000000;
  static const int silver = 0xC0C0C0;
  static const int maroon = 0x800000;
  static const int fuchsia = 0xFF00FF;
  static const int purple = 0x800080;
  static const int lime = 0x00FF00;
  static const int olive = 0x808000;
  static const int navy = 0x000080;
  static const int aqua = 0x00FFFF;
  static const int teal = 0x008080;

  static const string swhite;
  static const string syellow;
  static const string sred;
  static const string spink;
  static const string sorange;
  static const string smagenta;
  static const string sgreen;
  static const string scyan;
  static const string sblue;
  static const string slightGray;
  static const string sgray;
  static const string sdarkGray;
  static const string sblack;
  static const string ssilver;
  static const string smaroon;
  static const string sfuchsia;
  static const string spurple;
  static const string slime;
  static const string solive;
  static const string snavy;
  static const string saqua;
  static const string steal;

  static int colortoi (string color);
  Color ();
  Color (int r, int g, int b, int alpha = 255);
  Color (string color, int alpha = 255);
  void setColor (string color);
  void setColor (int red, int green, int blue);
  int getR ();
  int getG ();
  int getB ();
  int getAlpha ();
  uint32_t getRGBA ();
  uint32_t getARGB ();
};

GINGA_UTIL_END

#endif /* COLOR_H */
