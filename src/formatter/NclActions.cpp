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
#include "NclActions.h"

GINGA_FORMATTER_BEGIN

NclAction::NclAction (GingaTime delay)
{
  _satisfiedCondition = nullptr;
  this->_delay = delay;
}

void
NclAction::setSatisfiedCondition (NclLinkCondition *satisfiedCondition)
{
  this->_satisfiedCondition = satisfiedCondition;
}

void
NclAction::run (NclLinkCondition *satisfiedCondition)
{
  this->_satisfiedCondition = satisfiedCondition;
  run ();
}

void
NclAction::setWaitDelay (GingaTime delay)
{
  this->_delay = delay;
}

void
NclAction::addProgressListener (
    NclActionProgressListener *listener)
{
  auto i = find (_progressListeners.begin(), _progressListeners.end(),
                 listener);

  if (i != _progressListeners.end())
    {
      WARNING ("Trying to add the same listener twice.");
      return;
    }

  _progressListeners.push_back (listener);
}

void
NclAction::removeProgressListener (
    NclActionProgressListener *listener)
{
  xvectremove (_progressListeners, listener);
}

void
NclAction::notifyProgressListeners (bool start)
{
  vector<NclActionProgressListener *> notifyList (_progressListeners);

  for (size_t i = 0; i < notifyList.size (); i++)
    {
      notifyList[i]->actionProcessed (start);
    }
}

NclSimpleAction::NclSimpleAction (NclEvent *event, SimpleAction::Type type)
    : NclAction (0.)
{
  this->_event = event;
  this->_actType = type;
  this->listener = nullptr;
}

NclSimpleAction::~NclSimpleAction ()
{
}

NclEvent *
NclSimpleAction::getEvent ()
{
  return _event;
}

SimpleAction::Type
NclSimpleAction::getType ()
{
  return _actType;
}

void
NclSimpleAction::setSimpleActionListener (INclActionListener *listener)
{
  g_assert_nonnull (listener);
  this->listener = listener;
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
  NclAction::run ();

  if (listener != NULL)
    {
      g_assert_nonnull (_satisfiedCondition);
      listener->scheduleAction (this);
    }

  if (_actType == SimpleAction::START)
    {
      notifyProgressListeners (true);
    }
  else
    {
      notifyProgressListeners (false);
    }
}

NclRepeatAction::NclRepeatAction (NclEvent *evt,
                                  SimpleAction::Type actType)
    : NclSimpleAction (evt, actType)
{
  this->_repetitions = 0;
  this->_repetitionInterval = 0;
}

NclRepeatAction::~NclRepeatAction ()
{
}

void
NclRepeatAction::setRepetitions (int repetitions)
{
  this->_repetitions = repetitions;
}

void
NclRepeatAction::setRepetitionInterval (GingaTime delay)
{
  this->_repetitionInterval = delay;
}

void
NclRepeatAction::run ()
{
  if (_event != nullptr)
    {
      auto presentationEvt = cast (PresentationEvent *, _event);
      if (presentationEvt)
        {
          presentationEvt->setRepetitionSettings (_repetitions,
                                                  _repetitionInterval);
        }
    }
  else
    {
      g_assert_not_reached ();
    }

  NclSimpleAction::run ();
}

NclAssignmentAction::NclAssignmentAction (NclEvent *evt,
                                          SimpleAction::Type actType,
                                          const string &value,
                                          const string &duration)
    : NclRepeatAction (evt, actType)
{
  _value = value;
  _duration = duration;
}

NclAssignmentAction::~NclAssignmentAction ()
{
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

NclCompoundAction::NclCompoundAction () : NclAction (0.)
{
  _hasStart = false;
  _running = false;
  _listener = NULL;
}

NclCompoundAction::~NclCompoundAction ()
{
  for (NclAction *action : _actions)
    {
      action->removeProgressListener (this);
      delete action;
    }

  _actions.clear ();
}

void
NclCompoundAction::addAction (NclAction *action)
{
  vector<NclAction *>::iterator i;

  if (_running)
    {
      WARNING ("Can't add action: status = running.");
      return;
    }

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
NclCompoundAction::getSimpleActions (
    vector<NclSimpleAction *> *simpleActions)
{
  vector<NclAction *>::iterator i;
  NclAction *currentAction;

  if (_actions.empty ())
    {
      return;
    }

  i = _actions.begin ();
  while (i != _actions.end ())
    {
      currentAction = (*i);

      auto simpleAct = cast (NclSimpleAction *, currentAction);
      auto compoundAct = cast (NclCompoundAction *, currentAction);
      if (compoundAct)
        {
          compoundAct->getSimpleActions (simpleActions);
        }
      else if (simpleAct)
        {
          simpleActions->push_back (simpleAct);
        }

      ++i;
    }
}

void
NclCompoundAction::setCompoundActionListener (
    INclActionListener *listener)
{
  this->_listener = listener;
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

  if (_running)
    {
      return refActs;
    }

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

  _running = true;

  NclAction::run ();

  if (_actions.empty ())
    {
      TRACE ("There is no action to run.");
      return;
    }

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
