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
check_failure (const string &log, const string &expected, const string &buf)
{
  static int i = 1;
  Document *doc;
  string msg = "";
  bool status;

  g_printerr ("xfail #%d: %s\n", i++, log.c_str ());
  doc = Parser::parseBuffer (buf.c_str (), buf.length (), 100, 100, &msg);
  if (doc == nullptr)
    {
      if (expected != "" && !xstrhassuffix (msg, expected))
        {
          g_printerr ("*** Expected:\t\"%s\"\n", expected.c_str ());
          g_printerr ("*** Got:\t\"%s\"\n", msg.c_str ());
          status = false;
        }
      else
        {
          status = true;
        }
      g_printerr ("\n");
    }
  delete doc;
  return status;
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

#define XFAIL(log, exp, str)\
  g_assert (check_failure ((log), (exp), (str)))

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

  XFAIL ("XML error",
         "",                    // ignored
         "<a>");

  XFAIL ("Unknown element",
         "Unknown element",
         "<unknown/>");

  XFAIL ("Missing parent",
         "<head>: Missing parent",
         "<head/>");

  XFAIL ("Unknown child",
         "<ncl>: Unknown child <media>",
         "<ncl><media/></ncl>");

  XFAIL ("Unknown child",
         "<ncl>: Unknown child <unknown>",
         "<ncl><unknown/></ncl>");

  XFAIL ("Unknown attribute",
         "<ncl>: Unknown attribute 'unknown'",
         "<ncl unknown='unknown'/>");

  XFAIL ("ncl: Bad id",
         "<ncl>: Bad value '@' for attribute 'id' (must not contain '@')",
         "<ncl id='@'/>");


// -------------------------------------------------------------------------
// <region>
// -------------------------------------------------------------------------

  XFAIL ("region: Missing id",
         "<region>: Missing attribute 'id'", "\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <region/>\n\
  </regionBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// <descriptor>
// -------------------------------------------------------------------------

  XFAIL ("descriptor: Missing id",
         "<descriptor>: Missing attribute 'id'", "\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor/>\n\
  </descriptorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("descriptor: Bad region",
         "<descriptor>: Bad value 'nonexistent' for attribute 'region' "
         "(no such region)", "\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor id='d' region='nonexistent'/>\n\
  </descriptorBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  XFAIL ("descriptor: Bad region",
         "<descriptor>: Bad value 'r' for attribute 'region' "
         "(no such region)", "\
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


// -------------------------------------------------------------------------
// <descriptorParam>
// -------------------------------------------------------------------------

  XFAIL ("descriptorParam: Missing name",
         "<descriptorParam>: Missing attribute 'name'", "\
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

  XFAIL ("descriptorParam: Missing value",
         "<descriptorParam>: Missing attribute 'value'", "\
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
// <causalConnector>
// -------------------------------------------------------------------------

  XFAIL ("causalConnector: Missing id",
         "<causalConnector>: Missing attribute 'id'", "\
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

  XFAIL ("causalConnector: Missing condition",
         "<causalConnector>: Missing child <simpleCondition>", "\
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

  XFAIL ("causalConnector: Missing condition",
         "<causalConnector>: Missing child <simpleCondition>", "\
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

  XFAIL ("causalConnector: Missing condition",
         "<causalConnector>: Missing child <simpleCondition>", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition/>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("causalConnector: Missing action",
         "<causalConnector>: Missing child <simpleAction>", "\
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

  XFAIL ("causalConnector: Missing action",
         "<causalConnector>: Missing child <simpleAction>", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <compoundAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// <simpleCondition>
// -------------------------------------------------------------------------

  XFAIL ("simpleCondition: Missing role",
         "<simpleCondition>: Missing attribute 'role'", "\
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

  XFAIL ("simpleCondition: Reserved role",
         "<simpleCondition>: Bad value 'presentation' "
         "for attribute 'eventType' "
         "(role 'onBegin' is reserved and cannot be overwritten)", "\
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

  XFAIL ("simpleCondition: Reserved role",
         "<simpleCondition>: Bad value 'starts' "
         "for attribute 'transition' "
         "(role 'onBegin' is reserved and cannot be overwritten)", "\
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

  XFAIL ("simpleCondition: Missing eventType",
         "<simpleCondition>: Missing attribute 'eventType'", "\
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

  XFAIL ("simpleCondition: Bad eventType",
         "<simpleCondition>: Bad value 'unknown' "
         "for attribute 'eventType' (no such event type)", "\
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

  XFAIL ("simpleCondition: Missing transition",
         "<simpleCondition>: Missing attribute 'transition'", "\
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

  XFAIL ("simpleCondition: Bad transition",
         "<simpleCondition>: Bad value 'unknown' "
         "for attribute 'transition' (no such transition)", "\
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
// <simpleAction>
// -------------------------------------------------------------------------

  XFAIL ("simpleAction: Missing role",
         "<simpleAction>: Missing attribute 'role'", "\
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

  XFAIL ("simpleAction: Reserved role",
         "<simpleAction>: Bad value 'presentation' "
         "for attribute 'eventType' "
         "(role 'start' is reserved and cannot be overwritten)", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='start' eventType='presentation'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleAction: Reserved role",
         "<simpleAction>: Bad value 'starts' "
         "for attribute 'actionType' "
         "(role 'start' is reserved and cannot be overwritten)", "\
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

  XFAIL ("simpleAction: Missing eventType",
         "<simpleAction>: Missing attribute 'eventType'", "\
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

  XFAIL ("simpleAction: Bad eventType",
         "<simpleAction>: Bad value 'unknown' for attribute 'eventType' "
         "(no such event type)", "\
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

  XFAIL ("simpleAction: Missing actionType",
         "<simpleAction>: Missing attribute 'actionType'", "\
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

  XFAIL ("simpleAction: Bad actionType",
         "<simpleAction>: Bad value 'unknown' for attribute 'actionType' "
         "(no such transition)", "\
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

  XFAIL ("simpleAction: Missing value",
         "<simpleAction>: Missing attribute 'value'", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='set'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// <port>
// -------------------------------------------------------------------------

  XFAIL ("port: Missing id",
         "<port>: Missing attribute 'id'", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Missing component",
         "<port>: Missing attribute 'component'", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port id='p'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad component",
         "<port>: Bad value 'p' for attribute 'component' "
         "(no such object in scope)", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port id='p' component='p'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad component",
         "<port>: Bad value 'r' for attribute 'component' "
         "(no such object in scope)", "\
<ncl>\n\
 <head>\n\
  <regionBase id='r'/>\n\
 </head>\n\
 <body>\n\
  <port id='p' component='r'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad Component",
         "<port>: Bad value 'b' for attribute 'component' "
         "(no such object in scope)", "\
<ncl>\n\
 <body>\n\
  <port id='p' component='b'/>\n\
  <context id='a'>\n\
   <context id='b'/>\n\
  </context>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad interface",
         "<port>: Bad value 'r' for attribute 'interface' "
         "(no such interface in object 'm')", "\
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

  XFAIL ("port: Bad interface",
         "<port>: Bad value 'nonexistent' for attribute 'interface' "
         "(no such interface in object 'm')", "\
<ncl>\n\
 <body>\n\
  <port id='p' component='m' interface='nonexistent'/>\n\
  <media id='m'/>\n\
 </body>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// <media>
// -------------------------------------------------------------------------

  XFAIL ("media: Missing id",
         "<media>: Missing attribute 'id'", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("media: Duplicated id",
         "<media>: Bad value 'a' for attribute 'id' (duplicated id)", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='a'/>\n\
  <media id='a'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("media: Bad descriptor",
         "<media>: Bad value 'nonexistent' for attribute 'descriptor' "
         "(no such descriptor)", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='a' descriptor='nonexistent'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("media: Bad descriptor",
         "<media>: Bad value 'r' for attribute 'descriptor' "
         "(no such descriptor)", "\
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
// <link>
// -------------------------------------------------------------------------

  XFAIL ("link: Missing xconnector",
         "<link>: Missing attribute 'xconnector'", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <link>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("link: No such xconnector",
         "<link>: Bad value 'c' for attribute 'xconnector' "
         "(no such connector)", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <link xconnector='c'>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("link: Link does not match connector",
         "<link>: Bad value 'c' for attribute 'xconnector' "
         "(link does not match connector)", "\
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
// <linkParam>
// -------------------------------------------------------------------------

  XFAIL ("linkParam: Missing name",
         "<linkParam>: Missing attribute 'name'", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='set' value='$val'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
  <media id='x'>\n\
   <property name='p'/>\n\
  </media>\n\
  <link xconnector='c'>\n\
   <linkParam/>\n\
   <bind role='onBegin' component='x'/>\n\
   <bind role='set' component='x' interface='p'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("linkParam: Missing value",
         "<linkParam>: Missing attribute 'value'", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='set' value='$val'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
  <media id='x'>\n\
   <property name='p'/>\n\
  </media>\n\
  <link xconnector='c'>\n\
   <linkParam name='c'/>\n\
   <bind role='onBegin' component='x'/>\n\
   <bind role='set' component='x' interface='p'>\n\
   </bind>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// <bind>
// -------------------------------------------------------------------------

  XFAIL ("bind: missing attribute role",
         "<bind>: Missing attribute 'role'", "\
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
  <media id='x'/>\n\
  <link xconnector='c'>\n\
   <bind component='x'/>\n\
   <bind role='start' component='x'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bind: missing attribute component",
         "<bind>: Missing attribute 'component'", "\
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
  <media id='x'/>\n\
  <link xconnector='c'>\n\
   <bind role='onBegin'/>\n\
   <bind role='start' component='x'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bind: No such component",
         "<bind>: Bad value 'x' for attribute 'component' "
         "(no such component)", "\
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
   <bind role='onBegin' component='x'/>\n\
   <bind role='start' component='x'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bind: No such component (ghost bind)",
         "<bind>: Bad value 'y' for attribute 'component' "
         "(no such component)", "\
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
  <media id='x'/>\n\
  <link xconnector='c'>\n\
   <bind role='onBegin' component='x'/>\n\
   <bind role='ghost' component='y'/>\n\
   <bind role='start' component='x'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bind: No such interface (area)",
         "<bind>: Bad value 'a' for attribute 'interface' "
         "(no such area in object 'x')", "\
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
  <media id='x'/>\n\
  <link xconnector='c'>\n\
   <bind role='onBegin' component='x' interface='a'/>\n\
   <bind role='start' component='x'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bind: No such interface (property)",
         "<bind>: Bad value 'a' for attribute 'interface' "
         "(no such property in object 'x')", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='set' value='x'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
  <media id='x'/>\n\
  <link xconnector='c'>\n\
   <bind role='onBegin' component='x'/>\n\
   <bind role='set' component='x' interface='a'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bind: No such interface (ghost bind)",
         "<bind>: Bad value '' for attribute 'interface' "
         "(ghost bind requires nonempty interface)", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='set' value='x'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
  <media id='x'/>\n\
  <link xconnector='c'>\n\
   <bind role='onBegin' component='x'/>\n\
   <bind role='ghost' component='x'/>\n\
   <bind role='set' component='x'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");


// -------------------------------------------------------------------------
// <bindParam>
// -------------------------------------------------------------------------

  XFAIL ("bindParam: Missing name",
         "<bindParam>: Missing attribute 'name'", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='set' value='$val'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
  <media id='x'>\n\
   <property name='p'/>\n\
  </media>\n\
  <link xconnector='c'>\n\
   <bind role='onBegin' component='x'/>\n\
   <bind role='set' component='x' interface='p'>\n\
     <bindParam/>\n\
   </bind>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bindParam: Missing value",
         "<bindParam>: Missing attribute 'value'", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='set' value='$val'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
  <media id='x'>\n\
   <property name='p'/>\n\
  </media>\n\
  <link xconnector='c'>\n\
   <bind role='onBegin' component='x'/>\n\
   <bind role='set' component='x' interface='p'>\n\
     <bindParam name='c'/>\n\
   </bind>\n\
  </link>\n\
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
    g_assert (doc->getMedias ()->size () == 1);
    g_assert (doc->getContexts ()->size () == 1);
    delete doc;
  }

  // Success: Root aliases.
  {
    Document *doc;
    PASS (&doc, "Root alias in <ncl>", "\
<ncl id='x'>\n\
 <head/>\n\
 <body/>\n\
</ncl>\n\
");
    g_assert_nonnull (doc);
    g_assert ((*(doc->getRoot ()->getAliases ()))[0] == "x");
    delete doc;

    PASS (&doc, "Root alias in <body>", "\
<ncl id='x'>\n\
 <head/>\n\
 <body id='y'/>\n\
</ncl>\n\
");
    g_assert_nonnull (doc);
    g_assert ((*(doc->getRoot ()->getAliases ()))[0] == "x");
    g_assert ((*(doc->getRoot ()->getAliases ()))[1] == "y");
    delete doc;
  }

  // Success: Settings media.
  {
    Document *doc;
    PASS (&doc, "Settings media", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='m' type='application/x-ginga-settings'>\n\
   <property name='top' value='50%'/>\n\
   <property name='empty'/>\n\
   <property name='x' value='y'/>\n\
  </media>\n\
  <media id='n' type='application/x-ginga-settings'>\n\
   <property name='top' value='30%'/>\n\
   <property name='focusIndex' value='0'/>\n\
   <property name='service.currentFocus' value='0'/>\n\
  </media>\n\
 </body>\n\
</ncl>\n\
");
    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 2);
    g_assert (doc->getMedias ()->size () == 1);
    g_assert (doc->getContexts ()->size () == 1);

    Media *m = cast (MediaSettings *, doc->getObjectByIdOrAlias ("m"));
    g_assert_nonnull (m);
    g_assert (m->getEvents ()->size () == 6);

    g_assert (m->getAttributionEvent ("service.currentFocus") != nullptr);
    g_assert (m->getProperty ("service.currentFocus") == "0");

    g_assert (m->getAttributionEvent ("top") != nullptr);
    g_assert (m->getProperty ("top") == "30%");

    g_assert (m->getAttributionEvent ("empty") != nullptr);
    g_assert (m->getProperty ("empty") == "");

    g_assert (m->getAttributionEvent ("x") != nullptr);
    g_assert (m->getProperty ("x") == "y");

    g_assert (m->getAttributionEvent ("focusIndex") != nullptr);
    g_assert (m->getProperty ("focusIndex") == "0");
  }

  // Success: Single media.
  {
    Document *doc;
    PASS (&doc, "Single media", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='m'>\n\
   <area id='a1'/>\n\
   <area id='a2' begin='3s'/>\n\
   <area id='a3' end='3s'/>\n\
   <property name='top' value='50%'/>\n\
   <property name='empty'/>\n\
   <property name='x' value='y'/>\n\
  </media>\n\
 </body>\n\
</ncl>\n\
");
    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 3);
    g_assert (doc->getMedias ()->size () == 2);
    g_assert (doc->getContexts ()->size () == 1);

    Media *m = cast (Media *, doc->getObjectById ("m"));
    g_assert_nonnull (m);
    g_assert (m->getEvents ()->size () == 7);

    Time begin, end;
    Event *evt = m->getPresentationEvent ("a1");
    g_assert_nonnull (evt);
    evt->getInterval (&begin, &end);
    g_assert (begin == 0 && end == GINGA_TIME_NONE);

    evt = m->getPresentationEvent ("a2");
    g_assert_nonnull (evt);
    evt->getInterval (&begin, &end);
    g_assert (begin == 3 * GINGA_SECOND && end == GINGA_TIME_NONE);

    evt = m->getPresentationEvent ("a3");
    g_assert_nonnull (evt);
    evt->getInterval (&begin, &end);
    g_assert (begin == 0 && end == 3 * GINGA_SECOND);

    g_assert (m->getAttributionEvent ("top") != nullptr);
    g_assert (m->getProperty ("top") == "50%");

    g_assert (m->getAttributionEvent ("empty") != nullptr);
    g_assert (m->getProperty ("empty") == "");

    g_assert (m->getAttributionEvent ("x") != nullptr);
    g_assert (m->getProperty ("x") == "y");
    delete doc;
  }

  // Success: Single media with descriptor and region overrides.
  {
    Document *doc;
    PASS (&doc, "Single media with descriptor and region overrides", "\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <region id='r' top='50%'/>\n\
  </regionBase>\n\
  <descriptorBase>\n\
   <descriptor id='d' explicitDur='5s' freeze='True' region='r'>\n\
    <descriptorParam name='x' value='2'/>\n\
    <descriptorParam name='y' value='2'/>\n\
    <descriptorParam name='freeze' value='false'/>\n\
    <descriptorParam name='top' value='20%'/>\n\
   </descriptor>\n\
  </descriptorBase>\n\
 </head>\n\
 <body>\n\
  <media id='m' descriptor='d'>\n\
   <property name='x' value='1'/>\n\
  </media>\n\
 </body>\n\
</ncl>\n\
");
    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 3);
    g_assert (doc->getMedias ()->size () == 2);
    g_assert (doc->getContexts ()->size () == 1);

    Media *m = cast (Media *, doc->getObjectById ("m"));
    g_assert_nonnull (m);
    g_assert (m->getEvents ()->size () == 10);

    g_assert (m->getAttributionEvent ("x") != nullptr);
    g_assert (m->getProperty ("x") == "1");

    g_assert (m->getAttributionEvent ("explicitDur") != nullptr);
    g_assert (m->getProperty ("explicitDur") == "5s");

    g_assert (m->getAttributionEvent ("freeze") != nullptr);
    g_assert (m->getProperty ("freeze") == "false");

    g_assert (m->getAttributionEvent ("y") != nullptr);
    g_assert (m->getProperty ("y") == "2");

    g_assert (m->getAttributionEvent ("top") != nullptr);
    g_assert (m->getProperty ("top") == "50.00%");

    g_assert (m->getAttributionEvent ("left") != nullptr);
    g_assert (m->getProperty ("left") == "0.00%");

    g_assert (m->getAttributionEvent ("width") != nullptr);
    g_assert (m->getProperty ("width") == "100.00%");

    g_assert (m->getAttributionEvent ("height") != nullptr);
    g_assert (m->getProperty ("height") == "100.00%");

    g_assert (m->getAttributionEvent ("zorder") != nullptr);
    g_assert (m->getProperty ("zorder") == "1");

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
