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

#include "aux-ginga.h"
#include "FormatterMediaSettings.h"

#include "FormatterContext.h"
#include "FormatterSwitch.h"

GINGA_NAMESPACE_BEGIN

FormatterMediaSettings::FormatterMediaSettings (Formatter *formatter,
                                                const string &id)
  :FormatterMedia (formatter, id, "application/x-ginga-settings", "")
{
}

FormatterMediaSettings::~FormatterMediaSettings ()
{
}

void
FormatterMediaSettings::setProperty (const string &name,
                                     const string &value,
                                     GingaTime dur)
{
  if (name == "service.currentFocus")
    Player::setCurrentFocus (value);
  FormatterMedia::setProperty (name, value, dur);
}

void
FormatterMediaSettings::sendTickEvent (unused (GingaTime total),
                                       unused (GingaTime diff),
                                       unused (GingaTime frame))
{
  if (_hasNextFocus)            // effectuate pending focus index update
    {
      this->updateCurrentFocus (_nextFocus);
      _hasNextFocus = false;
    }
  FormatterMedia::sendTickEvent (total, diff, frame);
}

bool
FormatterMediaSettings::isFocused ()
{
  return false;
}

bool
FormatterMediaSettings::getZ (unused (int *z), unused (int *zorder))
{
  return false;
}

void
FormatterMediaSettings::redraw (unused (cairo_t *cr))
{
}

void
FormatterMediaSettings::updateCurrentFocus (const string &index)
{
  string next;
  string i;

  if (index != "")
    {
      next = index;
    }
  else
    {
      for (auto media: *_formatter->getMediaObjects ())
        {
          if (media->isFocused ())
            return;             // nothing to do
        }

      for (auto media: *_formatter->getMediaObjects ())
        {
          if (media->isOccurring ()
              && (i = media->getProperty ("focusIndex")) != ""
              && (next == "" || i < next))
            {
              next = i;
            }
        }
    }

  // Do the actual attribution.
  string name = "service.currentFocus";
  string value = next;

  FormatterEvent *evt = this->getEvent (NclEventType::ATTRIBUTION, name);
  if (evt == nullptr)           // do no trigger links
    {
      cast (FormatterObject *, this)->setProperty (name, value);
    }
  else                          // trigger links
    {
      evt->transition (NclEventStateTransition::START);
      cast (FormatterObject *, this)->setProperty (name, value);
      evt->transition (NclEventStateTransition::STOP);
    }
}

void
FormatterMediaSettings::scheduleFocusUpdate (const string &next)
{
  _hasNextFocus = true;
  _nextFocus = next;
}

GINGA_NAMESPACE_END
