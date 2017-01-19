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

#include "config.h"
#include "SDLInputEvent.h"

#include "CodeMap.h"
#include "DisplayManager.h"

GINGA_MB_BEGIN

const string SDLInputEvent::ET_WAKEUP = "GINGA_WAKEUP";
const string SDLInputEvent::ET_INPUTEVENT = "GINGA_INPUTEVENT";
const string SDLInputEvent::ET_USEREVENT = "GINGA_USEREVENT";

SDLInputEvent::SDLInputEvent (SDL_Event event)
{
  this->event = event;

  x = 0;
  y = 0;
  capsOn = false;
  shiftOn = false;
}

SDLInputEvent::SDLInputEvent (const int keyCode)
{
  event.type = SDL_KEYDOWN;
  event.key.type = SDL_KEYDOWN;
  event.key.state = SDL_PRESSED;
  event.key.repeat = 0;
  event.key.keysym.sym = keyCode;

  x = 0;
  y = 0;
}

SDLInputEvent::SDLInputEvent (int type, void *data)
{
  event.type = SDL_USEREVENT;
  event.user.code = type;
  event.user.data1 = (void *)(ET_USEREVENT.c_str ());
  event.user.data2 = data;

  x = 0;
  y = 0;
}

SDLInputEvent::~SDLInputEvent () {}

void
SDLInputEvent::setModifiers (bool capsOn, bool shiftOn)
{
  this->capsOn = capsOn;
  this->shiftOn = shiftOn;
}

void
SDLInputEvent::clearContent ()
{
}

void *
SDLInputEvent::getContent ()
{
  return &event;
}

void
SDLInputEvent::setKeyCode (GingaScreenID screenId, const int keyCode)
{
  int sdlCode;

  sdlCode = Ginga_Display->fromGingaToMB (screenId,
                                                                 keyCode);

  if (event.type == SDL_USEREVENT)
    {
      event.user.code = sdlCode;
    }
  else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
    {
      event.key.keysym.sym = sdlCode;
    }
}

const int
SDLInputEvent::getKeyCode (GingaScreenID screenId)
{
  int gingaValue;
  int sdlValue;

  if (event.type == SDL_FINGERUP || event.type == SDL_MOUSEBUTTONUP)
    {

      return CodeMap::KEY_TAP;
    }

  if (event.type == SDL_USEREVENT)
    {
      clog << "SDLInputEvent::getKeyCode user event" << endl;
      sdlValue = event.user.code;
    }
  else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
    {
      sdlValue = event.key.keysym.sym;
      clog << "SDLInputEvent::getKeyCode key pressed event sdlValue = '";
      clog << sdlValue << "'" << endl;
    }
  else
    {
      clog << "SDLInputEvent::getKeyCode unknown event type.";
      clog << "Returning KEY_NULL" << endl;
      return CodeMap::KEY_NULL;
      ;
    }

  gingaValue = Ginga_Display->fromMBToGinga (
      screenId, sdlValue);

  if (gingaValue >= CodeMap::KEY_SMALL_A
      && gingaValue <= CodeMap::KEY_SMALL_Z
      && ((capsOn && !shiftOn) || (!capsOn && shiftOn)))
    {

      gingaValue = Ginga_Display->fromMBToGinga (
          screenId, sdlValue + 5000);
    }

  // Mapping between keyboard and remote control
  if (gingaValue == CodeMap::KEY_F1)
    {
      gingaValue = CodeMap::KEY_RED;
    }
  else if (gingaValue == CodeMap::KEY_F2)
    {
      gingaValue = CodeMap::KEY_GREEN;
    }
  else if (gingaValue == CodeMap::KEY_F3)
    {
      gingaValue = CodeMap::KEY_YELLOW;
    }
  else if (gingaValue == CodeMap::KEY_F4)
    {
      gingaValue = CodeMap::KEY_BLUE;
    }
  else if (gingaValue == CodeMap::KEY_F5)
    {
      gingaValue = CodeMap::KEY_MENU;
    }
  else if (gingaValue == CodeMap::KEY_F6)
    {
      gingaValue = CodeMap::KEY_INFO;
    }
  else if (gingaValue == CodeMap::KEY_F7)
    {
      gingaValue = CodeMap::KEY_EPG;
    }
  else if (gingaValue == CodeMap::KEY_PLUS_SIGN)
    {
      gingaValue = CodeMap::KEY_VOLUME_UP;
    }
  else if (gingaValue == CodeMap::KEY_MINUS_SIGN)
    {
      gingaValue = CodeMap::KEY_VOLUME_DOWN;
    }
  else if (gingaValue == CodeMap::KEY_PAGE_UP)
    {
      gingaValue = CodeMap::KEY_CHANNEL_UP;
    }
  else if (gingaValue == CodeMap::KEY_PAGE_DOWN)
    {
      gingaValue = CodeMap::KEY_CHANNEL_DOWN;
    }
  else if (gingaValue == CodeMap::KEY_BACKSPACE)
    {
      gingaValue = CodeMap::KEY_BACK;
    }
  else if (gingaValue == CodeMap::KEY_ESCAPE)
    {
      gingaValue = CodeMap::KEY_EXIT;
    }

  return gingaValue;
}

void *
SDLInputEvent::getApplicationData ()
{
  if (isApplicationType ())
    {
      return event.user.data2;
    }

  return NULL;
}

unsigned int
SDLInputEvent::getType ()
{
  unsigned int result = CodeMap::KEY_NULL;

  if (event.type == SDL_USEREVENT)
    {
      result = event.user.code;
    }
  else
    {
      result = event.type;
    }

  return result;
}

bool
SDLInputEvent::isButtonPressType ()
{
  if (event.type == SDL_FINGERUP || event.type == SDL_MOUSEBUTTONUP
      || event.type == SDL_FINGERDOWN || event.type == SDL_MOUSEBUTTONDOWN)
    {

      return true;
    }

  return false;
}

bool
SDLInputEvent::isMotionType ()
{
  if (event.type == SDL_MOUSEMOTION || event.type == SDL_FINGERMOTION)
    {

      return true;
    }
  return false;
}

bool
SDLInputEvent::isPressedType ()
{
  if (event.type == SDL_KEYDOWN)
    {
      return true;
    }
  return false;
}

bool
SDLInputEvent::isKeyType ()
{
  if (event.type == SDL_KEYUP || event.type == SDL_KEYDOWN)
    {
      return true;
    }
  return false;
}

bool
SDLInputEvent::isApplicationType ()
{
  if (event.type == SDL_USEREVENT && event.user.data1 != NULL
      && event.user.data2 != NULL)
    {

      if (strcmp ((char *)(event.user.data1), ET_USEREVENT.c_str ()) == 0)
        {

          return true;
        }
    }

  return false;
}

void
SDLInputEvent::setAxisValue (int x, int y, int z)
{
  this->x = x;
  this->y = y;
}

void
SDLInputEvent::getAxisValue (int *x, int *y, int *z)
{
  *x = 0;
  *y = 0;

  switch (event.type)
    {
    case SDL_MOUSEMOTION:
      *x = event.motion.x;
      *y = event.motion.y;
      break;

    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
      *x = event.button.x;
      *y = event.button.y;
      break;

    case SDL_FINGERUP:
    case SDL_FINGERDOWN:
    case SDL_FINGERMOTION:
      *x = event.tfinger.x;
      *y = event.tfinger.y;
      break;
    }
}

GINGA_MB_END
