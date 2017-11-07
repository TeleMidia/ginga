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
#include "FormatterContext.h"

GINGA_NAMESPACE_BEGIN


// Public.

FormatterContext::FormatterContext (Formatter *formatter, const string &id)
  :FormatterComposition (formatter, id)
{
}

FormatterContext::~FormatterContext ()
{
  for (auto link: _links)
    delete link;
}


// Public: FormatterObject.

string G_GNUC_NORETURN
FormatterContext::getProperty (unused (const string &name))
{
  g_assert_not_reached ();
}

void G_GNUC_NORETURN
FormatterContext::setProperty (unused (const string &name),
                               unused (const string &value),
                               unused (GingaTime dur))
{
  g_assert_not_reached ();
}

void
FormatterContext::sendKeyEvent (unused (const string &key),
                                unused (bool press))
{
}

void
FormatterContext::sendTickEvent (unused (GingaTime total),
                                 unused (GingaTime diff),
                                 unused (GingaTime frame))
{
  // FormatterEvent *lambda;

  // Update object time.
  FormatterObject::sendTickEvent (total, diff, frame);

  // g_assert (this->isOccurring ());
  // for (auto child: _children)
  //   if (child->isOccurring ())
  //     return;

  // lambda = this->getLambda ();
  // g_assert_nonnull (lambda);
  // this->scheduleAction (lambda, NclEventStateTransition::STOP);
}

bool
FormatterContext::startTransition (FormatterEvent *evt,
                                   NclEventStateTransition transition)
{
  switch (evt->getType ())
    {
    case NclEventType::PRESENTATION:
      g_assert (evt->isLambda ());
      switch (transition)
        {
        case NclEventStateTransition::START:
          break;

        case NclEventStateTransition::STOP:
          for (auto child: _children)
            {
              FormatterEvent *lambda = child->getLambda ();
              g_assert_nonnull (lambda);
              lambda->transition (NclEventStateTransition::STOP);
            }
          break;

        default:
          g_assert_not_reached ();
        }
      break;

    case NclEventType::ATTRIBUTION:
      g_assert_not_reached ();
      break;

    case NclEventType::SELECTION:
      return false;             // fail

    default:
      g_assert_not_reached ();
    }
  return true;
}

void
FormatterContext::endTransition (FormatterEvent *evt,
                                 NclEventStateTransition transition)
{
  switch (evt->getType ())
    {
    case NclEventType::PRESENTATION:
      switch (transition)
        {
        case NclEventStateTransition::START:
          FormatterObject::doStart ();
          for (auto port: _ports)
            _formatter->evalAction (port, transition);
          TRACE ("start %s@lambda", _id.c_str ());
          break;
        case NclEventStateTransition::STOP:
          FormatterObject::doStop ();
          TRACE ("stop %s@lambda", _id.c_str ());
          break;
        default:
            g_assert_not_reached ();
        }
      break;

    case NclEventType::ATTRIBUTION:
    case NclEventType::SELECTION:
    default:
      g_assert_not_reached ();
    }
}


// Public.

const list<FormatterEvent *> *
FormatterContext::getPorts ()
{
  return &_ports;
}

void
FormatterContext::addPort (FormatterEvent *evt)
{
  g_assert_nonnull (evt);
  tryinsert (evt, _ports, push_back);
}

const list<FormatterLink *> *
FormatterContext::getLinks ()
{
  return &_links;
}

void
FormatterContext::addLink (FormatterLink *link)
{
  g_assert_nonnull (link);
  tryinsert (link, _links, push_back);
}


// Private.

void
FormatterContext::toggleLinks (bool status)
{
  for (auto link: _links)
    link->setDisabled (status);
}

GINGA_NAMESPACE_END
