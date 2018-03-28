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

#define CHECK_TEST(pred, exp_left, exp_test, exp_right)                    \
  G_STMT_START                                                             \
  {                                                                        \
    Predicate::Test test;                                                  \
    string left;                                                           \
    string right;                                                          \
    (pred)->getTest (&left, &test, &right);                                \
    g_assert (left == (exp_left));                                         \
    g_assert (test == (exp_test));                                         \
    g_assert (right == (exp_right));                                       \
  }                                                                        \
  G_STMT_END

int
main (void)
{
  // Predicate::ATOM
  {
    Predicate *pred;

    pred = new Predicate (Predicate::ATOM);
    g_assert (pred->getType () == Predicate::ATOM);
    g_assert (pred->getChildren ()->size () == 0);
    g_assert (!pred->toString ().empty ());

    CHECK_TEST (pred, "", Predicate::EQ, "");

    pred->setTest ("a", Predicate::EQ, "b");
    CHECK_TEST (pred, "a", Predicate::EQ, "b");

    pred->setTest ("x", Predicate::NE, "y");
    CHECK_TEST (pred, "x", Predicate::NE, "y");

    pred->setTest ("a", Predicate::LT, "b");
    CHECK_TEST (pred, "a", Predicate::LT, "b");

    pred->setTest ("x", Predicate::LE, "y");
    CHECK_TEST (pred, "x", Predicate::LE, "y");

    pred->setTest ("a", Predicate::GT, "b");
    CHECK_TEST (pred, "a", Predicate::GT, "b");

    pred->setTest ("x", Predicate::GE, "y");
    CHECK_TEST (pred, "x", Predicate::GE, "y");

    delete pred;
  }

  // Predicate::FALSUM
  {
    Predicate *pred;

    pred = new Predicate (Predicate::FALSUM);
    g_assert (pred->getType () == Predicate::FALSUM);
    g_assert (!pred->toString ().empty ());

    delete pred;
  }

  // Predicate::VERUM
  {
    Predicate *pred;

    pred = new Predicate (Predicate::VERUM);
    g_assert (pred->getType () == Predicate::VERUM);
    g_assert (pred->getChildren ()->size () == 0);
    g_assert (!pred->toString ().empty ());

    delete pred;
  }

  // Predicate::NEGATION
  {
    Predicate *pred;

    pred = new Predicate (Predicate::NEGATION);
    g_assert (pred->getType () == Predicate::NEGATION);
    g_assert (pred->getChildren ()->size () == 0);
    g_assert (!pred->toString ().empty ());

    delete pred;
  }

  // Predicate::CONJUNCTION
  {
    Predicate *pred, *child1, *child2;

    pred = new Predicate (Predicate::CONJUNCTION);
    g_assert (pred->getType () == Predicate::CONJUNCTION);
    g_assert (pred->getChildren ()->size () == 0);
    g_assert (!pred->toString ().empty ());

    child1 = new Predicate (Predicate::ATOM);
    child1->setTest ("a", Predicate::EQ, "a");
    pred->addChild (child1);
    g_assert (pred->getChildren ()->size () == 1);

    child2 = new Predicate (Predicate::ATOM);
    child2->setTest ("b", Predicate::EQ, "b");
    pred->addChild (child2);
    g_assert (pred->getChildren ()->size () == 2);

    delete pred;
  }

  // Predicate::DISJUNCTION
  {
    Predicate *pred, *child1, *child2;

    pred = new Predicate (Predicate::DISJUNCTION);
    g_assert (pred->getType () == Predicate::DISJUNCTION);
    g_assert (pred->getChildren ()->size () == 0);
    g_assert (!pred->toString ().empty ());

    child1 = new Predicate (Predicate::ATOM);
    child1->setTest ("a", Predicate::EQ, "a");
    pred->addChild (child1);
    g_assert (pred->getChildren ()->size () == 1);
    g_assert (child1->getParent () == pred);

    child2 = new Predicate (Predicate::ATOM);
    child2->setTest ("b", Predicate::EQ, "b");
    pred->addChild (child2);
    g_assert (pred->getChildren ()->size () == 2);
    g_assert (child2->getParent () == pred);

    delete pred;
  }

  exit (EXIT_SUCCESS);
}
