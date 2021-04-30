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

#include "Composition.h"
#include "Context.h"
#include "Document.h"
#include "Event.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Switch.h"

GINGA_NAMESPACE_BEGIN

// Public.

Object::Object (const string &id) : _id (id)
{
  _doc = nullptr;
  _parent = nullptr;
  _time = GINGA_TIME_NONE;
  _isPreparing = false;

  this->addPresentationEvent ("@lambda", 0, GINGA_TIME_NONE);
  _lambda = this->getPresentationEvent ("@lambda");
  g_assert_nonnull (_lambda);
}

Object::~Object ()
{
  for (auto evt : _events)
    delete evt;
}

string
Object::getId ()
{
  return _id;
}

Document *
Object::getDocument ()
{
  return _doc;
}

void
Object::initDocument (Document *doc)
{
  g_assert_nonnull (doc);
  g_assert_null (_doc);
  _doc = doc;
}

Composition *
Object::getParent ()
{
  return _parent;
}

void
Object::initParent (Composition *parent)
{
  g_assert_nonnull (parent);
  g_assert_null (_parent);
  _parent = parent;
}

string
Object::toString ()
{
  string str;

  str = xstrbuild ("%s (%p):\n", this->getObjectTypeAsString ().c_str (),
                   this);

  if (_parent != nullptr)
    {
      str += xstrbuild ("\
  parent: %p (%s, id: %s)\n",
                        _parent, _parent->getObjectTypeAsString ().c_str (),
                        _parent->getId ().c_str ());
    }
  str += "  id: " + _id + "\n";
  auto it = _aliases.begin ();
  if (it != _aliases.end ())
    {
      str += "  aliases: " + (*it).first;
      while (++it != _aliases.end ())
        str += ", " + (*it).first + "(at Composition "
               + xstrbuild ("%p", (*it).second) + ")";
      str += "\n";
    }

  str += "  time: ";
  if (GINGA_TIME_IS_VALID (_time))
    str += xstrbuild ("%" GINGA_TIME_FORMAT, GINGA_TIME_ARGS (_time));
  else
    str += "(none)";
  str += "\n";

  list<string> pres;
  list<string> attr;
  list<string> sel;
  list<string> prep;
  for (auto evt : _events)
    switch (evt->getType ())
      {
      case Event::PRESENTATION:
        pres.push_back (evt->getId () + " ("
                        + Event::getEventStateAsString (evt->getState ())
                        + ')');
        break;
      case Event::ATTRIBUTION:
        attr.push_back (evt->getId () + " ("
                        + Event::getEventStateAsString (evt->getState ())
                        + ')');
        break;
      case Event::SELECTION:
        sel.push_back (evt->getId () + " ("
                       + Event::getEventStateAsString (evt->getState ())
                       + ')');
        break;
      case Event::PREPARATION:
        prep.push_back (evt->getId () + " ("
                        + Event::getEventStateAsString (evt->getState ())
                        + ')');
        break;
      default:
        g_assert_not_reached ();
      }

  list<pair<string, list<string> *> > evts = {
    { "evts pres.", &pres },
    { "evts attr.", &attr },
    { "evts sel.", &sel },
    { "evts prep.", &prep },
  };

  for (auto it_evts : evts)
    {
      auto it_evt = it_evts.second->begin ();
      if (it_evt == it_evts.second->end ())
        continue;
      str += "  " + it_evts.first + ": " + *it_evt;
      while (++it_evt != it_evts.second->end ())
        str += ", " + *it_evt;
      str += "\n";
    }

  if (_properties.size () > 0)
    {
      str += "  properties:\n";
      for (auto it : _properties)
        str += "    " + it.first + "='" + it.second + "'\n";
    }

  return str;
}

const list<pair<string, Composition *> > *
Object::getAliases ()
{
  return &_aliases;
}

bool
Object::hasAlias (const string &alias)
{
  for (auto curr : _aliases)
    if (curr.first == alias)
      return true;
  return false;
}

void
Object::addAlias (const string &alias, Composition *parent)
{
  auto alias_pair = make_pair (alias, parent);
  // _aliases.push_back (alias_pair);
  tryinsert (alias_pair, _aliases, push_back);
}

const set<Event *> *
Object::getEvents ()
{
  return &_events;
}

Event *
Object::getEvent (Event::Type type, const string &id)
{
  for (auto evt : _events)
    if (evt->getType () == type && evt->getId () == id)
      return evt;
  return nullptr;
}

Event *
Object::getAttributionEvent (const string &propName)
{
  return this->getEvent (Event::ATTRIBUTION, propName);
}

void
Object::addAttributionEvent (const string &propName)
{
  Event *evt;

  if (this->getAttributionEvent (propName))
    return;

  evt = new Event (Event::ATTRIBUTION, this, propName);
  _events.insert (evt);
}

Event *
Object::getPresentationEvent (const string &id)
{
  return this->getEvent (Event::PRESENTATION, id);
}

Event *
Object::getPresentationEventByLabel (const string &label)
{
  for (Event *evt : _events)
    if (evt->getType () == Event::PRESENTATION && evt->getLabel () == label)
      return evt;
  return nullptr;
}

void
Object::addPresentationEvent (const string &id, Time begin, Time end)
{
  Event *evt;

  if (this->getPresentationEvent (id))
    return;

  evt = new Event (Event::PRESENTATION, this, id);
  evt->setInterval (begin, end);
  _events.insert (evt);
}

void
Object::addPresentationEvent (const string &id, const string &label)
{
  Event *evt;
  if (this->getPresentationEvent (id))
    return;

  evt = new Event (Event::PRESENTATION, this, id);
  evt->setLabel (label);
  _events.insert (evt);
}

Event *
Object::getSelectionEvent (const string &key)
{
  return this->getEvent (Event::SELECTION, key);
}

void
Object::addSelectionEvent (const string &key)
{
  Event *evt;

  if (this->getSelectionEvent (key))
    return;

  evt = new Event (Event::SELECTION, this, key);
  _events.insert (evt);
}

Event *
Object::getPreparationEvent (const string &key)
{
  return this->getEvent (Event::PREPARATION, key);
}

void
Object::addPreparationEvent (const string &id)
{
  Event *evt;
  if (this->getPreparationEvent (id))
    return;

  evt = new Event (Event::PREPARATION, this, id);
  _events.insert (evt);
}

void
Object::addPreparationEvent (const string &id, Time begin, Time end)
{
  Event *evt;

  if (this->getPreparationEvent (id))
    return;

  evt = new Event (Event::PREPARATION, this, id);
  evt->setInterval (begin, end);
  _events.insert (evt);
}

Event *
Object::getLookAtEvent (const string &id)
{
  return this->getEvent (Event::LOOKAT, id);
}

void
Object::addLookAtEvent (const string &id)
{
  Event *evt;
  if (this->getLookAtEvent (id))
    return;

  evt = new Event (Event::LOOKAT, this, id);
  _events.insert (evt);
}

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
Object::isPreparing ()
{
  return _isPreparing;
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

string
Object::getProperty (const string &name)
{
  auto it = _properties.find (name);
  if (it == _properties.end ())
    return "";
  return it->second;
}

void
Object::setProperty (const string &name, const string &value, Time dur)
{
  g_assert (GINGA_TIME_IS_VALID (dur));
  _properties[name] = value;
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
    return; // nothing to do

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

// Private.

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
    evt->reset ();
  _delayed.clear ();
  if (_parent != nullptr && instanceof (Context *, _parent))
    cast (Context *, _parent)->decAwakeChildren ();
}

GINGA_NAMESPACE_END
