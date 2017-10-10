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
  Context *context;
  Entity *entity;

  _links.clear ();
  _uncompiledLinks.clear ();
  _runningEvents.clear ();
  _pausedEvents.clear ();
  _pendingLinks.clear ();

  entity = cast (Entity *, dataObject);
  g_assert_nonnull (entity);

  if (!instanceof (Context *, entity))
    return;                     // switch, nothing to do

  context = cast (Context *, entity);
  g_assert_nonnull (context);

  g_assert_nonnull (context->getLinks ());
  for (auto link: *context->getLinks ())
    _uncompiledLinks.insert (link);
}

ExecutionObjectContext::~ExecutionObjectContext ()
{
  // ExecutionObject *object;
  NclLink *link;
  set<NclLink *>::iterator i;
  map<string, ExecutionObject *>::iterator j;

  _runningEvents.clear ();
  _pausedEvents.clear ();
  _pendingLinks.clear ();

  i = _links.begin ();
  while (i != _links.end ())
    {
      link = *i;
      if (link != NULL)
        {
          delete link;
          link = NULL;
        }
      ++i;
    }
  _links.clear ();
  _uncompiledLinks.clear ();
}

void
ExecutionObjectContext::suspendLinkEvaluation (bool suspend)
{
  for (NclLink *link : _links)
    link->disable (suspend);
}

set<Link *> *
ExecutionObjectContext::getUncompiledLinks ()
{
  set<Link *> *uLinks = new set<Link *> (_uncompiledLinks);
  return uLinks;
}

bool
ExecutionObjectContext::containsUncompiledLink (Link *dataLink)
{
  if (_uncompiledLinks.count (dataLink) != 0)
    return true;
  return false;
}

void
ExecutionObjectContext::removeLinkUncompiled (Link *ncmLink)
{
  set<Link *>::iterator i;
  i = _uncompiledLinks.find (ncmLink);
  if (i != _uncompiledLinks.end ())
    {
      _uncompiledLinks.erase (i);
      return;
    }
}

void
ExecutionObjectContext::setLinkCompiled (NclLink *link)
{
  g_assert_nonnull (link);
  _links.insert (link);
}

void
ExecutionObjectContext::eventStateChanged (
    NclEvent *event,
    EventStateTransition transition,
    EventState previousState)
{
  set<NclEvent *>::iterator i;
  NclEvent *lambda = this->getLambda ();

  if (!instanceof (PresentationEvent *, event))
    return;

  switch (transition)
    {
    case EventStateTransition::START:
      if (_runningEvents.empty () && _pausedEvents.empty ())
        {
          lambda->addListener (_parent);
          lambda->start ();
        }

      _runningEvents.insert (event);
      break;

    case EventStateTransition::ABORT:
      lastTransition = transition;
      if (previousState == EventState::OCCURRING)
        {
          i = _runningEvents.find (event);
          if (i != _runningEvents.end ())
            {
              _runningEvents.erase (i);
            }
        }
      else if (previousState == EventState::PAUSED)
        {
          i = _pausedEvents.find (event);
          if (i != _pausedEvents.end ())
            {
              _pausedEvents.erase (i);
            }
        }

      if (_runningEvents.empty () && _pausedEvents.empty ()
          && _pendingLinks.empty ())
        {
          lambda->abort ();
        }
      break;

    case EventStateTransition::STOP:
      if (((PresentationEvent *)event)->getRepetitions () == 0)
        {
          lastTransition = transition;
          if (previousState == EventState::OCCURRING)
            {
              i = _runningEvents.find (event);
              if (i != _runningEvents.end ())
                {
                  _runningEvents.erase (i);
                }
            }
          else if (previousState == EventState::PAUSED)
            {
              i = _pausedEvents.find (event);
              if (i != _pausedEvents.end ())
                {
                  _pausedEvents.erase (i);
                }
            }

          if (_runningEvents.empty () && _pausedEvents.empty ()
              && _pendingLinks.empty ())
            {
              checkLinkConditions ();
            }
        }
      break;

    case EventStateTransition::PAUSE:
      i = _runningEvents.find (event);
      if (i != _runningEvents.end ())
        {
          _runningEvents.erase (i);
        }

      _pausedEvents.insert (event);
      if (_runningEvents.empty ())
        {
          lambda->pause ();
        }
      break;

    case EventStateTransition::RESUME:
      i = _pausedEvents.find (event);
      if (i != _pausedEvents.end ())
        {
          _pausedEvents.erase (i);
        }

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

void
ExecutionObjectContext::checkLinkConditions ()
{
  if ((_runningEvents.empty () && _pausedEvents.empty ()
       && _pendingLinks.empty ()))
    {
      NclEvent *lambda = this->getLambda ();
      lambda->stop ();
      if (this->getParent () == nullptr)
        {
          TRACE ("*** ALL DONE ***");
          _ginga->setEOS (true);
        }
    }
}


GINGA_FORMATTER_END
