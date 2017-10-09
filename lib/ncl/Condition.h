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

#ifndef CONDITION_H
#define CONDITION_H

#include "Role.h"
#include "Predicate.h"

GINGA_NCL_BEGIN

class Condition: public Role
{
public:
  Condition (EventType, EventStateTransition,
             Predicate *, const string &, const string &);
  virtual ~Condition ();

  EventStateTransition getTransition ();
  Predicate *getPredicate ();
  string getKey ();

  static bool isReserved (const string &, EventType *,
                          EventStateTransition *);
private:
  EventStateTransition _transition;
  string _key;
  Predicate *_predicate;
};

GINGA_NCL_END

#endif // CONDITION_H