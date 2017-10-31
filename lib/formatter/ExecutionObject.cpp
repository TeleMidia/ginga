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
#include "ExecutionObject.h"

#include "ExecutionObjectContext.h"
#include "ExecutionObjectSettings.h"
#include "ExecutionObjectSwitch.h"
#include "NclEvent.h"
#include "Scheduler.h"

#include "player/Player.h"
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN

ExecutionObject::ExecutionObject (GingaInternal *ginga,
                                  const string &id,
                                  Node *node)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;

  _scheduler = ginga->getScheduler ();
  g_assert_nonnull (_scheduler);

  _node = node;
  _mainEvent = nullptr;

  _id = id;
  _parent = nullptr;
  _player = nullptr;
  _time = GINGA_TIME_NONE;
  _destroying = false;
}

ExecutionObject::~ExecutionObject ()
{
  _destroying = true;
  this->stop ();
}

bool
ExecutionObject::isSleeping ()
{
  return  _mainEvent
    && _mainEvent->getState () == EventState::SLEEPING;
}

bool
ExecutionObject::isPaused ()
{
  return _mainEvent
    && _mainEvent->getState () == EventState::PAUSED;
}

bool
ExecutionObject::isOccurring ()
{
  return _mainEvent
    && _mainEvent->getState () == EventState::OCCURRING;
}

Node *
ExecutionObject::getNode ()
{
  return _node;
}

string
ExecutionObject::getId ()
{
  return _id;
}

const vector <string> *
ExecutionObject::getAliases ()
{
  return &_aliases;
}

bool
ExecutionObject::hasAlias (const string &alias)
{
  for (auto curr: _aliases)
    if (curr == alias)
      return true;
  return false;
}

bool
ExecutionObject::addAlias (const string &alias)
{
  for (auto old: _aliases)
    if (old == alias)
      return false;
  _aliases.push_back (alias);
  return true;
}

ExecutionObjectContext *
ExecutionObject::getParent ()
{
  return _parent;
}

void
ExecutionObject::initParent (ExecutionObjectContext *parent)
{
  g_assert_nonnull (parent);
  g_assert_null (_parent);
  _parent = parent;
  g_assert (parent->addChild (this));
}

const set<NclEvent *> *
ExecutionObject::getEvents ()
{
  return &_events;
}

NclEvent *
ExecutionObject::getEvent (EventType type, Anchor *anchor,
                           const string &key)
{
  g_assert_nonnull (anchor);
  for (auto event: _events)
    {
      if (event->getAnchor () != anchor || event->getType () != type)
        continue;
      switch (type)
        {
        case EventType::ATTRIBUTION: // fall-through
        case EventType::PRESENTATION:
          return event;
        case EventType::SELECTION:
          {
            string evtkey;
            g_assert (event->getKey (&evtkey));
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

NclEvent *
ExecutionObject::getEventByAnchorId (EventType type, const string &id,
                                     const string &key)
{
  for (auto event: _events)
    {
      Anchor *anchor;
      if (event->getType () != type)
        continue;
      anchor = event->getAnchor ();
      g_assert_nonnull (anchor);
      if (anchor->getId () != id)
        continue;
      switch (type)
        {
        case EventType::ATTRIBUTION: // fall-through
        case EventType::PRESENTATION:
          return event;
        case EventType::SELECTION:
          {
            string evtkey;
            g_assert (event->getKey (&evtkey));
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

NclEvent *
ExecutionObject::getLambda (EventType type)
{
  g_assert (type != EventType::ATTRIBUTION);
  return this->getEventByAnchorId (type, _id + "@lambda", "");
}

NclEvent *
ExecutionObject::obtainEvent (EventType type, Anchor *anchor,
                              const string &key)
{
  NclEvent *event;

  event = this->getEvent (type, anchor, key);
  if (event != nullptr)
    return event;

  g_assert_nonnull (anchor);
  g_assert_null (this->getEventByAnchorId (type, anchor->getId (), key));

  if (instanceof (ExecutionObjectSwitch *, this))
    {
      g_assert_not_reached ();
    }
  else if (instanceof (ExecutionObjectContext *, this))
    {
      g_assert (type == EventType::PRESENTATION);
      event = new NclEvent (_ginga, type, this, (Area *) anchor, "");
    }
  else
    {
      switch (type)
        {
        case EventType::PRESENTATION:
          event = new NclEvent (_ginga, type, this, (Area *) anchor, "");
          break;
        case EventType::ATTRIBUTION:
          {
            Property *property = cast (Property *, anchor);
            g_assert_nonnull (property);
            event = new NclEvent (_ginga, type, this, property, "");
            break;
          }
        case EventType::SELECTION:
          event = new NclEvent (_ginga, type, this, (Area *) anchor, key);
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
ExecutionObject::addEvent (NclEvent *event)
{
  if (_events.find (event) != _events.end ())
    return false;
  _events.insert (event);
  return true;
}

bool
ExecutionObject::prepare (NclEvent *event)
{
  _mainEvent = event;
  return true;
}

bool
ExecutionObject::start ()
{
  Media *media;
  string src;
  string mime;

  g_assert_nonnull (_mainEvent);

  if (this->isOccurring ())
    return true;              // nothing to do

  TRACE ("%s", _id.c_str ());

  if (instanceof (ExecutionObjectContext *, this))
    goto done;

  media = cast (Media *, _node);
  g_assert_nonnull (media);

  // Allocate player.
  src = media->getSrc ();
  mime = media->getMimeType ();
  _player = Player::createPlayer (_ginga, _id, src, mime);

  // Initialize player properties.
  for (auto anchor: *media->getAnchors ())
    {
      Property *prop = cast (Property *, anchor);
      if (prop != nullptr)
        _player->setProperty (prop->getName (), prop->getValue ());
    }

  _time = 0;
  _player->start ();

 done:
  // Start main event.
  if (_mainEvent->getType () == EventType::PRESENTATION)
    _mainEvent->transition (EventStateTransition::START);
  return true;
}

bool
ExecutionObject::pause ()
{
  if (!this->isOccurring ())
    return true;

  for (auto event: *(this->getEvents ()))
    event->transition (EventStateTransition::PAUSE);

  g_assert_nonnull (_player);
  _player->pause ();

  return true;
}

bool
ExecutionObject::resume ()
{
  if (!this->isPaused ())
    return true;

  for (auto event: *(this->getEvents ()))
    event->transition (EventStateTransition::RESUME);

  g_assert_nonnull (_player);
  _player->resume ();

  return true;
}

bool
ExecutionObject::stop ()
{
  if (this->isSleeping ())
    return false;               // nothing to do

  TRACE ("%s", _id.c_str ());

  // Stop and destroy player.
  if (_player != nullptr)
    {
      if (_player->getState () != Player::SLEEPING)
        _player->stop ();
      delete _player;
      _player = nullptr;
      _time = GINGA_TIME_NONE;
    }

  if (_destroying)
    return true;                // done

  // Stop main event.
  if (_mainEvent->getType () == EventType::PRESENTATION)
    _mainEvent->transition (EventStateTransition::STOP);

  return true;
}

bool G_GNUC_NORETURN
ExecutionObject::abort ()
{
  g_assert_not_reached ();
}

bool
ExecutionObject::isFocused ()
{
  if (instanceof (ExecutionObjectContext *, this)
      || instanceof (ExecutionObjectSettings *, this)
      || instanceof (ExecutionObjectSwitch *, this)
      || _player == nullptr)
    {
      return false;
    }
  return _player->isFocused ();
}

string
ExecutionObject::getProperty (const string &name)
{
  return (_player) ? _player->getProperty (name) : "";
}

void
ExecutionObject::setProperty (const string &name,
                              const string &value,
                              GingaTime dur)
{
  string from;

  if (_player == nullptr)
    return;                     // nothing to do

  from = this->getProperty (name);
  g_assert (GINGA_TIME_IS_VALID (dur));
  TRACE ("%s.%s:='%s' (previous '%s')",
         _id.c_str (), name.c_str (), value.c_str (), from.c_str ());

  if (dur > 0)
    {
      _player->schedulePropertyAnimation (name, from, value, dur);
    }
  else
    {
      _player->setProperty (name, value);
    }

  if (instanceof (ExecutionObjectSettings *, this))
    cast (ExecutionObjectSettings *, this)
      ->setProperty (name, value, dur);
}

bool
ExecutionObject::getZ (int *z, int *zorder)
{
  if (_player == nullptr || _player->getState () == Player::SLEEPING) // fixme
    return false;
  g_assert_nonnull (_player);
  _player->getZ (z, zorder);
  return true;
}

void
ExecutionObject::redraw (cairo_t *cr)
{
  if (this->isSleeping ())
    return;                     // nothing to do
  if (_player == nullptr || _player->getState () == Player::SLEEPING) // fixme
    return;                     // nothing to do
  _player->redraw (cr);
}

void
ExecutionObject::sendKeyEvent (const string &key, bool press)
{
  list<NclEvent *> buf;

  if (!press
      || instanceof (ExecutionObjectSettings *, this)
      || _player == nullptr
      || !this->isOccurring ())
    {
      return;                     // nothing to do
    }

  g_assert (_mainEvent->getType () == EventType::PRESENTATION);
  if (xstrhasprefix (key, "CURSOR_") && _player->isFocused ())
    {
      string next;
      if ((key == "CURSOR_UP"
           && (next = _player->getProperty ("moveUp")) != "")
          || ((key == "CURSOR_DOWN"
               && (next = _player->getProperty ("moveDown")) != ""))
          || ((key == "CURSOR_LEFT"
               && (next = _player->getProperty ("moveLeft")) != ""))
          || ((key == "CURSOR_RIGHT"
               && (next = _player->getProperty ("moveRight")) != "")))
        {
          ExecutionObjectSettings *settings;
          settings = _scheduler->getSettings ();
          g_assert_nonnull (settings);
          settings->scheduleFocusUpdate (next);
        }
    }

  // Pass key to player.
  if (_player->isFocused ())
    _player->sendKeyEvent (key, press);

  // Collect the events to be triggered.
  for (auto evt: _events)
    {
      Area *anchor;
      string expected;

      if (evt->getType () != EventType::SELECTION)
        continue;

      g_assert (evt->getKey (&expected));
      if (!((expected == "" && key == "ENTER" && _player->isFocused ())
            || (expected != "" && key == expected)))
        {
          continue;
        }
      anchor = cast (Area *, evt->getAnchor ());
      g_assert_nonnull (anchor);

      if (instanceof (AreaLambda *, anchor))
        {
          buf.push_back (evt);
        }
      else if (instanceof (Area *, anchor))
        {
          ERROR_NOT_IMPLEMENTED
            ("selection of temporal anchors is no supported");
        }
      else
        {
          ERROR_NOT_IMPLEMENTED
            ("selection of property anchors is not supported");
        }
    }

  // Run collected events.
  for (NclEvent *evt: buf)
    {
      NclAction *fakeAct = new NclAction (evt, EventStateTransition::START);
      _scheduler->scheduleAction (fakeAct);
    }

  if (buf.size () == 0)
    return;

  TRACE ("%s selected via '%s'",
         _id.c_str (), key.c_str ());
}

void
ExecutionObject::sendTickEvent (unused (GingaTime total),
                                GingaTime diff,
                                unused (GingaTime frame))
{
  GingaTime dur;

  if (_player == nullptr || !this->isOccurring ())
    return;                     // nothing to do

  if (_player->getEOS ())
    {
      this->stop ();            // done
      return;
    }

  g_assert (GINGA_TIME_IS_VALID (_time));
  _time += diff;
  _player->incTime (diff);

  if (GINGA_TIME_IS_VALID (dur = _player->getDuration ()) && _time > dur)
    {
      this->stop ();            // done
      return;
    }

  g_assert (this->isOccurring ());
  g_assert (_mainEvent->getType () == EventType::PRESENTATION);

  for (auto evt: _events)
    {
      if (evt->getType () != EventType::PRESENTATION)
        continue;
      if (this->getLambda (EventType::PRESENTATION) == evt)
        continue;

      // TRACE ("[%s %" GINGA_TIME_FORMAT "]"
      //        " %s begin=%" GINGA_TIME_FORMAT " end=%" GINGA_TIME_FORMAT,
      //        this->getId ().c_str (),
      //        GINGA_TIME_ARGS (_time),
      //        evt->getId ().c_str (),
      //        GINGA_TIME_ARGS (evt->getBegin ()),
      //        GINGA_TIME_ARGS (evt->getEnd ()));

      GingaTime begin, end;
      evt->getInterval (&begin, &end);

      if (evt->getState () == EventState::SLEEPING && begin <= _time)
        {
          TRACE ("%s.%s timed-out at %" GINGA_TIME_FORMAT,
                 _id.c_str(), evt->getAnchor ()->getId ().c_str (),
                 GINGA_TIME_ARGS (time));
          evt->transition (EventStateTransition::START);
        }
      else if (evt->getState () == EventState::OCCURRING && end <= _time)
        {
          TRACE ("%s.%s timed-out at %" GINGA_TIME_FORMAT,
                 _id.c_str(), evt->getAnchor ()->getId ().c_str (),
                 GINGA_TIME_ARGS (time));
          evt->transition (EventStateTransition::STOP);
        }
    }
}

GINGA_FORMATTER_END
