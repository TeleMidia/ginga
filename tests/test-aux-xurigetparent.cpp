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

#include <iostream>

int
main (void)
{
  // Unsuccessful tests --------------------------------------------------

  // Successful tests ----------------------------------------------------
  {
    g_assert (xurigetparent ("file:///ginga/a.ext") == "file:///ginga");
    g_assert (xurigetparent ("file:///a.ext") == "file:///");
    g_assert (xurigetparent ("http:///ginga/a.ext") == "http:///ginga");
    g_assert (xurigetparent ("ftp:///ginga/a.ext") == "ftp:///ginga");
  }

  exit (EXIT_SUCCESS);
}
