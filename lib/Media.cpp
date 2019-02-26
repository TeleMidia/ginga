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

#include "Context.h"
#include "MediaSettings.h"
#include "Switch.h"
#include "Event.h"
#include "Player.h"

GINGA_NAMESPACE_BEGIN

// Public.

Media::Media (const string &id) : Object (id)
{
  _player = nullptr;
  _currentPreparationEvent = nullptr;
}

Media::~Media ()
{
  this->doStop ();
}

// Public: Object.

string
Media::getObjectTypeAsString ()
{
  return "Media";
}

string
Media::toString ()
{
  string str;
  string type;
  string uri;

  str = Object::toString ();
  str += xstrbuild ("  player: %p\n", _player);

  return str;
}

void
Media::setProperty (const string &name, const string &value, Time dur)
{
  string from = this->getProperty (name);
  Object::setProperty (name, value, dur);

  if (_player == nullptr)
    return;

  g_assert (GINGA_TIME_IS_VALID (dur));
  if (dur > 0)
    _player->schedulePropertyAnimation (name, from, value, dur);
  else
    _player->setProperty (name, value);
}

void
Media::sendKey (const string &key, bool press)
{
  list<Event *> buf;

  if (unlikely (this->isSleeping ()))
    return; // nothing to do

  if (_player == nullptr)
    return; // nothing to do

  if (press && xstrhasprefix (key, "CURSOR_") && _player->isFocused ())
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
          MediaSettings *settings;
          settings = _doc->getSettings ();
          g_assert_nonnull (settings);
          settings->scheduleFocusUpdate (next);
        }
    }

  // Pass key to player.
  if (_player->isFocused ())
    _player->sendKeyEvent (key, press);

  // Collect the events to be triggered.
  for (auto evt : _events)
    {
      if (evt->getType () != Event::SELECTION)
        continue;

      string expected = "";
      evt->getParameter ("key", &expected);

      if (expected[0] == '$')
        expected = ""; // A param could not be resolved.  Should we generate
                       // an error?

      if (!((expected == "" && key == "ENTER" && _player->isFocused ())
            || (expected != "" && key == expected)))
        {
          continue;
        }

      buf.push_back (evt);
    }

  // Run collected events.
  for (Event *evt : buf)
    _doc->evalAction (evt, press ? Event::START : Event::STOP);
}

void
Media::sendTick (Time total, Time diff, Time frame)
{
  Time dur;

  if (this->isPreparing ())
    {
      g_assert_nonnull (_player);

      if (_player->getPrepared () )
      {
        TRACE ("_player->getPrepared true");
        Event *currentPreparation = this->getCurrentPreparationEvent ();
        g_assert_nonnull (currentPreparation);
        _doc->evalAction (currentPreparation, Event::STOP);
        return;
      }
    }

  // Update object time.
  Object::sendTick (total, diff, frame);

  if (_player == nullptr)
    return; // nothing to do.

  // Update player time.
  g_assert_nonnull (_player);
  _player->incTime (diff);

  // Check EOS.
  if (_player->getEOS ()
      || (GINGA_TIME_IS_VALID (dur = _player->getDuration ())
          && _time > dur))
    {
      Event *lambda = this->getLambda ();
      g_assert_nonnull (lambda);
      TRACE ("eos %s at %" GINGA_TIME_FORMAT, lambda->getFullId ().c_str (),
             GINGA_TIME_ARGS (_time));
      _doc->evalAction (lambda, Event::STOP);
      return;
    }
}

bool
Media::beforeTransition (Event *evt, Event::Transition transition)
{
  switch (evt->getType ())
    {
    case Event::PRESENTATION:
      switch (transition)
        {
        case Event::START:
          {
            if (instanceof (Context *, _parent) && _parent->isSleeping ())
              {
                _parent->getLambda ()->setParameter ("fromport", "true");
                _parent->getLambda ()->transition (Event::START);
              }
            // Create underlying player.
            if (evt->getState () == Event::SLEEPING)
              {
                if (evt->isLambda ())
                  { // Lambda
                    Formatter *fmt;

                    createPlayer ();

                    if (unlikely (_player == nullptr))
                      return false; // fail

                    _player->start (); // Just lambda events reaches this!
                  }
                else
                  { // Anchor
                    Event *lambda = getPresentationEvent ("@lambda");
                    if (lambda->getState () != Event::SLEEPING)
                      break;

                    lambda->transition (Event::START);

                    Time begin, end, dur;
                    evt->getInterval (&begin, &end);

                    // Begin
                    if (begin == GINGA_TIME_NONE)
                      begin = 0;

                    string time_seek = xstrbuild ("%" G_GUINT64_FORMAT,
                                                  begin / GINGA_SECOND);
                    TRACE ("time_seek %ss", time_seek.c_str ());
                    _player->setProperty ("time", time_seek);

                    // End
                    if (end != GINGA_TIME_NONE)
                      {
                        dur = end - begin;
                        string time_end = xstrbuild ("%" G_GUINT64_FORMAT,
                                                     dur / GINGA_SECOND);
                        TRACE ("time_end in %ss", time_end.c_str ());
                        _player->setProperty ("duration", time_end);
                        this->addDelayedAction (evt, Event::STOP, "", end);
                      }

                    TRACE ("start %s (begin=%" GINGA_TIME_FORMAT
                           " and end=%" GINGA_TIME_FORMAT
                           ") at %" GINGA_TIME_FORMAT,
                           evt->getFullId ().c_str (),
                           GINGA_TIME_ARGS (begin), GINGA_TIME_ARGS (end),
                           GINGA_TIME_ARGS (_time));

                    // remove events of anchors that happens before or after
                    // anchor started.
                    for (auto it = _delayed.begin ();
                         it != _delayed.end ();)
                      {
                        if (it->second == GINGA_TIME_NONE
                            || it->second < begin
                            || (end != GINGA_TIME_NONE && it->second > end))
                          {
                            Action act = it->first;
                            Event *evt = act.event;
                            if (act.transition == Event::START
                                && it->second < begin)
                              evt->transition (act.transition);
                            it = _delayed.erase (it);
                          }
                        else if (it->second >= begin)
                          {
                            it->second = it->second - begin;
                            ++it;
                          }
                        else
                          ++it;
                      }
                  }
              }
            break;
          }

        case Event::PAUSE:
        case Event::RESUME:
          {
            if (!evt->isLambda ())
              break; // nothing to do

            // Pause/resume all the media anchors.
            for (auto e : _events)
              if (!e->isLambda () && e->getType () == Event::PRESENTATION)
                _doc->evalAction (e, transition);

            // Pause/resume the underlying player.
            g_assert_nonnull (_player);
            if (transition == Event::PAUSE)
              _player->pause ();
            else
              _player->resume ();
            break;
          }

        case Event::STOP:
          break; // nothing to do

        case Event::ABORT:
          break; // nothing to do

        default:
          g_assert_not_reached ();
        }
      break;

    case Event::ATTRIBUTION:
      break; // nothing to do

    case Event::SELECTION:
      break; // nothing to do

    case Event::PREPARATION:
      break; // nothing to do

    default:
      g_assert_not_reached ();
    }
  return true;
}

bool
Media::afterTransition (Event *evt, Event::Transition transition)
{
  switch (evt->getType ())
    {
    case Event::PRESENTATION:
      switch (transition)
        {
        case Event::START:
          if (evt->isLambda ())
            {
              // Start media as a whole.
              g_assert_nonnull (_player);
              Object::doStart ();

              // Schedule anchors.
              for (Event *e : _events)
                {
                  if (!e->isLambda ()
                      && (e->getType () == Event::PRESENTATION)
                      && !(e->hasLabel ()))
                    {
                      Time begin, end;
                      e->getInterval (&begin, &end);
                      this->addDelayedAction (e, Event::START, "", begin);
                      this->addDelayedAction (e, Event::STOP, "", end);
                    }
                }
              TRACE ("start %s at %" GINGA_TIME_FORMAT,
                     evt->getFullId ().c_str (), GINGA_TIME_ARGS (_time));
            }
          else if (evt->hasLabel ())
            {
              _player->sendPresentationEvent ("start", evt->getLabel ());
            }
          break;

        case Event::PAUSE:
          TRACE ("pause %s at %" GINGA_TIME_FORMAT,
                 evt->getFullId ().c_str (), GINGA_TIME_ARGS (_time));
          break; // nothing to do

        case Event::RESUME:
          TRACE ("resume %s at %" GINGA_TIME_FORMAT,
                 evt->getFullId ().c_str (), GINGA_TIME_ARGS (_time));
          break; // nothing to do

        case Event::STOP:
        case Event::ABORT:
          if (evt->isLambda ())
            {
              // Stop object.
              g_assert_nonnull (_player);
              if (transition == Event::ABORT)
                TRACE ("abort %s at %" GINGA_TIME_FORMAT,
                       evt->getFullId ().c_str (), GINGA_TIME_ARGS (_time));
              else
                TRACE ("stop %s at %" GINGA_TIME_FORMAT,
                       evt->getFullId ().c_str (), GINGA_TIME_ARGS (_time));
              this->doStop ();
            }
          else if (evt->getLabel () != "")
            {
              _player->sendPresentationEvent ("stop", evt->getLabel ());
            }
          else // non-lambda area
            {
              Time end;
              g_assert (this->isOccurring ());
              evt->getInterval (nullptr, &end);
              if (transition == Event::ABORT)
                TRACE ("abort %s (end=%" GINGA_TIME_FORMAT
                       ") at %" GINGA_TIME_FORMAT,
                       evt->getFullId ().c_str (), GINGA_TIME_ARGS (end),
                       GINGA_TIME_ARGS (_time));
              else
                TRACE ("stop %s (end=%" GINGA_TIME_FORMAT
                       ") at %" GINGA_TIME_FORMAT,
                       evt->getFullId ().c_str (), GINGA_TIME_ARGS (end),
                       GINGA_TIME_ARGS (_time));
            }
          break;

        default:
          g_assert_not_reached ();
        }
      break;

    case Event::ATTRIBUTION:
      {
        string value;
        evt->getParameter ("value", &value);
        switch (transition)
          {
          case Event::START:
            {
              string name;
              string s;
              Time dur;

              name = evt->getId ();
              _doc->evalPropertyRef (value, &value);

              dur = 0;
              if (evt->getParameter ("duration", &s))
                {
                  _doc->evalPropertyRef (s, &s);
                  dur = ginga::parse_time (s);
                }
              this->setProperty (name, value, dur);
              this->addDelayedAction (evt, Event::STOP, value, dur);
              TRACE ("start %s:='%s' (dur=%s) at %" GINGA_TIME_FORMAT,
                     evt->getFullId ().c_str (), value.c_str (),
                     (s != "") ? s.c_str () : "0s",
                     GINGA_TIME_ARGS (_time));
              break;
            }

          case Event::STOP:
            TRACE ("stop %s:='%s'", evt->getFullId ().c_str (),
                   value.c_str ());
            break;

          default:
            g_assert_not_reached ();
          }
        break;
      }
    case Event::SELECTION:
      {
        string key;
        evt->getParameter ("key", &key);
        switch (transition)
          {
          case Event::START:
            TRACE ("start %s", evt->getFullId ().c_str ());
            break;
          case Event::STOP:
            TRACE ("stop %s", evt->getFullId ().c_str ());
            break;
          default:
            g_assert_not_reached ();
          }
        break;
      }
    case Event::PREPARATION:
      {
        switch (transition)
          {
          case Event::START:
            TRACE ("start %s", evt->getFullId ().c_str ());

            createPlayer ();

            if(evt->getId() != "@lambda")
              {
                Time begin, end;
                evt->getInterval(&begin, &end);

                string offsetStart = xstrbuild ("%" G_GUINT64_FORMAT, begin / GINGA_SECOND);

                string offsetEnd = xstrbuild ("%" G_GUINT64_FORMAT, end / GINGA_SECOND);

                _player->setProperty("offsetBuffer", offsetStart);
                _player->setProperty("offsetEndBuffer",offsetEnd);
              }

            _player->startPreparation();
            _isPreparing = true;
            _currentPreparationEvent = evt;
            break;
          case Event::STOP:
            TRACE ("stop %s", evt->getFullId ().c_str ());
            _isPreparing = false;
            break;
          default:
            g_assert_not_reached ();
          }
        break;
      }
    default:
      g_assert_not_reached ();
    }

  return true;
}

Event *
Media::getCurrentPreparationEvent ()
{
  return _currentPreparationEvent;
}

void
Media::createPlayer ()
{
  if (_player)
    return;
  Formatter *fmt;
  g_assert (_doc->getData ("formatter", (void **) &fmt));
  g_assert_null (_player);
  _player = Player::createPlayer (fmt, this, _properties["uri"],
                                  _properties["type"]);
  g_assert_nonnull (_player);
  for (auto it : _properties)
    _player->setProperty (it.first, it.second);
}

// Public.

bool
Media::isFocused ()
{
  if (!this->isOccurring ())
    return false;
  g_assert_nonnull (_player);
  return _player->isFocused ();
}

bool
Media::getZ (int *zindex, int *zorder)
{
  if (this->isSleeping () || _player == nullptr)
    return false; // nothing to do
  g_assert_nonnull (_player);
  _player->getZ (zindex, zorder);
  return true;
}

void
Media::redraw (cairo_t *cr)
{
  if (this->isSleeping () || _player == nullptr)
    return; // nothing to do
  _player->redraw (cr);
}

// Protected.

void
Media::doStop ()
{
  if (_player == nullptr)
    {
      g_assert (this->isSleeping ());
      return; // nothing to do
    }

  if (_player->getState () != Player::SLEEPING)
    _player->stop ();
  delete _player;
  _player = nullptr;
  Object::doStop ();
}

GINGA_NAMESPACE_END
