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

NclLinkCompoundAction::NclLinkCompoundAction (short op) : NclLinkAction ()
{
  this->op = op;
  hasStart = false;
  runing = false;
  listener = NULL;

  Thread::mutexInit (&mutexActions, true);
  typeSet.insert ("NclLinkCompoundAction");
}

NclLinkCompoundAction::~NclLinkCompoundAction ()
{
  vector<NclLinkAction *>::iterator i;
  NclLinkAction *action;

  _isDeleting = true;

  // clog << "NclLinkCompoundAction::~NclLinkCompoundAction" << endl;
  Thread::mutexLock (&mutexActions);
  i = actions.begin ();
  while (i != actions.end ())
    {
      action = (NclLinkAction *)(*i);

      actions.erase (i);
      i = actions.begin ();

      action->removeActionProgressionListener (this);
      delete action;
      action = NULL;
    }

  actions.clear ();
  Thread::mutexUnlock (&mutexActions);
  Thread::mutexDestroy (&mutexActions);
}

short
NclLinkCompoundAction::getOperator ()
{
  return op;
}

void
NclLinkCompoundAction::addAction (NclLinkAction *action)
{
  vector<NclLinkAction *>::iterator i;

  if (runing)
    {
      clog << "NclLinkCompoundAction::addAction ";
      clog << "Warning! Can't add action: status = running" << endl;
      return;
    }

  action->addActionProgressionListener (this);
  Thread::mutexLock (&mutexActions);
  i = actions.begin ();
  while (i != actions.end ())
    {
      if (*i == action)
        {
          clog << "NclLinkCompoundAction::addAction Warning!";
          clog << " Trying to add same action twice";
          clog << endl;
          Thread::mutexUnlock (&mutexActions);
          return;
        }
      ++i;
    }
  actions.push_back (action);
  Thread::mutexUnlock (&mutexActions);
}

vector<NclLinkAction *> *
NclLinkCompoundAction::getActions ()
{
  vector<NclLinkAction *> *acts;

  if (runing)
    {
      return NULL;
    }

  Thread::mutexLock (&mutexActions);
  if (actions.empty ())
    {
      Thread::mutexUnlock (&mutexActions);
      return NULL;
    }

  acts = new vector<NclLinkAction *> (actions);
  Thread::mutexUnlock (&mutexActions);
  return acts;
}

void
NclLinkCompoundAction::getSimpleActions (
    vector<NclLinkSimpleAction *> *simpleActions)
{
  vector<NclLinkAction *>::iterator i;
  NclLinkAction *currentAction;

  Thread::mutexLock (&mutexActions);
  if (actions.empty ())
    {
      Thread::mutexUnlock (&mutexActions);
      return;
    }

  i = actions.begin ();
  while (i != actions.end ())
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

  Thread::mutexUnlock (&mutexActions);
}

void
NclLinkCompoundAction::setCompoundActionListener (
    INclLinkActionListener *listener)
{
  this->listener = listener;
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

  if (runing)
    {
      return NULL;
    }

  Thread::mutexLock (&mutexActions);
  if (actions.empty ())
    {
      Thread::mutexUnlock (&mutexActions);
      return NULL;
    }

  acts = new vector<NclLinkAction *> (actions);
  Thread::mutexUnlock (&mutexActions);

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

vector<NclLinkAction *> *
NclLinkCompoundAction::getImplicitRefRoleActions ()
{
  vector<NclLinkAction *> *acts;
  vector<NclLinkAction *>::iterator i;
  vector<NclLinkAction *> *assignmentActs;
  vector<NclLinkAction *> *refActs;
  vector<NclLinkAction *>::iterator j;

  if (runing)
    {
      return NULL;
    }

  Thread::mutexLock (&mutexActions);
  if (actions.empty ())
    {
      Thread::mutexUnlock (&mutexActions);
      return NULL;
    }

  acts = new vector<NclLinkAction *> (actions);
  Thread::mutexUnlock (&mutexActions);

  refActs = new vector<NclLinkAction *>;

  for (i = acts->begin (); i != acts->end (); ++i)
    {
      assignmentActs = (*i)->getImplicitRefRoleActions ();
      if (assignmentActs != NULL)
        {
          for (j = assignmentActs->begin (); j != assignmentActs->end ();
               ++j)
            {
              refActs->push_back (*j);
            }
          delete assignmentActs;
          assignmentActs = NULL;
        }
    }

  delete acts;
  if (refActs->empty ())
    {
      delete refActs;
      return NULL;
    }

  return refActs;
}

void
NclLinkCompoundAction::run ()
{
  int i, size;
  NclLinkAction *action = NULL;
  vector<NclLinkSimpleAction *> simpleActions;

  runing = true;

  NclLinkAction::run ();

  Thread::mutexLock (&mutexActions);
  if (actions.empty ())
    {
      clog << "NclLinkCompoundAction::run there is no action to run"
           << endl;
      Thread::mutexUnlock (&mutexActions);
      return;
    }
  size = (int) actions.size ();
  clog << "NclLinkCompoundAction::run '" << size << "' actions" << endl;
  Thread::mutexUnlock (&mutexActions);

  pendingActions = size;
  hasStart = false;

  if (op == CompoundAction::OP_PAR)
    {
      for (i = 0; i < size; i++)
        {
          try
            {
              Thread::mutexLock (&mutexActions);
              if (actions.empty ())
                {
                  Thread::mutexUnlock (&mutexActions);
                  return;
                }

              action = (NclLinkAction *)(actions.at (i));
              action->setSatisfiedCondition (satisfiedCondition);
              Thread::mutexUnlock (&mutexActions);
              // HELL: action->startThread ();
              action->run ();
            }
          catch (std::out_of_range &e)
            {
              clog << "NclLinkCompoundAction::run catch PAR out of range: ";
              clog << e.what ();
              clog << endl;
              Thread::mutexUnlock (&mutexActions);
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
              Thread::mutexLock (&mutexActions);
              if (actions.empty ())
                {
                  Thread::mutexUnlock (&mutexActions);
                  return;
                }
              action = (NclLinkAction *)(actions.at (i));
              Thread::mutexUnlock (&mutexActions);
              action->run (satisfiedCondition);
            }
          catch (std::out_of_range &e)
            {
              clog << "NclLinkCompoundAction::run catch SEQ out of range: ";
              clog << e.what ();
              clog << endl;
              Thread::mutexUnlock (&mutexActions);
              continue;
            }
        }
    }
}

void
NclLinkCompoundAction::actionProcessed (bool start)
{
  pendingActions--;
  hasStart = (hasStart || start);
  if (pendingActions == 0)
    {
      notifyProgressionListeners (hasStart);
    }
}

GINGA_FORMATTER_END
