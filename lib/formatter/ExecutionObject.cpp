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


// Public.

ExecutionObject::ExecutionObject (GingaInternal *ginga,
                                  const string &id,
                                  Node *node)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;

  _scheduler = ginga->getScheduler ();
  g_assert_nonnull (_scheduler);

  g_assert_nonnull (node);
  _node = node;

  _id = id;
  _parent = nullptr;
  _player = nullptr;
  _time = GINGA_TIME_NONE;
}

ExecutionObject::~ExecutionObject ()
{
  this->reset ();
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
        case EventType::ATTRIBUTION: // fall through
        case EventType::PRESENTATION:
          return event;
        case EventType::SELECTION:
          {
            if (event->getParameter ("key") == key)
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
        case EventType::ATTRIBUTION: // fall through
        case EventType::PRESENTATION:
          return event;
        case EventType::SELECTION:
          {
            if (event->getParameter ("key") == key)
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
      event = new NclEvent (_ginga, type, this, anchor);
    }
  else
    {
      switch (type)
        {
        case EventType::PRESENTATION:
          event = new NclEvent (_ginga, type, this, anchor);
          break;
        case EventType::ATTRIBUTION:
          {
            Property *property = cast (Property *, anchor);
            g_assert_nonnull (property);
            event = new NclEvent (_ginga, type, this, property);
            break;
          }
        case EventType::SELECTION:
          event = new NclEvent (_ginga, type, this, anchor);
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
ExecutionObject::addEvent (NclEvent *event)
{
  if (_events.find (event) != _events.end ())
    return false;
  _events.insert (event);
  return true;
}

NclEvent *
ExecutionObject::getLambda (EventType type)
{
  g_assert (type != EventType::ATTRIBUTION);
  return this->getEventByAnchorId (type, _id + "@lambda", "");
}

EventState
ExecutionObject::getLambdaState ()
{
  NclEvent *evt = this->getLambda (EventType::PRESENTATION);
  g_assert_nonnull (evt);
  return evt->getState ();
}

bool
ExecutionObject::isFocused ()
{
  if (instanceof (ExecutionObjectContext *, this)
      || instanceof (ExecutionObjectSettings *, this)
      || instanceof (ExecutionObjectSwitch *, this))
    {
      return false;
    }
  if (this->getLambdaState () != EventState::OCCURRING)
    return false;
  g_assert_nonnull (_player);
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
  if (dur > 0)
    {
      TRACE ("%s.%s:='%s' (previous '%s') over %" GINGA_TIME_FORMAT,
             _id.c_str (), name.c_str (), value.c_str (),
             from.c_str (), GINGA_TIME_ARGS (dur));
      _player->schedulePropertyAnimation (name, from, value, dur);
    }
  else
    {
      TRACE ("%s.%s:='%s' (previous '%s')",
             _id.c_str (), name.c_str (), value.c_str (), from.c_str ());
      _player->setProperty (name, value);
    }

  if (instanceof (ExecutionObjectSettings *, this))
    cast (ExecutionObjectSettings *, this)
      ->setProperty (name, value, dur);
}

bool
ExecutionObject::getZ (int *z, int *zorder)
{
  if (this->getLambdaState () == EventState::SLEEPING)
    return false;               // nothing to do
  if (_player == nullptr)
    return false;               // nothing to do
  g_assert_nonnull (_player);
  _player->getZ (z, zorder);
  return true;
}

void
ExecutionObject::redraw (cairo_t *cr)
{
  if (this->getLambdaState () == EventState::SLEEPING)
    return;                     // nothing to do
  if (_player == nullptr)
    return;                     // nothing to do
  _player->redraw (cr);
}

void
ExecutionObject::sendKeyEvent (const string &key, bool press)
{
  list<NclEvent *> buf;

  if (!press)
    return;                     // nothing to do

  if (instanceof (ExecutionObjectSettings *, this)
      || instanceof (ExecutionObjectContext *, this)
      || instanceof (ExecutionObjectSwitch *, this))
    return;                     // nothing to do

  if (_player == nullptr)
    return;                     // nothing to do

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

      expected = evt->getParameter ("key");
      if (!((expected == "" && key == "ENTER" && _player->isFocused ())
            || (expected != "" && key == expected)))
        {
          continue;
        }

      anchor = cast (Area *, evt->getAnchor ());
      g_assert_nonnull (anchor);
      g_assert (instanceof (AreaLambda *, anchor));
      buf.push_back (evt);
    }

  // Run collected events.
  for (NclEvent *evt: buf)
    {
      evt->transition (EventStateTransition::START);
      evt->transition (EventStateTransition::STOP);
    }
}

void
ExecutionObject::sendTickEvent (unused (GingaTime total),
                                GingaTime diff,
                                unused (GingaTime frame))
{
  GingaTime dur;

  g_assert (this->getLambdaState () == EventState::OCCURRING);
  if (_player == nullptr)
    return;

  // Update object time.
  g_assert (GINGA_TIME_IS_VALID (_time));
  _time += diff;
  _player->incTime (diff);

  // Check EOS.
  if (_player->getEOS ()
      || (GINGA_TIME_IS_VALID (dur = _player->getDuration ())
          && _time > dur))
    {
      NclEvent *lambda = this->getLambda (EventType::PRESENTATION);
      g_assert_nonnull (lambda);
      TRACE ("eos %s@lambda at %" GINGA_TIME_FORMAT, _id.c_str (),
             GINGA_TIME_ARGS (_time));
      lambda->transition (EventStateTransition::STOP);
      return;
    }

  // Evaluate current delayed actions.
  for (auto it = _delayed.begin (); it != _delayed.end ();)
    {
      NclAction *act;
      GingaTime timeout;

      act = (*it).first;
      timeout = (*it).second;
      if (timeout <= _time)
        {
          NclEvent *evt = act->getEvent ();
          g_assert_nonnull (evt);
          evt->transition (act->getEventStateTransition ());
          if (this->getLambdaState () != EventState::OCCURRING)
            {
              this->resetDelayed ();
              break;
            }
          delete act;
          it = _delayed.erase (it);
        }
      else
        {
          ++it;
        }
    }

  // Update current delayed actions.
  _delayed.insert (_delayed.end (),
                   _delayed_new.begin (), _delayed_new.end ());
  _delayed_new.clear ();
}

bool
ExecutionObject::exec (NclEvent *evt,
                       unused (EventState from), unused (EventState to),
                       EventStateTransition transition)
{
  switch (evt->getType ())
    {
    // ---------------------------------------------------------------------
    // Presentation event.
    // ---------------------------------------------------------------------
    case EventType::PRESENTATION:
      switch (transition)
        {
        case EventStateTransition::START:
          if (instanceof (AreaLambda *, evt->getAnchor ()))
            {
              //
              // Start lambda.
              //
              Media *media;
              string src;
              string mime;

              g_assert_null (_player);
              media = cast (Media *, _node);
              g_assert_nonnull (media);

              src = media->getSrc ();
              mime = media->getMimeType ();
              _player = Player::createPlayer (_ginga, _id, src, mime);
              g_assert_nonnull (_player);

              // Initialize properties.
              for (auto anchor: *media->getAnchors ())
                {
                  Property *prop = cast (Property *, anchor);
                  if (prop != nullptr)
                    _player->setProperty (prop->getName (),
                                          prop->getValue ());
                }

              // Install delayed actions for time anchors.
              for (auto e: _events)
                {
                  GingaTime begin, end;
                  NclAction *act;

                  if (e->getType () != EventType::PRESENTATION)
                    continue;
                  if (e == this->getLambda (EventType::PRESENTATION))
                    continue;

                  begin = 0;
                  end = GINGA_TIME_NONE;
                  g_assert (e->getInterval (&begin, &end));
                  act = new NclAction (e, EventStateTransition::START);
                  _delayed_new.push_back (std::make_pair (act, begin));

                  act = new NclAction (e, EventStateTransition::STOP);
                  _delayed_new.push_back (std::make_pair (act, end));
                }

              _time = 0;
              TRACE ("start %s@lambda", _id.c_str ());
              _player->start ();
            }
          else
            {
              //
              // Start area (non-lambda).
              //
              if (this->getLambdaState () == EventState::OCCURRING)
                {
                  //
                  // Implicit.
                  //
                  GingaTime begin;
                  g_assert (evt->getInterval (&begin, nullptr));
                  TRACE ("start %s@%s (begin=%"
                         GINGA_TIME_FORMAT ") at %" GINGA_TIME_FORMAT,
                         _id.c_str (),
                         evt->getAnchor ()->getId ().c_str (),
                         GINGA_TIME_ARGS (begin),
                         GINGA_TIME_ARGS (_time));
                }
              else
                {
                  //
                  // Explicit.
                  //
                  g_assert_not_reached ();
                }
            }
          break;
        case EventStateTransition::PAUSE:
          g_assert_not_reached ();
          break;
        case EventStateTransition::RESUME:
          g_assert_not_reached ();
          break;
        case EventStateTransition::STOP:
          if (instanceof (AreaLambda *, evt->getAnchor ()))
            {
              //
              // Stop lambda.
              //
              g_assert_nonnull (_player);
              TRACE ("stop %s@lambda", _id.c_str ());
              this->reset ();
            }
          else
            {
              //
              // Stop area (non-lambda).
              //
              if (this->getLambdaState () == EventState::OCCURRING)
                {
                  //
                  // Implicit.
                  //
                  GingaTime end;
                  g_assert (evt->getInterval (nullptr, &end));
                  TRACE ("stop %s@%s (end=%"
                         GINGA_TIME_FORMAT ") at %" GINGA_TIME_FORMAT,
                         _id.c_str (),
                         evt->getAnchor ()->getId ().c_str (),
                         GINGA_TIME_ARGS (end),
                         GINGA_TIME_ARGS (_time));
                }
              else
                {
                  //
                  // Explicit.
                  //
                  g_assert_not_reached ();
                }
            }
          break;
        case EventStateTransition::ABORT:
          g_assert_not_reached ();
          break;
        default:
          g_assert_not_reached ();
        }
      break;

    // ---------------------------------------------------------------------
    // Attribution event.
    // ---------------------------------------------------------------------
    case EventType::ATTRIBUTION:
      if (this->getLambdaState () != EventState::OCCURRING)
        return false;           // nothing to do
      switch (transition)
        {
        case EventStateTransition::START:
          {
            Property *prop;
            string name;
            string value;
            GingaTime dur;

            prop = cast (Property *, evt->getAnchor ());
            g_assert_nonnull (prop);

            name = prop->getName ();
            value = evt->getParameter ("value");
            if (value[0] == '$')
              _scheduler->getObjectPropertyByRef (value, &value);

            string s;
            s = evt->getParameter ("duration");
            if (s[0] == '$')
              _scheduler->getObjectPropertyByRef (s, &s);
            dur = ginga_parse_time (s);
            this->setProperty (name, value, dur);

            // Schedule stop.
            NclAction *act = new NclAction
              (evt, EventStateTransition::STOP);
            _delayed_new.push_back (std::make_pair (act, _time + dur));

            TRACE ("start %s.%s:=%s (duration=%s)", _id.c_str (),
                   name.c_str (), value.c_str (), s.c_str ());
            break;
          }
        case EventStateTransition::STOP:
          {
            Property *prop = cast (Property *, evt->getAnchor ());
            g_assert_nonnull (prop);
            TRACE ("stop %s.%s:=...", _id.c_str (),
                   prop->getName ().c_str ());
            break;
          }
        case EventStateTransition::PAUSE: // impossible
        case EventStateTransition::RESUME:
        case EventStateTransition::ABORT:
        default:
          g_assert_not_reached ();
        }
      break;

    //----------------------------------------------------------------------
    // Selection event.
    // ---------------------------------------------------------------------
    case EventType::SELECTION:
      if (this->getLambdaState () != EventState::OCCURRING)
        return false;           // nothing to do
      switch (transition)
        {
        case EventStateTransition::START:
          TRACE ("start %s<%s>", _id.c_str (),
                 evt->getParameter ("key").c_str ());
          break;
        case EventStateTransition::STOP:
          TRACE ("stop %s<%s>", _id.c_str (),
                 evt->getParameter ("key").c_str ());
          break;
        case EventStateTransition::PAUSE: // impossible
        case EventStateTransition::RESUME:
        case EventStateTransition::ABORT:
        default:
          g_assert_not_reached ();
        }
      break;
    default:
      g_assert_not_reached ();
    }
  return true;
}


// Private.

void
ExecutionObject::reset ()
{
  if (_player != nullptr)
    {
      if (_player->getState () != Player::SLEEPING)
        _player->stop ();
      delete _player;
      _player = nullptr;
    }
  this->resetDelayed ();
  _time = GINGA_TIME_NONE;
}

void
ExecutionObject::resetDelayed ()
{
  for (auto item: _delayed)
    delete item.first;
  _delayed.clear ();

  for (auto item: _delayed_new)
    delete item.first;
  _delayed_new.clear ();
}

GINGA_FORMATTER_END
