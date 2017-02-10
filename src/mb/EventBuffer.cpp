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
#include "EventBuffer.h"
#include "InputEvent.h"

#include "Display.h"

GINGA_MB_BEGIN

EventBuffer::EventBuffer ()
{
  Thread::mutexInit (&ebMutex);

  capsOn = false;
  shiftOn = false;

  isWaiting = false;
  Thread::condInit (&cond, NULL);
  Thread::mutexInit (&condMutex);
}

EventBuffer::~EventBuffer ()
{
  vector<SDL_Event>::iterator i;

  Thread::mutexLock (&ebMutex);
  eventBuffer.clear ();
  Thread::mutexUnlock (&ebMutex);
  Thread::mutexDestroy (&ebMutex);

  isWaiting = false;
  Thread::condDestroy (&cond);
  Thread::mutexDestroy (&condMutex);
}

bool
EventBuffer::checkEvent (Uint32 winId, SDL_Event event)
{
  Uint32 windowID = winId + 1;

  switch (event.type)
    {
    case SDL_MOUSEMOTION:
      windowID = event.motion.windowID;
      break;

    case SDL_KEYDOWN:
    case SDL_KEYUP:
      windowID = event.key.windowID;
      break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      windowID = event.button.windowID;
      break;

    case SDL_FINGERMOTION:
    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
      // TODO: how to get windowID from new SDL tfinger structure
      // windowID = event.tfinger.windowID;
      break;

    default:
      break;
    }
  return (windowID == winId) ? true : false;
}

void
EventBuffer::feed (SDL_Event event, bool capsOn, bool shiftOn)
{
  Thread::mutexLock (&ebMutex);
  this->capsOn = capsOn;
  this->shiftOn = shiftOn;

  eventBuffer.push_back (event);
  eventArrived ();
  Thread::mutexUnlock (&ebMutex);
}

void
EventBuffer::postInputEvent (InputEvent *event)
{
  SDL_Event ev;

  if (event != NULL && event->getContent () != NULL)
    {
      ev = *(SDL_Event *)(event->getContent ());
      feed (ev, capsOn, shiftOn);
    }

  if (event != NULL)
    {
      event->clearContent ();
      delete event;
    }
}

void
EventBuffer::waitEvent ()
{
  SDL_Event event;
  vector<SDL_Event>::iterator i;

  Thread::mutexLock (&ebMutex);
  if (!eventBuffer.empty ())
    {
      Thread::mutexUnlock (&ebMutex);
      return;
    }
  Thread::mutexUnlock (&ebMutex);

  waitForEvent ();

  Thread::mutexLock (&ebMutex);
  i = eventBuffer.begin ();
  if (i != eventBuffer.end ())
    {
      event = *i;
      if (event.type == SDL_USEREVENT && event.user.code == SDLK_APPLICATION
          && event.user.data1 != NULL && event.user.data2 == NULL)
        {
          if (strcmp ((char *)event.user.data1,
                      InputEvent::ET_WAKEUP.c_str ())
              == 0)
            {
              eventBuffer.erase (i);
              Thread::mutexUnlock (&ebMutex);
              return;
            }
        }
    }
  Thread::mutexUnlock (&ebMutex);
}

InputEvent *
EventBuffer::getNextEvent ()
{
  SDL_Event sdlEvent;
  InputEvent *gingaEvent = NULL;
  vector<SDL_Event>::iterator i;

  Thread::mutexLock (&ebMutex);
  if (!eventBuffer.empty ())
    {
      i = eventBuffer.begin ();
      sdlEvent = *i;

      gingaEvent = new InputEvent (sdlEvent);
      gingaEvent->setModifiers (capsOn, shiftOn);
      eventBuffer.erase (i);
    }
  Thread::mutexUnlock (&ebMutex);

  return gingaEvent;
}

void *
EventBuffer::getContent ()
{
  return (void *)&eventBuffer;
}

void
EventBuffer::waitForEvent ()
{
  isWaiting = true;
  Thread::mutexLock (&condMutex);
  Thread::condWait (&cond, &condMutex);
  isWaiting = false;
  Thread::mutexUnlock (&condMutex);
}

bool
EventBuffer::eventArrived ()
{
  if (isWaiting)
    {
      Thread::condSignal (&cond);
      return true;
    }
  return false;
}

GINGA_MB_END
