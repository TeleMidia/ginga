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

#include "Context.h"
#include "Switch.h"

GINGA_NAMESPACE_BEGIN

// Public.

MediaSettings::MediaSettings (const string &id)
    : Media (id), _nextFocus (""), _hasNextFocus (false)
{
  _properties["type"] = "application/x-ginga-settings";
  this->addAttributionEvent ("service.currentFocus");
}

MediaSettings::~MediaSettings ()
{
}

// Public: Object.

string
MediaSettings::getObjectTypeAsString ()
{
  return "MediaSettings";
}

void
MediaSettings::setProperty (const string &name, const string &value,
                            Time dur)
{
  if (name == "service.currentFocus")
    Player::setCurrentFocus (value);
  Media::setProperty (name, value, dur);
}

void
MediaSettings::sendTick (Time total, Time diff, Time frame)
{
  if (_hasNextFocus) // effectuate pending focus index update
    {
      this->updateCurrentFocus (_nextFocus);
      _hasNextFocus = false;
    }
  Media::sendTick (total, diff, frame);
}

// Public: Media.

bool
MediaSettings::isFocused ()
{
  return false;
}

bool MediaSettings::getZ (unused (int *z), unused (int *zorder))
{
  return false;
}

void MediaSettings::redraw (unused (cairo_t *cr))
{
}

// Public.

void
MediaSettings::updateCurrentFocus (const string &index)
{
  string next;
  string i;

  if (index != "")
    {
      next = index;
    }
  else
    {
      for (auto media : *_doc->getMedias ())
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
  string value = next;
  Event *evt = this->getAttributionEvent ("service.currentFocus");
  g_assert_nonnull (evt);
  _doc->evalAction (evt, Event::START, value);
}

void
MediaSettings::scheduleFocusUpdate (const string &next)
{
  _hasNextFocus = true;
  _nextFocus = next;
}

GINGA_NAMESPACE_END
