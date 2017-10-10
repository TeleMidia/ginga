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

#ifndef PREDICATE_H
#define PREDICATE_H

#include "Assessment.h"

GINGA_NCL_BEGIN

enum class PredicateType
{
  ATOM = 0,
  NEGATION,
  CONJUNCTION,
  DISJUNCTION,
};

enum class PredicateTestType
{
  EQ = 0,                       // ==
  NE,                           // !=
  LT,                           // <
  LE,                           // <=
  GT,                           // >
  GE                            // >=
};

class Predicate
{
 public:
  Predicate (PredicateType);
  virtual ~Predicate ();
  PredicateType getType ();

  // Atomic only.
  void initTest (Assessment *, PredicateTestType, Assessment *);
  void getTest (Assessment **, PredicateTestType *, Assessment **);

  // Non-atomic only.
  void addChild (Predicate *);
  const vector <Predicate *> *getChildren ();

private:
  PredicateType _type;
  struct
  {
    PredicateTestType test;
    Assessment *left;
    Assessment *right;
  } _atom;
  vector<Predicate *> _children;
};

GINGA_NCL_END

#endif // PREDICATE_H
