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
#include "ExecutionObjectContext.h"
#include "Scheduler.h"

GINGA_FORMATTER_BEGIN

ExecutionObjectContext::ExecutionObjectContext (GingaInternal *ginga,
                                                const string &id,
                                                Node *node)
  : ExecutionObject (ginga, id, node)
{
  g_assert_nonnull (node);
  _context = cast (Context *, node);
  g_assert_nonnull (_context);
}

ExecutionObjectContext::~ExecutionObjectContext ()
{
}

const set<ExecutionObject *> *
ExecutionObjectContext::getChildren ()
{
  return &_children;
}

ExecutionObject *
ExecutionObjectContext::getChildById (const string &id)
{
  for (auto child: _children)
    if (child->getId () == id)
      return child;
  return nullptr;
}

bool
ExecutionObjectContext::addChild (ExecutionObject *child)
{
  g_assert_nonnull (child);
  if (_children.find (child) != _children.end ())
    return false;
  _children.insert (child);
  return true;
}

const vector<NclLink *> *
ExecutionObjectContext::getLinks ()
{
  return &_links;
}

bool
ExecutionObjectContext::addLink (NclLink *link)
{
  g_assert_nonnull (link);
  for (auto other: _links)
    if (other == link)
      return false;
  _links.push_back (link);
  return true;
}

bool
ExecutionObjectContext::exec (NclEvent *evt, EventState from, EventState to,
                              EventStateTransition transition)
{
  TRACE (">>>>>>>>> CTX evt %s from %s to %s via %s",
         evt->getAnchor ()->getId ().c_str (),
         EventUtil::getEventStateAsString (from).c_str (),
         EventUtil::getEventStateAsString (to).c_str (),
         EventUtil::getEventStateTransitionAsString (transition).c_str ());

  switch (evt->getType ())
    {
    case EventType::PRESENTATION:
      switch (transition)
        {
        case EventStateTransition::START:
          for (auto port: *_context->getPorts ())
            {
              Node *target;
              Anchor *iface;
              ExecutionObject *child;
              NclEvent *evt;

              port->getTarget (&target, &iface);
              child = _scheduler->obtainExecutionObject (target);
              g_assert_nonnull (child);

              if (!instanceof (Area *, iface))
                continue;       // nothing to do

              evt = child->obtainEvent (EventType::PRESENTATION, iface, "");
              g_assert_nonnull (evt);
              evt->transition (transition);
            }
          break;
        case EventStateTransition::PAUSE:
          g_assert_not_reached ();
          break;
        case EventStateTransition::RESUME:
          g_assert_not_reached ();
          break;
        case EventStateTransition::STOP:
          g_assert_not_reached ();
          break;
        case EventStateTransition::ABORT:
          g_assert_not_reached ();
          break;
        default:
          g_assert_not_reached ();
        }
      break;
    case EventType::ATTRIBUTION:
      g_assert_not_reached ();
      break;
    case EventType::SELECTION:
      return false;             // fail: contexts cannot be selected
    default:
      g_assert_not_reached ();
    }
  return true;
}


GINGA_FORMATTER_END
