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
#include "FormatterEvent.h"
#include "FormatterObject.h"

GINGA_NAMESPACE_BEGIN

FormatterEvent::FormatterEvent (Formatter *formatter, NclEventType type,
                                FormatterObject *object, NclAnchor *anchor)
{
  g_assert_nonnull (formatter);
  _formatter = formatter;
  _type = type;
  g_assert_nonnull (object);
  _object = object;
  g_assert_nonnull (anchor);
  _anchor = anchor;
  _state = NclEventState::SLEEPING;
}

FormatterEvent::~FormatterEvent ()
{
}

NclEventType
FormatterEvent::getType ()
{
  return _type;
}

FormatterObject *
FormatterEvent::getObject ()
{
  return _object;
}

NclAnchor *
FormatterEvent::getAnchor ()
{
  return _anchor;
}

NclEventState
FormatterEvent::getState ()
{
  return _state;
}

const vector<IFormatterEventListener *> *
FormatterEvent::getListeners ()
{
  return &_listeners;
}

void
FormatterEvent::addListener (IFormatterEventListener *listener)
{
  _listeners.push_back (listener);
}

bool
FormatterEvent::getParameter (const string &name, string *value)
{
  map<string, string>::iterator it;
  if ((it = _parameters.find (name)) == _parameters.end ())
    return false;
  tryset (value, it->second);
  return true;
}

void
FormatterEvent::setParameter (const string &name, const string &value)
{
  _parameters[name] = value;
}

bool
FormatterEvent::transition (NclEventStateTransition trans)
{
  NclEventState curr = _state;
  NclEventState next;
  switch (trans)
    {
    case NclEventStateTransition::START:
      if (curr == NclEventState::OCCURRING)
        return false;
      next = NclEventState::OCCURRING;
      break;
    case NclEventStateTransition::PAUSE:
      if (curr != NclEventState::OCCURRING)
        return false;
      next = NclEventState::PAUSED;
      break;
    case NclEventStateTransition::RESUME:
      if (curr != NclEventState::PAUSED)
        return false;
      next = NclEventState::OCCURRING;
      break;
    case NclEventStateTransition::STOP: // fall through
    case NclEventStateTransition::ABORT:
      if (curr == NclEventState::SLEEPING)
        return false;
      next = NclEventState::SLEEPING;
      break;
    default:
      g_assert_not_reached ();
    }
  if (!_object->exec (this, curr, next, trans))
    return false;
  _state = next;
  for (IFormatterEventListener *lst: _listeners)
    lst->eventStateChanged (this, trans);
  return true;
}

GINGA_NAMESPACE_END
