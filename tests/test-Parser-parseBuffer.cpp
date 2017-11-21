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
#include "Parser.h"

#include "Context.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Switch.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wunused-macros)

static G_GNUC_UNUSED bool
check_failure (const string &log, const string &buf)
{
  static int i = 1;
  Document *doc;
  string msg = "";

  g_printerr ("xfail #%d: %s\n", i++, log.c_str ());
  doc = Parser::parseBuffer (buf.c_str (), buf.length (), 100, 100, &msg);
  if (doc == nullptr && msg != "")
    {
      g_printerr ("%s\n\n", msg.c_str ());
      return true;
    }
  delete doc;
  return false;
}

static G_GNUC_UNUSED Document *
check_success (const string &log, const string &buf)
{
  static int i = 1;
  Document *doc;
  string msg = "";

  g_printerr ("pass #%d: %s\n", i++, log.c_str ());
  doc = Parser::parseBuffer (buf.c_str (), buf.length (), 100, 100, &msg);
  if (msg != "")
    {
      g_printerr ("*** Unexpected error: %s", msg.c_str ());
      g_assert_not_reached ();
    }
  return doc;
}

#define XFAIL(log, str)\
  g_assert (check_failure ((log), (str)))

#define PASS(obj, log, str)                     \
  G_STMT_START                                  \
  {                                             \
    tryset (obj, check_success ((log), (str))); \
    g_assert_nonnull (*(obj));                  \
  }                                             \
  G_STMT_END

int
main (void)
{


// -------------------------------------------------------------------------
// General errors.
// -------------------------------------------------------------------------

  XFAIL ("XML error", "<a>");
  XFAIL ("Unknown element", "<unknown/>");
  XFAIL ("Missing parent", "<head/>");
  XFAIL ("Unknown child", "<ncl><media/></ncl>");
  XFAIL ("Unknown child", "<ncl><unknown/></ncl>");
  XFAIL ("Unknown attribute", "<ncl unknown='unknown'/>");
  XFAIL ("ncl: Bad id", "<ncl id='@'/>");


// -------------------------------------------------------------------------
// Port.
// -------------------------------------------------------------------------

  XFAIL ("port: Missing id", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Missing component", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port id='p'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad component", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port id='p' component='p'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad component", "\
<ncl>\n\
 <head>\n\
  <regionBase id='r'/>\n\
 </head>\n\
 <body>\n\
  <port id='p' component='r'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad Component", "\
<ncl>\n\
 <body>\n\
  <port id='p' component='b'/>\n\
  <context id='a'>\n\
   <context id='b'/>\n\
  </context>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad interface", "\
<ncl>\n\
 <head>\n\
  <regionBase id='r'/>\n\
 </head>\n\
 <body>\n\
  <port id='p' component='m' interface='r'/>\n\
  <media id='m'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad interface", "\
<ncl>\n\
 <body>\n\
  <port id='p' component='m' interface='nonexistent'/>\n\
  <media id='m'/>\n\
 </body>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// Media.
// -------------------------------------------------------------------------

  XFAIL ("media: Missing id", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("media: Duplicated id", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='a'/>\n\
  <media id='a'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("media: Bad descriptor", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='a' descriptor='nonexistent'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("media: Bad descriptor", "\
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
");


// -------------------------------------------------------------------------
// Link.
// -------------------------------------------------------------------------

  XFAIL ("link: Missing xconnector", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <link>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("link: No such xconnector", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <link xconnector='c'>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("link: Link does not match connector", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
  <link xconnector='c'>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// Region.
// -------------------------------------------------------------------------

  XFAIL ("region: Missing id", "\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <region/>\n\
  </regionBase>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// Descriptor.
// -------------------------------------------------------------------------

  XFAIL ("descriptor: Missing id", "\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor/>\n\
  </descriptorBase>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("descriptor: Bad region", "\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor id='d' region='nonexistent'/>\n\
  </descriptorBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  XFAIL ("descriptor: Bad region", "\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor id='r'/>\n\
   <descriptor id='d' region='r'/>\n\
  </descriptorBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  XFAIL ("descriptor: Bad descriptorParam", "\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor id='d'>\n\
    <descriptorParam/>\n\
   </descriptor>\n\
  </descriptorBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  XFAIL ("descriptor: Bad descriptorParam", "\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor id='d'>\n\
    <descriptorParam name='x'/>\n\
   </descriptor>\n\
  </descriptorBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// causalConnector.
// -------------------------------------------------------------------------

  XFAIL ("causalConnector: Missing id", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector/>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("causalConnector: Missing child condition", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'/>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("causalConnector: Missing child condition", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("causalConnector: Missing child action", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// simpleCondition.
// -------------------------------------------------------------------------

  XFAIL ("simpleCondition: Missing role", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleCondition: Reserved role", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin' eventType='presentation'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleCondition: Reserved role", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin' transition='starts'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleCondition: Missing eventType", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='x'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleCondition: Bad eventType", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='x' eventType='unknown'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleCondition: Missing transition", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='x' eventType='presentation'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleCondition: Bad transition", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='x' eventType='presentation'\n\
                     transition='unknown'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// simpleAction.
// -------------------------------------------------------------------------

  XFAIL ("simpleAction: Missing role", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleAction: Reserved role", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='start' actionType='starts'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleAction: Missing actionType", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='x'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleAction: Bad eventType", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='x' eventType='unknown'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleAction: Missing actionType", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='x' eventType='presentation'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleAction: Bad actionType", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='x' eventType='presentation'\n\
                           actionType='unknown'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// Sanity checks.
// -------------------------------------------------------------------------

  // Success: Empty document.
  {
    Document *doc;
    PASS (&doc, "Empty document", "\
<ncl>\n\
 <head/>\n\
 <body/>\n\
</ncl>\n\
");
    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 2);
    delete doc;
  }

  // Success: Misc checks.
  {
    Document *doc;
    PASS (&doc, "Misc checks", "\
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
   <connectorBase>\n\
    <causalConnector id='conn1'>\n\
     <simpleCondition role='onBegin'/>\n\
     <simpleAction role='start'/>\n\
    </causalConnector>\n\
   </connectorBase>\n\
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
    g_assert_nonnull (doc);

    MediaSettings *settings = doc->getSettings ();
    g_assert_nonnull (settings);

    Context *body = doc->getRoot ();
    g_assert_nonnull (body);
    g_assert (body->getId () == "__root__");
    g_assert (body->getPorts ()->size () == 3);
    g_assert (body->getChildren ()->size () == 3);
    g_assert (body->getLinks ()->size () == 0);

    g_assert (doc->getMedias ()->size () == 3);

    Media *m = cast (Media *, doc->getObjectById ("m"));
    g_assert_nonnull (m);
    g_assert (m->getId () == "m");
    g_assert_nonnull (m->getPresentationEvent ("@lambda"));
    g_assert_nonnull (m->getAttributionEvent ("background"));

    delete doc;
  }

  exit (EXIT_SUCCESS);
}
