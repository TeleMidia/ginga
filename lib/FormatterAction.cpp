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

GINGA_NAMESPACE_BEGIN

FormatterAction::FormatterAction (FormatterEvent *event,
                                  NclEventStateTransition transition)
{
  g_assert_nonnull (event);
  _event = event;
  _transition = transition;
}

FormatterAction::~FormatterAction ()
{
}

FormatterEvent *
FormatterAction::getEvent ()
{
  return _event;
}

NclEventType
FormatterAction::getEventType ()
{
  return _event->getType ();
}

NclEventStateTransition
FormatterAction::getEventStateTransition ()
{
  return _transition;
}

bool
FormatterAction::getParameter (const string &name, string *value)
{
  map<string, string>::iterator it;
  if ((it = _parameters.find (name)) == _parameters.end ())
    return false;
  tryset (value, it->second);
  return true;
}

void
FormatterAction::setParameter (const string &name, const string &value)
{
  _parameters[name] = value;
}

GINGA_NAMESPACE_END
