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

#include "tests.h"

int
main (void)
{

  // Document:evalPredicate ATOM
  {
    Document *doc;
    Context *root;
    MediaSettings *settings;
    Predicate *pred;

    tests_create_document (&doc, &root, &settings);
    settings->setProperty ("propA", "a", 0);
    settings->setProperty ("propB", "b", 0);

    string propAName ("$__settings__.propA");
    string propBName ("$__settings__.propB");

    pred = new Predicate (Predicate::ATOM);

    // a == a -> true
    pred->setTest (propAName, Predicate::EQ, propAName);
    g_assert (doc->evalPredicate (pred));

    // a == b -> false
    pred->setTest (propAName, Predicate::EQ, propBName);
    g_assert_false (doc->evalPredicate (pred));

    // a != b -> true
    pred->setTest (propAName, Predicate::NE, propBName);
    g_assert (doc->evalPredicate (pred));

    // a < b -> true
    pred->setTest (propAName, Predicate::LT, propBName);
    g_assert (doc->evalPredicate (pred));

    // a <= b -> true
    pred->setTest (propBName, Predicate::LE, propBName);
    g_assert (doc->evalPredicate (pred));

    // b > a -> true
    pred->setTest (propBName, Predicate::GT, propAName);
    g_assert (doc->evalPredicate (pred));

    // b >= b -> true
    pred->setTest (propBName, Predicate::GE, propBName);
    g_assert (doc->evalPredicate (pred));

    delete doc;
  }
  // Document:evalPredicate FALSUM
  {
    Document *doc;
    Context *root;
    MediaSettings *settings;
    Predicate *pred;

    tests_create_document (&doc, &root, &settings);

    pred = new Predicate (Predicate::FALSUM);

    g_assert_false (doc->evalPredicate (pred));

    delete doc;
  }

  // Document:evalPredicate VERUM
  {
    Document *doc;
    Context *root;
    MediaSettings *settings;
    Predicate *pred;

    tests_create_document (&doc, &root, &settings);

    pred = new Predicate (Predicate::VERUM);

    g_assert (doc->evalPredicate (pred));

    delete doc;
  }

  // Document:evalPredicate NEGATION
  {
  }

  // Document:evalPredicate CONJUNCTION
  {
  }

  // Document:evalPredicate DISJUNCTION
  {
  }

  exit (EXIT_SUCCESS);
}
