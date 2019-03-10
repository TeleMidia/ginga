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

Object::Object (Document *doc, unused (const string &id))
{
  g_return_if_fail (doc != NULL);

  _L = doc->getLuaState ();
  g_assert_nonnull (_L);

  _time = GINGA_TIME_NONE;      // DEPRECATED
}

Object::~Object ()
{
}

Document *
Object::getDocument ()
{
  Document *doc;

  LuaAPI::Object_call (_L, this, "getDocument", 0, 1);
  doc = LuaAPI::Document_check (_L, -1);
  lua_pop (_L, 1);

  return doc;
}

Object::Type
Object::getType ()
{
  Object::Type type;

  LuaAPI::Object_call (_L, this, "getType", 0, 1);
  type = LuaAPI::Object_Type_check (_L, -1);
  lua_pop (_L, 1);

  return type;
}

string
Object::getId ()
{
  const char *id;

  LuaAPI::Object_call (_L, this, "getId", 0, 1);
  id = luaL_checkstring (_L, -1);
  lua_pop (_L, 1);

  return string (id);
}

void
Object::getParents (set<Composition *> *parents)
{
  lua_Integer len;
  lua_Integer i;

  g_return_if_fail (parents != NULL);

  LuaAPI::Object_call (_L, this, "getParents", 0, 1);
  g_assert (lua_type (_L, -1) == LUA_TTABLE);

  len = luaL_len (_L, -1);
  for (i = 1; i <= len; i++)
    {
      Composition *comp;

      lua_rawgeti (_L, -1, i);
      comp = LuaAPI::Composition_check (_L, -1);
      parents->insert (comp);
      lua_pop (_L, 1);
    }
}

void
Object::getEvents (set<Event *> *events)
{
  lua_Integer len;
  lua_Integer i;

  g_return_if_fail (events != NULL);

  LuaAPI::Object_call (_L, this, "getEvents", 0, 1);
  g_assert (lua_type (_L, -1) == LUA_TTABLE);

  len = luaL_len (_L, -1);
  for (i = 1; i <= len; i++)
    {
      Event *evt;

      lua_rawgeti (_L, -1, i);
      evt = LuaAPI::Event_check (_L, -1);
      events->insert (evt);
      lua_pop (_L, 1);
    }
}

Event *
Object::getEvent (Event::Type type, const string &id)
{
  Event *evt = NULL;

  LuaAPI::Event_Type_push (_L, type);
  lua_pushstring (_L, id.c_str ());
  LuaAPI::Object_call (_L, this, "getEvent", 2, 1);
  if (!lua_isnil (_L, -1))
    {
      evt = LuaAPI::Event_check (_L, -1);
    }
  lua_pop (_L, 1);

  return evt;
}

Event *
Object::getLambda ()
{
  Event *evt;

  LuaAPI::Object_call (_L, this, "getLambda", 0, 1);
  evt = LuaAPI::Event_check (_L, -1);
  lua_pop (_L, 1);

  return evt;
}

Event *
Object::createEvent (Event::Type type, const string &id)
{
  Event *evt = NULL;

  LuaAPI::Event_Type_push (_L, type);
  lua_pushstring (_L, id.c_str ());
  LuaAPI::Object_call (_L, this, "createEvent", 2, 1);
  if (!lua_isnil (_L, -1))
    {
      evt = LuaAPI::Event_check (_L, -1);
    }
  lua_pop (_L, 1);

  return evt;
}

// TODO --------------------------------------------------------------------

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

// TODO --------------------------------------------------------------------

bool
Object::isOccurring ()
{
  return this->getLambda ()->getState () == Event::OCCURRING;
}

bool
Object::isPaused ()
{
  return this->getLambda ()->getState () == Event::PAUSED;
}

bool
Object::isSleeping ()
{
  return this->getLambda ()->getState () == Event::SLEEPING;
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
      this->getDocument ()->evalAction (action);
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

  set<Composition *> parents;
  this->getParents (&parents);
  auto it = parents.begin ();
  Composition *parent = (it == parents.end ()) ? NULL: *it;

  if (parent != NULL && instanceof (Context *, parent))
    cast (Context *, parent)->incAwakeChildren ();

  // schedule set currentFocus if the object have focusIndex
  if (!this->getProperty ("focusIndex").empty ())
    this->getDocument ()->getSettings ()->scheduleFocusUpdate ("");
}

void
Object::doStop ()
{
  set<Event *> events;
  this->getEvents (&events);

  _time = GINGA_TIME_NONE;
  for (auto evt: events)
    evt->setState (Event::SLEEPING);
  _delayed.clear ();

  set<Composition *> parents;
  this->getParents (&parents);
  auto it = parents.begin ();
  Composition *parent = (it == parents.end ()) ? NULL: *it;

  if (parent != nullptr && instanceof (Context *, parent))
    cast (Context *, parent)->decAwakeChildren ();
}

GINGA_NAMESPACE_END
