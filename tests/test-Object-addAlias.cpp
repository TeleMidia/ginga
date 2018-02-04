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
  list<Object *> objs;

  objs.push_back (new Media ("m"));
  objs.push_back (new MediaSettings ("m"));
  objs.push_back (new Context ("c"));
  objs.push_back (new Switch ("s"));

  for (auto obj : objs)
    {
      g_assert (obj->getAliases ()->size () == 0);

      g_assert_false (obj->hasAlias ("a1"));
      obj->addAlias ("a1");
      g_assert_true (obj->hasAlias ("a1"));

      g_assert_false (obj->hasAlias ("a2"));
      obj->addAlias ("a2");
      g_assert_true (obj->hasAlias ("a2"));

      g_assert_false (obj->hasAlias ("a3"));
      obj->addAlias ("a3");
      g_assert_true (obj->hasAlias ("a3"));
      g_assert (obj->getAliases ()->size () == 3);

      obj->addAlias ("a1");
      obj->addAlias ("a2");
      obj->addAlias ("a3");
      g_assert (obj->getAliases ()->size () == 3);
      delete obj;
    }

  exit (EXIT_SUCCESS);
}
