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

#include "FormatterCondition.h"
#include "aux-ginga.h"

GINGA_NAMESPACE_BEGIN

FormatterCondition::FormatterCondition (Predicate *predicate,
                                        FormatterEvent *event,
                                        NclEventStateTransition transition)
{
  _predicate = predicate;
  _transition = transition;
  _event = event;
  _event->addListener (this);
  _listener = nullptr;
}

FormatterCondition::~FormatterCondition ()
{
  if (_predicate != nullptr)
    delete _predicate;
}

Predicate *
FormatterCondition::getPredicate ()
{
  return _predicate;
}

FormatterEvent *
FormatterCondition::getEvent ()
{
  return _event;
}

void
FormatterCondition::setTriggerListener (IFormatterConditionListener *lst)
{
  g_assert_nonnull (lst);
  _listener = lst;
}

void
FormatterCondition::conditionSatisfied ()
{
  _listener->conditionSatisfied (this);
}

void
FormatterCondition::eventStateChanged (unused (FormatterEvent *event),
                                       NclEventStateTransition transition)
{
  if (_transition == transition)
    FormatterCondition::conditionSatisfied ();
}

GINGA_NAMESPACE_END
