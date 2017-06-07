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

#ifndef FORMATTERREGION_H_
#define FORMATTERREGION_H_

#include "ncl/LayoutRegion.h"
using namespace ::ginga::ncl;

#include "ncl/Descriptor.h"
#include "ncl/DescriptorUtil.h"
using namespace ::ginga::ncl;

#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

GINGA_FORMATTER_BEGIN

class NclCascadingDescriptor;

class NclFormatterRegion
{
private:
  NclCascadingDescriptor *descriptor;

  string objectId;
  LayoutRegion *region;
  SDLWindow *win;

  bool imVisible;

  short focusState;
  SDL_Color focusBorderColor;
  int focusBorderWidth;
  string focusComponentSrc;
  SDL_Color selBorderColor;
  string selComponentSrc;
  int selBorderWidth;

  int zIndex;

  SDL_Color bgColor;
  double transparency;

  string focusIndex;
  string moveUp;
  string moveDown;
  string moveLeft;
  string moveRight;

public:
  static const short UNSELECTED = 0;
  static const short FOCUSED = 1;
  static const short SELECTED = 2;
  NclFormatterRegion (const string &, NclCascadingDescriptor *);

  virtual ~NclFormatterRegion ();

  void setZIndex (int zIndex);
  int getZIndex ();

  void setFocusIndex (const string &focusIndex);
  string getFocusIndex ();

  void setMoveUp (const string &moveUp);
  string getMoveUp ();

  void setMoveDown (const string &moveDown);
  string getMoveDown ();

  void setMoveLeft (const string &moveLeft);
  string getMoveLeft ();

  void setMoveRight (const string &moveRight);
  string getMoveRight ();

  void setFocusBorderColor (SDL_Color);
  SDL_Color getFocusBorderColor ();

  void setFocusBorderWidth (int focusBorderWidth);
  int getFocusBorderWidth ();

  void setFocusComponentSrc (const string &focusComponentSrc);
  string getFocusComponentSrc ();

  void setSelBorderColor (SDL_Color selBorderColor);
  SDL_Color getSelBorderColor ();

  void setSelBorderWidth (int selBorderWidth);
  int getSelBorderWidth ();

  void setSelComponentSrc (const string &selComponentSrc);
  string getSelComponentSrc ();

  void setFocusInfo (SDL_Color focusBorderColor, int focusBorderWidth,
                     const string &focusComponentSrc, SDL_Color selBorderColor,
                     int selBorderWidth, const string &selComponentSrc);

  SDLWindow* getOutputId ();

public:
  void updateRegionBounds ();

private:
  void sizeRegion ();

public:
  LayoutRegion *getLayoutRegion ();
  LayoutRegion *getOriginalRegion ();
  SDLWindow* prepareOutputDisplay ();

  void showContent ();
  void hideContent ();

  void setRegionVisibility (bool visible);

private:
  void disposeOutputDisplay ();

public:
  void setGhostRegion (bool ghost);
  bool isVisible ();
  short getFocusState ();
  bool setSelection (bool selOn);
  void setFocus (bool focusOn);
  void unselect ();

  SDL_Color getBackgroundColor ();

  double getTransparency ();
  void setTransparency (const string &strTrans);
  void setTransparency (double transparency);
  void setBackgroundColor (const string &color);
  void setBackgroundColor (SDL_Color color);
};

typedef struct
{
  NclFormatterRegion *fr;
  Transition *t;
} TransInfo;

GINGA_FORMATTER_END

#endif /*FORMATTERREGION_H_*/
