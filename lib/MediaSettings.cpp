/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "MediaSettings.h"

#include "Document.h"
#include "Context.h"
#include "Switch.h"

GINGA_NAMESPACE_BEGIN

MediaSettings::MediaSettings (Document *doc, const string &id)
  : Media (doc, id)
{
  this->setPropertyBool ("ginga.debug", true);
  this->setPropertyString ("type", "application/x-ginga-settings");
  this->createEvent (Event::ATTRIBUTION, "service.currentFocus");
}

MediaSettings::~MediaSettings ()
{
}

void
MediaSettings::sendTick (Time total, Time diff, Time frame)
{
  string next;

  if (this->getPropertyString ("_nextFocus", &next))
    {
      this->unsetProperty ("_nextFocus");

      // Find object with the lowest focus index.
      if (next == "")
        {
          set<Object *> objects;
          string i;

          this->getDocument ()->getObjects (&objects);
          for (auto obj: objects)
            {
              if (obj->getType () != Object::MEDIA)
                continue;

              Media *media = cast (Media *, obj);
              g_assert_nonnull (media);

              if (media->getLambda ()->getState () != Event::OCCURRING)
                continue;

              if (!media->getPropertyString ("focusIndex", &i) || i == "")
                continue;

              if (next == "" || i < next)
                {
                  next = i;
                }
            }
        }

      Event *evt = this->getEvent
        (Event::ATTRIBUTION, "service.currentFocus");
      g_assert_nonnull (evt);
      this->getDocument ()->evalAction (evt, Event::START, next);
    }

  Media::sendTick (total, diff, frame);
}

GINGA_NAMESPACE_END
