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
#include "FormatterMediaSettings.h"
#include "FormatterSwitch.h"
#include "FormatterEvent.h"
#include "FormatterScheduler.h"
#include "player/Player.h"

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
  _player = nullptr;
  _time = GINGA_TIME_NONE;
}

FormatterObject::~FormatterObject ()
{
  this->reset ();
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

bool
FormatterObject::isFocused ()
{
  if (instanceof (FormatterContext *, this)
      || instanceof (FormatterMediaSettings *, this)
      || instanceof (FormatterSwitch *, this))
    {
      return false;
    }
  if (!this->isOccurring ())
    return false;
  g_assert_nonnull (_player);
  return _player->isFocused ();
}

string
FormatterObject::getProperty (const string &name)
{
  return (_player) ? _player->getProperty (name) : "";
}

void
FormatterObject::setProperty (const string &name,
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

  if (instanceof (FormatterMediaSettings *, this))
    cast (FormatterMediaSettings *, this)
      ->setProperty (name, value, dur);
}

bool
FormatterObject::getZ (int *z, int *zorder)
{
  if (this->isSleeping ())
    return false;               // nothing to do
  if (_player == nullptr)
    return false;               // nothing to do
  g_assert_nonnull (_player);
  _player->getZ (z, zorder);
  return true;
}

void
FormatterObject::redraw (cairo_t *cr)
{
  if (this->isSleeping ())
    return;                     // nothing to do
  if (_player == nullptr)
    return;                     // nothing to do
  _player->redraw (cr);
}

void
FormatterObject::sendKeyEvent (const string &key, bool press)
{
  list<FormatterEvent *> buf;

  if (!press)
    return;                     // nothing to do

  if (instanceof (FormatterMediaSettings *, this)
      || instanceof (FormatterContext *, this)
      || instanceof (FormatterSwitch *, this))
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
          FormatterMediaSettings *settings;
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
      NclArea *anchor;
      string expected;

      if (evt->getType () != NclEventType::SELECTION)
        continue;

      expected = "";
      evt->getParameter ("key", &expected);
      if (!((expected == "" && key == "ENTER" && _player->isFocused ())
            || (expected != "" && key == expected)))
        {
          continue;
        }

      anchor = cast (NclArea *, evt->getAnchor ());
      g_assert_nonnull (anchor);
      g_assert (instanceof (NclAreaLambda *, anchor));
      buf.push_back (evt);
    }

  // Run collected events.
  for (FormatterEvent *evt: buf)
    {
      evt->transition (NclEventStateTransition::START);
      evt->transition (NclEventStateTransition::STOP);
    }
}

void
FormatterObject::sendTickEvent (unused (GingaTime total),
                                GingaTime diff,
                                unused (GingaTime frame))
{
  GingaTime dur;

  g_assert (this->isOccurring ());
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
      FormatterEvent *lambda = this->obtainLambda ();
      g_assert_nonnull (lambda);
      TRACE ("eos %s@lambda at %" GINGA_TIME_FORMAT, _id.c_str (),
             GINGA_TIME_ARGS (_time));
      lambda->transition (NclEventStateTransition::STOP);
      return;
    }

  // Evaluate current delayed actions.
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
FormatterObject::exec (FormatterEvent *evt,
                       unused (NclEventState from),
                       unused (NclEventState to),
                       NclEventStateTransition transition)
{
  switch (evt->getType ())
    {
    // ---------------------------------------------------------------------
    // Presentation event.
    // ---------------------------------------------------------------------
    case NclEventType::PRESENTATION:
      switch (transition)
        {
        case NclEventStateTransition::START:
          if (instanceof (NclAreaLambda *, evt->getAnchor ()))
            {
              //
              // Start lambda.
              //
              NclMedia *media;
              string src;
              string mime;

              g_assert_null (_player);
              media = cast (NclMedia *, _node);
              g_assert_nonnull (media);

              src = media->getSrc ();
              mime = media->getMimeType ();
              _player = Player::createPlayer (_ginga, _id, src, mime);
              g_assert_nonnull (_player);

              // Initialize properties.
              for (auto anchor: *media->getAnchors ())
                {
                  NclProperty *prop = cast (NclProperty *, anchor);
                  if (prop != nullptr)
                    _player->setProperty (prop->getName (),
                                          prop->getValue ());
                }

              // Install delayed actions for time anchors.
              for (auto e: _events)
                {
                  NclAnchor *anchor;
                  NclArea *area;
                  GingaTime begin, end;
                  FormatterAction *act;

                  if (e->getType () != NclEventType::PRESENTATION)
                    continue;
                  if (e == this->obtainLambda ())
                    continue;

                  anchor = e->getAnchor ();
                  g_assert_nonnull (anchor);

                  area = cast (NclArea *, anchor);
                  g_assert_nonnull (area);

                  begin = area->getBegin ();
                  end = area->getEnd ();

                  act = new FormatterAction
                    (e, NclEventStateTransition::START);
                  _delayed_new.push_back (std::make_pair (act, begin));
                  act = new FormatterAction
                    (e, NclEventStateTransition::STOP);
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
              if (this->isOccurring ())
                {
                  //
                  // Implicit.
                  //
                  NclAnchor *anchor;
                  NclArea *area;

                  anchor = evt->getAnchor ();
                  g_assert_nonnull (anchor);

                  area = cast (NclArea *, anchor);
                  g_assert_nonnull (area);

                  TRACE ("start %s@%s (begin=%"
                         GINGA_TIME_FORMAT ") at %" GINGA_TIME_FORMAT,
                         _id.c_str (),
                         evt->getAnchor ()->getId ().c_str (),
                         GINGA_TIME_ARGS (area->getBegin ()),
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
        case NclEventStateTransition::PAUSE:
          g_assert_not_reached ();
          break;
        case NclEventStateTransition::RESUME:
          g_assert_not_reached ();
          break;
        case NclEventStateTransition::STOP:
          if (instanceof (NclAreaLambda *, evt->getAnchor ()))
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
              if (this->isOccurring ())
                {
                  //
                  // Implicit.
                  //
                  NclAnchor *anchor;
                  NclArea *area;

                  anchor = evt->getAnchor ();
                  g_assert_nonnull (anchor);

                  area = cast (NclArea *, anchor);
                  g_assert_nonnull (area);

                  TRACE ("stop %s@%s (end=%"
                         GINGA_TIME_FORMAT ") at %" GINGA_TIME_FORMAT,
                         _id.c_str (),
                         evt->getAnchor ()->getId ().c_str (),
                         GINGA_TIME_ARGS (area->getEnd ()),
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
        case NclEventStateTransition::ABORT:
          g_assert_not_reached ();
          break;
        default:
          g_assert_not_reached ();
        }
      break;

    // ---------------------------------------------------------------------
    // Attribution event.
    // ---------------------------------------------------------------------
    case NclEventType::ATTRIBUTION:
      if (!this->isOccurring ())
        return false;           // nothing to do
      switch (transition)
        {
        case NclEventStateTransition::START:
          {
            NclProperty *prop;
            string name;
            string value;
            GingaTime dur;

            prop = cast (NclProperty *, evt->getAnchor ());
            g_assert_nonnull (prop);

            name = prop->getName ();
            value = "";
            evt->getParameter ("value", &value);
            if (value[0] == '$')
              _scheduler->getObjectPropertyByRef (value, &value);

            string s;
            if (evt->getParameter ("duration", &s))
              {
                if (s[0] == '$')
                  _scheduler->getObjectPropertyByRef (s, &s);
                dur = ginga_parse_time (s);
              }
            else
              {
                dur = 0;
              }
            this->setProperty (name, value, dur);

            // Schedule stop.
            FormatterAction *act = new FormatterAction
              (evt, NclEventStateTransition::STOP);
            _delayed_new.push_back (std::make_pair (act, _time + dur));

            TRACE ("start %s.%s:=%s (duration=%s)", _id.c_str (),
                   name.c_str (), value.c_str (), s.c_str ());
            break;
          }
        case NclEventStateTransition::STOP:
          {
            NclProperty *prop = cast (NclProperty *, evt->getAnchor ());
            g_assert_nonnull (prop);
            TRACE ("stop %s.%s:=...", _id.c_str (),
                   prop->getName ().c_str ());
            break;
          }
        case NclEventStateTransition::PAUSE: // impossible
        case NclEventStateTransition::RESUME:
        case NclEventStateTransition::ABORT:
        default:
          g_assert_not_reached ();
        }
      break;

    //----------------------------------------------------------------------
    // Selection event.
    // ---------------------------------------------------------------------
    case NclEventType::SELECTION:
      if (!this->isOccurring ())
        return false;           // nothing to do
      switch (transition)
        {
        case NclEventStateTransition::START:
          {
            string key = "";
            evt->getParameter ("key", &key);
            TRACE ("start %s<%s>", _id.c_str (), key.c_str ());
            break;
          }
        case NclEventStateTransition::STOP:
          {
            string key = "";
            evt->getParameter ("key", &key);
            TRACE ("stop %s<%s>", _id.c_str (), key.c_str ());
            break;
          }
        case NclEventStateTransition::PAUSE: // impossible
        case NclEventStateTransition::RESUME:
        case NclEventStateTransition::ABORT:
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
FormatterObject::reset ()
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
FormatterObject::resetDelayed ()
{
  for (auto item: _delayed)
    delete item.first;
  _delayed.clear ();

  for (auto item: _delayed_new)
    delete item.first;
  _delayed_new.clear ();
}

GINGA_NAMESPACE_END
