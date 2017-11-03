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
#include "FormatterObject.h"

#include "FormatterContext.h"
#include "FormatterEvent.h"
#include "FormatterMedia.h"
#include "FormatterMediaSettings.h"
#include "FormatterScheduler.h"
#include "FormatterSwitch.h"

GINGA_NAMESPACE_BEGIN


// Public.

FormatterObject::FormatterObject (Formatter *ginga,
                                  const string &id,
                                  NclNode *node)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;

  _scheduler = ginga->getScheduler ();
  g_assert_nonnull (_scheduler);

  g_assert_nonnull (node);
  _node = node;

  _id = id;
  _parent = nullptr;
  _time = GINGA_TIME_NONE;
}

FormatterObject::~FormatterObject ()
{
  this->doStop ();
}

NclNode *
FormatterObject::getNode ()
{
  return _node;
}

string
FormatterObject::getId ()
{
  return _id;
}

const vector <string> *
FormatterObject::getAliases ()
{
  return &_aliases;
}

bool
FormatterObject::hasAlias (const string &alias)
{
  for (auto curr: _aliases)
    if (curr == alias)
      return true;
  return false;
}

bool
FormatterObject::addAlias (const string &alias)
{
  for (auto old: _aliases)
    if (old == alias)
      return false;
  _aliases.push_back (alias);
  return true;
}

FormatterContext *
FormatterObject::getParent ()
{
  return _parent;
}

void
FormatterObject::initParent (FormatterContext *parent)
{
  g_assert_nonnull (parent);
  g_assert_null (_parent);
  _parent = parent;
  g_assert (parent->addChild (this));
}

const set<FormatterEvent *> *
FormatterObject::getEvents ()
{
  return &_events;
}

FormatterEvent *
FormatterObject::getEvent (NclEventType type, NclAnchor *anchor,
                           const string &key)
{
  g_assert_nonnull (anchor);
  for (auto event: _events)
    {
      if (event->getAnchor () != anchor || event->getType () != type)
        continue;
      switch (type)
        {
        case NclEventType::ATTRIBUTION: // fall through
        case NclEventType::PRESENTATION:
          return event;
        case NclEventType::SELECTION:
          {
            string evtkey = "";
            event->getParameter ("key", &evtkey);
            if (evtkey == key)
              return event;
            break;
          }
        default:
          g_assert_not_reached ();
        }
    }
  return nullptr;
}

FormatterEvent *
FormatterObject::getEventByAnchorId (NclEventType type, const string &id,
                                     const string &key)
{
  for (auto event: _events)
    {
      NclAnchor *anchor;
      if (event->getType () != type)
        continue;
      anchor = event->getAnchor ();
      g_assert_nonnull (anchor);
      if (anchor->getId () != id)
        continue;
      switch (type)
        {
        case NclEventType::ATTRIBUTION: // fall through
        case NclEventType::PRESENTATION:
          return event;
        case NclEventType::SELECTION:
          {
            string evtkey = "";
            event->getParameter ("key", &evtkey);
            if (evtkey == key)
              return event;
            break;
          }
        default:
          g_assert_not_reached ();
        }
    }
  return nullptr;
}

FormatterEvent *
FormatterObject::obtainEvent (NclEventType type, NclAnchor *anchor,
                              const string &key)
{
  FormatterEvent *event;

  event = this->getEvent (type, anchor, key);
  if (event != nullptr)
    return event;

  g_assert_nonnull (anchor);
  g_assert_null (this->getEventByAnchorId (type, anchor->getId (), key));

  if (instanceof (FormatterSwitch *, this))
    {
      g_assert (type == NclEventType::PRESENTATION);
      event = new FormatterEvent (_ginga, type, this, anchor);
    }
  else if (instanceof (FormatterContext *, this))
    {
      g_assert (type == NclEventType::PRESENTATION);
      event = new FormatterEvent (_ginga, type, this, anchor);
    }
  else
    {
      switch (type)
        {
        case NclEventType::PRESENTATION:
          event = new FormatterEvent (_ginga, type, this, anchor);
          break;
        case NclEventType::ATTRIBUTION:
          {
            NclProperty *property = cast (NclProperty *, anchor);
            g_assert_nonnull (property);
            event = new FormatterEvent (_ginga, type, this, property);
            break;
          }
        case NclEventType::SELECTION:
          event = new FormatterEvent (_ginga, type, this, anchor);
          event->setParameter ("key", key);
          break;
        default:
          g_assert_not_reached ();
        }
    }

  g_assert_nonnull (event);
  g_assert (this->addEvent (event));
  return event;
}

bool
FormatterObject::addEvent (FormatterEvent *event)
{
  if (_events.find (event) != _events.end ())
    return false;
  _events.insert (event);
  return true;
}

FormatterEvent *
FormatterObject::obtainLambda ()
{
  FormatterEvent *lambda;
  g_assert_nonnull (_node);
  lambda = this->obtainEvent (NclEventType::PRESENTATION,
                              _node->getLambda (), "");
  g_assert_nonnull (lambda);
  return lambda;
}

bool
FormatterObject::isOccurring ()
{
  return this->obtainLambda ()->getState () == NclEventState::OCCURRING;
}

bool
FormatterObject::isPaused ()
{
  return this->obtainLambda ()->getState () == NclEventState::PAUSED;
}

bool
FormatterObject::isSleeping ()
{
  return this->obtainLambda ()->getState () == NclEventState::SLEEPING;
}

void
FormatterObject::scheduleAction (FormatterAction *action, GingaTime delay)
{
  g_assert_nonnull (action);
  _delayed_new.push_back (std::make_pair (action, _time + delay));
}

void
FormatterObject::sendKeyEvent (unused (const string &key),
                               unused (bool press))
{
}

void
FormatterObject::sendTickEvent (unused (GingaTime total),
                                GingaTime diff,
                                unused (GingaTime frame))
{
  g_assert (this->isOccurring ());

  // Update time.
  g_assert (GINGA_TIME_IS_VALID (_time));
  _time += diff;

  // Evaluate delayed actions.
  for (auto it = _delayed.begin (); it != _delayed.end ();)
    {
      FormatterAction *act;
      GingaTime timeout;

      act = (*it).first;
      timeout = (*it).second;
      if (timeout <= _time)
        {
          FormatterEvent *evt = act->getEvent ();
          g_assert_nonnull (evt);
          evt->transition (act->getEventStateTransition ());
          if (!this->isOccurring ())
            return;
          delete act;
          it = _delayed.erase (it);
        }
      else
        {
          ++it;
        }
    }

  // Update current delayed actions.  (Move this up?)
  _delayed.insert
    (_delayed.end (), _delayed_new.begin (), _delayed_new.end ());
  _delayed_new.clear ();
}


// Private.

void
FormatterObject::doStart ()
{
  _time = 0;
}

void
FormatterObject::doStop ()
{
  _time = GINGA_TIME_NONE;
  for (auto item: _delayed)
    delete item.first;
  _delayed.clear ();

  for (auto item: _delayed_new)
    delete item.first;
  _delayed_new.clear ();
}

GINGA_NAMESPACE_END
