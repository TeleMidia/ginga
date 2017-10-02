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
                                                  Node *node,
                                                  INclActionListener *lst)
  : ExecutionObject (ginga, id, node, lst)
{
  Node *nodeEntity = cast (Node *, node->derefer ());
  g_assert_nonnull (nodeEntity);
  auto media = cast (Media *, nodeEntity);
  g_assert_nonnull (media);
  g_assert (media->isSettings ());
  _player = Player::createPlayer (_ginga, _id, "", media->getMimeType ());
  g_assert (_ginga->registerEventListener (this));
}

void
ExecutionObjectSettings::setProperty (const string &name,
                                      unused (const string &from),
                                      const string &to,
                                      unused (GingaTime dur))
{
  if (name == "service.currentFocus")
    Player::setCurrentFocus (to);
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
  string from = this->getProperty (name);
  string to = next;

  NclEvent *evt = nullptr;
  for (auto pair: _events)
    {
      if (xstrhasprefix (pair.first.c_str (), "service.currentFocus"))
        {
          evt = pair.second;
          break;
        }
    }

  if (evt == nullptr)           // do no trigger links
    {
      cast (ExecutionObject *, this)->setProperty (name, from, to, 0);
    }
  else                          // trigger links
    {
      AttributionEvent *attevt = cast (AttributionEvent *, evt);
      g_assert_nonnull (attevt);
      attevt->start ();
      attevt->setValue (to);
      cast (ExecutionObject *, this)->setProperty (name, from, to, 0);
      attevt->stop ();
    }
}

void
ExecutionObjectSettings::scheduleFocusUpdate (const string &next)
{
  _hasNextFocus = true;
  _nextFocus = next;
}

void
ExecutionObjectSettings::handleKeyEvent (unused (const string &key),
                                         unused (bool press))
{
}

void
ExecutionObjectSettings::handleTickEvent (unused (GingaTime total),
                                          unused (GingaTime diff),
                                          unused (int frame))
{
  if (_hasNextFocus)            // effectuate pending focus index update
    {
      this->updateCurrentFocus (_nextFocus);
      _hasNextFocus = false;
    }
}

GINGA_FORMATTER_END
