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
#include "Event.h"
#include "Object.h"

GINGA_NAMESPACE_BEGIN

// Public.

Event::Event (Event::Type type, Object *object, const string &id)
{
  _type = type;
  g_assert_nonnull (object);
  _object = object;
  _id = id;
  _state = Event::SLEEPING;
  _begin = 0;
  _end = GINGA_TIME_NONE;
}

Event::~Event ()
{
}

Event::Type
Event::getType ()
{
  return _type;
}

Object *
Event::getObject ()
{
  return _object;
}

string
Event::getId ()
{
  return _id;
}

string
Event::getFullId ()
{
  string obj_id;
  g_assert_nonnull (_object);
  obj_id = _object->getId ();
  switch (_type)
    {
    case Event::PRESENTATION:
      if (_id == "@lambda")
        return obj_id + _id;
      else
        return obj_id + "@" + _id;
    case Event::ATTRIBUTION:
      return obj_id + "." + _id;
    case Event::SELECTION:
      return obj_id + "<" + _id + ">";
    case Event::PREPARATION:
      if (_id == "@lambda")
        return obj_id + "$" + _id.substr (1, _id.length() - 1);
      else
        return obj_id + "$" + _id;
    default:
      g_assert_not_reached ();
    }
  g_assert_not_reached ();
}

Event::State
Event::getState ()
{
  return _state;
}

string
Event::toString ()
{
  string str;

  str = xstrbuild (
      "\
Event (%p)\n\
  object: %p (%s, id: %s)\n\
  id: %s\n\
  full-id: %s\n\
  type: %s\n\
  state: %s\n",
      this, _object, _object->getObjectTypeAsString ().c_str (),
      _object->getId ().c_str (), _id.c_str (), this->getFullId ().c_str (),
      Event::getEventTypeAsString (_type).c_str (),
      Event::getEventStateAsString (_state).c_str ());

  if (_type == Event::PRESENTATION)
    {
      str += xstrbuild ("\
    begin: %" GINGA_TIME_FORMAT "\n\
    end: %" GINGA_TIME_FORMAT "\n",
                        GINGA_TIME_ARGS (_begin), GINGA_TIME_ARGS (_end));
    }

  if (_parameters.size () > 0)
    {
      str += xstrbuild ("    params:\n");
      for (auto it : _parameters)
        {
          str += xstrbuild ("    %s='%s'\n", it.first.c_str (),
                            it.second.c_str ());
        }
    }

  return str;
}

bool
Event::isLambda ()
{
  return _type == Event::PRESENTATION && _id == "@lambda";
}

void
Event::getInterval (Time *begin, Time *end)
{
  tryset (begin, _begin);
  tryset (end, _end);
}

void
Event::setInterval (Time begin, Time end)
{
  _begin = begin;
  _end = end;
}

bool
Event::hasLabel ()
{
  return _label != "";
}

string
Event::getLabel ()
{
  return _label;
}

void
Event::setLabel (const string &label)
{
  _label = label;
}

bool
Event::getParameter (const string &name, string *value)
{
  MAP_GET_IMPL (_parameters, name, value);
}

bool
Event::setParameter (const string &name, const string &value)
{
  MAP_SET_IMPL (_parameters, name, value);
}

/**
 * @brief Transitions event.
 * @param trans The desired transition.
 * @return \c true if successful, or \c false otherwise.
 */
bool
Event::transition (Event::Transition trans)
{
  Event::State curr = _state;
  Event::State next;
  switch (trans)
    {
    case Event::START:
      if (curr == Event::OCCURRING)
        return false;
      next = Event::OCCURRING;
      break;
    case Event::PAUSE:
      if (curr != Event::OCCURRING)
        return false;
      next = Event::PAUSED;
      break;
    case Event::RESUME:
      if (curr != Event::PAUSED)
        return false;
      next = Event::OCCURRING;
      break;
    case Event::STOP: // fall through
    case Event::ABORT:
      if (curr == Event::SLEEPING)
        return false;
      next = Event::SLEEPING;
      break;
    default:
      g_assert_not_reached ();
    }

  // Initiate transition.
  if (unlikely (!_object->beforeTransition (this, trans)))
    return false;

  // Update event state.
  _state = next;

  // Finish transition.
  if (unlikely (!_object->afterTransition (this, trans)))
    {
      _state = curr;
      return false;
    }

  return true;
}

void
Event::reset ()
{
  _state = Event::SLEEPING;
}

// Public: Static.

string
Event::getEventTypeAsString (Event::Type type)
{
  switch (type)
    {
    case Event::PRESENTATION:
      return "presentation";
    case Event::ATTRIBUTION:
      return "attribution";
    case Event::SELECTION:
      return "selection";
    case Event::PREPARATION:
      return "preparation";
    default:
      g_assert_not_reached ();
    }
}

string
Event::getEventStateAsString (Event::State state)
{
  switch (state)
    {
    case Event::SLEEPING:
      return "sleeping";
    case Event::OCCURRING:
      return "occurring";
    case Event::PAUSED:
      return "paused";
    default:
      g_assert_not_reached ();
    }
}

string
Event::getEventTransitionAsString (Event::Transition tr)
{
  switch (tr)
    {
    case Event::START:
      return "start";
    case Event::PAUSE:
      return "pause";
    case Event::RESUME:
      return "resume";
    case Event::STOP:
      return "stop";
    case Event::ABORT:
      return "abort";
    default:
      g_assert_not_reached ();
    }
}

Event::Transition
Event::getStringAsTransition (string str)
{
  if (xstrcasecmp (str, "start") == 0)
    return Event::START;
  else if (xstrcasecmp (str, "pause") == 0)
    return Event::PAUSE;
  else if (xstrcasecmp (str, "resume") == 0)
    return Event::RESUME;
  else if (xstrcasecmp (str, "stop") == 0)
    return Event::STOP;
  else if (xstrcasecmp (str, "abort") == 0)
    return Event::ABORT;
  else
    g_assert_not_reached ();
}

GINGA_NAMESPACE_END
