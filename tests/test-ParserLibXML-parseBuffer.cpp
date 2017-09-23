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

  // XML error.
  {
    string errmsg;
    const char *buf = "<a>";
    ncl = ParserLibXML::parseBuffer (buf, strlen (buf), 0, 0, &errmsg);
    g_assert_null (ncl);
    g_assert (errmsg != "");
  }

  // Error: Unknown element.
  {
    string errmsg;
    const char *buf = "<unknown/>";
    ncl = ParserLibXML::parseBuffer (buf, strlen (buf), 0, 0, &errmsg);
    g_assert_null (ncl);
    g_assert (errmsg != "");
  }

  // Error: Bad parent.
  {
    string errmsg;
    const char *buf = "<head/>";
    ncl = ParserLibXML::parseBuffer (buf, strlen (buf), 0, 0, &errmsg);
    g_assert_null (ncl);
    g_assert (errmsg != "");
  }

  // Empty document.
  {
    string errmsg;
    const char *buf = "\
<ncl>\n\
 <head/>\n\
 <body/>\n\
</ncl>\n\
";
    ncl = ParserLibXML::parseBuffer (buf, strlen (buf), 0, 0, &errmsg);
    g_assert_nonnull (ncl);
    g_assert (errmsg == "");
    g_assert (ncl->getId () == "ncl");
    Context *body = ncl->getBody ();
    g_assert_nonnull (body);
    g_assert (body->getId () == ncl->getId ());
    g_assert (body->getPorts ()->size () == 0);
    g_assert (body->getNodes ()->size () == 0);
    g_assert (body->getLinks ()->size () == 0);
    delete ncl;
  }

  // Error: Port: Missing id.
  {
    string errmsg;
    const char *buf = "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port/>\n\
 </body>\n\
</ncl>\n\
";
    ncl = ParserLibXML::parseBuffer (buf, strlen (buf), 0, 0, &errmsg);
    g_assert_null (ncl);
    g_assert (errmsg != "");
  }

  // Error: Port: Missing component.
  {
    string errmsg;
    const char *buf = "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port id='p'/>\n\
 </body>\n\
</ncl>\n\
";
    ncl = ParserLibXML::parseBuffer (buf, strlen (buf), 0, 0, &errmsg);
    g_assert_null (ncl);
    g_assert (errmsg != "");
  }

  // Error: Media: Missing id.
  {
    string errmsg;
    const char *buf = "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media/>\n\
 </body>\n\
</ncl>\n\
";
    ncl = ParserLibXML::parseBuffer (buf, strlen (buf), 0, 0, &errmsg);
    g_assert_null (ncl);
    g_assert (errmsg != "");
  }

  // Success.
  {
    string errmsg;
    const char *buf = "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port id='p' component='m'/>\n\
  <media id='m'/>\n\
 </body>\n\
</ncl>\n\
";
    ncl = ParserLibXML::parseBuffer (buf, strlen (buf), 0, 0, &errmsg);
    g_assert_nonnull (ncl);
    g_assert (errmsg == "");
    delete ncl;
  }

  exit (EXIT_SUCCESS);
}
