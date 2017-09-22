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

#include <string.h>
#include "ncl/ParserLibXML.h"
using namespace ginga::ncl;

int
main (void)
{
  NclDocument *ncl;
  string errmsg;

  // XML error.
  {
    const char *buf = "<a>";
    ncl = ParserLibXML::parseBuffer (buf, strlen (buf), 0, 0, &errmsg);
    g_assert_null (ncl);
    g_debug ("%s", errmsg.c_str ());
  }

  // Unknown element.
  {
    const char *buf = "<unknown/>";
    ncl = ParserLibXML::parseBuffer (buf, strlen (buf), 0, 0, &errmsg);
    g_assert_null (ncl);
    g_debug ("%s", errmsg.c_str ());
  }

  {
    const char *buf = "<ncl id='hello'/>";
    ncl = ParserLibXML::parseBuffer (buf, strlen (buf), 0, 0, &errmsg);
    g_assert_nonnull (ncl);
    g_assert (ncl->getId () == "hello");
    delete ncl;
  }

  {
    const char *buf = "\
<ncl>\n\
 <head/>\n\
 <body/>\n\
</ncl>\n\
";
    ncl = ParserLibXML::parseBuffer (buf, strlen (buf), 0, 0, &errmsg);
    g_assert_nonnull (ncl);
    g_assert (ncl->getId () == "ncl");
    delete ncl;
  }

  exit (EXIT_SUCCESS);
}
