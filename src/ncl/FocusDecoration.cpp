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

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_NCL_BEGIN

FocusDecoration::FocusDecoration ()
{
  focusBorderTransparency = NaN ();
  focusSrc = "";
  focusBorderWidth = -3;
  focusBorderColor = NULL;

  selBorderColor = NULL;
  focusSelSrc = "";
}

FocusDecoration::~FocusDecoration ()
{
  if (focusBorderColor != NULL)
    {
      delete focusBorderColor;
      focusBorderColor = NULL;
    }

  if (selBorderColor != NULL)
    {
      delete selBorderColor;
      selBorderColor = NULL;
    }
}

Color *
FocusDecoration::getFocusBorderColor ()
{
  return focusBorderColor;
}

double
FocusDecoration::getFocusBorderTransparency ()
{
  return focusBorderTransparency;
}

string
FocusDecoration::getFocusSelSrc ()
{
  return focusSelSrc;
}

string
FocusDecoration::getFocusSrc ()
{
  return focusSrc;
}

int
FocusDecoration::getFocusBorderWidth ()
{
  return focusBorderWidth;
}

Color *
FocusDecoration::getSelBorderColor ()
{
  return selBorderColor;
}

void
FocusDecoration::setFocusBorderColor (Color *color)
{
  focusBorderColor = color;
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

  focusBorderTransparency = alfa;
}

void
FocusDecoration::setFocusBorderWidth (int width)
{
  focusBorderWidth = width;
}

void
FocusDecoration::setFocusSelSrc (string src)
{
  focusSelSrc = src;
}

void
FocusDecoration::setFocusSrc (string src)
{
  focusSrc = src;
}

void
FocusDecoration::setSelBorderColor (Color *color)
{
  selBorderColor = color;
}

GINGA_NCL_END
