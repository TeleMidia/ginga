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

#include "NclLinkCondition.h"
#include "NclLinkStatement.h"
#include "ginga-internal.h"

#include "mb/Display.h"
using namespace ginga::mb;

GINGA_FORMATTER_BEGIN

NclLinkTriggerCondition::NclLinkTriggerCondition () : NclLinkCondition ()
{
  _delay = 0;
  _listener = nullptr;
}

void
NclLinkTriggerCondition::conditionSatisfied (
    arg_unused (NclLinkCondition *condition))
{
  if (_delay > 0)
    ERROR_NOT_IMPLEMENTED ("condition delays are not supported");

  notifyListeners (NclLinkConditionStatus::CONDITION_SATISFIED);
}

void
NclLinkTriggerCondition::notifyListeners (NclLinkConditionStatus status)
{
  switch (status)
    {
    case NclLinkConditionStatus::CONDITION_SATISFIED:
      _listener->conditionSatisfied (this);
      break;

    case NclLinkConditionStatus::EVALUATION_STARTED:
      _listener->evaluationStarted ();
      break;

    case NclLinkConditionStatus::EVALUATION_ENDED:
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
  for (NclLinkCondition *condition : _conditions)
    {
      g_assert_nonnull (condition);
      delete condition;
    }
}

void
NclLinkCompoundTriggerCondition::addCondition (NclLinkCondition *condition)
{
  g_assert_nonnull (condition);

  _conditions.push_back (condition);

  auto linkTriggerCondition = cast (NclLinkTriggerCondition *, condition);
  if (linkTriggerCondition)
    {
      linkTriggerCondition->setTriggerListener (this);
    }
}

void
NclLinkCompoundTriggerCondition::conditionSatisfied (
    NclLinkCondition *condition)
{
  NclLinkTriggerCondition::conditionSatisfied (condition);
}

vector<NclEvent *>
NclLinkCompoundTriggerCondition::getEvents ()
{
  vector<NclEvent *> events;
  for (NclLinkCondition *condition : _conditions)
    {
      for (NclEvent *evt : condition->getEvents ())
        {
          events.push_back (evt);
        }
    }

  return events;
}

void
NclLinkCompoundTriggerCondition::evaluationStarted ()
{
  notifyListeners (NclLinkConditionStatus::EVALUATION_STARTED);
}

void
NclLinkCompoundTriggerCondition::evaluationEnded ()
{
  notifyListeners (NclLinkConditionStatus::EVALUATION_ENDED);
}

NclLinkAndCompoundTriggerCondition::NclLinkAndCompoundTriggerCondition ()
    : NclLinkCompoundTriggerCondition ()
{
}

NclLinkAndCompoundTriggerCondition::~NclLinkAndCompoundTriggerCondition ()
{
  for (NclLinkCondition *l : _statements)
    {
      g_assert_nonnull (l);
      delete l;
    }
}

void
NclLinkAndCompoundTriggerCondition::addCondition (
    NclLinkCondition *condition)
{
  if (condition == nullptr)
    {
      return;
    }

  if (instanceof (NclLinkTriggerCondition *, condition))
    {
      _unsatisfiedConditions.push_back (condition);
      NclLinkCompoundTriggerCondition::addCondition (condition);
    }
  else if (instanceof (NclLinkStatement *, condition))
    {
      _statements.push_back (condition);
    }
  else
    {
      WARNING (
          "Trying to add a condition !instanceof(NclLinkStatement) and "
          "!instanceof(NclLinkTriggerCondition)");
    }
}

void
NclLinkAndCompoundTriggerCondition::conditionSatisfied (
    NclLinkCondition *condition)
{
  auto i = _unsatisfiedConditions.begin ();
  while (i != _unsatisfiedConditions.end ())
    {
      if ((*i) == condition)
        {
          i = _unsatisfiedConditions.erase (i);
        }
      else
        {
          ++i;
        }
    }

  if (_unsatisfiedConditions.empty ())
    {
      for (i = _conditions.begin (); i != _conditions.end (); ++i)
        {
          _unsatisfiedConditions.push_back (*i);
        }

      for (i = _statements.begin (); i != _statements.end (); ++i)
        {
          NclLinkStatement *statement = cast (NclLinkStatement *, *i);
          if (!statement->evaluate ())
            {
              notifyListeners (NclLinkConditionStatus::EVALUATION_ENDED);

              return;
            }
        }

      NclLinkTriggerCondition::conditionSatisfied (condition);
    }
  else
    {
      notifyListeners (NclLinkConditionStatus::EVALUATION_ENDED);
    }
}

vector<NclEvent *>
NclLinkAndCompoundTriggerCondition::getEvents ()
{
  vector<NclEvent *> events = NclLinkCompoundTriggerCondition::getEvents ();
  for (NclLinkCondition *cond : _statements)
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
  this->_bind = bind;
  this->_event = nullptr;
  this->_transition = transition;

  if (NclEvent::hasInstance (event, false))
    {
      this->_event = event;
      this->_event->addListener (this);
    }
  else
    {
      ERROR ("Creating a link with null event.");
    }
}

NclLinkTransitionTriggerCondition::~NclLinkTransitionTriggerCondition ()
{
  if (NclEvent::hasInstance (_event, false))
    {
      _event->removeListener (this);
    }
}

Bind *
NclLinkTransitionTriggerCondition::getBind ()
{
  return _bind;
}

void
NclLinkTransitionTriggerCondition::eventStateChanged (
    arg_unused (NclEvent *_event), EventStateTransition transition,
    arg_unused (EventState previousState))
{
  if (this->_transition == transition)
    {
      notifyListeners (NclLinkConditionStatus::EVALUATION_STARTED);
      NclLinkTriggerCondition::conditionSatisfied (this);
    }
}

NclEvent *
NclLinkTransitionTriggerCondition::getEvent ()
{
  return _event;
}

EventStateTransition
NclLinkTransitionTriggerCondition::getTransition ()
{
  return _transition;
}

vector<NclEvent *>
NclLinkTransitionTriggerCondition::getEvents ()
{
  vector<NclEvent *> events;

  if (NclEvent::hasInstance (_event, false))
    events.push_back (_event);

  return events;
}

GINGA_FORMATTER_END
