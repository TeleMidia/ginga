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

#ifndef SWITCH_H
#define SWITCH_H

#include "Composition.h"
#include "Predicate.h"

GINGA_NCL_BEGIN

class Switch: public Composition
{
public:
  Switch (NclDocument *, const string &);
  virtual ~Switch ();

  void addNode (Node *, Predicate *);
  Node *getNode (const string &);

  Node *getDefault ();
  void initDefault (Node *);

  const vector <pair<Node *, Predicate *>> *getRules ();
  Anchor *getMapInterface (Port *);

private:
  Node *_default;
  vector<pair<Node *, Predicate *>> _rules;
};

GINGA_NCL_END

#endif // SWITCH_H
