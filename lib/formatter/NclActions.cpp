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
}


// NclSimpleAction

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
  g_assert_nonnull (_listener);
  _listener->scheduleAction (this);
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


// NclCompoundAction

NclCompoundAction::NclCompoundAction () : NclAction ()
{
  _hasStart = false;
  _listener = nullptr;
}

NclCompoundAction::~NclCompoundAction ()
{
  for (auto action: _actions)
    delete action;
}

void
NclCompoundAction::addAction (NclSimpleAction *action)
{
  _actions.push_back (action);
}

const vector <NclSimpleAction *> *
NclCompoundAction::getSimpleActions ()
{
  return &_actions;
}

vector<NclEvent *>
NclCompoundAction::getEvents ()
{
  vector<NclEvent *> events;

  for (auto action: _actions)
    for (auto evt: action->getEvents ())
      events.push_back (evt);

  return events;
}

vector<NclAction *>
NclCompoundAction::getImplicitRefRoleActions ()
{
  vector<NclAction *> refActs;
  for (auto act: _actions)
    {
      vector<NclAction *> assignmentActs = act->getImplicitRefRoleActions ();
      for (NclAction *assignmentAct : assignmentActs)
        refActs.push_back (assignmentAct);
    }
  return refActs;
}

void
NclCompoundAction::run ()
{
  for (auto action: _actions)
    action->run ();
}

GINGA_FORMATTER_END
