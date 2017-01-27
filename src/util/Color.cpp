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
#include "Color.h"
#include "functions.h"

GINGA_UTIL_BEGIN

const string Color::swhite = "#FFFFFF";
const string Color::syellow = "#FFFF00";
const string Color::sred = "#FF0000";
const string Color::spink = "#FFC0CB";
const string Color::sorange = "#FFA500";
const string Color::smagenta = "#FF00FF";
const string Color::sgreen = "#008000";
const string Color::scyan = "#00FFFF";
const string Color::sblue = "#0000FF";
const string Color::slightGray = "#D3D3D3";
const string Color::sgray = "#808080";
const string Color::sdarkGray = "#A9A9A9";
const string Color::sblack = "#101010";
const string Color::ssilver = "#C0C0C0";
const string Color::smaroon = "#800000";
const string Color::sfuchsia = "#FF00FF";
const string Color::spurple = "#800080";
const string Color::slime = "#00FF00";
const string Color::solive = "#808000";
const string Color::snavy = "#000080";
const string Color::saqua = "#00FFFF";
const string Color::steal = "#008080";

void
Color::setColorToI (const string &c)
{
  string color = c;

  if (color == "")
    {
      color = "#FFFFFF";
    }

  if (color.substr (0, 1) == "#" && color.length () > 6)
    {
      r = xstrto_uint8 (color.substr (1, 2));
      g = xstrto_uint8 (color.substr (3, 2));
      b = xstrto_uint8 (color.substr (5, 2));
      return;
    }

  if (color == "black")
    {
      setColor ((guint8) 0x10, (guint8) 0x10, (guint8) 0x10);
    }
  else if (color == "blue")
    {
      setColor ((guint8) 0x00, (guint8) 0x00, (guint8) 0xFF);
    }
  else if (color == "cyan")
    {
      setColor ((guint8) 0x00, (guint8) 0xFF, (guint8) 0xFF);
    }
  else if (color == "darkGray")
    {
      setColor ((guint8) 0xA9, (guint8) 0xA9, (guint8) 0xA9);
    }
  else if (color == "gray")
    {
      setColor ((guint8) 0x80, (guint8) 0x80, (guint8) 0x80);
    }
  else if (color == "green")
    {
      setColor ((guint8) 0x00, (guint8) 0x80, (guint8) 0x00);
    }
  else if (color == "lightGray")
    {
      setColor ((guint8) 0xD3, (guint8) 0xD3, (guint8) 0xD3);
    }
  else if (color == "magenta")
    {
      setColor ((guint8) 0xFF, (guint8) 0x00, (guint8) 0xFF);
    }
  else if (color == "orange")
    {
      setColor ((guint8) 0xFF, (guint8) 0xA5, (guint8) 0x00);
    }
  else if (color == "pink")
    {
      setColor ((guint8) 0xFF, (guint8) 0xC0, (guint8) 0xCB);
    }
  else if (color == "red")
    {
      setColor ((guint8) 0xFF, (guint8) 0x00, (guint8) 0x00);
    }
  else if (color == "yellow")
    {
      setColor ((guint8) 0xFF, (guint8) 0xFF, (guint8) 0x00);
    }
  else if (color == "silver")
    {
      setColor ((guint8) 0xC0, (guint8) 0xC0, (guint8) 0xC0);
    }
  else if (color == "maroon")
    {
      setColor ((guint8) 0x80, (guint8) 0x00, (guint8) 0x00);
    }
  else if (color == "fuchsia")
    {
      setColor ((guint8) 0xFF, (guint8) 0x00, (guint8) 0xFF);
    }
  else if (color == "purple")
    {
      setColor ((guint8) 0x80, (guint8) 0x00, (guint8) 0x80);
    }
  else if (color == "lime")
    {
      setColor ((guint8) 0x00, (guint8) 0xFF, (guint8) 0x00);
    }
  else if (color == "olive")
    {
      setColor ((guint8) 0x80, (guint8) 0x80, (guint8) 0x00);
    }
  else if (color == "navy")
    {
      setColor ((guint8) 0x00, (guint8) 0x00, (guint8) 0x80);
    }
  else if (color == "aqua")
    {
      setColor ((guint8) 0x00, (guint8) 0xFF, (guint8) 0xFF);
    }
  else if (color == "teal")
    {
      setColor ((guint8) 0x00, (guint8) 0x80, (guint8) 0x80);
    }
  else
    {
      setColor ((guint8) 0xFF, (guint8) 0xFF, (guint8) 0xFF);
    }
}

int
Color::colortoi (const string &color)
{
  if (color == "")
    {
      return Color::white;
    }

  if (color.substr (0, 1) == "#" && color.length () > 6)
    {
      int red, green, blue;

      red = xstrto_uint8 (color.substr (1, 2));
      green = xstrto_uint8 (color.substr (3, 2));
      blue = xstrto_uint8 (color.substr (5, 2));

      return 256 * 256 * red + 256 * green + blue;
    }

  if (color == "black")
    {
      return Color::black;
    }
  else if (color == "blue")
    {
      return Color::blue;
    }
  else if (color == "cyan")
    {
      return Color::cyan;
    }
  else if (color == "darkGray")
    {
      return Color::darkGray;
    }
  else if (color == "gray")
    {
      return Color::gray;
    }
  else if (color == "green")
    {
      return Color::green;
    }
  else if (color == "lightGray")
    {
      return Color::lightGray;
    }
  else if (color == "magenta")
    {
      return Color::magenta;
    }
  else if (color == "orange")
    {
      return Color::orange;
    }
  else if (color == "pink")
    {
      return Color::pink;
    }
  else if (color == "red")
    {
      return Color::red;
    }
  else if (color == "yellow")
    {
      return Color::yellow;
    }
  else if (color == "silver")
    {
      return Color::silver;
    }
  else if (color == "maroon")
    {
      return Color::maroon;
    }
  else if (color == "fuchsia")
    {
      return Color::fuchsia;
    }
  else if (color == "purple")
    {
      return Color::purple;
    }
  else if (color == "lime")
    {
      return Color::lime;
    }
  else if (color == "olive")
    {
      return Color::olive;
    }
  else if (color == "navy")
    {
      return Color::navy;
    }
  else if (color == "aqua")
    {
      return Color::aqua;
    }
  else if (color == "teal")
    {
      return Color::teal;
    }
  else
    {
      return Color::white;
    }
};

Color::Color () { this->alpha = 255; }

Color::Color (guint8 r, guint8 g, guint8 b, guint8 alpha)
{
  this->r = r;
  this->g = g;
  this->b = b;
  this->alpha = alpha;
}

Color::Color (const string &color, guint8 alpha)
{
  this->alpha = alpha;
  setColorToI (color);
}

void
Color::setColor (const string &color)
{
  setColorToI (color);
}

void
Color::setColor (guint8 red, guint8 green, guint8 blue, arg_unused (guint8 alpha))
{
  this->r = red;
  this->g = green;
  this->b = blue;
  this->alpha = 0xff;
}

guint8
Color::getR ()
{
  return r;
}

guint8
Color::getG ()
{
  return g;
}

guint8
Color::getB ()
{
  return b;
}

guint8
Color::getAlpha ()
{
  return alpha;
}

guint32
Color::getRGBA ()
{
  return ((r << 24) & 0xFF000000)
    + ((g << 16) & 0x00FF0000)
    + ((b << 8) & 0x0000FF00)
    + (alpha & 0x000000FF);
}

guint32
Color::getARGB ()
{
  return ((alpha << 24) & 0xFF000000)
    + ((r << 16) & 0x00FF0000)
    + ((g << 8) & 0x0000FF00)
    + (b & 0x000000FF);
}

SDL_Color
Color::getColor ()
{
  SDL_Color c;
  c.r = this->r;
  c.g = this->g;
  c.b = this->b;
  c.a = this->alpha;
  return c;
}

GINGA_UTIL_END
