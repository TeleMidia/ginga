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
#include "Context.h"
#include "LuaAPI.h"

#include "Document.h"
#include "MediaSettings.h"

GINGA_NAMESPACE_BEGIN

void
Context::getPorts (list<Event *> *ports)
{
  lua_Integer len;
  lua_Integer i;

  g_return_if_fail (ports != NULL);

  LuaAPI::Context_call (_L, this, "getPorts", 0, 1);
  g_assert (lua_type (_L, -1) == LUA_TTABLE);

  len = luaL_len (_L, -1);
  for (i = 1; i <= len; i++)
    {
      const char *qualId;
      Event *evt;

      lua_rawgeti (_L, -1, i);
      qualId = luaL_checkstring (_L, -1);
      evt = this->getDocument ()->getEvent (qualId);
      if (evt != NULL)
        ports->push_back (evt);
      lua_pop (_L, 1);
    }
}

bool
Context::addPort (Event *event)
{
  bool status;

  g_return_val_if_fail (event != NULL, false);

  LuaAPI::Event_push (_L, event);
  LuaAPI::Context_call (_L, this, "addPort", 1, 1);
  status = lua_toboolean (_L, -1);
  lua_pop (_L, 1);

  return status;
}

bool
Context::removePort (Event *event)
{
  bool status;

  g_return_val_if_fail (event != NULL, false);

  LuaAPI::Event_push (_L, event);
  LuaAPI::Context_call (_L, this, "removePort", 1, 1);
  status = lua_toboolean (_L, -1);
  lua_pop (_L, 1);

  return status;
}

// TODO --------------------------------------------------------------------

Context::Context (Document *doc, const string &id) : Composition (doc, id)
{
  _awakeChildren = 0;
  _status = true;

  LuaAPI::Object_attachWrapper (_L, this, doc, Object::CONTEXT, id);
}

Context::~Context ()
{
  for (auto link: _links)
    {
      for (auto &cond : link.first)
        if (cond.predicate != NULL)
          delete cond.predicate;
      for (auto &act : link.second)
        if (act.predicate != NULL)
          delete act.predicate;
    }

  LuaAPI::Object_detachWrapper (_L, this);
}

void
Context::sendTick (Time total, Time diff, Time frame)
{
  // Update object time.
  Object::sendTick (total, diff, frame);

  // set<Composition *> parents;
  // this->getParents (&parents);
  // auto it = parents.begin ();
  // Composition *parent = (it == parents.end ()) ? NULL: *it;

  // Check for EOS.
  // if (parent == nullptr && _awakeChildren == 0)
  //   {
  //     if (this->getDocument ()->getSettings ()->isOccurring ())
  //       {
  //         this->getDocument ()->evalAction (this->getLambda (), Event::STOP);
  //       }
  //     else
  //       {
  //         g_assert_not_reached ();
  //       }
  //   }
  // else if (_awakeChildren == 0)
  //   {
  //     this->getDocument ()->evalAction (this->getLambda (), Event::STOP);
  //   }
}

bool
Context::beforeTransition (Event *evt, Event::Transition transition,
                           map<string, string> &params)
{
  switch (evt->getType ())
    {
    case Event::PRESENTATION:
      g_assert (evt->isLambda ());
      switch (transition)
        {
        case Event::START:
          break;

        case Event::STOP:
          {
            set<Object *> children;
            this->getChildren (&children);
            for (auto child : children)
              {
                Event *lambda = child->getLambda ();
                g_assert_nonnull (lambda);
                lambda->transition (Event::STOP, params);
              }
            break;
          }

        case Event::PAUSE:
          {
            set<Object *> children;
            this->getChildren (&children);
            for (auto child : children)
              {
                Event *lambda = child->getLambda ();
                g_assert_nonnull (lambda);
                lambda->transition (Event::PAUSE, params);
              }
            break;
          }

        case Event::RESUME:
          {
            set<Object *> children;
            this->getChildren (&children);
            for (auto child : children)
              {
                Event *lambda = child->getLambda ();
                g_assert_nonnull (lambda);
                lambda->transition (Event::RESUME, params);
              }
            break;
          }

        case Event::ABORT:
          {
            set<Object *> children;
            this->getChildren (&children);
            for (auto child : children)
              {
                Event *lambda = child->getLambda ();
                g_assert_nonnull (lambda);
                lambda->transition (Event::ABORT, params);
              }
            break;
          }

        default:
          g_assert_not_reached ();
        }
      break;

    case Event::ATTRIBUTION:
      break; // nothing to do

    case Event::SELECTION:
      return false; // fail

    default:
      g_assert_not_reached ();
    }
  return true;
}

bool
Context::afterTransition (Event *evt, Event::Transition transition,
                          map<string, string> &params)
{
  set<Composition *> parents;
  this->getParents (&parents);
  auto it = parents.begin ();
  Composition *parent = (it == parents.end ()) ? NULL: *it;

  string param_test;
  switch (evt->getType ())
    {
    case Event::PRESENTATION:
      g_assert (evt->isLambda ());
      switch (transition)
        {
        case Event::START:
          {
            // Start context as a whole.
            Object::doStart ();

            if (instanceof (Context *, parent) && parent->isSleeping ())
              {
                //parent->getLambda ()->setParameter ("fromport", "true");
                parent->getLambda ()->transition (Event::START, params);
              }

            // Start all ports in the next tick if not started from port
            //evt->getParameter ("fromport", &param_test);
            if (param_test != "")
              break;

            list<Event *> ports;
            this->getPorts (&ports);
            for (auto port : ports)
              {
                if (port->getType () == Event::PRESENTATION)
                  this->addDelayedAction (port, Event::START, "", 0);
              }
            //evt->setParameter ("fromport", "");
            break;
          }
        case Event::PAUSE:
          // TRACE ("pause %s at %" GINGA_TIME_FORMAT,
          //        evt->getQualifiedId ().c_str (), GINGA_TIME_ARGS (_time));
          break;
        case Event::RESUME:
          // TRACE ("resume %s at %" GINGA_TIME_FORMAT,
          //        evt->getQualifiedId ().c_str (), GINGA_TIME_ARGS (_time));
          break;
        case Event::STOP:
          // TRACE ("stop %s at %" GINGA_TIME_FORMAT,
          //        evt->getQualifiedId ().c_str (), GINGA_TIME_ARGS (_time));
          Object::doStop ();
          break;
        case Event::ABORT:
          // TRACE ("abort %s at %" GINGA_TIME_FORMAT,
          //        evt->getQualifiedId ().c_str (), GINGA_TIME_ARGS (_time));
          Object::doStop ();
          break;
        default:
          g_assert_not_reached ();
        }
      break;

    case Event::ATTRIBUTION:
      switch (transition)
        {
        case Event::START:
          {
            string name;
            string s;
            Time dur;

            name = evt->getId ();

            dur = 0;
            s = params["duration"];
            if (s != "")
              {
                this->getDocument ()->evalPropertyRef (s, &s);
                dur = ginga::parse_time (s);
              }
            this->setPropertyString (name, params["value"]);
            this->addDelayedAction (evt, Event::STOP, params["value"], dur);
            break;
          }
        case Event::STOP:
          {
            TRACE ("stop %s:=...", evt->getQualifiedId ().c_str ());
            break;
          }
        default:
          {
            g_assert_not_reached ();
          }
        }
      break;

    case Event::SELECTION:
      g_assert_not_reached ();
      break;

    default:
      g_assert_not_reached ();
    }
  return true;
}

// Public.

const list<pair<list<Action>, list<Action> > > *
Context::getLinks ()
{
  return &_links;
}

void
Context::addLink (list<Action> conds, list<Action> acts)
{
  g_assert (conds.size () > 0);
  g_assert (acts.size () > 0);
  _links.push_back (std::make_pair (conds, acts));
}

void
Context::incAwakeChildren ()
{
  _awakeChildren++;
}

void
Context::decAwakeChildren ()
{
  _awakeChildren--;
}

bool
Context::getLinksStatus ()
{
  return _status;
}

void
Context::setLinksStatus (bool status)
{
  _status = status;
}

GINGA_NAMESPACE_END
