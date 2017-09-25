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
#include "ncl/Ncl.h"
using namespace ginga::ncl;

static G_GNUC_UNUSED bool
check_failure (const string &buf)
{
  NclDocument *ncl;
  string msg = "";

  ncl = ParserLibXML::parseBuffer (buf.c_str (), buf.length (), 100, 100, &msg);
  if (ncl == nullptr && msg != "")
    {
      g_printerr ("%s\n", msg.c_str ());
      return true;
    }
  return false;
}

static G_GNUC_UNUSED NclDocument *
check_success (const string &buf)
{
  NclDocument *ncl;
  string msg = "";

  ncl = ParserLibXML::parseBuffer (buf.c_str (), buf.length (), 100, 100, &msg);
  if (msg != "")
    {
      g_printerr ("Unexpected error: %s", msg.c_str ());
      g_assert_not_reached ();
    }
  return ncl;
}

int
main (void)
{


// -------------------------------------------------------------------------
// General errors.
// -------------------------------------------------------------------------

  // Error: XML error.
  g_assert (check_failure ("<a>"));

  // Error: Unknown element.
  g_assert (check_failure ("<unknown/>"));

  // Error: Bad parent.
  g_assert (check_failure ("<head/>"));

  // Error: Bad child.
  g_assert (check_failure ("<ncl><media/></ncl>"));

  // Error: Unknown child.
  g_assert (check_failure ("<ncl><unknown/></ncl>"));

  // Error: Unknown attribute.
  g_assert (check_failure ("<ncl unknown='unknown'/>"));

  // Error: ncl: Bad id.
  g_assert (check_failure ("<ncl id='@'/>"));


// -------------------------------------------------------------------------
// Port.
// -------------------------------------------------------------------------

  // Error: Port: Missing id.
  g_assert (check_failure ("\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port/>\n\
 </body>\n\
</ncl>\n\
"));

  // Error: Port: Missing component.
  g_assert (check_failure ("\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port id='p'/>\n\
 </body>\n\
</ncl>\n\
"));

  // Error: Port: Bad component.
  g_assert (check_failure ("\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port id='p' component='p'/>\n\
 </body>\n\
</ncl>\n\
"));

  g_assert (check_failure ("\
<ncl>\n\
 <head>\n\
  <regionBase id='r'/>\n\
 </head>\n\
 <body>\n\
  <port id='p' component='r'/>\n\
 </body>\n\
</ncl>\n\
"));

  g_assert (check_failure ("\
<ncl>\n\
 <body>\n\
  <port id='p' component='b'/>\n\
  <context id='a'>\n\
   <context id='b'/>\n\
  </context>\n\
 </body>\n\
</ncl>\n\
"));

  // Error: Port: Bad interface.
  g_assert (check_failure ("\
<ncl>\n\
 <head>\n\
  <regionBase id='r'/>\n\
 </head>\n\
 <body>\n\
  <port id='p' component='m' interface='r'/>\n\
  <media id='m'/>\n\
 </body>\n\
</ncl>\n\
"));

  g_assert (check_failure ("\
<ncl>\n\
 <body>\n\
  <port id='p' component='m' interface='nonexistent'/>\n\
  <media id='m'/>\n\
 </body>\n\
</ncl>\n\
"));


// -------------------------------------------------------------------------
// Media.
// -------------------------------------------------------------------------

  // Error: Media: Missing id.
  g_assert (check_failure ("\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media/>\n\
 </body>\n\
</ncl>\n\
"));

  // Error: Media: Duplicated id.
  g_assert (check_failure ("\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='a'/>\n\
  <media id='a'/>\n\
 </body>\n\
</ncl>\n\
"));

  // Error: Media: Bad descriptor.
  g_assert (check_failure ("\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='a' descriptor='nonexistent'/>\n\
 </body>\n\
</ncl>\n\
"));

  g_assert (check_failure ("\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <region id='r'/>\n\
  </regionBase>\n\
 </head>\n\
 <body>\n\
  <media id='a' descriptor='r'/>\n\
 </body>\n\
</ncl>\n\
"));


// -------------------------------------------------------------------------
// Descriptor.
// -------------------------------------------------------------------------

  // Error: Descriptor: Bad region.
  g_assert (check_failure ("\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor id='d' region='nonexistent'/>\n\
  </descriptorBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
"));

  g_assert (check_failure ("\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor id='r'/>\n\
   <descriptor id='d' region='r'/>\n\
  </descriptorBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
"));


// -------------------------------------------------------------------------
// Sanity checks.
// -------------------------------------------------------------------------

  // Success: Empty document.
  {
    NclDocument *ncl = check_success ("\
<ncl>\n\
 <head/>\n\
 <body/>\n\
</ncl>\n\
");
    g_assert_nonnull (ncl);
    g_assert (ncl->getId () == "ncl");
    Context *body = ncl->getRoot ();
    g_assert_nonnull (body);
    g_assert (body->getId () == ncl->getId ());
    g_assert (body->getPorts ()->size () == 0);
    g_assert (body->getNodes ()->size () == 0);
    g_assert (body->getLinks ()->size () == 0);
    delete ncl;
  }

  // Success: Misc checks.
  {
    NclDocument *ncl = check_success ("\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <region id='r' top='100%' left='25%'>\n\
    <region id='r1' width='30%'>\n\
     <region id='r2' bottom='25%' right='25%'>\n\
      <region id='r3' height='15%' width='50%' zIndex='1'/>\n\
     </region>\n\
    </region>\n\
   </region>\n\
  </regionBase>\n\
  <descriptorBase>\n\
   <descriptor id='d' left='50%' top='0%' region='r3'>\n\
    <descriptorParam name='top' value='50%'/>\n\
    <descriptorParam name='zIndex' value='2'/>\n\
   </descriptor>\n\
   <descriptor id='d1'/>\n\
  </descriptorBase>\n\
 </head>\n\
 <body>\n\
  <port id='p' component='m'/>\n\
  <port id='q' component='m' interface='background'/>\n\
  <media id='m' descriptor='d'>\n\
   <property name='background' value='red'/>\n\
   <property name='size' value='100%,100%'/>\n\
   <property name='zIndex' value='3'/>\n\
  </media>\n\
  <port id='p2' component='c'/>\n\
  <context id='c'>\n\
   <port id='p3' component='m2'/>\n\
   <media id='m2' src='samples/gnu.png'>\n\
   </media>\n\
  </context>\n\
 </body>\n\
</ncl>\n\
");
    g_assert_nonnull (ncl);
    g_assert (ncl->getId () == "ncl");
    Context *body = ncl->getRoot ();
    g_assert_nonnull (body);
    g_assert (body->getId () == ncl->getId ());
    g_assert (body->getPorts ()->size () == 3);
    g_assert (body->getNodes ()->size () == 2);
    g_assert (body->getLinks ()->size () == 0);

    Entity *port = ncl->getEntityById ("p");
    g_assert (instanceof (Port *, port));
    Port *p = cast (Port *, port);
    g_assert (p->getId () == "p");

    port = ncl->getEntityById ("q");
    g_assert (instanceof (Port *, port));
    Port *q = cast (Port *, port);
    g_assert (q->getId () == "q");

    Entity *media = ncl->getEntityById ("m");
    g_assert (instanceof (Media *, media));
    Media *m = cast (Media *, media);
    g_assert (m->getId () == "m");

    g_assert (p->getNode () == m);
    g_assert (p->getInterface ()->getId () == "m@lambda");
    g_assert (q->getNode () == m);
    g_assert (q->getInterface ()->getId () == "background");

    g_assert (m->getProperty ("background") == "red");
    g_assert (m->getProperty ("top") == "50%");
    g_assert (m->getProperty ("left") == "50%");
    g_assert (m->getProperty ("width") == "15.00%");
    g_assert (m->getProperty ("height") == "15.00%");
    g_assert (m->getProperty ("zIndex") == "3");

    delete ncl;
  }


  exit (EXIT_SUCCESS);
}
