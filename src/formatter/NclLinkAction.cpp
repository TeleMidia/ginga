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
#include "NclLinkAction.h"

GINGA_FORMATTER_BEGIN

NclLinkAction::NclLinkAction (GingaTime delay)
{
  initLinkAction (delay);
}

void
NclLinkAction::initLinkAction (GingaTime delay)
{
  satisfiedCondition = nullptr;
  this->delay = delay;
  typeSet.insert ("NclLinkAction");
}

bool
NclLinkAction::instanceOf (const string &s)
{
  if (typeSet.empty ())
    {
      return false;
    }
  else
    {
      return (typeSet.find (s) != typeSet.end ());
    }
}

void
NclLinkAction::setSatisfiedCondition (NclLinkCondition *satisfiedCondition)
{
  this->satisfiedCondition = satisfiedCondition;
}

void
NclLinkAction::run (NclLinkCondition *satisfiedCondition)
{
  this->satisfiedCondition = satisfiedCondition;
  run ();
}

void
NclLinkAction::setWaitDelay (GingaTime delay)
{
  this->delay = delay;
}

void
NclLinkAction::addActionProgressionListener (
    NclLinkActionProgressListener *listener)
{
  auto i = find (progressListeners.begin(), progressListeners.end(),
                 listener);

  if (i != progressListeners.end())
    {
      WARNING ("Trying to add the same listener twice.");
      return;
    }

  progressListeners.push_back (listener);
}

void
NclLinkAction::removeActionProgressionListener (
    NclLinkActionProgressListener *listener)
{
  xvectremove (progressListeners, listener);
}

void
NclLinkAction::notifyProgressionListeners (bool start)
{
  vector<NclLinkActionProgressListener *> notifyList (progressListeners);

  for (size_t i = 0; i < notifyList.size (); i++)
    {
      notifyList[i]->actionProcessed (start);
    }
}

NclLinkSimpleAction::NclLinkSimpleAction (NclEvent *event,
                                          SimpleActionType type)
    : NclLinkAction (0.)
{
  this->event = event;
  this->actionType = type;
  this->listener = nullptr;

  typeSet.insert ("NclLinkSimpleAction");
}

NclLinkSimpleAction::~NclLinkSimpleAction ()
{
  if (listener != nullptr)
    {
      listener->removeAction (this);
    }
}

NclEvent *
NclLinkSimpleAction::getEvent ()
{
  return event;
}

SimpleActionType
NclLinkSimpleAction::getType ()
{
  return actionType;
}

string
NclLinkSimpleAction::getTypeString ()
{
  return SimpleAction::actionTypeToString (this->actionType);
}

void
NclLinkSimpleAction::setSimpleActionListener (
    INclLinkActionListener *listener)
{
  g_assert_nonnull (listener);
  listener->addAction (this);
  this->listener = listener;
}

vector<NclEvent *> *
NclLinkSimpleAction::getEvents ()
{
  if (event == nullptr)
    return nullptr;

  vector<NclEvent *> *events = new vector<NclEvent *>;

  events->push_back (event);
  return events;
}

vector<NclLinkAction *>
NclLinkSimpleAction::getImplicitRefRoleActions ()
{
  vector<NclLinkAction *> actions;
  string attVal = "", durVal = "", byVal = "";
  Animation *anim;

  auto assignmentAct = dynamic_cast<NclLinkAssignmentAction *> (this);
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
          AttributionEvent *attrEvt = dynamic_cast<AttributionEvent *> (event);
          if (attrEvt)
            {
              actions.push_back (this);
            }
        }
    }

  return actions;
}

void
NclLinkSimpleAction::run ()
{
  NclLinkAction::run ();

  if (listener != NULL)
    {
      g_assert_nonnull (satisfiedCondition);
      listener->scheduleAction (this);
    }

  if (actionType == ACT_START)
    {
      notifyProgressionListeners (true);
    }
  else
    {
      notifyProgressionListeners (false);
    }
}

NclLinkRepeatAction::NclLinkRepeatAction (NclEvent *evt,
                                          SimpleActionType actType)
    : NclLinkSimpleAction (evt, actType)
{
  this->repetitions = 0;
  this->repetitionInterval = 0;

  typeSet.insert ("NclLinkRepeatAction");
}

NclLinkRepeatAction::~NclLinkRepeatAction ()
{
}

void
NclLinkRepeatAction::setRepetitions (int repetitions)
{
  this->repetitions = repetitions;
}

void
NclLinkRepeatAction::setRepetitionInterval (GingaTime delay)
{
  this->repetitionInterval = delay;
}

void
NclLinkRepeatAction::run ()
{
  if (event != nullptr)
    {
      auto presentationEvt = dynamic_cast <PresentationEvent *> (event);
      if (presentationEvt)
        {
          presentationEvt->setRepetitionSettings (repetitions,
                                                  repetitionInterval);
        }
    }
  else
    {
      g_assert_not_reached ();
    }

  NclLinkSimpleAction::run ();
}

NclLinkAssignmentAction::NclLinkAssignmentAction (NclEvent *evt,
                                                  SimpleActionType actType,
                                                  const string &value)
    : NclLinkRepeatAction (evt, actType)
{
  this->value = value;
  this->animation = nullptr;
  typeSet.insert ("NclLinkAssignmentAction");
}

NclLinkAssignmentAction::~NclLinkAssignmentAction ()
{
  if (animation != nullptr)
    {
      delete animation;
    }
}

string
NclLinkAssignmentAction::getValue ()
{
  return value;
}

Animation *
NclLinkAssignmentAction::getAnimation ()
{
  return animation;
}

void
NclLinkAssignmentAction::setAnimation (Animation *animation)
{
  this->animation = animation;
}

NclLinkCompoundAction::NclLinkCompoundAction (short op) : NclLinkAction (0.)
{
  this->_op = op;
  _hasStart = false;
  _running = false;
  _listener = NULL;
  typeSet.insert ("NclLinkCompoundAction");
}

NclLinkCompoundAction::~NclLinkCompoundAction ()
{
  for (NclLinkAction *action : _actions)
    {
      action->removeActionProgressionListener (this);
      delete action;
    }

  _actions.clear ();
}

void
NclLinkCompoundAction::addAction (NclLinkAction *action)
{
  vector<NclLinkAction *>::iterator i;

  if (_running)
    {
      WARNING ("Can't add action: status = running.");
      return;
    }

  action->addActionProgressionListener (this);
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
NclLinkCompoundAction::getSimpleActions (
    vector<NclLinkSimpleAction *> *simpleActions)
{
  vector<NclLinkAction *>::iterator i;
  NclLinkAction *currentAction;

  if (_actions.empty ())
    {
      return;
    }

  i = _actions.begin ();
  while (i != _actions.end ())
    {
      currentAction = (*i);

      auto simpleAct = dynamic_cast<NclLinkSimpleAction *> (currentAction);
      auto compoundAct = dynamic_cast<NclLinkCompoundAction *> (currentAction);
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
NclLinkCompoundAction::setCompoundActionListener (
    INclLinkActionListener *listener)
{
  this->_listener = listener;
}

vector<NclEvent *> *
NclLinkCompoundAction::getEvents ()
{
  vector<NclLinkAction *> *acts;
  vector<NclLinkAction *>::iterator i;
  NclLinkAction *action;
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

  acts = new vector<NclLinkAction *> (_actions);
  events = new vector<NclEvent *>;

  for (i = acts->begin (); i != acts->end (); ++i)
    {
      action = (NclLinkAction *)(*i);
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

vector<NclLinkAction *>
NclLinkCompoundAction::getImplicitRefRoleActions ()
{
  vector<NclLinkAction *> refActs;
  vector<NclLinkAction *> acts (_actions);

  if (_running)
    {
      return refActs;
    }

  for (NclLinkAction *act: acts)
    {
      vector<NclLinkAction *> assignmentActs
          = act->getImplicitRefRoleActions ();

      for (NclLinkAction *assignmentAct : assignmentActs)
        {
          refActs.push_back (assignmentAct);
        }
    }

  return refActs;
}

void
NclLinkCompoundAction::run ()
{
  int i, size;
  NclLinkAction *action = nullptr;

  _running = true;

  NclLinkAction::run ();

  if (_actions.empty ())
    {
      TRACE ("There is no action to run.");
      return;
    }

  size = (int) _actions.size ();
  _pendingActions = size;
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
          action->setSatisfiedCondition (satisfiedCondition);
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
          action->run (satisfiedCondition);
        }
    }
}

void
NclLinkCompoundAction::actionProcessed (bool start)
{
  _pendingActions--;
  _hasStart = (_hasStart || start);
  if (_pendingActions == 0)
    {
      notifyProgressionListeners (_hasStart);
    }
}

GINGA_FORMATTER_END
