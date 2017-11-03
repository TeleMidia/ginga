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
#include "FormatterAction.h"

GINGA_BEGIN

/**
 * @brief Creates a new action.
 * @param event Target event.
 * @param transition NclAction transition.
 */
FormatterAction::FormatterAction (FormatterEvent *event,
                                  NclEventStateTransition transition)
{
  g_assert_nonnull (event);
  _event = event;
  _transition = transition;
  _duration = "";
  _value = "";
}

/**
 * @brief Destroys action.
 */
FormatterAction::~FormatterAction ()
{
}

/**
 * @brief Gets target event.
 * @return Target event.
 */
FormatterEvent *
FormatterAction::getEvent ()
{
  return _event;
}

/**
 * @brief Gets target event type.
 * @return Target event type.
 */
NclEventType
FormatterAction::getEventType ()
{
  return _event->getType ();
}

/**
 * @brief Gets action transition.
 * @return NclAction transition.
 */
NclEventStateTransition
FormatterAction::getEventStateTransition ()
{
  return _transition;
}

/**
 * @brief Gets action duration.
 * @return NclAction duration.
 */
string
FormatterAction::getDuration ()
{
  return _duration;
}

/**
 * @brief Sets action duration.
 * @param duration Duration.
 */
void
FormatterAction::setDuration (const string &duration)
{
  _duration = duration;
}

/**
 * @brief Gets action value.
 * @return NclAction value.
 */
string
FormatterAction::getValue (void)
{
  return _value;
}

/**
 * @brief Sets action value.
 * @param value Value.
 */
void
FormatterAction::setValue (const string &value)
{
  _value = value;
}

GINGA_END
