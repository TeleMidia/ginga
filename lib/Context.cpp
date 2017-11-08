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
#include "Context.h"

GINGA_NAMESPACE_BEGIN


// Public.

Context::Context (const string &id): Composition (id)
{
}

Context::~Context ()
{
  for (auto link: _links)
    delete link;
}


// Public: Object.

string G_GNUC_NORETURN
Context::getProperty (unused (const string &name))
{
  g_assert_not_reached ();
}

void G_GNUC_NORETURN
Context::setProperty (unused (const string &name),
                      unused (const string &value),
                      unused (Time dur))
{
  g_assert_not_reached ();
}

void
Context::sendKeyEvent (unused (const string &key),
                       unused (bool press))
{
}

void
Context::sendTickEvent (Time total, Time diff, Time frame)
{
  Object::sendTickEvent (total, diff, frame);

  // g_assert (this->isOccurring ());
  // for (auto child: _children)
  //   if (child->isOccurring ())
  //     return;
  // Event *lambda = this->getLambda ();
  // g_assert_nonnull (lambda);
  // _formatter->evalAction (lambda, Event::STOP);
}

bool
Context::startTransition (Event *event, Event::Transition transition)
{
  switch (event->getType ())
    {
    case Event::PRESENTATION:
      g_assert (event->isLambda ());
      switch (transition)
        {
        case Event::START:
          break;

        case Event::STOP:
          for (auto child: _children)
            {
              Event *lambda = child->getLambda ();
              g_assert_nonnull (lambda);
              lambda->transition (Event::STOP);
            }
          break;

        default:
          g_assert_not_reached ();
        }
      break;

    case Event::ATTRIBUTION:
      g_assert_not_reached ();
      break;

    case Event::SELECTION:
      return false;             // fail

    default:
      g_assert_not_reached ();
    }
  return true;
}

void
Context::endTransition (Event *event, Event::Transition transition)
{
  switch (event->getType ())
    {
    case Event::PRESENTATION:
      switch (transition)
        {
        case Event::START:
          Object::doStart ();
          for (auto port: _ports)
            _formatter->evalAction (port, transition);
          TRACE ("start %s@lambda", _id.c_str ());
          break;
        case Event::STOP:
          Object::doStop ();
          TRACE ("stop %s@lambda", _id.c_str ());
          break;
        default:
            g_assert_not_reached ();
        }
      break;

    case Event::ATTRIBUTION:
    case Event::SELECTION:
    default:
      g_assert_not_reached ();
    }
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

const list<Link *> *
Context::getLinks ()
{
  return &_links;
}

void
Context::addLink (Link *link)
{
  g_assert_nonnull (link);
  tryinsert (link, _links, push_back);
}


// Private.

void
Context::toggleLinks (bool status)
{
  for (auto link: _links)
    link->setDisabled (status);
}

GINGA_NAMESPACE_END
