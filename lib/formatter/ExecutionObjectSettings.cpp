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
#include "ExecutionObjectSettings.h"

#include "ExecutionObjectContext.h"
#include "ExecutionObjectSwitch.h"
#include "Scheduler.h"

GINGA_FORMATTER_BEGIN

ExecutionObjectSettings::ExecutionObjectSettings (GingaInternal *ginga,
                                                  const string &id,
                                                  NclNode *node)
  : ExecutionObject (ginga, id, node)
{
  NclNode *nodeEntity = cast (NclNode *, node->derefer ());
  g_assert_nonnull (nodeEntity);
  auto media = cast (NclMedia *, nodeEntity);
  g_assert_nonnull (media);
  g_assert (media->isSettings ());
  _player = Player::createPlayer (_ginga, _id, "", media->getMimeType ());
}

void
ExecutionObjectSettings::setProperty (const string &name,
                                      const string &value,
                                      unused (GingaTime dur))
{
  if (name == "service.currentFocus")
    Player::setCurrentFocus (value);
}

void
ExecutionObjectSettings::updateCurrentFocus (const string &index)
{
  string next;
  string i;

  if (index != "")
    {
      next = index;
    }
  else
    {
      Scheduler *sched = _ginga->getScheduler ();
      g_assert_nonnull (sched);

      for (auto obj: *sched->getObjects ())
        if (obj->isFocused ())
          return;                   // nothing to do

      for (auto obj: *sched->getObjects ())
        {
          if (!instanceof (ExecutionObjectContext *, obj)
              && !instanceof (ExecutionObjectSettings *, obj)
              && !instanceof (ExecutionObjectSwitch *, obj)
              && obj->isOccurring ()
              && (i = obj->getProperty ("focusIndex")) != ""
              && (next == "" || g_strcmp0 (i.c_str (), next.c_str ()) < 0))
            {
              next = i;
            }
        }
    }

  // Do the actual attribution.
  string name = "service.currentFocus";
  string value = next;

  FormatterEvent *evt = this->getEventByAnchorId
    (NclEventType::ATTRIBUTION, name, "");
  if (evt == nullptr)           // do no trigger links
    {
      cast (ExecutionObject *, this)->setProperty (name, value);
    }
  else                          // trigger links
    {
      evt->transition (NclEventStateTransition::START);
      cast (ExecutionObject *, this)->setProperty (name, value);
      evt->transition (NclEventStateTransition::STOP);
    }
}

void
ExecutionObjectSettings::scheduleFocusUpdate (const string &next)
{
  _hasNextFocus = true;
  _nextFocus = next;
}

void
ExecutionObjectSettings::sendTickEvent (unused (GingaTime total),
                                        unused (GingaTime diff),
                                        unused (GingaTime frame))
{
  if (_hasNextFocus)            // effectuate pending focus index update
    {
      this->updateCurrentFocus (_nextFocus);
      _hasNextFocus = false;
    }
}

GINGA_FORMATTER_END
