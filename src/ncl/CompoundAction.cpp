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
#include "CompoundAction.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCL_BEGIN

CompoundAction::CompoundAction () : Action ()
{
  _actions = new vector<Action *>;
}

CompoundAction::CompoundAction (Action *a1, Action *a2, short op)
    : Action ()
{
  _actions = new vector<Action *>;
  _actions->push_back (a1);
  _actions->push_back (a2);
  _myOperator = op;
}

CompoundAction::~CompoundAction ()
{
  vector<Action *>::iterator i;

  if (_actions != NULL)
    {
      i = _actions->begin ();
      while (i != _actions->end ())
        {
          delete (*i);
          ++i;
        }

      delete _actions;
      _actions = NULL;
    }
}

void
CompoundAction::setOperator (short op)
{
  _myOperator = op;
}

short
CompoundAction::getOperator ()
{
  return _myOperator;
}

vector<Action *> *
CompoundAction::getActions ()
{
  if (_actions->begin () == _actions->end ())
    return NULL;

  return _actions;
}

void
CompoundAction::addAction (Action *action)
{
  vector<Action *>::iterator i;

  i = _actions->begin ();
  while (i != _actions->end ())
    {
      if (action == *i)
        {
          clog << "CompoundAction::addAction ";
          clog << "Warning! Trying to add the action twice";
          clog << endl;
          return;
        }
      ++i;
    }
  _actions->push_back (action);
}

void
CompoundAction::removeAction (Action *action)
{
  vector<Action *>::iterator iterator;
  vector<Action *>::iterator i;

  iterator = _actions->begin ();
  while (iterator != _actions->end ())
    {
      if ((*iterator) == action)
        {
          i = _actions->erase (iterator);
          if (i == _actions->end ())
            return;
        }
      ++iterator;
    }
}

vector<Role *> *
CompoundAction::getRoles ()
{
  vector<Role *> *roles;
  int i, size;
  Action *action;
  vector<Role *> *childRoles;

  roles = new vector<Role *>;
  size = (int) _actions->size ();
  for (i = 0; i < size; i++)
    {
      action = (Action *)((*_actions)[i]);
      if (instanceof (SimpleAction *, action))
        {
          roles->push_back ((SimpleAction *)action);
        }
      else
        {
          childRoles = ((CompoundAction *)action)->getRoles ();
          vector<Role *>::iterator it;
          for (it = childRoles->begin (); it != childRoles->end (); ++it)
            {
              roles->push_back (*it);
            }

          delete childRoles;
        }
    }
  return roles;
}

GINGA_NCL_END
