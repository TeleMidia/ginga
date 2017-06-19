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
#include "NclLinkCompoundAction.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_FORMATTER_BEGIN

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

vector<NclFormatterEvent *> *
NclLinkCompoundAction::getEvents ()
{
  vector<NclLinkAction *> *acts;
  vector<NclLinkAction *>::iterator i;
  NclLinkAction *action;
  vector<NclFormatterEvent *> *events;
  vector<NclFormatterEvent *> *actionEvents;
  vector<NclFormatterEvent *>::iterator j;

  if (_running)
    {
      return NULL;
    }

  if (_actions.empty ())
    {
      return NULL;
    }

  acts = new vector<NclLinkAction *> (_actions);
  events = new vector<NclFormatterEvent *>;

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
