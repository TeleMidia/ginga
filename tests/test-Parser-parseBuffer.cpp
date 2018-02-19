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

#define DOUBLE_PROP_EQ(m, prop, value)                                     \
  doubleeq (xstrtodorpercent ((m)->getProperty (prop), nullptr), (value))

static bool
check_failure (const string &log, const string &expected, const string &buf)
{
  static int i = 1;
  Document *doc;
  string msg = "";

  g_printerr ("XFAIL #%d: %s\n", i++, log.c_str ());
  doc = Parser::parseBuffer (buf.c_str (), buf.length (), 100, 100, &msg);
  if (doc != nullptr)
    {
      delete doc;
      return false;
    }

  if (expected != "" && !xstrhassuffix (msg, expected))
    {
      g_printerr ("*** Expected:\t\"%s\"\n", expected.c_str ());
      g_printerr ("*** Got:\t\"%s\"\n", msg.c_str ());
      return false;
    }

  g_printerr ("\n");
  return true;
}

static Document *
check_success (const string &log, const string &buf)
{
  static int i = 1;
  Document *doc;
  string msg = "";

  g_printerr ("PASS #%d: %s\n", i++, log.c_str ());
  doc = Parser::parseBuffer (buf.c_str (), buf.length (), 100, 100, &msg);
  if (msg != "")
    {
      g_printerr ("*** Unexpected error: %s", msg.c_str ());
      g_assert_not_reached ();
    }
  return doc;
}

#define XFAIL(log, exp, str) g_assert (check_failure ((log), (exp), (str)))

#define PASS(obj, log, str)                                                \
  G_STMT_START                                                             \
  {                                                                        \
    tryset (obj, check_success ((log), (str)));                            \
    g_assert_nonnull (*(obj));                                             \
  }                                                                        \
  G_STMT_END

int
main (void)
{
  string tmp;
  string buf;

  // -------------------------------------------------------------------------
  // General errors.
  // -------------------------------------------------------------------------

  XFAIL ("XML error",
         "", // ignored
         "<a>");

  XFAIL ("Unknown element", "Unknown element", "<unknown/>");

  XFAIL ("Missing parent", "<head> at line 1: Missing parent", "<head/>");

  XFAIL ("Unknown child", "<ncl> at line 1: Unknown child <media>",
         "<ncl><media/></ncl>");

  XFAIL ("Unknown child", "<ncl> at line 1: Unknown child <unknown>",
         "<ncl><unknown/></ncl>");

  XFAIL ("Unknown attribute",
         "<ncl> at line 1: Unknown attribute 'unknown'",
         "<ncl unknown='unknown'/>");

  XFAIL ("ncl: Empty id",
         "<ncl> at line 1: Bad value '' for attribute 'id' "
         "(must not be empty)",
         "<ncl id=''/>");

  XFAIL ("ncl: Bad id",
         "<ncl> at line 1: Bad value '@' for attribute 'id' "
         "(must not contain '@')",
         "<ncl id='@'/>");

  // -------------------------------------------------------------------------
  // <region>
  // -------------------------------------------------------------------------

  XFAIL ("region: Missing id", "<region> at line 4: Missing attribute 'id'",
         "\
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
         "<descriptor> at line 4: Missing attribute 'id'", "\
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
         "<descriptor> at line 4: Bad value 'nonexistent' "
         "for attribute 'region' "
         "(no such region)",
         "\
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
         "<descriptor> at line 5: Bad value 'r' for attribute 'region' "
         "(no such region)",
         "\
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

  XFAIL ("descriptor: Bad transIn",
         "<descriptor> at line 4: Bad value 'nonexistent' "
         "for attribute 'transIn' "
         "(no such transition)",
         "\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor id='d' transIn='nonexistent'/>\n\
  </descriptorBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  XFAIL ("descriptor: Bad transIn",
         "<descriptor> at line 5: Bad value 't' for attribute 'transIn' "
         "(no such transition)",
         "\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor id='t'/>\n\
   <descriptor id='d' transIn='t'/>\n\
  </descriptorBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  XFAIL ("descriptor: Bad transOut",
         "<descriptor> at line 4: Bad value 'nonexistent' "
         "for attribute 'transOut' "
         "(no such transition)",
         "\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor id='d' transOut='nonexistent'/>\n\
  </descriptorBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  XFAIL ("descriptor: Bad transOut",
         "<descriptor> at line 5: Bad value 't' for attribute 'transOut' "
         "(no such transition)",
         "\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor id='t'/>\n\
   <descriptor id='d' transOut='t'/>\n\
  </descriptorBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <descriptorParam>
  // -------------------------------------------------------------------------

  XFAIL ("descriptorParam: Missing name",
         "<descriptorParam> at line 5: Missing attribute 'name'", "\
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
         "<descriptorParam> at line 5: Missing attribute 'value'", "\
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
         "<causalConnector> at line 4: Missing attribute 'id'", "\
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
         "<causalConnector> at line 4: Missing child <simpleCondition>",
         "\
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
         "<causalConnector> at line 4: Missing child <simpleCondition>",
         "\
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
         "<causalConnector> at line 4: Missing child <simpleCondition>",
         "\
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

  XFAIL ("causalConnector: Missing condition",
         "<causalConnector> at line 4: Missing child <simpleCondition>",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition/>\n\
    <compoundAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("causalConnector: Missing condition",
         "<causalConnector> at line 4: Missing child <simpleCondition>",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition/>\n\
    <compoundCondition/>\n\
    <compoundAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("causalConnector: Missing action",
         "<causalConnector> at line 4: Missing child <simpleAction>", "\
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
         "<causalConnector> at line 4: Missing child <simpleAction>", "\
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
         "<simpleCondition> at line 5: Missing attribute 'role'", "\
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

  XFAIL ("simpleCondition: Bad role",
         "<simpleCondition> at line 5: Bad value '' for attribute 'role' "
         "(must not be empty)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role=''/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleCondition: Bad role",
         "<simpleCondition> at line 5: Bad value '$x' for attribute 'role' "
         "(must not contain '$')",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='$x'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleCondition: Bad role",
         "<simpleCondition> at line 5: Bad value 'start' "
         "for attribute 'role' "
         "(reserved role 'start' must be an action)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleCondition: Reserved role",
         "<simpleCondition> at line 5: Bad value 'selection' "
         "for attribute 'eventType' "
         "(reserved role 'onBegin' cannot be overwritten)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin' eventType='selection'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleCondition: Reserved role",
         "<simpleCondition> at line 5: Bad value 'stops' "
         "for attribute 'transition' "
         "(reserved role 'onBegin' cannot be overwritten)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin' transition='stops'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleCondition: Missing eventType",
         "<simpleCondition> at line 5: Missing attribute 'eventType'", "\
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
         "<simpleCondition> at line 5: Bad value 'unknown' "
         "for attribute 'eventType'",
         "\
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
         "<simpleCondition> at line 5: Missing attribute 'transition'", "\
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
         "<simpleCondition> at line 6: Bad value 'unknown' "
         "for attribute 'transition'",
         "\
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
         "<simpleAction> at line 6: Missing attribute 'role'", "\
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

  XFAIL ("simpleAction: Bad role",
         "<simpleAction> at line 6: Bad value '' for attribute 'role' "
         "(must not be empty)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role=''/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleAction: Bad role",
         "<simpleAction> at line 6: Bad value '$x' for attribute 'role' "
         "(must not contain '$')",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='$x'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleAction: Bad role",
         "<simpleAction> at line 6: Bad value 'onBegin' "
         "for attribute 'role' "
         "(reserved role 'onBegin' must be a condition)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='onBegin'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleAction: Reserved role",
         "<simpleAction> at line 6: Bad value 'selection' "
         "for attribute 'eventType' "
         "(reserved role 'start' cannot be overwritten)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='start' eventType='selection'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleAction: Reserved role",
         "<simpleAction> at line 6: Bad value 'stops' "
         "for attribute 'actionType' "
         "(reserved role 'start' cannot be overwritten)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='start' actionType='stops'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("simpleAction: Missing eventType",
         "<simpleAction> at line 6: Missing attribute 'eventType'", "\
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
         "<simpleAction> at line 6: Bad value 'unknown' "
         "for attribute 'eventType'",
         "\
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
         "<simpleAction> at line 6: Missing attribute 'actionType'", "\
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
         "<simpleAction> at line 7: Bad value 'unknown' "
         "for attribute 'actionType'",
         "\
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
         "<simpleAction> at line 6: Missing attribute 'value'", "\
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
  // <compoundStatement>
  // -------------------------------------------------------------------------

  XFAIL ("compoundStatement: Missing operator",
         "<compoundStatement> at line 7: Missing attribute 'operator'", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onBegin'/>\n\
     <compoundStatement>\n\
      <assessmentStatement comparator='eq'>\n\
       <valueAssessment value='x'/>\n\
       <valueAssessment value='x'/>\n\
      </assessmentStatement>\n\
     </compoundStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("compoundStatement: Bad operator",
         "<compoundStatement> at line 7: Bad value 'xx' "
         "for attribute 'operator'",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onBegin'/>\n\
     <compoundStatement operator='xx'>\n\
      <assessmentStatement comparator='eq'>\n\
       <valueAssessment value='x'/>\n\
       <valueAssessment value='x'/>\n\
      </assessmentStatement>\n\
     </compoundStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("compoundStatement: Bad isNegated",
         "<compoundStatement> at line 7: Bad value 'xx' "
         "for attribute 'isNegated'",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onBegin'/>\n\
     <compoundStatement operator='or' isNegated='xx'>\n\
      <assessmentStatement comparator='eq'>\n\
       <valueAssessment value='x'/>\n\
       <valueAssessment value='x'/>\n\
      </assessmentStatement>\n\
     </compoundStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("compoundStatement: Missing child",
         "<compoundStatement> at line 7: Missing child "
         "<compoundStatement> or <assessmentStatement>",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onBegin'/>\n\
     <compoundStatement operator='not'>\n\
     </compoundStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("compoundStatement: Missing child",
         "<compoundStatement> at line 7: Missing child "
         "<compoundStatement> or <assessmentStatement>",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onBegin'/>\n\
     <compoundStatement operator='and'>\n\
     </compoundStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("compoundStatement: Missing child",
         "<compoundStatement> at line 7: Missing child "
         "<compoundStatement> or <assessmentStatement>",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onBegin'/>\n\
     <compoundStatement operator='or'>\n\
     </compoundStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("compoundStatement: Too many children",
         "<compoundStatement> at line 7: Bad child <assessmentStatement> "
         "(too many children)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onBegin'/>\n\
     <compoundStatement operator='not'>\n\
      <assessmentStatement comparator='eq'>\n\
       <valueAssessment value='x'/>\n\
       <valueAssessment value='x'/>\n\
      </assessmentStatement>\n\
      <assessmentStatement comparator='eq'>\n\
       <valueAssessment value='x'/>\n\
       <valueAssessment value='x'/>\n\
      </assessmentStatement>\n\
     </compoundStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("compoundStatement: Too many children",
         "<compoundStatement> at line 7: Bad child <compoundStatement> "
         "(too many children)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onBegin'/>\n\
     <compoundStatement operator='not'>\n\
      <assessmentStatement comparator='eq'>\n\
       <valueAssessment value='x'/>\n\
       <valueAssessment value='x'/>\n\
      </assessmentStatement>\n\
      <compoundStatement operator='not'>\n\
       <assessmentStatement comparator='eq'>\n\
        <valueAssessment value='x'/>\n\
        <valueAssessment value='x'/>\n\
       </assessmentStatement>\n\
      </compoundStatement>\n\
     </compoundStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <assessmentStatement>
  // -------------------------------------------------------------------------

  XFAIL ("assessmentStatement: Missing comparator",
         "<assessmentStatement> at line 6: "
         "Missing attribute 'comparator'",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <assessmentStatement/>\n\
    </compoundCondition>\n\
    <compoundAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("assessmentStatement: Bad comparator",
         "<assessmentStatement> at line 6: Bad value 'xx' "
         "for attribute 'comparator'",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <assessmentStatement comparator='xx'/>\n\
    </compoundCondition>\n\
    <compoundAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("assessmentStatement: Missing child",
         "<assessmentStatement> at line 6: Missing child "
         "<attributeAssessment> or <valueAssessment>",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <assessmentStatement comparator='eq'/>\n\
    </compoundCondition>\n\
    <compoundAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("assessmentStatement: Missing child",
         "<assessmentStatement> at line 6: Missing child "
         "<attributeAssessment> or <valueAssessment>",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <assessmentStatement comparator='eq'>\n\
      <attributeAssessment role='x'/>\n\
     </assessmentStatement>\n\
    </compoundCondition>\n\
    <compoundAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("assessmentStatement: Too many children",
         "<assessmentStatement> at line 6: Bad child "
         "<valueAssessment> (too many children)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <assessmentStatement comparator='eq'>\n\
      <valueAssessment value='0'/>\n\
      <valueAssessment value='0'/>\n\
      <valueAssessment value='0'/>\n\
     </assessmentStatement>\n\
    </compoundCondition>\n\
    <compoundAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <attributeAssessment>
  // -------------------------------------------------------------------------

  XFAIL ("attributeAssessment: Missing role",
         "<attributeAssessment> at line 7: Missing attribute 'role'", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <assessmentStatement comparator='eq'>\n\
      <attributeAssessment/>\n\
     </assessmentStatement>\n\
    </compoundCondition>\n\
    <compoundAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("attributeAssessment: Bad role",
         "<attributeAssessment> at line 7: Bad value '' "
         "for attribute 'role' "
         "(must not be empty)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <assessmentStatement comparator='eq'>\n\
      <attributeAssessment role=''/>\n\
     </assessmentStatement>\n\
    </compoundCondition>\n\
    <compoundAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("attributeAssessment: Bad role",
         "<attributeAssessment> at line 7: Bad value '$x' "
         "for attribute 'role' "
         "(must not contain '$')",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <assessmentStatement comparator='eq'>\n\
      <attributeAssessment role='$x'/>\n\
     </assessmentStatement>\n\
    </compoundCondition>\n\
    <compoundAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <valueAssessment>
  // -------------------------------------------------------------------------

  XFAIL ("valueAssessment: Missing value",
         "<valueAssessment> at line 7: Missing attribute 'value'", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <assessmentStatement comparator='eq'>\n\
      <valueAssessment/>\n\
     </assessmentStatement>\n\
    </compoundCondition>\n\
    <compoundAction/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <rule>
  // -------------------------------------------------------------------------

  XFAIL ("rule: Missing id", "<rule> at line 4: Missing attribute 'id'",
         "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <rule/>\n\
  </ruleBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("rule: Missing var", "<rule> at line 4: Missing attribute 'var'",
         "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <rule id='r'/>\n\
  </ruleBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("rule: Bad var",
         "<rule> at line 4: Bad value '$' for attribute 'var' "
         "(must not contain '$')",
         "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <rule id='r' var='$'/>\n\
  </ruleBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("rule: Missing comparator",
         "<rule> at line 4: Missing attribute 'comparator'", "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <rule id='r' var='v'/>\n\
  </ruleBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("rule: Bad comparator",
         "<rule> at line 4: Bad value 'x' for attribute 'comparator'", "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <rule id='r' var='v' comparator='x' value='x'/>\n\
  </ruleBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("rule: Missing value",
         "<rule> at line 4: Missing attribute 'value'", "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <rule id='r' var='v' comparator='eq'/>\n\
  </ruleBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <compositeRule>
  // -------------------------------------------------------------------------

  XFAIL ("compositeRule:",
         "<compositeRule> at line 4: Missing attribute 'id'", "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <compositeRule/>\n\
  </ruleBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  XFAIL ("compositeRule: Missing operator",
         "<compositeRule> at line 4: Missing attribute 'operator'", "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <compositeRule id='r'/>\n\
  </ruleBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  XFAIL ("compositeRule: Bad operator",
         "<compositeRule> at line 4: Bad value 'x' "
         "for attribute 'operator'",
         "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <compositeRule id='r' operator='x'/>\n\
  </ruleBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  XFAIL ("compositeRule: Too many children",
         "<compositeRule> at line 4: Bad child <compositeRule> "
         "(too many children)",
         "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <compositeRule id='r' operator='not'>\n\
    <compositeRule id='r1' operator='not'/>\n\
    <compositeRule id='r2' operator='not'/>\n\
   </compositeRule>\n\
  </ruleBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  XFAIL ("compositeRule: Too many children",
         "<compositeRule> at line 4: Bad child <rule> "
         "(too many children)",
         "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <compositeRule id='r' operator='not'>\n\
    <rule id='r1' var='x' comparator='eq' value='1'/>\n\
    <rule id='r2' var='y' comparator='eq' value='2'/>\n\
   </compositeRule>\n\
  </ruleBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <transition>
  // -------------------------------------------------------------------------

  XFAIL ("transition: Missing id",
         "<transition> at line 4: Missing attribute 'id'", "\
<ncl>\n\
 <head>\n\
  <transitionBase>\n\
   <transition/>\n\
  </transitionBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("transition: Missing type",
         "<transition> at line 4: Missing attribute 'type'", "\
<ncl>\n\
 <head>\n\
  <transitionBase>\n\
   <transition id='t'/>\n\
  </transitionBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("transition: Bad type",
         "<transition> at line 4: Bad value '$' for attribute 'type' "
         "(must not contain '$')",
         "\
<ncl>\n\
 <head>\n\
  <transitionBase>\n\
   <transition id='t' type='$'/>\n\
  </transitionBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("transition: Bad subtype",
         "<transition> at line 4: Bad value '$' for attribute 'subtype' "
         "(must not contain '$')",
         "\
<ncl>\n\
 <head>\n\
  <transitionBase>\n\
   <transition id='t' type='x' subtype='$'/>\n\
  </transitionBase>\n\
 </head>\n\
 <body>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <importBase>
  // -------------------------------------------------------------------------

  XFAIL ("importBase: Missing alias",
         "<importBase> at line 4: Missing attribute 'alias'", "\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <importBase/>\n\
  </regionBase>\n\
 </head>\n\
</ncl>\n\
");

  XFAIL ("importBase: Bad alias",
         "<importBase> at line 4: Bad value '$' "
         "for attribute 'alias' (must not contain '$')",
         "\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <importBase alias='$'/>\n\
  </regionBase>\n\
 </head>\n\
</ncl>\n\
");

  XFAIL ("importBase: Missing documentURI",
         "<importBase> at line 4: Missing attribute 'documentURI'", "\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <importBase alias='r'/>\n\
  </regionBase>\n\
 </head>\n\
</ncl>\n\
");

  XFAIL ("importBase: Bad documentURI",
         xstrbuild ("<importBase> at line 4: "
                    "Syntax error in imported document "
                    "(XML error: failed to load external entity \"%s\")",
                    xurifromsrc (xpathbuildabs (".", "nonexistent"), "").c_str ()),
         "\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <importBase alias='r' documentURI='nonexistent'/>\n\
  </regionBase>\n\
 </head>\n\
</ncl>\n\
");

  tmp = tests_write_tmp_file ("<x>\n");
  XFAIL ("importBase: Bad imported document",
         "<importBase> at line 4: Syntax error in imported document "
             + xstrbuild ("(%s: XML error at line 2: Premature end of data "
                          "in tag x line 1)",
                          xurifromsrc (tmp, "").c_str ()),
         xstrbuild ("\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <importBase alias='r' documentURI='%s'/>\n\
  </regionBase>\n\
 </head>\n\
</ncl>\n\
",
                    tmp.c_str ()));
  g_remove (tmp.c_str ());

  tmp = tests_write_tmp_file ("<x/>\n");
  XFAIL ("importBase: Missing root",
         "<x> at line 1: Unknown element", xstrbuild ("\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <importBase alias='r' documentURI='%s'/>\n\
  </regionBase>\n\
 </head>\n\
</ncl>\n\
", tmp.c_str ()));
  g_remove (tmp.c_str ());

  tmp = tests_write_tmp_file ("<ncl/>");
  XFAIL ("importBase: Bad imported document",
         "<importBase> at line 4: Syntax error in imported document "
         "(no <regionBase> in imported document)",
         xstrbuild ("\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <importBase alias='r' documentURI='%s'/>\n\
  </regionBase>\n\
 </head>\n\
</ncl>\n\
",
                    tmp.c_str ()));
  g_remove (tmp.c_str ());

  tmp = tests_write_tmp_file ("<ncl><head/></ncl>");
  XFAIL ("importBase: Bad imported document",
         "<importBase> at line 4: Syntax error in imported document "
         "(no <regionBase> in imported document)",
         xstrbuild ("\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <importBase alias='r' documentURI='%s'/>\n\
  </regionBase>\n\
 </head>\n\
</ncl>\n\
",
                    tmp.c_str ()));
  g_remove (tmp.c_str ());

  tmp = tests_write_tmp_file ("<ncl><x/></ncl>");
  XFAIL (
      "importBase: Missing head",
      "<ncl> at line 1: Unknown child <x>", xstrbuild ("\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <importBase alias='r' documentURI='%s'/>\n\
  </regionBase>\n\
 </head>\n\
</ncl>\n\
", tmp.c_str ()));
  g_remove (tmp.c_str ());

  tmp = tests_write_tmp_file ("<ncl><head><connectorBase/></head></ncl>");
  XFAIL ("importBase: Bad imported document",
         "<importBase> at line 4: Syntax error in imported document "
         "(no <regionBase> in imported document)",
         xstrbuild ("\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <importBase alias='r' documentURI='%s'/>\n\
  </regionBase>\n\
 </head>\n\
</ncl>\n\
",
                    tmp.c_str ()));
  g_remove (tmp.c_str ());

  tmp = tests_write_tmp_file ("");
  buf = xstrbuild ("\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <importBase alias='b' documentURI='%s'/>\n\
  </connectorBase>\n\
 </head>\n\
</ncl>\n\
",tmp.c_str ());
  g_assert (g_file_set_contents (tmp.c_str (), buf.c_str (), -1, nullptr));

  XFAIL ("importBase: Circular import",
         "<importBase> at line 4: Syntax error in imported document "
         "(circular import)",
         xstrbuild ("\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <importBase alias='r' documentURI='%s'/>\n\
  </connectorBase>\n\
 </head>\n\
</ncl>\n\
",
                    tmp.c_str ()));
  g_remove (tmp.c_str ());

  // -------------------------------------------------------------------------
  // <context>
  // -------------------------------------------------------------------------

  XFAIL ("context: Missing id",
         "<context> at line 4: Missing attribute 'id'", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <context/>\n\
 <port/>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <port>
  // -------------------------------------------------------------------------

  XFAIL ("port: Missing id", "<port> at line 4: Missing attribute 'id'",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Missing component",
         "<port> at line 4: Missing attribute 'component'", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port id='p'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad component",
         "<port> at line 4: Bad value 'p' for attribute 'component' "
         "(no such object in scope)",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <port id='p' component='p'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad component",
         "<port> at line 6: Bad value 'r' for attribute 'component' "
         "(no such object in scope)",
         "\
<ncl>\n\
 <head>\n\
  <regionBase id='r'/>\n\
 </head>\n\
 <body>\n\
  <port id='p' component='r'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad component",
         "<port> at line 3: Bad value 'b' for attribute 'component' "
         "(no such object in scope)",
         "\
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
         "<port> at line 6: Bad value 'r' for attribute 'interface' "
         "(no such interface)",
         "\
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
         "<port> at line 3: Bad value 'nonexistent' "
         "for attribute 'interface' "
         "(no such interface)",
         "\
<ncl>\n\
 <body>\n\
  <port id='p' component='m' interface='nonexistent'/>\n\
  <media id='m'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad interface",
         "<port> at line 3: Bad value 'nonexistent' "
         "for attribute 'interface' "
         "(no such interface)",
         "\
<ncl>\n\
 <body>\n\
  <port id='p' component='c' interface='nonexistent'/>\n\
  <context id='c'/>\n\
  <media id='m'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad interface",
         "<port> at line 3: Bad value 'p' for attribute 'interface' "
         "(no such interface)",
         "\
<ncl>\n\
 <body>\n\
  <port id='p' component='c' interface='p'/>\n\
  <context id='c'/>\n\
  <media id='m'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad interface",
         "<port> at line 3: Bad value 'm' for attribute 'interface' "
         "(no such interface)",
         "\
<ncl>\n\
 <body>\n\
  <port id='p' component='c' interface='m'/>\n\
  <context id='c'/>\n\
  <media id='m'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad interface",
         "<port> at line 7: Bad value 'p2' for attribute 'interface' "
         "(no such interface)",
         "\
<ncl>\n\
 <body>\n\
  <port id='p0' component='c1' interface='p1'/>\n\
  <context id='c1'>\n\
   <port id='p1' component='c2' interface='p2'/>\n\
   <context id='c2'>\n\
    <port id='p2' component='c2' interface='p2'/>\n\
   </context>\n\
  </context>\n\
  <media id='m'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("port: Bad interface",
         "<port> at line 3: Bad value 'p2' for attribute 'interface' "
         "(no such interface)",
         "\
<ncl>\n\
 <body>\n\
  <port id='p0' component='c1' interface='p2'/>\n\
  <context id='c1'>\n\
   <port id='p1' component='c2' interface='p2'/>\n\
   <context id='c2'>\n\
    <port id='p2' component='c2'/>\n\
   </context>\n\
  </context>\n\
  <media id='m'/>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <switch>
  // -------------------------------------------------------------------------

  XFAIL ("switch: Missing id", "<switch> at line 4: Missing attribute 'id'",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <switch/>\n\
 <port/>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <defaultComponent>
  // -------------------------------------------------------------------------

  XFAIL ("defaultComponent: Missing component",
         "<defaultComponent> at line 5: Missing attribute 'component'", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <switch id='s'>\n\
   <defaultComponent/>\n\
  </switch>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("defaultComponent: Bad constituent",
         "<defaultComponent> at line 5: Bad value 'x' "
         "for attribute 'component' "
         "(no such object in scope)",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <switch id='s'>\n\
   <defaultComponent component='x'/>\n\
  </switch>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("defaultComponent: Bad constituent",
         "<defaultComponent> at line 5: Bad value '__root__' "
         "for attribute 'component' (no such object in scope)",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <switch id='s'>\n\
   <defaultComponent component='__root__'/>\n\
  </switch>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <bindRule>
  // -------------------------------------------------------------------------

  XFAIL ("bindRule: Missing constituent",
         "<bindRule> at line 5: Missing attribute 'constituent'", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <switch id='s'>\n\
   <bindRule/>\n\
  </switch>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bindRule: Missing rule",
         "<bindRule> at line 5: Missing attribute 'rule'", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <switch id='s'>\n\
   <bindRule constituent='x'/>\n\
  </switch>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bindRule: Bad constituent",
         "<bindRule> at line 5: Bad value 'x' for attribute 'constituent' "
         "(no such object in scope)",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <switch id='s'>\n\
   <bindRule constituent='x' rule='x'/>\n\
  </switch>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bindRule: Bad constituent",
         "<bindRule> at line 5: Bad value '__root__' "
         "for attribute 'constituent' "
         "(no such object in scope)",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <switch id='s'>\n\
   <bindRule constituent='__root__' rule='x'/>\n\
  </switch>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bindRule: Bad constituent",
         "<bindRule> at line 6: Bad value 'x' for attribute 'rule' "
         "(no such rule)",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <switch id='s'>\n\
   <media id='m'/>\n\
   <bindRule constituent='m' rule='x'/>\n\
  </switch>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <media>
  // -------------------------------------------------------------------------

  XFAIL ("media: Missing id", "<media> at line 4: Missing attribute 'id'",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("media: Duplicated id",
         "<media> at line 5: Bad value 'a' "
         "for attribute 'id' (must be unique)",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='a'/>\n\
  <media id='a'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("media: Bad descriptor",
         "<media> at line 4: Bad value 'nonexistent' "
         "for attribute 'descriptor' "
         "(no such descriptor)",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='a' descriptor='nonexistent'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("media: Bad descriptor",
         "<media> at line 8: Bad value 'r' for attribute 'descriptor' "
         "(no such descriptor)",
         "\
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

  XFAIL ("media: Mutually exclusive src and refer",
         "<media> at line 3: Attributes 'src' and 'refer' "
         "are mutually exclusive",
         "\
<ncl>\n\
 <body>\n\
  <media id='a' src='b' refer='r'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("media: Mutually exclusive type and refer",
         "<media> at line 3: Attributes 'type' and 'refer' "
         "are mutually exclusive",
         "\
<ncl>\n\
 <body>\n\
  <media id='a' type='b' refer='r'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("media: Bad refer",
         "<media> at line 3: Bad value 'r' for attribute 'refer' "
         "(no such media object)",
         "\
<ncl>\n\
 <body>\n\
  <media id='a' refer='r'/>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("media: Bad refer",
         "<media> at line 5: Bad value 'b' for attribute 'refer' "
         "(cannot refer to a reference)",
         "\
<ncl>\n\
 <body>\n\
  <media id='a'/>\n\
  <media id='b' refer='a'/>\n\
  <media id='c' refer='b'/>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <area>
  // -------------------------------------------------------------------------

  XFAIL ("area: Missing id",
         "Element <area> at line 5: Missing attribute 'id'", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='m'>\n\
   <area/>\n\
  </media>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("area: Duplicated id",
         "Element <area> at line 5: Bad value 'm'"
         " for attribute 'id' (must be unique)",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='m'>\n\
   <area id='m'/>\n\
  </media>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("area: Bad begin",
         "Element <area> at line 5: Bad value 'a'"
         " for attribute 'begin'",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='m'>\n\
   <area id='a' begin='a'/>\n\
  </media>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("area: Bad end",
         "Element <area> at line 5: Bad value 'a' "
         "for attribute 'end'",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='m'>\n\
   <area id='a' end='a'/>\n\
  </media>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("area: Mutually exclusive",
         "Element <area> at line 5: Attributes 'label' and 'begin' "
         "are mutually exclusive",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='m'>\n\
   <area id='a' begin='1' label='a'/>\n\
  </media>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("area: Mutually exclusive",
         "Element <area> at line 5: Attributes 'label' and 'end' "
         "are mutually exclusive",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <media id='m'>\n\
   <area id='a' end='1' label='a'/>\n\
  </media>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <link>
  // -------------------------------------------------------------------------

  XFAIL ("link: Missing xconnector",
         "<link> at line 4: Missing attribute 'xconnector'", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <link>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("link: No such xconnector",
         "<link> at line 4: Bad value 'c' for attribute 'xconnector' "
         "(no such connector)",
         "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <link xconnector='c'>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("link: Link does not match connector",
         "<link> at line 11: Bad value 'c' for attribute 'xconnector' "
         "(link does not match connector, "
         "role 'onBegin' not bound)",
         "\
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

  XFAIL ("link: Link does not match connector",
         "<link> at line 11: Bad value 'c' for attribute 'xconnector' "
         "(link does not match connector, "
         "role 'start' not bound)",
         "\
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
   <bind role='onBegin' component='__root__'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("link: Link does not match connector",
         "<link> at line 17: Bad value 'c' for attribute 'xconnector' "
         "(link does not match connector, "
         "role 'test' not bound)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onBegin'/>\n\
     <assessmentStatement comparator='eq'>\n\
      <attributeAssessment role='test'/>\n\
      <valueAssessment value='0'/>\n\
     </assessmentStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
  <link xconnector='c'>\n\
   <bind role='onBegin' component='__root__'/>\n\
   <bind role='start' component='__root__'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <linkParam>
  // -------------------------------------------------------------------------

  XFAIL ("linkParam: Missing name",
         "<linkParam> at line 15: Missing attribute 'name'", "\
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
         "<linkParam> at line 15: Missing attribute 'value'", "\
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

  XFAIL ("bind: Missing role",
         "<bind> at line 13: Missing attribute 'role'", "\
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

  XFAIL ("bind: Missing component",
         "<bind> at line 13: Missing attribute 'component'", "\
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

  XFAIL ("bind: Bad component",
         "<bind> at line 12: Bad value 'x' for attribute 'component' "
         "(no such object in scope)",
         "\
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

  XFAIL ("bind: Bad component (ghost bind)",
         "<bind> at line 14: Bad value 'y' for attribute 'component' "
         "(no such object in scope)",
         "\
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

  XFAIL ("bind: Bad interface (area)",
         "<bind> at line 13: Bad value 'a' for attribute 'interface' "
         "(no such interface)",
         "\
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

  XFAIL ("bind: Bad interface (area)",
         "<bind> at line 16: Bad value 'p' for attribute 'interface' "
         "(expected a presentation event)",
         "\
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
  <media id='x'>\n\
   <property name='p' value=''/>\n\
  </media>\n\
  <link xconnector='c'>\n\
   <bind role='onBegin' component='x'/>\n\
   <bind role='start' component='x' interface='p'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bind: Bad interface (property)",
         "<bind> at line 14: Bad value '' for attribute 'interface' "
         "(expected an attribution event)",
         "\
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
   <bind role='set' component='x'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bind: Bad interface (selection)",
         "<bind> at line 15: Bad value 'a' for attribute 'interface' "
         "(must be empty)",
         "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onSelection'/>\n\
    <simpleAction role='set' value='x'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
  <media id='x'>\n\
   <area id='a'/>\n\
  </media>\n\
  <link xconnector='c'>\n\
   <bind role='onSelection' component='x' interface='a'/>\n\
   <bind role='set' component='x'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  XFAIL ("bind: Bad interface (ghost bind)",
         "<bind> at line 15: Bad value '' for attribute 'interface' "
         "(expected an attribution event)",
         "\
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

  XFAIL ("bind: Bad interface (switch child)",
         "<bind> at line 17: Bad value 'm' for attribute 'interface' "
         "(no such interface)",
         "\
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
  <switch id='s'>\n\
   <media id='m'/>\n\
  </switch>\n\
  <link xconnector='c'>\n\
   <bind role='onBegin' component='x'/>\n\
   <bind role='start' component='s' interface='m'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");

  // -------------------------------------------------------------------------
  // <bindParam>
  // -------------------------------------------------------------------------

  XFAIL ("bindParam: Missing name",
         "<bindParam> at line 17: Missing attribute 'name'", "\
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
         "<bindParam> at line 17: Missing attribute 'value'", "\
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
    g_assert (doc->getRoot ()->getAliases ()->front () == "x");
    delete doc;

    PASS (&doc, "Root alias in <body>", "\
<ncl id='x'>\n\
 <head/>\n\
 <body id='y'/>\n\
</ncl>\n\
");
    g_assert_nonnull (doc);
    auto it = doc->getRoot ()->getAliases ()->begin ();
    g_assert (*it++ == "x");
    g_assert (*it++ == "y");
    g_assert (it == doc->getRoot ()->getAliases ()->end ());
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

    Context *root = cast (Context *, doc->getRoot ());
    g_assert_nonnull (root);

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

    TRACE ("\n%s", m->toString ().c_str ());
    TRACE ("\n%s", root->toString ().c_str ());
    delete doc;
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
    g_assert (DOUBLE_PROP_EQ (m, "top", .5));

    g_assert (m->getAttributionEvent ("left") != nullptr);
    g_assert (DOUBLE_PROP_EQ (m, "left", 0.));

    g_assert (m->getAttributionEvent ("width") != nullptr);
    g_assert (DOUBLE_PROP_EQ (m, "width", 1.));

    g_assert (m->getAttributionEvent ("height") != nullptr);
    g_assert (DOUBLE_PROP_EQ (m, "height", 1.));

    g_assert (m->getAttributionEvent ("zOrder") != nullptr);
    g_assert (m->getProperty ("zOrder") == "1");

    delete doc;
  }

  // Success: Single media with descriptor and transitions.
  {
    Document *doc;
    PASS (&doc, "Single media with descriptor and transitions", "\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <descriptor id='d' transIn='t1' transOut='t1'/>\n\
  </descriptorBase>\n\
  <transitionBase>\n\
   <transition id='t1' type='barWipe'/>\n\
  </transitionBase>\n\
 </head>\n\
 <body>\n\
  <media id='m' descriptor='d'>\n\
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
    g_assert (m->getEvents ()->size () == 3);

    g_assert (m->getAttributionEvent ("transIn") != nullptr);
    g_assert (m->getProperty ("transIn") == "\
{type='barWipe',subtype='',dur='0',startProgress='0',endProgress='0',\
direction='forward',fadeColor='',horzRepeat='0',vertRepeat='0',\
borderWidth='0',borderColor=''}");

    map<string, string> tab = parse_table (m->getProperty ("transIn"));
    g_assert (tab["type"] == "barWipe");
    g_assert (tab["subtype"] == "");
    g_assert (tab["dur"] == "0");
    g_assert (tab["startProgress"] == "0");
    g_assert (tab["endProgress"] == "0");
    g_assert (tab["direction"] == "forward");
    g_assert (tab["fadeColor"] == "");
    g_assert (tab["horzRepeat"] == "0");
    g_assert (tab["vertRepeat"] == "0");
    g_assert (tab["borderWidth"] == "0");
    g_assert (tab["borderColor"] == "");

    g_assert (m->getAttributionEvent ("transOut") != nullptr);
    g_assert (m->getProperty ("transOut") == "\
{type='barWipe',subtype='',dur='0',startProgress='0',endProgress='0',\
direction='forward',fadeColor='',horzRepeat='0',vertRepeat='0',\
borderWidth='0',borderColor=''}");

    tab = parse_table (m->getProperty ("transIn"));
    g_assert (tab["type"] == "barWipe");
    g_assert (tab["subtype"] == "");
    g_assert (tab["dur"] == "0");
    g_assert (tab["startProgress"] == "0");
    g_assert (tab["endProgress"] == "0");
    g_assert (tab["direction"] == "forward");
    g_assert (tab["fadeColor"] == "");
    g_assert (tab["horzRepeat"] == "0");
    g_assert (tab["vertRepeat"] == "0");
    g_assert (tab["borderWidth"] == "0");
    g_assert (tab["borderColor"] == "");

    TRACE ("\n%s", m->toString ().c_str ());
    delete doc;
  }

  // Success: Single media with imported bases.
  {
    string B0 = tests_write_tmp_file ("\
<ncl>\n\
 <head>\n\
  <regionBase>\n\
   <region id='r' top='50%' left='30%'/>\n\
  </regionBase>\n\
  <descriptorBase>\n\
   <descriptor id='d' focusIndex='3' region='r'/>\n\
  </descriptorBase>\n\
 </head>\n\
</ncl>\n\
");
    g_remove (tmp.c_str ());

    string B1 = tests_write_tmp_file ("\
<ncl>\n\
 <head>\n\
  <transitionBase>\n\
   <transition id='t' type='barWipe'/>\n\
  </transitionBase>\n\
 </head>\n\
 <body/>\n\
</ncl>\n\
");
    g_remove (tmp.c_str ());

    string B2 = tests_write_tmp_file (xstrbuild ("\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <importBase alias='X' documentURI='%s'/>\n\
  </descriptorBase>\n\
 </head>\n\
</ncl>\n\
", B0.c_str ()));
    g_remove (tmp.c_str ());

    Document *doc;
    PASS (&doc, "Single media with imported bases",
          xstrbuild ("\
<ncl>\n\
 <head>\n\
  <descriptorBase>\n\
   <importBase alias='B0' documentURI='%s'/>\n\
   <descriptor id='d' transIn='B1#t'/>\n\
  </descriptorBase>\n\
  <transitionBase>\n\
   <importBase alias='B1' documentURI='%s'/>\n\
  </transitionBase>\n\
  <descriptorBase>\n\
   <importBase alias='B2' documentURI='%s'/>\n\
  </descriptorBase>\n\
 </head>\n\
 <body>\n\
  <media id='m1' descriptor='B0#d'/>\n\
  <media id='m2' descriptor='d'/>\n\
  <media id='m3' descriptor='B2#X#d'/>\n\
 </body>\n\
</ncl>\n\
",
                     B0.c_str (), B1.c_str (), B2.c_str ()));
    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 5);
    g_assert (doc->getMedias ()->size () == 4);
    g_assert (doc->getContexts ()->size () == 1);

    Media *m1 = cast (Media *, doc->getObjectById ("m1"));
    g_assert_nonnull (m1);
    g_assert (DOUBLE_PROP_EQ (m1, "top", .5));
    g_assert (DOUBLE_PROP_EQ (m1, "left", .3));
    g_assert (DOUBLE_PROP_EQ (m1, "width", 1.0));
    g_assert (DOUBLE_PROP_EQ (m1, "height", 1.0));
    g_assert (m1->getProperty ("focusIndex") == "3");
    g_assert (m1->getProperty ("transIn") == "");

    Media *m2 = cast (Media *, doc->getObjectById ("m2"));
    g_assert_nonnull (m2);
    g_assert (m2->getProperty ("top") == "");
    g_assert (m2->getProperty ("left") == "");
    g_assert (m2->getProperty ("width") == "");
    g_assert (m2->getProperty ("height") == "");
    g_assert (m2->getProperty ("transIn") == "{type='barWipe',subtype='',\
dur='0',startProgress='0',endProgress='0',direction='forward',fadeColor='',\
horzRepeat='0',vertRepeat='0',borderWidth='0',borderColor=''}");

    Media *m3 = cast (Media *, doc->getObjectById ("m3"));
    g_assert_nonnull (m3);
    g_assert (DOUBLE_PROP_EQ (m3, "top", .5));
    g_assert (DOUBLE_PROP_EQ (m3, "left", .3));
    g_assert (DOUBLE_PROP_EQ (m3, "width", 1.));
    g_assert (DOUBLE_PROP_EQ (m3, "height", 1.));
    g_assert (m3->getProperty ("focusIndex") == "3");
    g_assert (m3->getProperty ("transIn") == "");

    TRACE ("%s", m1->toString ().c_str ());
    TRACE ("%s", m2->toString ().c_str ());
    TRACE ("%s", m3->toString ().c_str ());
    delete doc;
  }

  // Success: Media with refer.
  {
    Document *doc;
    PASS (&doc, "Media with refer", "\
<ncl>\n\
 <body>\n\
  <context id='c'>\n\
    <media id='r1' refer='m'>\n\
     <property name='x' value='2'/>\n\
     <property name='y' value='a'/>\n\
    </media>\n\
    <media id='r2' refer='m'>\n\
     <property name='y' value='b'/>\n\
    </media>\n\
  </context>\n\
  <media id='m'>\n\
   <property name='x' value='1'/>\n\
  </media>\n\
 </body>\n\
</ncl>\n\
");
    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 4);
    g_assert (doc->getMedias ()->size () == 2);
    g_assert (doc->getContexts ()->size () == 2);

    Media *m = cast (Media *, doc->getObjectByIdOrAlias ("m"));
    g_assert_nonnull (m);

    Media *r1 = cast (Media *, doc->getObjectByIdOrAlias ("r1"));
    g_assert_nonnull (r1);

    Media *r2 = cast (Media *, doc->getObjectByIdOrAlias ("r2"));
    g_assert_nonnull (r2);

    g_assert (m->getAttributionEvent ("x") != nullptr);
    g_assert (m->getProperty ("x") == "1");
    g_assert (m->getAttributionEvent ("y") != nullptr);
    g_assert (m->getProperty ("y") == "b");

    TRACE ("\n%s", m->toString ().c_str ());
    TRACE ("\n%s", r1->toString ().c_str ());
    TRACE ("\n%s", r2->toString ().c_str ());

    delete doc;
  }

  // Success: Nested contexts and ports.
  {
    Document *doc;
    PASS (&doc, "Nested contexts and ports", "\
<ncl>\n\
 <head>\n\
 </head>\n\
 <body id='body'>\n\
  <port id='p1' component='c1'/>\n\
  <port id='p2' component='__root__'/>\n\
  <port id='p3' component='body' interface='x'/>\n\
  <port id='p4' component='c1' interface='p11'/>\n\
  <port id='p5' component='c1' interface='p12'/>\n\
  <context id='c1'>\n\
   <port id='p11' component='c2' interface='p21'/>\n\
   <port id='p12' component='c2' interface='p22'/>\n\
   <context id='c2'>\n\
    <port id='p21' component='c2' interface='y'/>\n\
    <port id='p22' component='m' interface='a'/>\n\
    <property name='y' value='2'/>\n\
    <media id='m'>\n\
     <area id='a'/>\n\
    </media>\n\
   </context>\n\
  </context>\n\
  <property name='x' value='1'/>\n\
 </body>\n\
</ncl>\n\
");
    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 5);
    g_assert (doc->getMedias ()->size () == 2);
    g_assert (doc->getContexts ()->size () == 3);

    Context *root = cast (Context *, doc->getRoot ());
    g_assert_nonnull (root);

    Context *c1 = cast (Context *, doc->getObjectById ("c1"));
    g_assert_nonnull (c1);

    Context *c2 = cast (Context *, doc->getObjectById ("c2"));
    g_assert_nonnull (c2);

    Media *m = cast (Media *, doc->getObjectById ("m"));
    g_assert_nonnull (m);

    g_assert (root->getPorts ()->size () == 5);
    auto port = root->getPorts ()->begin ();
    g_assert (*port++ == c1->getLambda ());
    g_assert (*port++ == root->getLambda ());
    g_assert (*port++ == root->getAttributionEvent ("x"));
    g_assert (*port++ == c2->getAttributionEvent ("y"));
    g_assert (*port++ == m->getPresentationEvent ("a"));
    g_assert (port == root->getPorts ()->end ());

    TRACE ("\n%s", root->toString ().c_str ());
    delete doc;
  }

  // Success: Simple link.
  {
    Document *doc;
    PASS (&doc, "Simple link", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='onBeginStart'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
  <media id='m'>\n\
   <area id='a1'/>\n\
   <area id='a2' begin='3s'/>\n\
   <area id='a3' end='3s'/>\n\
   <property name='top' value='50%'/>\n\
   <property name='empty'/>\n\
   <property name='x' value='y'/>\n\
  </media>\n\
  <link xconnector='onBeginStart'>\n\
   <bind role='onBegin' component='m' interface='a1'/>\n\
   <bind role='start' component='m'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");
    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 3);
    g_assert (doc->getMedias ()->size () == 2);
    g_assert (doc->getContexts ()->size () == 1);

    Context *root = cast (Context *, doc->getRoot ());
    g_assert_nonnull (root);

    Media *m = cast (Media *, doc->getObjectById ("m"));
    g_assert_nonnull (m);

    auto links = doc->getRoot ()->getLinks ();
    g_assert (links->size () == 1);

    auto link = links->begin ();
    g_assert (link->first.size () == 1);
    g_assert (link->second.size () == 1);

    auto cond = link->first.begin ();
    g_assert (cond->event == m->getPresentationEvent ("a1"));
    g_assert (cond->transition == Event::START);
    g_assert (cond->predicate == nullptr);
    g_assert (cond->value == "");

    cond++;
    g_assert (cond == link->first.end ());

    auto act = link->second.begin ();
    g_assert (act->event == m->getLambda ());
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "");

    act++;
    g_assert (act == link->second.end ());

    TRACE ("\n%s", m->toString ().c_str ());
    TRACE ("\n%s", root->toString ().c_str ());
    delete doc;
  }

  // Success: Simple link (vacuous conditions and actions).
  {
    Document *doc;
    PASS (&doc, "Simple link (vacuous conditions and actiosn)", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='onPauseSet'>\n\
    <compoundCondition>\n\
     <compoundCondition>\n\
      <compoundCondition>\n\
       <simpleCondition role='onPause'/>\n\
      </compoundCondition>\n\
     </compoundCondition>\n\
    </compoundCondition>\n\
    <compoundAction>\n\
     <compoundAction>\n\
      <compoundAction>\n\
       <simpleAction role='set' value='33'/>\n\
      </compoundAction>\n\
     </compoundAction>\n\
     <compoundAction/>\n\
    </compoundAction>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
  <media id='m'>\n\
   <area id='a1'/>\n\
   <area id='a2' begin='3s'/>\n\
   <area id='a3' end='3s'/>\n\
   <property name='top' value='50%'/>\n\
   <property name='empty'/>\n\
   <property name='x' value='y'/>\n\
  </media>\n\
  <link xconnector='onPauseSet'>\n\
   <bind role='onPause' component='m' interface='a1'/>\n\
   <bind role='set' component='m' interface='x'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");
    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 3);
    g_assert (doc->getMedias ()->size () == 2);
    g_assert (doc->getContexts ()->size () == 1);

    Context *root = cast (Context *, doc->getRoot ());
    g_assert_nonnull (root);

    Media *m = cast (Media *, doc->getObjectById ("m"));
    g_assert_nonnull (m);

    auto links = doc->getRoot ()->getLinks ();
    g_assert (links->size () == 1);

    auto link = links->begin ();
    g_assert (link->first.size () == 1);
    g_assert (link->second.size () == 1);

    auto cond = link->first.begin ();
    g_assert (cond->event == m->getPresentationEvent ("a1"));
    g_assert (cond->transition == Event::PAUSE);
    g_assert (cond->predicate == nullptr);
    g_assert (cond->value == "");

    cond++;
    g_assert (cond == link->first.end ());

    auto act = link->second.begin ();
    g_assert (act->event == m->getAttributionEvent ("x"));
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "33");

    act++;
    g_assert (act == link->second.end ());

    TRACE ("\n%s", m->toString ().c_str ());
    TRACE ("\n%s", root->toString ().c_str ());
    delete doc;
  }

  // Success: Link and bind parameters.
  {
    Document *doc;
    PASS (&doc, "Link and bind parameters", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='c'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleCondition role='onSelection' key='$key'/>\n\
    <simpleAction role='start'/>\n\
    <simpleAction role='set' value='$key'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body id='body'>\n\
  <port id='pt1' component='m'/>\n\
  <port id='pt2' component='m'/>\n\
  <property name='p'/>\n\
  <media id='m'>\n\
   <area id='a1'/>\n\
   <area id='a2' begin='3s'/>\n\
   <area id='a3' end='3s'/>\n\
   <property name='top' value='50%'/>\n\
   <property name='empty'/>\n\
   <property name='x' value='y'/>\n\
  </media>\n\
  <link xconnector='c'>\n\
   <linkParam name='key' value='RED'/>\n\
   <bind role='onBegin' component='m' interface='a1'/>\n\
   <bind role='onSelection' component='m'/>\n\
   <bind role='start' component='m'/>\n\
   <bind role='set' component='body' interface='p'>\n\
    <bindParam name='key' value='GREEN'/>\n\
   </bind>\n\
  </link>\n\
  <link xconnector='c'>\n\
   <bind role='onBegin' component='__root__'/>\n\
   <bind role='get' component='m' interface='top'/>\n\
   <bind role='onSelection' component='m'>\n\
     <bindParam name='key' value='$get'/>\n\
   </bind>\n\
   <bind role='start' component='m' interface='a1'/>\n\
   <bind role='set' component='body' interface='p'>\n\
    <bindParam name='key' value='$get'/>\n\
   </bind>\n\
   <bind role='set' component='body' interface='p'>\n\
    <bindParam name='key' value='$unknown'/>\n\
   </bind>\n\
   <bind role='set' component='body' interface='p'/>\n\
  </link>\n\
 </body>\n\
</ncl>\n\
");
    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 3);
    g_assert (doc->getMedias ()->size () == 2);
    g_assert (doc->getContexts ()->size () == 1);

    Context *root = doc->getRoot ();
    g_assert_nonnull (root);

    Media *m = cast (Media *, doc->getObjectById ("m"));
    g_assert_nonnull (m);

    auto links = doc->getRoot ()->getLinks ();
    g_assert (links->size () == 2);

    auto link = links->begin ();
    g_assert (link->first.size () == 2);
    g_assert (link->second.size () == 2);

    auto cond = link->first.begin ();
    g_assert (cond->event == m->getPresentationEvent ("a1"));
    g_assert (cond->transition == Event::START);
    g_assert (cond->predicate == nullptr);
    g_assert (cond->value == "");

    cond++;
    g_assert (cond->event == m->getSelectionEvent ("RED"));
    g_assert (cond->transition == Event::START);
    g_assert (cond->predicate == nullptr);
    g_assert (cond->value == "RED");

    auto act = link->second.begin ();
    g_assert (act->event == m->getLambda ());
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "");

    act++;
    g_assert (act->event == root->getAttributionEvent ("p"));
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "GREEN");

    link++;
    cond = link->first.begin ();
    g_assert (cond->event == root->getLambda ());
    g_assert (cond->transition == Event::START);
    g_assert (cond->predicate == nullptr);
    g_assert (cond->value == "");

    cond++;
    TRACE ("%s", cond->event->getFullId ().c_str ());
    g_assert (cond->event == m->getSelectionEvent ("$m.top"));
    g_assert (cond->transition == Event::START);
    g_assert (cond->predicate == nullptr);
    g_assert (cond->value == "$m.top");

    cond++;
    g_assert (cond == link->first.end ());

    act = link->second.begin ();
    g_assert (act->event == m->getPresentationEvent ("a1"));
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "");

    act++;
    g_assert (act->event == root->getAttributionEvent ("p"));
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "$m.top");

    act++;
    g_assert (act->event == root->getAttributionEvent ("p"));
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "$unknown");

    act++;
    g_assert (act->event == root->getAttributionEvent ("p"));
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "$key");

    act++;
    g_assert (act == link->second.end ());

    TRACE ("\n%s", m->toString ().c_str ());
    TRACE ("\n%s", root->toString ().c_str ());
    delete doc;
  }

  // Success: Binds pointing to ports and properties.
  {
    Document *doc;
    PASS (&doc, "Binds pointing to ports and properties", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='conn1'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='start'/>\n\
    <simpleAction role='set' value='13'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body id='body'>\n\
  <port id='pt1' component='m'/>\n\
  <port id='pt2' component='m'/>\n\
  <media id='m'>\n\
   <area id='a1'/>\n\
  </media>\n\
  <link xconnector='conn1'>\n\
   <bind role='onBegin' component='c1'/>\n\
   <bind role='onBegin' component='c1' interface='pt14'/>\n\
   <bind role='start' component='c1' interface='pt11'/>\n\
   <bind role='start' component='c1' interface='pt12'/>\n\
   <bind role='set' component='c1' interface='pt13'/>\n\
   <bind role='set' component='c1' interface='pt15'/>\n\
  </link>\n\
  <context id='c1'>\n\
   <port id='pt11' component='c1'/>\n\
   <port id='pt12' component='m2'/>\n\
   <port id='pt13' component='c1' interface='p11'/>\n\
   <port id='pt14' component='c2' interface='pt21'/>\n\
   <port id='pt15' component='c2' interface='pt22'/>\n\
   <context id='c2'>\n\
     <media id='m3'>\n\
      <area id='a2'/>\n\
      <property name='pm3'/>\n\
     </media>\n\
     <port id='pt21' component='m3' interface='a2'/>\n\
     <port id='pt22' component='m3' interface='pm3'/>\n\
   </context>\n\
   <property name='p11' value=''/>\n\
   <media id='m2'/>\n\
   <link xconnector='conn1'>\n\
    <bind role='onBegin' component='c1'/>\n\
    <bind role='start' component='c1'/>\n\
    <bind role='set' component='c1' interface='p11'/>\n\
   </link>\n\
  </context>\n\
 </body>\n\
</ncl>");

    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 7);
    g_assert (doc->getMedias ()->size () == 4);
    g_assert (doc->getContexts ()->size () == 3);

    Context *root = doc->getRoot ();
    g_assert_nonnull (root);

    Media *m = cast (Media *, doc->getObjectById ("m"));
    g_assert_nonnull (m);

    Media *m2 = cast (Media *, doc->getObjectById ("m2"));
    g_assert_nonnull (m2);

    Media *m3 = cast (Media *, doc->getObjectById ("m3"));
    g_assert_nonnull (m3);

    Context *c1 = cast (Context *, doc->getObjectById ("c1"));
    g_assert_nonnull (c1);

    Context *c2 = cast (Context *, doc->getObjectById ("c2"));
    g_assert_nonnull (c2);

    auto links = doc->getRoot ()->getLinks ();
    g_assert (links->size () == 1);

    auto link = links->begin ();
    g_assert (link->first.size () == 2);
    g_assert (link->second.size () == 4);

    auto cond = link->first.begin ();
    g_assert (cond->event == c1->getLambda ());
    g_assert (cond->transition == Event::START);
    g_assert (cond->predicate == nullptr);
    g_assert (cond->value == "");

    cond++;
    g_assert (cond->event == m3->getPresentationEvent ("a2"));
    g_assert (cond->transition == Event::START);
    g_assert (cond->predicate == nullptr);
    g_assert (cond->value == "");

    cond++;
    g_assert (cond == link->first.end ());

    auto act = link->second.begin ();
    g_assert (act->event == c1->getLambda ());
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "");

    act++;
    g_assert (act->event == m2->getLambda ());
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "");

    act++;
    g_assert (act->event == c1->getAttributionEvent ("p11"));
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "13");

    act++;
    g_assert (act->event == m3->getAttributionEvent ("pm3"));
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "13");

    act++;
    g_assert (act == link->second.end ());

    TRACE ("\n%s", root->toString ().c_str ());
    TRACE ("\n%s", m->toString ().c_str ());
    TRACE ("\n%s", c1->toString ().c_str ());
    delete doc;
  }

  // Success: Simple statement.
  {
    Document *doc;
    PASS (&doc, "Simple statement", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='conn'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onBegin'/>\n\
     <assessmentStatement comparator='eq'>\n\
      <attributeAssessment role='test1'/>\n\
      <valueAssessment value='1'/>\n\
     </assessmentStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body id='body'>\n\
  <property name='p'/>\n\
  <link xconnector='conn'>\n\
   <bind role='onBegin' component='body'/>\n\
   <bind role='test1' component='body' interface='p'/>\n\
   <bind role='start' component='body'/>\n\
  </link>\n\
 </body>\n\
</ncl>");

    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 2);
    g_assert (doc->getMedias ()->size () == 1);
    g_assert (doc->getContexts ()->size () == 1);

    Context *root = doc->getRoot ();
    g_assert_nonnull (root);

    auto links = doc->getRoot ()->getLinks ();
    g_assert (links->size () == 1);

    auto link = links->begin ();
    g_assert (link->first.size () == 1);
    g_assert (link->second.size () == 1);

    auto cond = link->first.begin ();
    g_assert (cond->event == root->getLambda ());
    g_assert (cond->transition == Event::START);
    g_assert (cond->predicate->toString () == "$__root__.p=='1'");
    g_assert (cond->value == "");
    g_assert (++cond == link->first.end ());

    auto act = link->second.begin ();
    g_assert (act->event == root->getLambda ());
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "");
    g_assert (++act == link->second.end ());

    TRACE ("\n%s", root->toString ().c_str ());
    delete doc;
  }

  // Success: More simple statements.
  {
    Document *doc;
    PASS (&doc, "More simple statements", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='conn1'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onBegin'/>\n\
     <assessmentStatement comparator='eq'>\n\
      <attributeAssessment role='test1'/>\n\
      <valueAssessment value='1'/>\n\
     </assessmentStatement>\n\
     <assessmentStatement comparator='ne'>\n\
      <valueAssessment value='2'/>\n\
      <attributeAssessment role='test2'/>\n\
     </assessmentStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body id='body'>\n\
  <property name='p'/>\n\
  <property name='q'/>\n\
  <media id='m'>\n\
   <property name='x'/>\n\
  </media>\n\
  <link xconnector='conn1'>\n\
   <bind role='onBegin' component='body'/>\n\
   <bind role='test1' component='body' interface='p'/>\n\
   <bind role='test2' component='body' interface='p'/>\n\
   <bind role='start' component='body'/>\n\
  </link>\n\
  <link xconnector='conn1'>\n\
   <bind role='onBegin' component='m'/>\n\
   <bind role='test1' component='m' interface='x'/>\n\
   <bind role='test2' component='body' interface='q'/>\n\
   <bind role='start' component='m'/>\n\
  </link>\n\
 </body>\n\
</ncl>");

    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 3);
    g_assert (doc->getMedias ()->size () == 2);
    g_assert (doc->getContexts ()->size () == 1);

    Context *root = doc->getRoot ();
    g_assert_nonnull (root);

    Media *m = cast (Media *, doc->getObjectById ("m"));
    g_assert_nonnull (m);

    auto links = doc->getRoot ()->getLinks ();
    g_assert (links->size () == 2);

    // First link.
    auto link = links->begin ();
    g_assert (link->first.size () == 1);
    g_assert (link->second.size () == 1);

    auto cond = link->first.begin ();
    g_assert (cond->event == root->getLambda ());
    g_assert (cond->transition == Event::START);
    g_assert (cond->predicate->toString ()
              == "and($__root__.p=='1', '2'!=$__root__.p)");
    g_assert (cond->value == "");
    g_assert (++cond == link->first.end ());

    auto act = link->second.begin ();
    g_assert (act->event == root->getLambda ());
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "");
    g_assert (++act == link->second.end ());

    // Second link.
    link++;
    cond = link->first.begin ();
    g_assert (cond->event == m->getLambda ());
    g_assert (cond->transition == Event::START);
    g_assert (cond->predicate->toString ()
              == "and($m.x=='1', '2'!=$__root__.q)");
    g_assert (cond->value == "");
    g_assert (++cond == link->first.end ());

    act = link->second.begin ();
    g_assert (act->event == m->getLambda ());
    g_assert (act->transition == Event::START);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "");
    g_assert (++act == link->second.end ());
    g_assert (++link == links->end ());

    TRACE ("\n%s", root->toString ().c_str ());
    delete doc;
  }

  // Success: Complex statements.
  {
    Document *doc;
    PASS (&doc, "Complex statements", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='conn1'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onAbort'/>\n\
     <assessmentStatement comparator='lte'>\n\
      <attributeAssessment role='test1'/>\n\
      <valueAssessment value='1'/>\n\
     </assessmentStatement>\n\
     <compoundStatement operator='and'>\n\
      <assessmentStatement comparator='ne'>\n\
       <valueAssessment value='2'/>\n\
       <attributeAssessment role='test2'/>\n\
      </assessmentStatement>\n\
      <compoundStatement operator='not'>\n\
       <compoundStatement operator='or' isNegated='true'>\n\
        <assessmentStatement comparator='gt'>\n\
         <valueAssessment value='3'/>\n\
         <valueAssessment value='4'/>\n\
        </assessmentStatement>\n\
        <assessmentStatement comparator='gte'>\n\
         <valueAssessment value='4'/>\n\
         <valueAssessment value='3'/>\n\
        </assessmentStatement>\n\
       </compoundStatement>\n\
      </compoundStatement>\n\
     </compoundStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='resume'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body id='body'>\n\
  <property name='p'/>\n\
  <property name='q'/>\n\
  <link xconnector='conn1'>\n\
   <bind role='onAbort' component='body'/>\n\
   <bind role='test1' component='body' interface='p'/>\n\
   <bind role='test2' component='body' interface='q'/>\n\
   <bind role='resume' component='body'/>\n\
  </link>\n\
 </body>\n\
</ncl>");

    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 2);
    g_assert (doc->getMedias ()->size () == 1);
    g_assert (doc->getContexts ()->size () == 1);

    Context *root = doc->getRoot ();
    g_assert_nonnull (root);

    auto links = doc->getRoot ()->getLinks ();
    g_assert (links->size () == 1);

    auto link = links->begin ();
    g_assert (link->first.size () == 1);
    g_assert (link->second.size () == 1);

    auto cond = link->first.begin ();
    g_assert (cond->event == root->getLambda ());
    g_assert (cond->transition == Event::ABORT);
    g_assert (cond->predicate->toString () == "\
and($__root__.p<='1',\
 and('2'!=$__root__.q, not(not(or('3'>'4', '4'>='3')))))");
    g_assert (cond->value == "");
    g_assert (++cond == link->first.end ());

    auto act = link->second.begin ();
    g_assert (act->event == root->getLambda ());
    g_assert (act->transition == Event::RESUME);
    g_assert (act->predicate == nullptr);
    g_assert (act->value == "");
    g_assert (++act == link->second.end ());
    g_assert (++link == links->end ());

    TRACE ("\n%s", root->toString ().c_str ());
    delete doc;
  }

  // Success: Complex conditions and statements.
  {
    Document *doc;
    PASS (&doc, "Complex statements", "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='conn1'>\n\
    <compoundCondition>\n\
     <simpleCondition role='onEndSelection' key='RED'/>\n\
     <assessmentStatement comparator='ne'>\n\
      <valueAssessment value='0'/>\n\
      <valueAssessment value='0'/>\n\
     </assessmentStatement>\n\
     <assessmentStatement comparator='gt'>\n\
      <valueAssessment value='1'/>\n\
      <valueAssessment value='1'/>\n\
     </assessmentStatement>\n\
     <compoundCondition operator='not'>\n\
      <simpleCondition role='onEndAttribution'/>\n\
      <compoundStatement operator='not'>\n\
        <assessmentStatement comparator='eq'>\n\
         <attributeAssessment role='test1'/>\n\
         <attributeAssessment role='test2'/>\n\
        </assessmentStatement>\n\
      </compoundStatement>\n\
      <compoundCondition>\n\
       <assessmentStatement comparator='lt'>\n\
        <attributeAssessment role='test3'/>\n\
        <valueAssessment value='33'/>\n\
       </assessmentStatement>\n\
       <compoundCondition>\n\
        <compoundCondition>\n\
         <compoundCondition>\n\
          <simpleCondition role='onPause'/>\n\
         </compoundCondition>\n\
        </compoundCondition>\n\
       </compoundCondition>\n\
      </compoundCondition>\n\
     </compoundCondition>\n\
    </compoundCondition>\n\
    <simpleAction role='abort'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body id='body'>\n\
  <property name='q'/>\n\
  <context id='c'>\n\
   <property name='p'/>\n\
   <port id='pt' component='c' interface='p'/>\n\
   <port id='pt2' component='m2' interface='a2'/>\n\
   <media id='m2'>\n\
    <area id='a2'/>\n\
   </media>\n\
  </context>\n\
  <media id='m'>\n\
   <property name='r'/>\n\
  </media>\n\
  <link xconnector='conn1'>\n\
   <bind role='onEndSelection' component='m'/>\n\
   <bind role='onEndAttribution' component='c' interface='pt'/>\n\
   <bind role='onPause' component='c' interface='pt2'/>\n\
   <bind role='test1' component='body' interface='q'/>\n\
   <bind role='test2' component='c' interface='pt'/>\n\
   <bind role='test3' component='m' interface='r'/>\n\
   <bind role='abort' component='__root__'/>\n\
  </link>\n\
 </body>\n\
</ncl>");

    g_assert_nonnull (doc);
    g_assert (doc->getObjects ()->size () == 5);
    g_assert (doc->getMedias ()->size () == 3);
    g_assert (doc->getContexts ()->size () == 2);

    Context *root = doc->getRoot ();
    g_assert_nonnull (root);

    Context *c = cast (Context *, doc->getObjectById ("c"));
    g_assert_nonnull (c);

    Media *m = cast (Media *, doc->getObjectById ("m"));
    g_assert_nonnull (m);

    Media *m2 = cast (Media *, doc->getObjectById ("m2"));
    g_assert_nonnull (m2);

    auto links = doc->getRoot ()->getLinks ();
    g_assert (links->size () == 1);

    auto link = links->begin ();
    g_assert (link->first.size () == 3);
    g_assert (link->second.size () == 1);

    auto cond = link->first.begin ();
    g_assert (cond->event == m->getSelectionEvent ("RED"));
    g_assert (cond->transition == Event::STOP);
    g_assert (cond->predicate->toString () == "and('0'!='0', '1'>'1')");
    g_assert (cond->value == "RED");

    cond++;
    g_assert (cond->event == c->getAttributionEvent ("p"));
    g_assert (cond->transition == Event::STOP);
    g_assert (cond->predicate->toString ()
              == "and(and('0'!='0', '1'>'1'), not($__root__.q==$c.p))");
    g_assert (cond->value == "");

    cond++;
    g_assert (cond->event == m2->getPresentationEvent ("a2"));
    g_assert (cond->transition == Event::PAUSE);
    g_assert (cond->predicate->toString () == "\
and(and('0'!='0', '1'>'1'), not($__root__.q==$c.p), $m.r<'33')");
    g_assert (cond->value == "");
    g_assert (++cond == link->first.end ());

    TRACE ("\n%s", root->toString ().c_str ());
    delete doc;
  }

  // Success: Single switch with default components.
  {
    Document *doc;
    PASS (&doc, "Misc checks", "\
<ncl>\n\
 <head/>\n\
 <body>\n\
  <switch id='s'>\n\
   <media id='m'/>\n\
   <media id='n'/>\n\
   <defaultComponent component='m'/>\n\
   <defaultComponent component='n'/>\n\
   <defaultComponent component='m'/>\n\
  </switch>\n\
 </body>\n\
</ncl>");
    g_assert_nonnull (doc);

    Switch *s = cast (Switch *, doc->getObjectById ("s"));
    g_assert_nonnull (s);

    Media *m = cast (Media *, doc->getObjectById ("m"));
    g_assert_nonnull (m);

    Media *n = cast (Media *, doc->getObjectById ("n"));
    g_assert_nonnull (n);

    auto rules = s->getRules ();
    auto it = rules->begin ();

    g_assert (it->first == m);
    g_assert (it->second->getType () == Predicate::VERUM);

    it++;
    g_assert (it->first == n);
    g_assert (it->second->getType () == Predicate::VERUM);

    it++;
    g_assert (it->first == m);
    g_assert (it->second->getType () == Predicate::VERUM);
    g_assert (++it == rules->end ());

    TRACE ("\n%s", s->toString ().c_str ());
    delete doc;
  }

  // Success: Single switch with atomic rules and default components.
  {
    Document *doc;
    PASS (&doc, "Misc checks", "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <rule id='r1' var='x' comparator='eq' value='1'/>\n\
   <rule id='r2' var='y' comparator='lte' value='0'/>\n\
  </ruleBase>\n\
 </head>\n\
 <body>\n\
  <switch id='s'>\n\
   <media id='m1'/>\n\
   <media id='m2'/>\n\
   <media id='m3'/>\n\
   <defaultComponent component='m1'/>\n\
   <bindRule constituent='m2' rule='r1'/>\n\
   <bindRule constituent='m3' rule='r2'/>\n\
   <defaultComponent component='m3'/>\n\
  </switch>\n\
 </body>\n\
</ncl>");
    g_assert_nonnull (doc);

    Switch *s = cast (Switch *, doc->getObjectById ("s"));
    g_assert_nonnull (s);

    Media *m1 = cast (Media *, doc->getObjectById ("m1"));
    g_assert_nonnull (m1);

    Media *m2 = cast (Media *, doc->getObjectById ("m2"));
    g_assert_nonnull (m2);

    Media *m3 = cast (Media *, doc->getObjectById ("m3"));
    g_assert_nonnull (m3);

    auto rules = s->getRules ();
    auto it = rules->begin ();

    g_assert (it->first == m2);
    g_assert (it->second->toString () == "$__settings__.x=='1'");

    it++;
    g_assert (it->first == m3);
    g_assert (it->second->toString () == "$__settings__.y<='0'");

    it++;
    g_assert (it->first == m1);
    g_assert (it->second->getType () == Predicate::VERUM);

    it++;
    g_assert (it->first == m3);
    g_assert (it->second->getType () == Predicate::VERUM);
    g_assert (++it == rules->end ());

    TRACE ("\n%s", s->toString ().c_str ());
    delete doc;
  }

  // Success: Single switch with complex rules.
  {
    Document *doc;
    PASS (&doc, "Misc checks", "\
<ncl>\n\
 <head>\n\
  <ruleBase>\n\
   <compositeRule id='r0' operator='not'/>\n\
   <rule id='r1' var='x' comparator='eq' value='1'/>\n\
   <compositeRule id='r2' operator='and'>\n\
    <rule id='r3' var='y' comparator='lte' value='0'/>\n\
    <compositeRule id='r4' operator='or'>\n\
     <compositeRule id='r5' operator='not'>\n\
      <rule id='r6' var='z' comparator='gte' value='2'/>\n\
     </compositeRule>\n\
     <rule id='r7' var='w' comparator='lt' value='3'/>\n\
    </compositeRule>\n\
   </compositeRule>\n\
  </ruleBase>\n\
 </head>\n\
 <body>\n\
  <switch id='s'>\n\
   <media id='m1'/>\n\
   <media id='m2'/>\n\
   <media id='m3'/>\n\
   <defaultComponent component='m1'/>\n\
   <bindRule constituent='m2' rule='r0'/>\n\
   <bindRule constituent='m3' rule='r2'/>\n\
   <bindRule constituent='m3' rule='r5'/>\n\
  </switch>\n\
 </body>\n\
</ncl>");
    g_assert_nonnull (doc);

    Switch *s = cast (Switch *, doc->getObjectById ("s"));
    g_assert_nonnull (s);

    Media *m1 = cast (Media *, doc->getObjectById ("m1"));
    g_assert_nonnull (m1);

    Media *m2 = cast (Media *, doc->getObjectById ("m2"));
    g_assert_nonnull (m2);

    Media *m3 = cast (Media *, doc->getObjectById ("m3"));
    g_assert_nonnull (m3);

    auto rules = s->getRules ();
    auto it = rules->begin ();

    g_assert (it->first == m2);
    g_assert (it->second->toString () == "false");

    it++;
    g_assert (it->first == m3);
    g_assert (it->second->toString ()
              == "and($__settings__.y<='0', or(not($__settings__.z>='2'), "
                 "$__settings__.w<'3'))");

    it++;
    g_assert (it->first == m3);
    g_assert (it->second->toString () == "not($__settings__.z>='2')");

    it++;
    g_assert (it->first == m1);
    g_assert (it->second->getType () == Predicate::VERUM);

    TRACE ("\n%s", s->toString ().c_str ());
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
</ncl>");
    g_assert_nonnull (doc);

    MediaSettings *settings = doc->getSettings ();
    g_assert_nonnull (settings);

    Context *root = doc->getRoot ();
    g_assert_nonnull (root);
    g_assert (root->getId () == "__root__");
    g_assert (root->getPorts ()->size () == 3);
    g_assert (root->getChildren ()->size () == 3);
    g_assert (root->getLinks ()->size () == 0);

    g_assert (doc->getMedias ()->size () == 3);

    Media *m = cast (Media *, doc->getObjectById ("m"));
    g_assert_nonnull (m);
    g_assert (m->getId () == "m");
    g_assert_nonnull (m->getPresentationEvent ("@lambda"));
    g_assert_nonnull (m->getAttributionEvent ("background"));

    TRACE ("\n%s", m->toString ().c_str ());
    TRACE ("\n%s", root->toString ().c_str ());
    delete doc;
  }

  exit (EXIT_SUCCESS);
}
