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
  objs.push_back (new MediaSettings ("stgs"));
  objs.push_back (new Context ("c"));
  objs.push_back (new Switch ("s"));

  for (auto obj : objs)
    {
      g_assert_cmpint (obj->getTime (), ==, GINGA_TIME_NONE);

      // only advance time after a Object::doStart, which
      // is done when start @lambada
      obj->sendTick (1 * GINGA_SECOND, 1 * GINGA_SECOND, 0);
      g_assert_cmpint (obj->getTime (), ==, GINGA_TIME_NONE);

      delete obj;
    }

  exit (EXIT_SUCCESS);
}
