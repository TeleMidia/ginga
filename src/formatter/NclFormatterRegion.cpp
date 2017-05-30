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
#include "ginga-color-table.h"
#include "NclFormatterRegion.h"

#include "NclCascadingDescriptor.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_FORMATTER_BEGIN

NclFormatterRegion::NclFormatterRegion (const string &objectId,
                                        NclCascadingDescriptor *descriptor)
{
  this->objectId = objectId;
  this->descriptor = descriptor;

  this->region = NULL;
  if (this->descriptor != NULL)
    this->region = this->descriptor->getRegion ();

  this->win = 0;
  this->imVisible = false;
  this->focusState = NclFormatterRegion::UNSELECTED;
  this->focusBorderColor = NULL;
  this->focusBorderWidth = 0;
  this->focusComponentSrc = "";
  this->selBorderColor = NULL;
  this->selBorderWidth = 0;
  this->selComponentSrc = "";
  this->bgColor = NULL;
  this->focusIndex = "";
  this->moveUp = "";
  this->moveDown = "";
  this->moveLeft = "";
  this->moveRight = "";
  this->zIndex = -1;

  // TODO: look for descriptor parameters overriding region attributes
  string value;
  value = descriptor->getParameterValue ("transparency");
  this->setTransparency (value);

  value = descriptor->getParameterValue ("background");

  if (xstrchomp (value) != "")
    {
      if (value.find (",") == std::string::npos)
        {
          this->setBackgroundColor (value);
        }
      else
        {
          SDL_Color *bg = NULL;
          vector<string> params;

          params = xstrsplit (xstrchomp (value), ',');
          if (params.size () == 3)
            {
              bg = new SDL_Color();
              bg->r =  xstrtouint8 (params[0], 10);
              bg->g =  xstrtouint8 (params[1], 10);
              bg->b =  xstrtouint8 (params[2], 10);
              bg->a = 255;
              setBackgroundColor (bg);
            }
          else if (params.size () == 4)
            {
              bg = new SDL_Color();
              bg->r =  xstrtouint8 (params[0], 10);
              bg->g =  xstrtouint8 (params[1], 10);
              bg->b =  xstrtouint8 (params[2], 10);
              bg->a =  xstrtouint8 (params[3], 10);
              setBackgroundColor (bg);
            }
        }
    }
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

  disposeOutputDisplay ();

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

  if (bgColor != NULL)
    {
      delete bgColor;
      bgColor = NULL;
    }
}
void
NclFormatterRegion::setZIndex (int zIndex)
{
  string layoutId;

  this->zIndex = zIndex;

  if (region != NULL)
    region->setZIndex (zIndex);

  if (region != NULL)
    {
      layoutId = region->getId ();
      if (win != NULL)
        win->setZ (zIndex);
    }
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
NclFormatterRegion::setFocusBorderColor (SDL_Color *focusBorderColor)
{
  if (this->focusBorderColor == focusBorderColor)
    {
      return;
    }

  if (this->focusBorderColor != NULL)
    {
      delete this->focusBorderColor;
      this->focusBorderColor = NULL;
    }

  if (focusBorderColor != NULL)
    {
      this->focusBorderColor = focusBorderColor;
    }
}

SDL_Color *
NclFormatterRegion::getFocusBorderColor ()
{
  SDL_Color *bColor = NULL;

  if (focusBorderColor != NULL)
    {
      bColor = focusBorderColor;
    }
  else if (descriptor != NULL)
    {
      bColor
          = descriptor->getFocusBorderColor ();
    }

  return bColor;
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
NclFormatterRegion::setSelBorderColor (SDL_Color *selBorderColor)
{
  if (this->selBorderColor == selBorderColor)
    {
      return;
    }

  if (this->selBorderColor != NULL)
    {
      this->selBorderColor = NULL;
    }

  if (selBorderColor != NULL)
    {
      this->selBorderColor = selBorderColor;
    }
}

SDL_Color *
NclFormatterRegion::getSelBorderColor ()
{
  SDL_Color *sColor = NULL;

  if (selBorderColor != NULL)
    {
      sColor = selBorderColor;
    }
  else if (descriptor != NULL)
    {
      sColor = descriptor->getSelBorderColor ();
    }

  return sColor;
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
NclFormatterRegion::setFocusInfo (SDL_Color *focusBorderColor,
                                  int focusBorderWidth,
                                  const string &focusComponentSrc,
                                  SDL_Color *selBorderColor, int selBorderWidth,
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
  int left = 0;
  int top = 0;
  int width = 0;
  int height = 0;

  if (region != NULL)
    {
      left = region->getLeftInPixels ();
      top = region->getTopInPixels ();
      width = region->getWidthInPixels ();
      height = region->getHeightInPixels ();
    }

  if (left < 0)
    left = 0;

  if (top < 0)
    top = 0;

  if (width <= 0)
    width = 1;

  if (height <= 0)
    height = 1;

  if (this->win != 0)
    this->win->setBounds (left, top, width, height);
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

  this->win = Ginga_Display->createWindow (r.x, r.y, r.w, r.h, z, zorder);
  return this->win;
}

void
NclFormatterRegion::showContent ()
{
  string value;

  value = descriptor->getParameterValue ("visible");
  if (value != "false")
    {
      imVisible = true;
      setRegionVisibility (true);
    }
}

void
NclFormatterRegion::hideContent ()
{
  setRegionVisibility (false);
  disposeOutputDisplay ();
}

void
NclFormatterRegion::setRegionVisibility (bool visible)
{
  if (this->win != NULL)
    {
      if (!visible)
        {
          clog << "NclFormatterRegion::setRegionVisibility (" << this;
          clog << ") object '" << objectId << "' display '";
          clog << this->win;
          clog << "' HIDE" << endl;

          this->win->hide ();
        }
      else
        {
          clog << "NclFormatterRegion::setRegionVisibility (" << this;
          clog << ") object '" << objectId << "' display '";
          clog << this->win;
          clog << "' SHOW" << endl;

          this->win->show ();
        }
    }
  imVisible = visible;
}

void
NclFormatterRegion::disposeOutputDisplay ()
{
  if (this->win != NULL)
    {
      Ginga_Display->destroyWindow (this->win);
      this->win = NULL;
    }
}

void
NclFormatterRegion::setGhostRegion (bool ghost)
{
  if (this->win != NULL)
    {
      this->win->setGhostWindow (ghost);
    }
}

bool
NclFormatterRegion::isVisible ()
{
  return imVisible;
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
          if (selComponentSrc == "")
            {
            }
          if (selBorderColor != NULL)
            {
              this->win->setBorder (*selBorderColor,
                  selBorderWidth);
            }
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
          if (focusComponentSrc == "")
            {
            }
          if (focusBorderColor != NULL)
            {
              this->win->setBorder (*focusBorderColor,
                                    focusBorderWidth);
            }
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

SDL_Color *
NclFormatterRegion::getBackgroundColor ()
{
  return bgColor;
}

void
NclFormatterRegion::setTransparency (const string &strTrans)
{
  double trans;
  if (strTrans == "")
    {
      trans = 0.0;
    }
  else
    {
      trans = xstrtod (strTrans);
    }

  setTransparency (trans);
}

double
NclFormatterRegion::getTransparency ()
{
  return this->transparency;
}

void
NclFormatterRegion::setTransparency (double transparency)
{
  if (transparency < 0.0)
    {
      this->transparency = 0.0;
    }
  else if (transparency > 1.0)
    {
      this->transparency = 1.0;
    }
  else
    {
      this->transparency = transparency;
    }
}

void
NclFormatterRegion::setBackgroundColor (const string &c)
{
    SDL_Color *bg = new SDL_Color();
    ginga_color_input_to_sdl_color(c,bg);
    bg->a = (guint8)(transparency * 255);
    setBackgroundColor (bg);
}

void
NclFormatterRegion::setBackgroundColor (SDL_Color *color)
{
  if (color != bgColor && bgColor != NULL)
    {
      delete bgColor;
      bgColor = NULL;
    }

  this->bgColor = color;
}

GINGA_FORMATTER_END
