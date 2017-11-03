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

#ifndef NCL_CONNECTOR_H
#define NCL_CONNECTOR_H

#include "NclAction.h"
#include "NclCondition.h"
#include "NclEntity.h"
#include "NclRole.h"
#include "Predicate.h"

GINGA_NCL_BEGIN

class NclConnector: public NclEntity
{
public:
  NclConnector (NclDocument *, const string &);
  virtual ~NclConnector ();

  const vector<NclCondition *> *getConditions ();
  bool addCondition (NclCondition *);

  const vector<NclAction *> *getActions ();
  bool addAction (NclAction *);

  // void addPredicateRole (const string &, Predicate *);
  // Predicate *getPredicateRole (const string &);

  NclRole *getRole (const string &);

private:
  vector<NclCondition *> _conditions;
  vector<NclAction *> _actions;
  // map<string, Predicate *> _predicates;
};

GINGA_NCL_END

#endif // NCL_CONNECTOR_H
