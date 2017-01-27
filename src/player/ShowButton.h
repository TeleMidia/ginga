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

#ifndef SHOWBUTTON_H_
#define SHOWBUTTON_H_

#include "ginga.h"

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "system/Thread.h"
using namespace ::ginga::system;

#include "mb/SDLWindow.h"
#include "mb/SDLDisplay.h"
using namespace ::ginga::mb;

#include "ImagePlayer.h"

GINGA_PLAYER_BEGIN

class ShowButton : public Thread
{
private:
  short status;
  static const short NONE = 0;
  static const short PLAY = 1;
  static const short PAUSE = 2;
  static const short STOP = 3;
  short previousStatus;
  SDLWindow* win;
  bool running;

public:
  ShowButton ();
  virtual ~ShowButton ();

  void initializeWindow ();
  void stop ();
  void pause ();
  void resume ();

private:
  void release ();
  void render (string mrl);
  void run ();
};

GINGA_PLAYER_END

#endif /*ShowButton_H_*/
