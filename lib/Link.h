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

#ifndef LINK_H
#define LINK_H

#include "Action.h"
#include "Condition.h"
#include "Event.h"

GINGA_NAMESPACE_BEGIN

class Link
{
public:
  Link ();
  virtual ~Link ();

  const list<Condition *> *getConditions ();
  void addCondition (Condition *);

  const list<Action *> *getActions ();
  void addAction (Action *);

private:
  list<Condition *> _conditions; // list of conditions
  list<Action *> _actions;       // list of actions
};

GINGA_NAMESPACE_END

#endif // LINK_H
