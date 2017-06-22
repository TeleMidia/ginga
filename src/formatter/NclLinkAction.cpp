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

NclLinkAction::~NclLinkAction () {}

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

GingaTime
NclLinkAction::getWaitDelay ()
{
  return this->delay;
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

bool
NclLinkAction::hasDelay ()
{
  return this->delay > 0;
}

void
NclLinkAction::addActionProgressionListener (
    NclLinkActionProgressListener *listener)
{
  auto i = progressListeners.begin ();
  while (i != progressListeners.end ())
    {
      if (listener == *i)
        {
          WARNING ("Trying to add the same listener twice.");
          return;
        }
      ++i;
    }
  progressListeners.push_back (listener);
}

void
NclLinkAction::removeActionProgressionListener (
    NclLinkActionProgressListener *listener)
{
  for (auto i = progressListeners.begin ();
       i != progressListeners.end (); ++i)
    {
      if (*i == listener)
        {
          progressListeners.erase (i);
          break;
        }
    }
}

void
NclLinkAction::notifyProgressionListeners (bool start)
{
  NclLinkActionProgressListener *listener;
  vector<NclLinkActionProgressListener *> notifyList (progressListeners);

  for (size_t i = 0; i < notifyList.size (); i++)
    {
      listener = notifyList[i];
      listener->actionProcessed (start);
    }
}

NclLinkSimpleAction::NclLinkSimpleAction (NclEvent *event,
                                          SimpleActionType type)
    : NclLinkAction (0.)
{
  this->event = event;
  this->actionType = type;
  this->listener = NULL;

  typeSet.insert ("NclLinkSimpleAction");
}

NclLinkSimpleAction::~NclLinkSimpleAction ()
{
  if (listener != NULL)
    {
      listener->removeAction (this);
    }

  listener = NULL;
  event = NULL;
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
  if (listener != NULL)
    {
      listener->addAction (this);
    }

  this->listener = listener;
}

vector<NclEvent *> *
NclLinkSimpleAction::getEvents ()
{
  if (event == NULL)
    return NULL;

  vector<NclEvent *> *events;
  events = new vector<NclEvent *>;

  events->push_back (event);
  return events;
}

vector<NclLinkAction *>
NclLinkSimpleAction::getImplicitRefRoleActions ()
{
  vector<NclLinkAction *> actions;
  string attVal = "", durVal = "", byVal = "";
  Animation *anim;

  if (this->instanceOf ("NclLinkAssignmentAction"))
    {
      attVal = ((NclLinkAssignmentAction *)this)->getValue ();
      anim = ((NclLinkAssignmentAction *)this)->getAnimation ();

      if (anim != NULL)
        {
          durVal = anim->getDuration ();
          byVal = anim->getBy ();
        }

      if ((byVal != "" && byVal.substr (0, 1) == "$")
          || (durVal != "" && durVal.substr (0, 1) == "$")
          || (attVal != "" && attVal.substr (0, 1) == "$"))
        {
          if (event->instanceOf ("AttributionEvent"))
            {
              actions.push_back ((NclLinkAction *)this);
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

NclLinkRepeatAction::NclLinkRepeatAction (NclEvent *event,
                                          SimpleActionType actionType)
    : NclLinkSimpleAction (event, actionType)
{
  this->repetitions = 0;
  this->repetitionInterval = 0;

  typeSet.insert ("NclLinkRepeatAction");
}

NclLinkRepeatAction::~NclLinkRepeatAction ()
{
}

int
NclLinkRepeatAction::getRepetitions ()
{
  return repetitions;
}

GingaTime
NclLinkRepeatAction::getRepetitionInterval ()
{
  return repetitionInterval;
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
  if (NclLinkSimpleAction::event != NULL)
    {
      if (NclLinkSimpleAction::event->instanceOf ("PresentationEvent"))
        {
          ((PresentationEvent *)event)
              ->setRepetitionSettings (repetitions, repetitionInterval);
        }
    }
  else
    {
      clog << "NclLinkRepeatAction::run Warning! event == NULL" << endl;
    }

  NclLinkSimpleAction::run ();
}

NclLinkAssignmentAction::NclLinkAssignmentAction (NclEvent *event,
                                                  SimpleActionType actionType,
                                                  string value)
    : NclLinkRepeatAction (event, actionType)
{
  this->value = value;
  this->animation = NULL;
  typeSet.insert ("NclLinkAssignmentAction");
}

NclLinkAssignmentAction::~NclLinkAssignmentAction ()
{
  if (animation != NULL)
    {
      delete animation;
      animation = NULL;
    }
}

string
NclLinkAssignmentAction::getValue ()
{
  return value;
}

void
NclLinkAssignmentAction::setValue (const string &value)
{
  this->value = value;
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

short
NclLinkCompoundAction::getOperator ()
{
  return _op;
}

void
NclLinkCompoundAction::addAction (NclLinkAction *action)
{
  vector<NclLinkAction *>::iterator i;

  if (_running)
    {
      clog << "NclLinkCompoundAction::addAction ";
      clog << "Warning! Can't add action: status = running" << endl;
      return;
    }

  action->addActionProgressionListener (this);
  i = _actions.begin ();
  while (i != _actions.end ())
    {
      if (*i == action)
        {
          clog << "NclLinkCompoundAction::addAction Warning!";
          clog << " Trying to add same action twice";
          clog << endl;
          return;
        }
      ++i;
    }
  _actions.push_back (action);
}

vector<NclLinkAction *> *
NclLinkCompoundAction::getActions ()
{
  vector<NclLinkAction *> *acts;

  if (_running)
    {
      return NULL;
    }

  if (_actions.empty ())
    {
      return NULL;
    }

  acts = new vector<NclLinkAction *> (_actions);
  return acts;
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

      if (currentAction->instanceOf ("NclLinkCompoundAction"))
        {
          ((NclLinkCompoundAction *)currentAction)
              ->getSimpleActions (simpleActions);
        }
      else if (currentAction->instanceOf ("NclLinkSimpleAction"))
        {
          simpleActions->push_back ((NclLinkSimpleAction *)currentAction);
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
      return NULL;
    }

  if (_actions.empty ())
    {
      return NULL;
    }

  acts = new vector<NclLinkAction *> (_actions);
  events = new vector<NclEvent *>;

  for (i = acts->begin (); i != acts->end (); ++i)
    {
      action = (NclLinkAction *)(*i);
      actionEvents = action->getEvents ();
      if (actionEvents != NULL)
        {
          for (j = actionEvents->begin (); j != actionEvents->end (); ++j)
            {
              events->push_back (*j);
            }
          delete actionEvents;
          actionEvents = NULL;
        }
    }

  delete acts;
  if (events->empty ())
    {
      delete events;
      return NULL;
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
  NclLinkAction *action = NULL;

  _running = true;

  NclLinkAction::run ();

  if (_actions.empty ())
    {
      clog << "NclLinkCompoundAction::run there is no action to run"
           << endl;
      return;
    }
  size = (int) _actions.size ();
  clog << "NclLinkCompoundAction::run '" << size << "' actions" << endl;

  _pendingActions = size;
  _hasStart = false;

  if (_op == CompoundAction::OP_PAR)
    {
      for (i = 0; i < size; i++)
        {
          try
            {
              if (_actions.empty ())
                {
                  return;
                }

              action = (NclLinkAction *)(_actions.at (i));
              action->setSatisfiedCondition (satisfiedCondition);
              action->run ();
            }
          catch (std::out_of_range &e)
            {
              clog << "NclLinkCompoundAction::run catch PAR out of range: ";
              clog << e.what ();
              clog << endl;
              continue;
            }
        }
    }
  else
    {
      for (i = 0; i < size; i++)
        {
          try
            {
              if (_actions.empty ())
                {
                  return;
                }
              action = (NclLinkAction *)(_actions.at (i));
              action->run (satisfiedCondition);
            }
          catch (std::out_of_range &e)
            {
              clog << "NclLinkCompoundAction::run catch SEQ out of range: ";
              clog << e.what ();
              clog << endl;
              continue;
            }
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
