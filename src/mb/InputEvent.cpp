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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>. 

#include "ginga.h"
#include "InputEvent.h"
#include "Display.h"
#include "Key.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_MB_BEGIN
const string InputEvent::ET_WAKEUP = "GINGA_WAKEUP";
const string InputEvent::ET_INPUTEVENT = "GINGA_INPUTEVENT";
const string InputEvent::ET_USEREVENT = "GINGA_USEREVENT";

InputEvent::InputEvent (SDL_Event event)
{
  this->event = event;

  x = y = 0;
  capsOn = false;
  shiftOn = false;
}

InputEvent::InputEvent (const int keyCode)
{
  event.type = SDL_KEYDOWN;
  event.key.type = SDL_KEYDOWN;
  event.key.state = SDL_PRESSED;
  event.key.repeat = 0;
  event.key.keysym.sym = keyCode;

  x = y = 0;
}

InputEvent::InputEvent (int type, void *data)
{
  event.type = SDL_USEREVENT;
  event.user.code = type;
  event.user.data1 = deconst (void *, ET_USEREVENT.c_str ());
  event.user.data2 = data;

  x = y = 0;
}

InputEvent::~InputEvent () {}

void
InputEvent::setModifiers (bool capsOn, bool shiftOn)
{
  this->capsOn = capsOn;
  this->shiftOn = shiftOn;
}

void
InputEvent::clearContent ()
{
}

void *
InputEvent::getContent ()
{
  return &event;
}

void
InputEvent::setKeyCode (Key::KeyCode keyCode)
{
  int sdlCode;

  sdlCode = Ginga_Display->fromGingaToMB (keyCode);

  if (event.type == SDL_USEREVENT)
    {
      event.user.code = sdlCode;
    }
  else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
    {
      event.key.keysym.sym = sdlCode;
    }
}

Key::KeyCode
InputEvent::getKeyCode ()
{
  Key::KeyCode gingaValue;
  int sdlValue;

  if (event.type == SDL_FINGERUP || event.type == SDL_MOUSEBUTTONUP)
    {
      return Key::KEY_TAP;
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
      return Key::KEY_NULL;
      ;
    }

  gingaValue = Ginga_Display->fromMBToGinga (sdlValue);

  if (gingaValue >= Key::KEY_SMALL_A
      && gingaValue <= Key::KEY_SMALL_Z
      && ((capsOn && !shiftOn) || (!capsOn && shiftOn)))
    {
      gingaValue = Ginga_Display->fromMBToGinga (sdlValue + 5000);
    }

  map <Key::KeyCode, Key::KeyCode> keyboardToRemoteControl
    = {
        {Key::KEY_F1, Key::KEY_RED},
        {Key::KEY_F2, Key::KEY_GREEN},
        {Key::KEY_F3, Key::KEY_YELLOW},
        {Key::KEY_F4, Key::KEY_BLUE},
        {Key::KEY_F5, Key::KEY_MENU},
        {Key::KEY_F6, Key::KEY_INFO},
        {Key::KEY_F7, Key::KEY_EPG},
        {Key::KEY_PLUS_SIGN, Key::KEY_VOLUME_UP},
        {Key::KEY_MINUS_SIGN, Key::KEY_VOLUME_DOWN},
        {Key::KEY_PAGE_UP, Key::KEY_CHANNEL_UP},
        {Key::KEY_PAGE_DOWN, Key::KEY_CHANNEL_DOWN},
        {Key::KEY_BACKSPACE, Key::KEY_BACK},
        {Key::KEY_ESCAPE, Key::KEY_EXIT}
      };

  if (keyboardToRemoteControl.count (gingaValue))
    {
      gingaValue = keyboardToRemoteControl[gingaValue];
    }

  // Mapping between keyboard and remote control

  return gingaValue;
}

void *
InputEvent::getApplicationData ()
{
  if (isApplicationType ())
    {
      return event.user.data2;
    }

  return NULL;
}

unsigned int
InputEvent::getType ()
{
  unsigned int result = Key::KEY_NULL;

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
InputEvent::isButtonPressType ()
{
  if (event.type == SDL_FINGERUP || event.type == SDL_MOUSEBUTTONUP
      || event.type == SDL_FINGERDOWN || event.type == SDL_MOUSEBUTTONDOWN)
    {
      return true;
    }

  return false;
}

bool
InputEvent::isMotionType ()
{
  if (event.type == SDL_MOUSEMOTION || event.type == SDL_FINGERMOTION)
    {
      return true;
    }
  return false;
}

bool
InputEvent::isPressedType ()
{
  if (event.type == SDL_KEYDOWN)
    {
      return true;
    }
  return false;
}

bool
InputEvent::isKeyType ()
{
  if (event.type == SDL_KEYUP || event.type == SDL_KEYDOWN)
    {
      return true;
    }
  return false;
}

bool
InputEvent::isApplicationType ()
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
InputEvent::setAxisValue (int x, int y, arg_unused (int z))
{
  this->x = x;
  this->y = y;
}

void
InputEvent::getAxisValue (int *x, int *y, arg_unused (int *z))
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
      *x = (int) event.button.x;
      *y = (int) event.button.y;
      break;

    case SDL_FINGERUP:
    case SDL_FINGERDOWN:
    case SDL_FINGERMOTION:
      *x = (int) event.tfinger.x;
      *y = (int) event.tfinger.y;
      break;

    default:
      g_assert_not_reached ();
    }
}

GINGA_MB_END  */
