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


// Public.

FormatterEvent::FormatterEvent (FormatterEvent::Type type,
                                FormatterObject *object,
                                const string &id)
{
  _type = type;
  g_assert_nonnull (object);
  _object = object;
  _id = id;
  _state = FormatterEvent::SLEEPING;
  _begin = 0;
  _end = GINGA_TIME_NONE;
}

FormatterEvent::~FormatterEvent ()
{
}

FormatterEvent::Type
FormatterEvent::getType ()
{
  return _type;
}

FormatterObject *
FormatterEvent::getObject ()
{
  return _object;
}

string
FormatterEvent::getId ()
{
  return _id;
}

FormatterEvent::State
FormatterEvent::getState ()
{
  return _state;
}

bool
FormatterEvent::isLambda ()
{
  return _type == FormatterEvent::PRESENTATION && _id == "@lambda";
}

void
FormatterEvent::getInterval (GingaTime *begin, GingaTime *end)
{
  tryset (begin, _begin);
  tryset (end, _end);
}

void
FormatterEvent::setInterval (GingaTime begin, GingaTime end)
{
  _begin = begin;
  _end = end;
}

bool
FormatterEvent::getParameter (const string &name, string *value)
{
  MAP_GET_IMPL (_parameters, name, value);
}

bool
FormatterEvent::setParameter (const string &name, const string &value)
{
  MAP_SET_IMPL (_parameters, name, value);
}

bool
FormatterEvent::transition (FormatterEvent::Transition trans)
{
  FormatterEvent::State curr = _state;
  FormatterEvent::State next;
  switch (trans)
    {
    case FormatterEvent::START:
      if (curr == FormatterEvent::OCCURRING)
        return false;
      next = FormatterEvent::OCCURRING;
      break;
    case FormatterEvent::PAUSE:
      if (curr != FormatterEvent::OCCURRING)
        return false;
      next = FormatterEvent::PAUSED;
      break;
    case FormatterEvent::RESUME:
      if (curr != FormatterEvent::PAUSED)
        return false;
      next = FormatterEvent::OCCURRING;
      break;
    case FormatterEvent::STOP: // fall through
    case FormatterEvent::ABORT:
      if (curr == FormatterEvent::SLEEPING)
        return false;
      next = FormatterEvent::SLEEPING;
      break;
    default:
      g_assert_not_reached ();
    }
  if (!_object->startTransition (this, trans))
    return false;
  _state = next;
  _object->endTransition (this, trans);
  return true;
}

void
FormatterEvent::reset ()
{
  _state = FormatterEvent::SLEEPING;
}


// Public: Static.

string
FormatterEvent::getEventTypeAsString (FormatterEvent::Type type)
{
  switch (type)
    {
    case FormatterEvent::PRESENTATION:
      return "presentation";
    case FormatterEvent::ATTRIBUTION:
      return "attribution";
    case FormatterEvent::SELECTION:
      return "selection";
    default:
      g_assert_not_reached ();
    }
}

string
FormatterEvent::getEventStateAsString (FormatterEvent::State state)
{
  switch (state)
    {
    case FormatterEvent::SLEEPING:
      return "sleeping";
    case FormatterEvent::OCCURRING:
      return "occurring";
    case FormatterEvent::PAUSED:
      return "paused";
    default:
      g_assert_not_reached ();
    }
}

string
FormatterEvent::getEventTransitionAsString (FormatterEvent::Transition tr)
{
  switch (tr)
    {
    case FormatterEvent::START:
      return "start";
    case FormatterEvent::PAUSE:
      return "pause";
    case FormatterEvent::RESUME:
      return "resume";
    case FormatterEvent::STOP:
      return "stop";
    case FormatterEvent::ABORT:
      return "abort";
    default:
      g_assert_not_reached ();
    }
}

GINGA_NAMESPACE_END
