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

#include "ginga.h"
#include "NclLinkTriggerCondition.h"

#include "mb/Display.h"
using namespace ginga::mb;

GINGA_FORMATTER_BEGIN

NclLinkTriggerCondition::NclLinkTriggerCondition () : NclLinkCondition ()
{
  _delay = 0;
  _listener = NULL;
}

void
NclLinkTriggerCondition::setTriggerListener (
    NclLinkTriggerListener *listener)
{
  _listener = listener;
}

NclLinkTriggerListener *
NclLinkTriggerCondition::getTriggerListener ()
{
  return _listener;
}

GingaTime
NclLinkTriggerCondition::getDelay ()
{
  return _delay;
}

void
NclLinkTriggerCondition::setDelay (GingaTime delay)
{
  _delay = delay;
}

void
NclLinkTriggerCondition::conditionSatisfied (arg_unused (NclLinkCondition *condition))
{
  if (_delay > 0)
    ERROR_NOT_IMPLEMENTED ("condition delays are not supported");
  notifyConditionObservers (NclLinkTriggerListener::CONDITION_SATISFIED);
}

void
NclLinkTriggerCondition::notifyConditionObservers (short status)
{
  switch (status)
    {
    case NclLinkTriggerListener::CONDITION_SATISFIED:
      _listener->conditionSatisfied (this);
      break;

    case NclLinkTriggerListener::EVALUATION_STARTED:
      _listener->evaluationStarted ();
      break;

    case NclLinkTriggerListener::EVALUATION_ENDED:
      _listener->evaluationEnded ();
      break;

    default:
      g_assert_not_reached ();
    }
}


NclLinkCompoundTriggerCondition::NclLinkCompoundTriggerCondition ()
    : NclLinkTriggerCondition ()
{
}

NclLinkCompoundTriggerCondition::~NclLinkCompoundTriggerCondition ()
{
  for (NclLinkCondition *condition : conditions)
    {
      g_assert_nonnull (condition);
      delete condition;
    }
}

void
NclLinkCompoundTriggerCondition::addCondition (NclLinkCondition *condition)
{
  g_assert_nonnull (condition);

  conditions.push_back (condition);
  if (instanceof (NclLinkTriggerCondition *, condition))
    {
      ((NclLinkTriggerCondition *)condition)->setTriggerListener (this);
    }
}

vector<NclEvent *>
NclLinkCompoundTriggerCondition::getEvents ()
{
  vector<NclEvent *> events;
  for (NclLinkCondition *condition : conditions)
    {
      for (NclEvent *evt : condition->getEvents ())
        {
          events.push_back (evt);
        }
    }

  return events;
}

void
NclLinkCompoundTriggerCondition::conditionSatisfied (NclLinkCondition *condition)
{
  NclLinkTriggerCondition::conditionSatisfied (condition);
}

void
NclLinkCompoundTriggerCondition::evaluationStarted ()
{
  notifyConditionObservers (NclLinkTriggerListener::EVALUATION_STARTED);
}

void
NclLinkCompoundTriggerCondition::evaluationEnded ()
{
  notifyConditionObservers (NclLinkTriggerListener::EVALUATION_ENDED);
}

NclLinkAndCompoundTriggerCondition::NclLinkAndCompoundTriggerCondition ()
    : NclLinkCompoundTriggerCondition ()
{
}

NclLinkAndCompoundTriggerCondition::~NclLinkAndCompoundTriggerCondition ()
{
  vector<NclLinkCondition *>::iterator i;
  NclLinkCondition *l;

  unsatisfiedConditions.clear ();

  i = statements.begin ();
  while (i != statements.end ())
    {
      l = *i;
      if (l != NULL)
        {
          delete l;
          l = NULL;
        }
      ++i;
    }

  statements.clear ();
}

void
NclLinkAndCompoundTriggerCondition::addCondition (NclLinkCondition *condition)
{
  if (condition == NULL)
    {
      return;
    }

  if (instanceof (NclLinkTriggerCondition *, condition))
    {
      unsatisfiedConditions.push_back (condition);
      NclLinkCompoundTriggerCondition::addCondition (condition);
    }
  else if (instanceof (NclLinkStatement *, condition))
    {
      statements.push_back (condition);
    }
  else
    {
      WARNING ("Trying to add a condition !instanceof(NclLinkStatement) and "
               "!instanceof(NclLinkTriggerCondition)");
    }
}

void
NclLinkAndCompoundTriggerCondition::conditionSatisfied (
    NclLinkCondition *condition)
{
  vector<NclLinkCondition *>::iterator i;
  i = unsatisfiedConditions.begin ();
  while (i != unsatisfiedConditions.end ())
    {
      if ((*i) == condition)
        {
          unsatisfiedConditions.erase (i);
          if (unsatisfiedConditions.empty ())
            {
              break;
            }
          else
            {
              i = unsatisfiedConditions.begin ();
            }
        }
      else
        {
          ++i;
        }
    }

  if (unsatisfiedConditions.empty ())
    {
      for (i = conditions.begin (); i != conditions.end (); ++i)
        {
          unsatisfiedConditions.push_back (*i);
        }

      for (i = statements.begin (); i != statements.end (); ++i)
        {
          if (!(((NclLinkStatement *)(*i))->evaluate ()))
            {
              notifyConditionObservers (
                  NclLinkTriggerListener::EVALUATION_ENDED);

              return;
            }
        }

      NclLinkTriggerCondition::conditionSatisfied (condition);
    }
  else
    {
      notifyConditionObservers (NclLinkTriggerListener::EVALUATION_ENDED);
    }
}

vector<NclEvent *>
NclLinkAndCompoundTriggerCondition::getEvents ()
{
  vector<NclEvent *> events = NclLinkCompoundTriggerCondition::getEvents ();
  for (NclLinkCondition *cond : statements)
    {
      for (NclEvent *evt : cond->getEvents ())
        {
          events.push_back (evt);
        }
    }

  return events;
}

NclLinkTransitionTriggerCondition::NclLinkTransitionTriggerCondition (
    NclEvent *event, EventStateTransition transition, Bind *bind)
    : NclLinkTriggerCondition ()
{
  this->bind = bind;
  this->event = NULL;
  this->transition = transition;

  if (NclEvent::hasInstance (event, false))
    {
      this->event = event;
      this->event->addListener (this);
    }
  else
    {
      clog << "NclLinkTransitionTriggerCondition::";
      clog << "NclLinkTransitionTriggerCondition Warning! ";
      clog << "creating a link with NULL event" << endl;
    }
}

NclLinkTransitionTriggerCondition::~NclLinkTransitionTriggerCondition ()
{
  _listener = NULL;
  bind = NULL;

  if (NclEvent::hasInstance (event, false))
    {
      event->removeListener (this);
      event = NULL;
    }
}

Bind *
NclLinkTransitionTriggerCondition::getBind ()
{
  return bind;
}

void
NclLinkTransitionTriggerCondition::eventStateChanged (
    arg_unused (NclEvent *event),
    EventStateTransition transition,
    arg_unused (EventState previousState))
{
  if (this->transition == transition)
    {
      notifyConditionObservers (NclLinkTriggerListener::EVALUATION_STARTED);

      NclLinkTriggerCondition::conditionSatisfied (this);
    }
}

NclEvent *
NclLinkTransitionTriggerCondition::getEvent ()
{
  return event;
}

EventStateTransition
NclLinkTransitionTriggerCondition::getTransition ()
{
  return transition;
}

vector<NclEvent *>
NclLinkTransitionTriggerCondition::getEvents ()
{
  vector<NclEvent *> events;

  if (NclEvent::hasInstance (event, false))
    events.push_back (event);

  return events;
}

GINGA_FORMATTER_END
