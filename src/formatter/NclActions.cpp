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
  _typeSet.insert ("NclLinkAction");
}

bool
NclAction::instanceOf (const string &s)
{
  return (_typeSet.find (s) != _typeSet.end ());
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

NclSimpleAction::NclSimpleAction (NclEvent *event,
                                          SimpleActionType type)
    : NclAction (0.)
{
  this->_event = event;
  this->_actType = type;
  this->listener = nullptr;

  _typeSet.insert ("NclLinkSimpleAction");
}

NclSimpleAction::~NclSimpleAction ()
{
  if (listener != nullptr)
    {
      listener->removeAction (this);
    }
}

NclEvent *
NclSimpleAction::getEvent ()
{
  return _event;
}

SimpleActionType
NclSimpleAction::getType ()
{
  return _actType;
}

string
NclSimpleAction::getTypeString ()
{
  return SimpleAction::actionTypeToString (this->_actType);
}

void
NclSimpleAction::setSimpleActionListener (INclActionListener *listener)
{
  g_assert_nonnull (listener);
  listener->addAction (this);
  this->listener = listener;
}

vector<NclEvent *> *
NclSimpleAction::getEvents ()
{
  if (_event == nullptr)
    return nullptr;

  vector<NclEvent *> *events = new vector<NclEvent *>;

  events->push_back (_event);
  return events;
}

vector<NclAction *>
NclSimpleAction::getImplicitRefRoleActions ()
{
  vector<NclAction *> actions;
  string attVal = "", durVal = "", byVal = "";
  Animation *anim;

  auto assignmentAct = dynamic_cast<NclAssignmentAction *> (this);
  if (assignmentAct)
    {
      attVal = assignmentAct->getValue ();
      anim = assignmentAct->getAnimation ();

      if (anim != NULL)
        {
          durVal = anim->getDuration ();
          byVal = anim->getBy ();
        }

      if ((byVal != "" && byVal.substr (0, 1) == "$")
          || (durVal != "" && durVal.substr (0, 1) == "$")
          || (attVal != "" && attVal.substr (0, 1) == "$"))
        {
          AttributionEvent *attrEvt = dynamic_cast<AttributionEvent *> (_event);
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

  if (_actType == ACT_START)
    {
      notifyProgressListeners (true);
    }
  else
    {
      notifyProgressListeners (false);
    }
}

NclRepeatAction::NclRepeatAction (NclEvent *evt,
                                          SimpleActionType actType)
    : NclSimpleAction (evt, actType)
{
  this->_repetitions = 0;
  this->_repetitionInterval = 0;

  _typeSet.insert ("NclLinkRepeatAction");
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
      auto presentationEvt = dynamic_cast <PresentationEvent *> (_event);
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
                                                  SimpleActionType actType,
                                                  const string &value)
    : NclRepeatAction (evt, actType)
{
  this->_value = value;
  this->_anim = nullptr;
  _typeSet.insert ("NclLinkAssignmentAction");
}

NclAssignmentAction::~NclAssignmentAction ()
{
  if (_anim != nullptr)
    {
      delete _anim;
    }
}

string
NclAssignmentAction::getValue ()
{
  return _value;
}

Animation *
NclAssignmentAction::getAnimation ()
{
  return _anim;
}

void
NclAssignmentAction::setAnimation (Animation *animation)
{
  this->_anim = animation;
}

NclCompoundAction::NclCompoundAction (short op) : NclAction (0.)
{
  this->_op = op;
  _hasStart = false;
  _running = false;
  _listener = NULL;
  _typeSet.insert ("NclLinkCompoundAction");
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

      auto simpleAct = dynamic_cast<NclSimpleAction *> (currentAction);
      auto compoundAct = dynamic_cast<NclCompoundAction *> (currentAction);
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

vector<NclEvent *> *
NclCompoundAction::getEvents ()
{
  vector<NclAction *> *acts;
  vector<NclAction *>::iterator i;
  NclAction *action;
  vector<NclEvent *> *events;
  vector<NclEvent *> *actionEvents;
  vector<NclEvent *>::iterator j;

  if (_running)
    {
      return nullptr;
    }

  if (_actions.empty ())
    {
      return nullptr;
    }

  acts = new vector<NclAction *> (_actions);
  events = new vector<NclEvent *>;

  for (i = acts->begin (); i != acts->end (); ++i)
    {
      action = (NclAction *)(*i);
      actionEvents = action->getEvents ();
      if (actionEvents != nullptr)
        {
          for (j = actionEvents->begin (); j != actionEvents->end (); ++j)
            {
              events->push_back (*j);
            }
          delete actionEvents;
          actionEvents = nullptr;
        }
    }

  delete acts;
  if (events->empty ())
    {
      delete events;
      return nullptr;
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
      vector<NclAction *> assignmentActs
          = act->getImplicitRefRoleActions ();

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

  if (_op == CompoundAction::OP_PAR)
    {
      for (i = 0; i < size; i++)
        {

          if (_actions.empty ())
            {
              return;
            }

          action = _actions.at (i);
          action->setSatisfiedCondition (_satisfiedCondition);
          action->run ();

        }
    }
  else
    {
      for (i = 0; i < size; i++)
        {

          if (_actions.empty ())
            {
              return;
            }

          action = _actions.at (i);
          action->run (_satisfiedCondition);
        }
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
