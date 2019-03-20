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

#include "Document.h"
#include "Context.h"
#include "MediaSettings.h"
#include "Switch.h"
#include "Event.h"
#include "Player.h"

GINGA_NAMESPACE_BEGIN

Media::Media (Document *doc, const string &id) : Object (doc, id)
{
  LuaAPI::Object_attachWrapper (_L, this, doc, Object::MEDIA, id);
}

Media::~Media ()
{
  Player *player;

  player = _getPlayer ();
  if (player != NULL)
    delete player;

  LuaAPI::Object_detachWrapper (_L, this);
}

bool
Media::isFocused ()
{
  bool status;

  LuaAPI::Media_call (_L, this, "isFocused", 0, 1);
  status = lua_toboolean (_L, -1);
  lua_pop (_L, 1);

  return status;
}

// Private.

Player *
Media::_getPlayer ()
{
  Player *player = NULL;

  LuaAPI::Media_call (_L, this, "_getPlayer", 0, 1);
  if (!lua_isnil (_L, -1))
    {
      player = LuaAPI::Player_check (_L, -1);
    }
  lua_pop (_L, 1);

  return player;
}

// TODO --------------------------------------------------------------------

void
Media::sendKey (const string &key, bool press)
{
  list<Event *> buf;

  if (this->isSleeping ())
    return;

  if (_getPlayer () == NULL)
    return;

  // if (press && xstrhasprefix (key, "CURSOR_") && this->isFocused ())
  //   {
  //     string next;
  //
  //     if ((key == "CURSOR_UP"
  //          && (next = this->getProperty ("moveUp")) != "")
  //         || ((key == "CURSOR_DOWN"
  //              && (next = this->getProperty ("moveDown")) != ""))
  //         || ((key == "CURSOR_LEFT"
  //              && (next = this->getProperty ("moveLeft")) != ""))
  //         || ((key == "CURSOR_RIGHT"
  //              && (next = this->getProperty ("moveRight")) != "")))
  //       {
  //         this->getDocument ()->getSettings ()
  //           ->setPropertyString ("_nextFocus", next);
  //       }
  //   }

  // Pass key to player.
  // if (this->isFocused ())
  //   _getPlayer ()->sendKeyEvent (key, press);

  set<Event *> events;
  this->getEvents (&events);

  // Collect the events to be triggered.
  for (auto evt: events)
    {
      if (evt->getType () != Event::SELECTION)
        continue;

      string expected = evt->getId ();
      if (expected[0] == '$')
        expected = ""; // A param could not be resolved.  Should we generate
                       // an error?

      if (!((expected == "" && key == "ENTER" && this->isFocused ())
            || (expected != "" && key == expected)))
        {
          continue;
        }

      buf.push_back (evt);
    }

  // Run collected events.
  for (Event *evt : buf)
    this->getDocument ()
      ->evalAction (evt, press ? Event::START : Event::STOP);
}

// void
// Media::sendTick (Time total, Time diff, Time frame)
// {
  // Update object time.
  // Object::sendTick (total, diff, frame);

  // if (_getPlayer () == NULL)
  //   return;

  // Update player time.
  // g_assert_nonnull (_getPlayer ());

  // Check EOS.
  // if (_getPlayer ()->getEOS ()) // TODO: "duration" property
  //   {
  //     this->getDocument ()->evalAction (this->getLambda (), Event::STOP);
  //   }
// }

bool
Media::beforeTransition (Event *evt, Event::Transition transition,
                         map<string, string> &params)
{
  set<Composition *> parents;
  this->getParents (&parents);
  auto it = parents.begin ();
  Composition *parent = (it == parents.end ()) ? NULL: *it;

  switch (evt->getType ())
    {
    case Event::PRESENTATION:
      switch (transition)
        {
        case Event::START:
          {
            if (instanceof (Context *, parent) && parent->isSleeping ())
              {
                //parent->getLambda ()->setParameter ("fromport", "true");
                parent->getLambda ()->transition (Event::START, params);
              }
            // Create underlying player.
            if (evt->getState () == Event::SLEEPING)
              {
                if (evt->isLambda ())
                  {
                    // string type;
                    // string uri;

                    g_assert_not_reached ();

                    // g_assert_null (_getPlayer ());

                    // LuaAPI::Media_push (_L, this);
                    // if (this->getPropertyString ("type", &type))
                    //   {
                    //     lua_pushstring (_L, type.c_str ());
                    //   }
                    // else
                    //   {
                    //     lua_pushnil (_L);
                    //   }
                    // if (this->getPropertyString ("uri", &uri))
                    //   {
                    //     lua_pushstring (_L, uri.c_str ());
                    //   }
                    // else
                    //   {
                    //     lua_pushnil (_L);
                    //   }
                    // LuaAPI::Document_call (_L, this->getDocument (),
                    //                        "_createPlayer", 3, 2);
                    // type = string (luaL_checkstring (_L, -2));
                    // _player = LuaAPI::Player_check (_L, -1);
                    // this->setPropertyString ("type", type);

                    // // _player = Player::createPlayer (this, uri, type);
                    // // if (unlikely (_player == nullptr))
                    // //   return false; // fail

                    // map<string, GValue> props;
                    // this->getProperties (&props);

                    // for (auto it: props)
                    //   {
                    //     GValue *value = &it.second;
                    //     if (G_VALUE_HOLDS (value, G_TYPE_STRING))
                    //       {
                    //         string str = string (g_value_get_string (value));
                    //         _getPlayer ()->setProperty (it.first, str);
                    //       }
                    //     g_value_unset (value);
                    //   }

                    // g_assert_nonnull (_player);
                    // _getPlayer ()->start ();
                  }
                else
                  { // Anchor
                    Event *lambda;

                    lambda = this->getLambda ();
                    if (lambda->getState () != Event::SLEEPING)
                      break;

                    lambda->transition (Event::START, params);

                    // begin = evt->getBeginTime ();
                    // end = evt->getEndTime ();

                    // string time_seek = xstrbuild ("%" G_GUINT64_FORMAT,
                    //                               begin / GINGA_SECOND);
                    // TRACE ("time_seek %ss", time_seek.c_str ());
                    //_getPlayer ()->setProperty ("time", time_seek);

                    // End
                    // if (end != GINGA_TIME_NONE)
                    //   {
                    //     dur = end - begin;
                    //     string time_end = xstrbuild ("%" G_GUINT64_FORMAT,
                    //                                  dur / GINGA_SECOND);
                    //     TRACE ("time_end in %ss", time_end.c_str ());
                    //     _getPlayer ()->setProperty ("duration", time_end);
                    //     this->addDelayedAction (evt, Event::STOP, "", end);
                    //   }

                    // remove events of anchors that happens before or after
                    // anchor started.
                    // for (auto it = _delayed.begin ();
                    //      it != _delayed.end ();)
                    //   {
                    //     if (it->second == GINGA_TIME_NONE
                    //         || it->second < begin
                    //         || (end != GINGA_TIME_NONE && it->second > end))
                    //       {
                    //         Action act = it->first;
                    //         Event *evt = this->getDocument ()
                    //           ->getEvent (act.event);
                    //         if (act.transition == Event::START
                    //             && it->second < begin)
                    //           evt->transition (act.transition, params);
                    //         it = _delayed.erase (it);
                    //       }
                    //     else if (it->second >= begin)
                    //       {
                    //         it->second = it->second - begin;
                    //         ++it;
                    //       }
                    //     else
                    //       ++it;
                    //   }
                  }
              }
            break;
          }

        case Event::PAUSE:
        case Event::RESUME:
          {

            if (!evt->isLambda ())
              break; // nothing to do

            set<Event *> events;
            this->getEvents (&events);

            // Pause/resume all the media anchors.
            for (auto e: events)
              if (!e->isLambda () && e->getType () == Event::PRESENTATION)
                this->getDocument ()->evalAction (e, transition);

            // Pause/resume the underlying player.
            g_assert_nonnull (_getPlayer ());
            if (transition == Event::PAUSE)
              _getPlayer ()->pause ();
            else
              _getPlayer ()->start ();
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

    default:
      g_assert_not_reached ();
    }
  return true;
}

bool
Media::afterTransition (Event *evt, Event::Transition transition,
                        map<string, string> &params)
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
              g_assert_nonnull (_getPlayer ());
              Object::doStart ();

              set<Event *> events;
              this->getEvents (&events);

              // Schedule anchors.
              for (Event *e : events)
                {
                  if (!e->isLambda ()
                      && (e->getType () == Event::PRESENTATION)
                      && e->getLabel () == "")
                    {
                      this->addDelayedAction (e, Event::START, "",
                                              e->getBeginTime ());
                      this->addDelayedAction (e, Event::STOP, "",
                                              e->getEndTime ());

                    }
                }
            }
          else if (evt->getLabel () != "")
            {
              //_getPlayer ()->sendPresentationEvent ("start", evt->getLabel ());
            }
          break;

        case Event::PAUSE:
          break; // nothing to do

        case Event::RESUME:
          break; // nothing to do

        case Event::STOP:
        case Event::ABORT:
          if (evt->isLambda ())
            {
              // Stop object.
              g_assert_nonnull (_getPlayer ());
              this->doStop ();
            }
          else if (evt->getLabel () != "")
            {
              //_getPlayer ()->sendPresentationEvent ("stop", evt->getLabel ());
            }
          else // non-lambda area
            {
              g_assert (this->isOccurring ());
            }
          break;

        default:
          g_assert_not_reached ();
        }
      break;

    case Event::ATTRIBUTION:
      {
        string value = params["value"];
        switch (transition)
          {
          case Event::START:
            {
              string name;
              string s;
              Time dur;

              name = evt->getId ();
              this->getDocument ()->evalPropertyRef (value, &value);

              dur = 0;
              s = params["duration"];
              if (s != "")
                {
                  this->getDocument ()->evalPropertyRef (s, &s);
                  dur = ginga::parse_time (s);
                }
              this->setPropertyString (name, value);
              this->addDelayedAction (evt, Event::STOP, value, dur);
              break;
            }

          case Event::STOP:
            TRACE ("stop %s:='%s'", evt->getQualifiedId ().c_str (),
                   value.c_str ());
            break;

          default:
            g_assert_not_reached ();
          }
        break;
      }
    case Event::SELECTION:
      {
        string key = params["key"];
        switch (transition)
          {
          case Event::START:
            TRACE ("start %s", evt->getQualifiedId ().c_str ());
            break;
          case Event::STOP:
            TRACE ("stop %s", evt->getQualifiedId ().c_str ());
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

void
Media::doStop ()
{
  if (_getPlayer () == nullptr)
    return;

  if (_getPlayer ()->getState () != Player::STOPPED)
    _getPlayer ()->stop ();

  delete _getPlayer ();
  Object::doStop ();
}

GINGA_NAMESPACE_END
