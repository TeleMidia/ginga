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
#include "SDLInputEvent.h"
#include "Display.h"
#include "CodeMap.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

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
  event.user.data1 = deconst (void *, ET_USEREVENT.c_str ());
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
SDLInputEvent::setKeyCode (MbKey::KeyCode keyCode)
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

MbKey::KeyCode
SDLInputEvent::getKeyCode ()
{
  MbKey::KeyCode gingaValue;
  int sdlValue;

  if (event.type == SDL_FINGERUP || event.type == SDL_MOUSEBUTTONUP)
    {
      return MbKey::KEY_TAP;
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
      return MbKey::KEY_NULL;
      ;
    }

  gingaValue = Ginga_Display->fromMBToGinga (sdlValue);

  if (gingaValue >= MbKey::KEY_SMALL_A
      && gingaValue <= MbKey::KEY_SMALL_Z
      && ((capsOn && !shiftOn) || (!capsOn && shiftOn)))
    {
      gingaValue = Ginga_Display->fromMBToGinga (sdlValue + 5000);
    }

  map <MbKey::KeyCode, MbKey::KeyCode> keyboardToRemoteControl
    = {
        {MbKey::KEY_F1, MbKey::KEY_RED},
        {MbKey::KEY_F2, MbKey::KEY_GREEN},
        {MbKey::KEY_F3, MbKey::KEY_YELLOW},
        {MbKey::KEY_F4, MbKey::KEY_BLUE},
        {MbKey::KEY_F5, MbKey::KEY_MENU},
        {MbKey::KEY_F6, MbKey::KEY_INFO},
        {MbKey::KEY_F7, MbKey::KEY_EPG},
        {MbKey::KEY_PLUS_SIGN, MbKey::KEY_VOLUME_UP},
        {MbKey::KEY_MINUS_SIGN, MbKey::KEY_VOLUME_DOWN},
        {MbKey::KEY_PAGE_UP, MbKey::KEY_CHANNEL_UP},
        {MbKey::KEY_PAGE_DOWN, MbKey::KEY_CHANNEL_DOWN},
        {MbKey::KEY_BACKSPACE, MbKey::KEY_BACK},
        {MbKey::KEY_ESCAPE, MbKey::KEY_EXIT}
      };

  if (keyboardToRemoteControl.count (gingaValue))
    {
      gingaValue = keyboardToRemoteControl[gingaValue];
    }

  // Mapping between keyboard and remote control

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
  unsigned int result = MbKey::KEY_NULL;

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
SDLInputEvent::setAxisValue (int x, int y, arg_unused (int z))
{
  this->x = x;
  this->y = y;
}

void
SDLInputEvent::getAxisValue (int *x, int *y, arg_unused (int *z))
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

GINGA_MB_END
