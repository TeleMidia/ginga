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
#include "NclFormatterRegion.h"

#include "NclCascadingDescriptor.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_FORMATTER_BEGIN

NclFormatterRegion::NclFormatterRegion (const string &objectId,
                                        NclCascadingDescriptor *descriptor)
{
  SDL_Color color;
  this->objectId = objectId;
  this->descriptor = descriptor;

  this->region = NULL;
  if (this->descriptor != NULL)
    this->region = this->descriptor->getRegion ();

  this->win = 0;
  this->focusState = NclFormatterRegion::UNSELECTED;
  this->focusBorderColor = {0, 0, 255, 255};
  this->focusBorderWidth = 0;
  this->focusComponentSrc = "";
  this->selBorderColor = {0, 255, 0, 255};
  this->selBorderWidth = 0;
  this->selComponentSrc = "";
  this->bgColor = {0, 0, 0, 0};
  this->focusIndex = "";
  this->moveUp = "";
  this->moveDown = "";
  this->moveLeft = "";
  this->moveRight = "";
  this->zIndex = -1;

  // TODO: look for descriptor parameters overriding region attributes
  string value;

  value = descriptor->getParameterValue ("background");
  if (value == "")
    color = {0, 0, 0, 0};
  else
    color = ginga_parse_color (value);
  this->setBackgroundColor (color);
}

NclFormatterRegion::~NclFormatterRegion ()
{
  descriptor = NULL;

  region = NULL;
  if (region != NULL)
    {
      // delete region;
      region = NULL;
    }
}

void
NclFormatterRegion::setZIndex (int zIndex)
{
  this->zIndex = zIndex;
}

int
NclFormatterRegion::getZIndex ()
{
  return zIndex;
}

void
NclFormatterRegion::setFocusIndex (const string &focusIndex)
{
  this->focusIndex = focusIndex;
}

string
NclFormatterRegion::getFocusIndex ()
{
  if (focusIndex != "")
    {
      return focusIndex;
    }
  else if (descriptor != NULL)
    {
      return descriptor->getFocusIndex ();
    }

  return "";
}

void
NclFormatterRegion::setMoveUp (const string &moveUp)
{
  this->moveUp = moveUp;
}

string
NclFormatterRegion::getMoveUp ()
{
  if (moveUp != "")
    {
      return moveUp;
    }
  else if (descriptor != NULL)
    {
      return descriptor->getMoveUp ();
    }

  return "";
}

void
NclFormatterRegion::setMoveDown (const string &moveDown)
{
  this->moveDown = moveDown;
}

string
NclFormatterRegion::getMoveDown ()
{
  if (moveDown != "")
    {
      return moveDown;
    }
  else if (descriptor != NULL)
    {
      return descriptor->getMoveDown ();
    }

  return "";
}

void
NclFormatterRegion::setMoveLeft (const string &moveLeft)
{
  this->moveLeft = moveLeft;
}

string
NclFormatterRegion::getMoveLeft ()
{
  if (moveLeft != "")
    {
      return moveLeft;
    }
  else if (descriptor != NULL)
    {
      return descriptor->getMoveLeft ();
    }

  return "";
}

void
NclFormatterRegion::setMoveRight (const string &moveRight)
{
  this->moveRight = moveRight;
}

string
NclFormatterRegion::getMoveRight ()
{
  if (moveRight != "")
    {
      return moveRight;
    }
  else if (descriptor != NULL)
    {
      return descriptor->getMoveRight ();
    }

  return "";
}

void
NclFormatterRegion::setFocusBorderColor (SDL_Color focusBorderColor)
{
  this->focusBorderColor = focusBorderColor;
}

SDL_Color
NclFormatterRegion::getFocusBorderColor ()
{
  if (this->descriptor != NULL)
    return this->descriptor->getFocusBorderColor ();
  return this->focusBorderColor;
}

void
NclFormatterRegion::setFocusBorderWidth (int focusBorderWidth)
{
  this->focusBorderWidth = focusBorderWidth;
}

int
NclFormatterRegion::getFocusBorderWidth ()
{
  if (focusBorderWidth != 0)
    {
      return focusBorderWidth;
    }
  else if (descriptor != NULL)
    {
      return descriptor->getFocusBorderWidth ();
    }

  return 0;
}

void
NclFormatterRegion::setFocusComponentSrc (const string &focusComponentSrc)
{
  this->focusComponentSrc = focusComponentSrc;
}

string
NclFormatterRegion::getFocusComponentSrc ()
{
  if (focusComponentSrc != "")
    {
      return focusComponentSrc;
    }
  else if (descriptor != NULL)
    {
      return descriptor->getFocusSrc ();
    }

  return "";
}

void
NclFormatterRegion::setSelBorderColor (SDL_Color selBorderColor)
{
  this->selBorderColor = selBorderColor;
}

SDL_Color
NclFormatterRegion::getSelBorderColor ()
{
  if (this->descriptor != NULL)
    return this->descriptor->getSelBorderColor ();
  return this->selBorderColor;
}

void
NclFormatterRegion::setSelBorderWidth (int selBorderWidth)
{
  this->selBorderWidth = selBorderWidth;
}

int
NclFormatterRegion::getSelBorderWidth ()
{
  if (selBorderWidth != 0)
    {
      return selBorderWidth;
    }
  else if (descriptor != NULL)
    {
      return descriptor->getSelBorderWidth ();
    }

  return 0;
}

void
NclFormatterRegion::setSelComponentSrc (const string &selComponentSrc)
{
  this->selComponentSrc = selComponentSrc;
}

string
NclFormatterRegion::getSelComponentSrc ()
{
  if (selComponentSrc != "")
    {
      return selComponentSrc;
    }
  else if (descriptor != NULL)
    {
      return descriptor->getSelectionSrc ();
    }

  return "";
}

void
NclFormatterRegion::setFocusInfo (SDL_Color focusBorderColor,
                                  int focusBorderWidth,
                                  const string &focusComponentSrc,
                                  SDL_Color selBorderColor,
                                  int selBorderWidth,
                                  const string &selComponentSrc)
{
  setFocusBorderColor (focusBorderColor);
  setFocusBorderWidth (focusBorderWidth);
  setFocusComponentSrc (focusComponentSrc);
  setSelBorderColor (selBorderColor);
  setSelBorderWidth (selBorderWidth);
  setSelComponentSrc (selComponentSrc);
}

SDLWindow *
NclFormatterRegion::getOutputId ()
{
  return this->win;
}


void
NclFormatterRegion::updateRegionBounds ()
{
  sizeRegion ();
  if (focusState == NclFormatterRegion::UNSELECTED)
    {
      unselect ();
    }
  else
    {
      if (focusState == NclFormatterRegion::SELECTED)
        {
          setSelection (true);
        }
      else if (focusState == NclFormatterRegion::FOCUSED)
        {
          setFocus (true);
        }
    }
}

void
NclFormatterRegion::sizeRegion ()
{
  SDL_Rect rect = {0, 0, 0, 0};

  if (region != NULL)
    rect = region->getRect ();
}

LayoutRegion *
NclFormatterRegion::getLayoutRegion ()
{
  return region;
}

LayoutRegion *
NclFormatterRegion::getOriginalRegion ()
{
  return region;
}

SDLWindow *
NclFormatterRegion::prepareOutputDisplay ()
{
  SDL_Rect r;
  int z, zorder;

  if (this->win != NULL)
    return this->win;           // nothing to do

  g_assert_nonnull (this->region);
  r = region->getRect ();
  region->getZ (&z, &zorder);

  this->win = new SDLWindow ();
  return this->win;
}

void
NclFormatterRegion::setGhostRegion (bool ghost)
{
  if (this->win != NULL)
    {
      this->win->setGhostWindow (ghost);
    }
}

short
NclFormatterRegion::getFocusState ()
{
  return focusState;
}

bool
NclFormatterRegion::setSelection (bool selOn)
{

  if (selOn && focusState == NclFormatterRegion::SELECTED)
    {
      return false;
    }

  if (selOn)
    {
      focusState = NclFormatterRegion::SELECTED;
      if (this->win != 0)
        {
          this->win->setBorder (selBorderColor, selBorderWidth);
        }
    }
  else
    {
      unselect ();
    }

  return selOn;
}

void
NclFormatterRegion::setFocus (bool focusOn)
{
  if (focusOn)
    {
      focusState = NclFormatterRegion::FOCUSED;

      if (focusComponentSrc != "")
        {
        }
      if (this->win != NULL)
        {
          this->win->setBorder (focusBorderColor, focusBorderWidth);
        }
    }
  else
    {
      unselect ();
    }
}

void
NclFormatterRegion::unselect ()
{
  focusState = NclFormatterRegion::UNSELECTED;
  if (this->win != NULL)
    {
      SDL_Color c = {0, 0, 0, 0};
      this->win->setBorder (c, 0);
    }
}

SDL_Color
NclFormatterRegion::getBackgroundColor ()
{
  return this->bgColor;
}

void
NclFormatterRegion::setBackgroundColor (const string &str)
{
  this->setBackgroundColor (ginga_parse_color (str));
}

void
NclFormatterRegion::setBackgroundColor (SDL_Color color)
{
  this->bgColor = color;
}

GINGA_FORMATTER_END
