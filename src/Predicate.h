/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef PREDICATE_H
#define PREDICATE_H

#include "aux-ginga.h"

GINGA_NAMESPACE_BEGIN

class Predicate
{
public:
  enum Type
  {
    FALSUM = 0, // false
    VERUM,      // true
    ATOM,
    NEGATION,
    CONJUNCTION,
    DISJUNCTION,
  };

  enum Test
  {
    EQ = 0, // ==
    NE,     // !=
    LT,     // <
    LE,     // <=
    GT,     // >
    GE      // >=
  };

  explicit Predicate (Predicate::Type);
  ~Predicate ();
  Predicate::Type getType ();
  string toString ();
  Predicate *clone ();

  // Atomic only.
  void getTest (string *, Predicate::Test *, string *);
  void setTest (const string &, Predicate::Test, const string &);

  // Non-atomic only.
  const list<Predicate *> *getChildren ();
  void addChild (Predicate *);

  // Both.
  Predicate *getParent ();
  void initParent (Predicate *);

private:
  Predicate::Type _type;
  struct
  {
    Predicate::Test test;
    string left;
    string right;
  } _atom;
  list<Predicate *> _children;
  Predicate *_parent;
};

GINGA_NAMESPACE_END

#endif // PREDICATE_H
