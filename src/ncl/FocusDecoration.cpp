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
#include "FocusDecoration.h"

GINGA_NCL_BEGIN

FocusDecoration::FocusDecoration ()
{
  _focusBorderTransparency = (double)NAN;
  _focusSrc = "";
  _focusBorderWidth = -3;
  _focusBorderColor = {0, 0, 255, 255};
  _selBorderColor = {0, 255, 0, 255};
  _focusSelSrc = "";
}

FocusDecoration::~FocusDecoration ()
{
}

SDL_Color
FocusDecoration::getFocusBorderColor ()
{
  return _focusBorderColor;
}

double
FocusDecoration::getFocusBorderTransparency ()
{
  return _focusBorderTransparency;
}

string
FocusDecoration::getFocusSelSrc ()
{
  return _focusSelSrc;
}

string
FocusDecoration::getFocusSrc ()
{
  return _focusSrc;
}

int
FocusDecoration::getFocusBorderWidth ()
{
  return _focusBorderWidth;
}

SDL_Color
FocusDecoration::getSelBorderColor ()
{
  return _selBorderColor;
}

void
FocusDecoration::setFocusBorderColor (SDL_Color color)
{
  _focusBorderColor = color;
}

void
FocusDecoration::setFocusBorderTransparency (double alfa)
{
  if (alfa < 0)
    {
      alfa = 0;
    }
  else if (alfa > 1)
    {
      alfa = 1;
    }

  _focusBorderTransparency = alfa;
}

void
FocusDecoration::setFocusBorderWidth (int width)
{
  _focusBorderWidth = width;
}

void
FocusDecoration::setFocusSelSrc (const string &src)
{
  _focusSelSrc = src;
}

void
FocusDecoration::setFocusSrc (const string &src)
{
  _focusSrc = src;
}

void
FocusDecoration::setSelBorderColor (SDL_Color color)
{
  _selBorderColor = color;
}

GINGA_NCL_END
