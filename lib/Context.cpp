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

#include "Document.h"
#include "MediaSettings.h"

GINGA_NAMESPACE_BEGIN

// Public.

Context::Context (const string &id) : Composition (id)
{
  _awakeChildren = 0;
  _status = true;
}

Context::~Context ()
{
  // Stop and delete children.
  _lambda->transition (Event::STOP);
  for (auto child : _children)
    delete child;

  // Delete predicates in links.
  for (auto link : _links)
    {
      for (auto &cond : link.first)
        if (cond.predicate != nullptr)
          delete cond.predicate;
      for (auto &act : link.second)
        if (act.predicate != nullptr)
          delete act.predicate;
    }
}

// Public: Object.

string
Context::getObjectTypeAsString ()
{
  return "Context";
}

string
Context::toString ()
{
  string str;

  str = Object::toString ();

  if (_ports.size () > 0)
    {
      auto it = _ports.begin ();
      str += "  ports: " + (*it)->getFullId ();
      while (++it != _ports.end ())
        str += ", " + (*it)->getFullId ();
      str += "\n";
    }

  if (_links.size () > 0)
    {

#define COND_TOSTRING(str, act)                                            \
  G_STMT_START                                                             \
  {                                                                        \
    (str) += Event::getEventTransitionAsString ((act).transition);         \
    (str) += "(" + (act).event->getFullId ();                              \
    if ((act).predicate != nullptr)                                        \
      (str) += " ? " + (act).predicate->toString ();                       \
    (str) += ")";                                                          \
  }                                                                        \
  G_STMT_END

#define ACT_TOSTRING(str, act)                                             \
  G_STMT_START                                                             \
  {                                                                        \
    (str) += Event::getEventTransitionAsString ((act).transition);         \
    (str) += "(" + (act).event->getFullId ();                              \
    if ((act).event->getType () == Event::ATTRIBUTION)                     \
      (str) += ":='" + (act).value + "'";                                  \
    (str) += ")";                                                          \
  }                                                                        \
  G_STMT_END

      int i = 1;
      str += "  links:\n";
      for (auto link : _links)
        {
          str += xstrbuild ("    #%d ", i++);
          auto it = link.first.begin ();
          COND_TOSTRING (str, *it);
          while (++it != link.first.end ())
            {
              str += ", ";
              COND_TOSTRING (str, *it);
            }
          str += "\n    -> ";
          it = link.second.begin ();
          ACT_TOSTRING (str, *it);
          while (++it != link.second.end ())
            {
              str += ", ";
              ACT_TOSTRING (str, *it);
            }
          str += "\n";
        }
    }

  return str;
}

string Context::getProperty (unused (const string &name))
{
  return Object::getProperty (name);
}

void
Context::setProperty (const string &name, const string &value, Time dur)
{
  Object::setProperty (name, value, dur);
}

void Context::sendKey (unused (const string &key), unused (bool press))
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
  switch (evt->getType ())
    {
    case Event::PRESENTATION:
      g_assert (evt->isLambda ());
      switch (transition)
        {
        case Event::START:
          // Start context as a whole.
          Object::doStart ();

          // Start all ports in the next tick.
          for (auto port : _ports)
            {
              if (port->getType () == Event::PRESENTATION)
                this->addDelayedAction (port, Event::START, "", 0);
            }
          TRACE ("start %s", evt->getFullId ().c_str ());
          break;
        case Event::PAUSE:
          TRACE ("pause %s", evt->getFullId ().c_str ());
          break;
        case Event::RESUME:
          TRACE ("resume %s", evt->getFullId ().c_str ());
          break;
        case Event::STOP:
          Object::doStop ();
          TRACE ("stop %s", evt->getFullId ().c_str ());
          break;
        case Event::ABORT:
          Object::doStop ();
          TRACE ("abort %s", evt->getFullId ().c_str ());
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
            TRACE ("start %s:='%s' (dur=%s)", evt->getFullId ().c_str (),
                   value.c_str (), (s != "") ? s.c_str () : "0s");
            break;
          }
        case Event::STOP:
          {
            TRACE ("stop %s:=...", evt->getFullId ().c_str ());
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
