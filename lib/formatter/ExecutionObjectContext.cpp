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

GINGA_FORMATTER_BEGIN

ExecutionObjectContext::ExecutionObjectContext (GingaInternal *ginga,
                                                const string &id,
                                                Node *dataObject)
  : ExecutionObject (ginga, id, dataObject)
{
}

ExecutionObjectContext::~ExecutionObjectContext ()
{
}

void
ExecutionObjectContext::eventStateChanged (NclEvent *event,
                                           EventStateTransition transition)
{
  set<NclEvent *>::iterator i;
  NclEvent *lambda = this->getLambda (EventType::PRESENTATION);
  g_assert_nonnull (lambda);

  switch (transition)
    {
    case EventStateTransition::START:
      if (_runningEvents.empty ())
        {
          lambda->addListener (_parent);
          lambda->start ();
        }

      _runningEvents.insert (event);
      break;

    case EventStateTransition::ABORT:
      if (_runningEvents.empty ())
        {
          lambda->abort ();
        }
      break;

    case EventStateTransition::STOP:
      lastTransition = transition;
      i = _runningEvents.find (event);
      if (i != _runningEvents.end ())
        {
          _runningEvents.erase (i);
        }

      if (_runningEvents.empty ())
        {
          NclEvent *lambda = this->getLambda (EventType::PRESENTATION);
          lambda->stop ();
          if (this->getParent () == nullptr)
            {
              TRACE ("*** ALL DONE ***");
              _ginga->setEOS (true);
            }
        }
      break;

    case EventStateTransition::PAUSE:
      i = _runningEvents.find (event);
      if (i != _runningEvents.end ())
        {
          _runningEvents.erase (i);
        }

      if (_runningEvents.empty ())
        {
          lambda->pause ();
        }
      break;

    case EventStateTransition::RESUME:
      _runningEvents.insert (event);
      if (_runningEvents.size () == 1)
        {
          lambda->resume ();
        }
      break;

    default:
      g_assert_not_reached ();
    }
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

GINGA_FORMATTER_END
