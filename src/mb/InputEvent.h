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

#ifndef INPUTEVENT_H_
#define INPUTEVENT_H_

#include "Key.h"

GINGA_MB_BEGIN

class InputEvent
{
public:
  static const string ET_WAKEUP;
  static const string ET_INPUTEVENT;
  static const string ET_USEREVENT;

private:
  SDL_Event event;
  int x;
  int y;

  bool capsOn;
  bool shiftOn;

public:
  InputEvent (SDL_Event event);
  InputEvent (const int keyCode);
  InputEvent (int type, void *data);

  virtual ~InputEvent ();

  void setModifiers (bool capsOn, bool shiftOn);

  void clearContent ();
  void setKeyCode (Key::KeyCode keyCode);
  Key::KeyCode getKeyCode ();

  unsigned int getType ();
  void *getApplicationData ();

  bool isButtonPressType ();
  bool isMotionType ();
  bool isPressedType ();
  bool isKeyType ();
  bool isApplicationType ();

  void setAxisValue (int x, int y, int z);
  void getAxisValue (int *x, int *y, int *z);
  void *getContent ();
};

GINGA_MB_END

#endif /*SDLINPUTEVENT_H_*/
