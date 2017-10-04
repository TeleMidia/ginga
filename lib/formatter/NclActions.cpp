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
#include "NclActions.h"

GINGA_FORMATTER_BEGIN

NclAction::NclAction ()
{
  _satisfiedCondition = nullptr;
}

void
NclAction::run (NclLinkCondition *satisfiedCondition)
{
  _satisfiedCondition = satisfiedCondition;
  run ();
}

void
NclAction::addProgressListener (INclActionListener *listener)
{
  auto i = find (_listeners.begin(), _listeners.end(),
                 listener);

  if (i != _listeners.end())
    {
      WARNING ("Trying to add the same listener twice.");
      return;
    }

  _listeners.push_back (listener);
}

void
NclAction::notifyProgressListeners (bool start)
{
  vector<INclActionListener *> notifyList (_listeners);

  for (size_t i = 0; i < notifyList.size (); i++)
    {
      notifyList[i]->actionProcessed (start);
    }
}

NclSimpleAction::NclSimpleAction (NclEvent *event, EventStateTransition type)
  : NclAction ()
{
  _event = event;
  _actType = type;
  _listener = nullptr;
}

NclEvent *
NclSimpleAction::getEvent ()
{
  return _event;
}

EventStateTransition
NclSimpleAction::getType ()
{
  return _actType;
}

void
NclSimpleAction::setSimpleActionListener (INclActionListener *listener)
{
  g_assert_nonnull (listener);
  this->_listener = listener;
}

vector<NclEvent *>
NclSimpleAction::getEvents ()
{
  vector<NclEvent *> events;
  if (_event)
    events.push_back (_event);
  return events;
}

vector<NclAction *>
NclSimpleAction::getImplicitRefRoleActions ()
{
  vector<NclAction *> actions;
  string attVal = "", durVal = "", byVal = "";

  auto assignmentAct = cast (NclAssignmentAction *, this);
  if (assignmentAct)
    {
      attVal = assignmentAct->getValue ();
      durVal = assignmentAct->getDuration ();

      if ((durVal != "" && durVal.substr (0, 1) == "$")
          || (attVal != "" && attVal.substr (0, 1) == "$"))
        {
          AttributionEvent *attrEvt = cast (AttributionEvent *, _event);
          if (attrEvt)
            {
              actions.push_back (this);
            }
        }
    }

  return actions;
}

void
NclSimpleAction::run ()
{
  if (_listener != nullptr)
    {
      g_assert_nonnull (_satisfiedCondition);
      _listener->scheduleAction (this);
    }

  if (_actType == EventStateTransition::START)
    {
      notifyProgressListeners (true);
    }
  else
    {
      notifyProgressListeners (false);
    }
}

// NclAssignmentAction

NclAssignmentAction::NclAssignmentAction (NclEvent *evt,
                                          EventStateTransition actType,
                                          const string &value,
                                          const string &duration)
  : NclSimpleAction (evt, actType)
{
  _value = value;
  _duration = duration;
}

string
NclAssignmentAction::getValue ()
{
  return _value;
}

string
NclAssignmentAction::getDuration ()
{
  return _duration;
}

NclCompoundAction::NclCompoundAction () : NclAction ()
{
  _hasStart = false;
  _listener = nullptr;
}

NclCompoundAction::~NclCompoundAction ()
{
  for (NclAction *action : _actions)
    delete action;
  _actions.clear ();
}

void
NclCompoundAction::addAction (NclAction *action)
{
  vector<NclAction *>::iterator i;

  action->addProgressListener (this);
  i = _actions.begin ();
  while (i != _actions.end ())
    {
      if (*i == action)
        {
          WARNING ("Trying to add same action twice.");
          return;
        }
      ++i;
    }

  _actions.push_back (action);
}

void
NclCompoundAction::getSimpleActions (vector<NclSimpleAction *> &simpleActions)
{
  for (NclAction *action : _actions)
    {
      auto simpleAct = cast (NclSimpleAction *, action);
      auto compoundAct = cast (NclCompoundAction *, action);
      if (compoundAct)
        {
          compoundAct->getSimpleActions (simpleActions);
        }
      else if (simpleAct)
        {
          simpleActions.push_back (simpleAct);
        }
    }
}

void
NclCompoundAction::setCompoundActionListener (
    INclActionListener *listener)
{
  _listener = listener;
}

vector<NclEvent *>
NclCompoundAction::getEvents ()
{
  vector<NclAction *> acts (_actions);
  vector<NclEvent *> events;

  for (NclAction *action : acts)
    {
      for (NclEvent *actEvt : action->getEvents ())
        {
          events.push_back (actEvt);
        }
    }

  return events;
}

vector<NclAction *>
NclCompoundAction::getImplicitRefRoleActions ()
{
  vector<NclAction *> refActs;
  vector<NclAction *> acts (_actions);

  for (NclAction *act: acts)
    {
      vector<NclAction *> assignmentActs = act->getImplicitRefRoleActions ();

      for (NclAction *assignmentAct : assignmentActs)
        {
          refActs.push_back (assignmentAct);
        }
    }

  return refActs;
}

void
NclCompoundAction::run ()
{
  size_t i, size;
  NclAction *action = nullptr;

  if (_actions.empty ())
    return;

  size = _actions.size ();
  _pendingActions = (int) size;
  _hasStart = false;

  for (i = 0; i < size; i++)
    {
      if (_actions.empty ())
        return;
      action = _actions.at (i);
      action->run (_satisfiedCondition);
    }
}

void
NclCompoundAction::actionProcessed (bool start)
{
  _pendingActions--;
  _hasStart = (_hasStart || start);
  if (_pendingActions == 0)
    {
      notifyProgressListeners (_hasStart);
    }
}

GINGA_FORMATTER_END
