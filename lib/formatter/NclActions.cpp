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
#include "NclActions.h"

GINGA_FORMATTER_BEGIN

/**
 * @brief Creates a new action.
 * @param event Target event.
 * @param transition Action transition.
 * @param listener Action listener.
 */
NclSimpleAction::NclSimpleAction (NclEvent *event,
                                  EventStateTransition transition,
                                  INclActionListener *listener)
{
  g_assert_nonnull (event);
  g_assert_nonnull (listener);

  _event = event;
  _transition = transition;
  _listener = listener;
  _duration = "";
  _value = "";
}

/**
 * @brief Destroys action.
 */
NclSimpleAction::~NclSimpleAction ()
{
}

/**
 * @brief Gets target event.
 * @return Target event.
 */
NclEvent *
NclSimpleAction::getEvent ()
{
  return _event;
}

/**
 * @brief Gets target event type.
 * @return Target event type.
 */
EventType
NclSimpleAction::getEventType ()
{
  return _event->getType ();
}

/**
 * @brief Gets action transition.
 * @return Action transition.
 */
EventStateTransition
NclSimpleAction::getEventStateTransition ()
{
  return _transition;
}

/**
 * @brief Gets action duration.
 * @return Action duration.
 */
string
NclSimpleAction::getDuration ()
{
  return _duration;
}

/**
 * @brief Sets action duration.
 * @param duration Duration.
 */
void
NclSimpleAction::setDuration (const string &duration)
{
  _duration = duration;
}

/**
 * @brief Gets action value.
 * @return Action value.
 */
string
NclSimpleAction::getValue (void)
{
  return _value;
}

/**
 * @brief Sets action value.
 * @param value Value.
 */
void
NclSimpleAction::setValue (const string &value)
{
  _value = value;
}

/**
 * @brief Runs action.
 */
void
NclSimpleAction::run ()
{
  _listener->scheduleAction (this);
}

GINGA_FORMATTER_END
