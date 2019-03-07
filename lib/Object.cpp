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
#include "Object.h"
#include "LuaAPI.h"

#include "Composition.h"
#include "Context.h"
#include "Document.h"
#include "Event.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Switch.h"

GINGA_NAMESPACE_BEGIN

// Public.

Object::Object (Document *doc, Composition *parent, const string &id)
{
  g_return_if_fail (doc != NULL);

  _id = id;

  _doc = doc;
  _L = doc->getLuaState ();

  _parent = parent;
  if (_parent != NULL)
    _parent->addChild (this);

  _time = GINGA_TIME_NONE;
}

Object::~Object ()
{
}

Document *
Object::getDocument ()
{
  return _doc;
}

Composition *
Object::getParent ()
{
  return _parent;
}

string
Object::getId ()
{
  return _id;
}

const list<pair<string, Composition *> > *
Object::getAliases ()
{
  return &_aliases;
}

bool
Object::hasAlias (const string &alias)
{
  for (auto curr: _aliases)
    if (curr.first == alias)
      return true;
  return false;
}

void
Object::addAlias (const string &alias, Composition *parent)
{
  auto alias_pair = make_pair (alias, parent);
  tryinsert (alias_pair, _aliases, push_back);
}

string
Object::getProperty (const string &name)
{
  auto it = _properties.find (name);
  if (it == _properties.end ())
    return "";
  return it->second;
}

void
Object::setProperty (const string &name, const string &value, Time duration)
{
  g_return_if_fail (GINGA_TIME_IS_VALID (duration));
  _properties[name] = value;
}

void
Object::getEvents (set<Event *> *events, unsigned int mask)
{
  g_return_if_fail (events != NULL);

  for (auto evt: _events)
    if (evt->getType () & mask)
      events->insert (evt);
}

Event *
Object::getEvent (Event::Type type, const string &id)
{
  for (auto evt: _events)
    if (evt->getType () == type && evt->getId () == id)
      return evt;
  return NULL;
}

Event *
Object::createEvent (Event::Type type, const string &id)
{
  Event *evt;

  if (this->getEvent (type, id) != NULL)
    return NULL;                // id already in use

  evt = new Event (type, this, id);
  _events.insert (evt);

  return evt;
}
// Protected.

void
Object::_initEvents ()
{
  _lambda = this->createEvent (Event::PRESENTATION, "@lambda");
  g_assert_nonnull (_lambda);
}

void
Object::_finiEvents ()
{
  for (auto evt: _events)
    delete evt;
  _lambda = NULL;
}

// TODO --------------------------------------------------------------------

Event *
Object::getLambda ()
{
  g_assert_nonnull (_lambda);
  return _lambda;
}

bool
Object::isOccurring ()
{
  g_assert_nonnull (_lambda);
  return _lambda->getState () == Event::OCCURRING;
}

bool
Object::isPaused ()
{
  g_assert_nonnull (_lambda);
  return _lambda->getState () == Event::PAUSED;
}

bool
Object::isSleeping ()
{
  g_assert_nonnull (_lambda);
  return _lambda->getState () == Event::SLEEPING;
}

const list<pair<Action, Time> > *
Object::getDelayedActions ()
{
  return &_delayed;
}

void
Object::addDelayedAction (Event *event, Event::Transition transition,
                          const string &value, Time delay)
{
  Action act;

  act.event = event;
  act.transition = transition;
  act.value = value;
  _delayed.push_back (std::make_pair (act, _time + delay));
}

void
Object::sendKey (unused (const string &key), unused (bool press))
{
}

void
Object::sendTick (unused (Time total), Time diff, unused (Time frame))
{
  if (unlikely (!this->isOccurring ()))
    return;                     // nothing to do

  g_assert (GINGA_TIME_IS_VALID (_time));
  _time += diff;

  list<Action> trigger;
  for (auto &it : _delayed)
    {
      if (_time >= it.second)
        {
          it.second = GINGA_TIME_NONE;
          trigger.push_back (it.first);
        }
    }

  for (auto action : trigger)
    {
      _doc->evalAction (action);
      if (!this->isOccurring ())
        return;
    }

  for (auto it = _delayed.begin (); it != _delayed.end ();)
    {
      if (it->second == GINGA_TIME_NONE)
        it = _delayed.erase (it);
      else
        ++it;
    }
}

Time
Object::getTime ()
{
  return _time;
}

void
Object::doStart ()
{
  _time = 0;
  if (_parent != nullptr && instanceof (Context *, _parent))
    cast (Context *, _parent)->incAwakeChildren ();

  // schedule set currentFocus if the object have focusIndex
  if (!this->getProperty ("focusIndex").empty ())
    _doc->getSettings ()->scheduleFocusUpdate ("");
}

void
Object::doStop ()
{
  _time = GINGA_TIME_NONE;
  for (auto evt : _events)
    evt->setState (Event::SLEEPING);
  _delayed.clear ();
  if (_parent != nullptr && instanceof (Context *, _parent))
    cast (Context *, _parent)->decAwakeChildren ();
}

GINGA_NAMESPACE_END
