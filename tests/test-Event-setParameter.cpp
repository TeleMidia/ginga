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

#define CHECK_PARMS(e)                                                     \
  G_STMT_START                                                             \
  {                                                                        \
    string str;                                                            \
    map<string, string> params = {                                         \
      {"a", "1"},                                                        \
      {"b", "2"},                                                        \
      {"c", "3"},                                                        \
      {"d", "4"},                                                        \
    };                                                                     \
    for (auto it : params)                                                 \
      g_assert_false ((e)->getParameter (it.first, nullptr));              \
    for (auto it : params)                                                 \
      g_assert ((e)->setParameter (it.first, it.second));                  \
    for (auto it : params)                                                 \
      {                                                                    \
        g_assert ((e)->getParameter (it.first, &str));                     \
        g_assert (str == it.second);                                       \
      }                                                                    \
    g_assert_false ((e)->setParameter ("a", "z"));                         \
    g_assert ((e)->getParameter ("a", &str));                              \
    g_assert (str == "z");                                                 \
  }                                                                        \
  G_STMT_END

int
main (void)
{
  Media *m;

  m = new Media ("m");
  g_assert_nonnull (m);

  // Presentation.
  {
    m->addPresentationEvent ("pres", 0, 0);
    Event *e = m->getPresentationEvent ("pres");
    g_assert_nonnull (e);
    CHECK_PARMS (e);
  }

  // Attribution.
  {
    m->addAttributionEvent ("attr");
    Event *e = m->getAttributionEvent ("attr");
    g_assert_nonnull (e);
    CHECK_PARMS (e);
  }

  // Selection.
  {
    m->addSelectionEvent ("sel");
    Event *e = m->getSelectionEvent ("sel");
    g_assert_nonnull (e);
    CHECK_PARMS (e);
  }

  delete m;

  exit (EXIT_SUCCESS);
}
