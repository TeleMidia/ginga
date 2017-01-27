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
#include "ShowButton.h"

#include "mb/DisplayManager.h"
using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN

ShowButton::ShowButton () : Thread ()
{
  status = NONE;
  previousStatus = NONE;
  win = 0;
  running = false;
}

ShowButton::~ShowButton ()
{
  isDeleting = true;

  while (running)
    {
      g_usleep (50000);
    }
}

void
ShowButton::initializeWindow ()
{
  int x = 0, y, w, h;

  x = (int)(Ginga_Display->getWidthResolution () - 70);
  y = 10;
  w = 60;
  h = 60;

  win = Ginga_Display->createWindow (x, y, w, h, 4.0);

  int caps = win->getCap ("ALPHACHANNEL");
  win->setCaps (caps);
  win->draw ();
}

void
ShowButton::stop ()
{
  // lock();
  previousStatus = status;
  status = STOP;
  // Thread::start();
  // unlock();
  run ();
}

void
ShowButton::pause ()
{
  // lock();
  previousStatus = status;
  if (status != PAUSE)
    {
      status = PAUSE;
    }
  else
    {
      status = PLAY;
    }

  run ();
  // Thread::start();
  // unlock();
}

void
ShowButton::resume ()
{
  pause ();
}

void
ShowButton::release ()
{
  lock ();
  if (win != 0)
    {
      win->hide ();
      delete win;
      win = 0;
    }
  unlock ();
}

void
ShowButton::render (string mrl)
{
  SDLSurface* surface;

  surface = Ginga_Display->createRenderedSurfaceFromImageFile (mrl.c_str ());

  lock ();
  if (win == 0)
    {
      initializeWindow ();
    }

  if (surface->setParentWindow (win))
    {
      win->renderFrom (surface);
    }
  win->show ();
  win->raiseToTop ();
  unlock ();
}

void
ShowButton::run ()
{
  running = true;
  if (isDeleting)
    goto done;

  switch (status)
    {
    case PAUSE:
      render (string (GINGA_BUTTON_DATADIR) + "pauseButton.png");
      break;
    case STOP:
      if (previousStatus == PAUSE)
        {
          release ();
        }
      render (string (GINGA_BUTTON_DATADIR) + "stopButton.png");
      g_usleep (1000000);
      release ();
      break;
    case PLAY:
      if (previousStatus == PAUSE)
        {
          release ();
        }
      render (string (GINGA_BUTTON_DATADIR) + "playButton.png");
      g_usleep (1000000);
      release ();
      break;
    default:
      break;
    }
done:
  running = false;
}

GINGA_PLAYER_END
