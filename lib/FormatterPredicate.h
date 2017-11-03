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

#ifndef FORMATTER_PREDICATE_H
#define FORMATTER_PREDICATE_H

GINGA_NAMESPACE_BEGIN

enum class PredicateType
{
  FALSUM = 0,                   // false
  VERUM,                        // true
  ATOM,
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

class FormatterPredicate
{
 public:
  FormatterPredicate (PredicateType);
  ~FormatterPredicate ();
  PredicateType getType ();
  FormatterPredicate *clone ();

  // Atomic only.
  void getTest (string *, PredicateTestType *, string *);
  void setTest (const string &, PredicateTestType, const string &);

  // Non-atomic only.
  void addChild (FormatterPredicate *);
  const vector <FormatterPredicate *> *getChildren ();

  // Both.
  FormatterPredicate *getParent ();
  void initParent (FormatterPredicate *);

private:
  PredicateType _type;
  struct
  {
    PredicateTestType test;
    string left;
    string right;
  } _atom;
  vector<FormatterPredicate *> _children;
  FormatterPredicate *_parent;
};

GINGA_NAMESPACE_END

#endif // FORMATTER_PREDICATE_H
