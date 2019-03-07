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

// TODO --------------------------------------------------------------------

// Public.

Context::Context (Document *doc,
                  Composition *parent,
                  const string &id) : Composition (doc, parent, id)
{
  _awakeChildren = 0;
  _status = true;

  LuaAPI::Context_attachWrapper (_L, this);
  _initEvents ();
}

Context::~Context ()
{
  // Stop.
  _lambda->transition (Event::STOP);

  // Delete children.
  for (auto child: _children)
    delete child;

  // Delete predicates in links.
  for (auto link: _links)
    {
      for (auto &cond : link.first)
        if (cond.predicate != NULL)
          delete cond.predicate;
      for (auto &act : link.second)
        if (act.predicate != NULL)
          delete act.predicate;
    }

  _finiEvents ();
  LuaAPI::Context_detachWrapper (_L, this);
}

// Public: Object.

Object::Type
Context::getType ()
{
  return Object::CONTEXT;
}

// TODO

string
Context::getProperty (unused (const string &name))
{
  return Object::getProperty (name);
}

void
Context::setProperty (const string &name, const string &value, Time dur)
{
  Object::setProperty (name, value, dur);
}

void
Context::sendKey (unused (const string &key), unused (bool press))
{
}

void
Context::sendTick (Time total, Time diff, Time frame)
{
  // Update object time.
  Object::sendTick (total, diff, frame);

  // Check for EOS.
  if (_parent == nullptr && _awakeChildren == 1)
    {
      if (_doc->getSettings ()->isOccurring ())
        {
          _doc->evalAction (_lambda, Event::STOP);
        }
      else
        {
          g_assert_not_reached ();
        }
    }
  else if (_awakeChildren == 0)
    {
      _doc->evalAction (_lambda, Event::STOP);
    }
}

bool
Context::beforeTransition (Event *evt, Event::Transition transition)
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
          for (auto child : _children)
            {
              Event *lambda = child->getLambda ();
              g_assert_nonnull (lambda);
              lambda->transition (Event::STOP);
            }
          break;

        case Event::PAUSE:
          for (auto child : _children)
            {
              Event *lambda = child->getLambda ();
              g_assert_nonnull (lambda);
              lambda->transition (Event::PAUSE);
            }
          break;

        case Event::RESUME:
          for (auto child : _children)
            {
              Event *lambda = child->getLambda ();
              g_assert_nonnull (lambda);
              lambda->transition (Event::RESUME);
            }
          break;

        case Event::ABORT:
          for (auto child : _children)
            {
              Event *lambda = child->getLambda ();
              g_assert_nonnull (lambda);
              lambda->transition (Event::ABORT);
            }
          break;

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
Context::afterTransition (Event *evt, Event::Transition transition)
{
  string param_test;
  switch (evt->getType ())
    {
    case Event::PRESENTATION:
      g_assert (evt->isLambda ());
      switch (transition)
        {
        case Event::START:
          // Start context as a whole.
          Object::doStart ();

          if (instanceof (Context *, _parent) && _parent->isSleeping ())
            {
              _parent->getLambda ()->setParameter ("fromport", "true");
              _parent->getLambda ()->transition (Event::START);
            }

          // Start all ports in the next tick if not started from port
          evt->getParameter ("fromport", &param_test);
          if (param_test != "")
            break;
          for (auto port : _ports)
            {
              if (port->getType () == Event::PRESENTATION)
                this->addDelayedAction (port, Event::START, "", 0);
            }
          evt->setParameter ("fromport", "");
          TRACE ("start %s at %" GINGA_TIME_FORMAT,
                 evt->getQualifiedId ().c_str (), GINGA_TIME_ARGS (_time));
          break;
        case Event::PAUSE:
          TRACE ("pause %s at %" GINGA_TIME_FORMAT,
                 evt->getQualifiedId ().c_str (), GINGA_TIME_ARGS (_time));
          break;
        case Event::RESUME:
          TRACE ("resume %s at %" GINGA_TIME_FORMAT,
                 evt->getQualifiedId ().c_str (), GINGA_TIME_ARGS (_time));
          break;
        case Event::STOP:
          TRACE ("stop %s at %" GINGA_TIME_FORMAT,
                 evt->getQualifiedId ().c_str (), GINGA_TIME_ARGS (_time));
          Object::doStop ();
          break;
        case Event::ABORT:
          TRACE ("abort %s at %" GINGA_TIME_FORMAT,
                 evt->getQualifiedId ().c_str (), GINGA_TIME_ARGS (_time));
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
            string value;
            string s;
            Time dur;

            name = evt->getId ();
            evt->getParameter ("value", &value);
            _doc->evalPropertyRef (value, &value);

            dur = 0;
            if (evt->getParameter ("duration", &s))
              {
                _doc->evalPropertyRef (s, &s);
                dur = ginga::parse_time (s);
              }
            this->setProperty (name, value, dur);
            this->addDelayedAction (evt, Event::STOP, value, dur);
            TRACE ("start %s:='%s' (dur=%s)", evt->getQualifiedId ().c_str (),
                   value.c_str (), (s != "") ? s.c_str () : "0s");
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

const list<Event *> *
Context::getPorts ()
{
  return &_ports;
}

void
Context::addPort (Event *event)
{
  g_assert_nonnull (event);
  tryinsert (event, _ports, push_back);
}

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
