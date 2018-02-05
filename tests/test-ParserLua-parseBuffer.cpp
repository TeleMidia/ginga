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

static bool
check_failure (const string &log, const string &expected, const string &buf)
{
  static int i = 1;
  Document *doc;
  string msg = "";

  g_printerr ("XFAIL #%d: %s\n", i++, log.c_str ());
  doc = ParserLua::parseBuffer (buf.c_str (), buf.length (), &msg);
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
  doc = ParserLua::parseBuffer (buf.c_str (), buf.length (), &msg);
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

  // -------------------------------------------------------------------------
  // General errors.
  // -------------------------------------------------------------------------

  XFAIL ("Syntax error", "unexpected symbol near '<'", "<a>");

  XFAIL ("Bad body",
         "", // ignored
         "return 0");

  XFAIL ("Bad body",
         "", // ignored
         "return nil");

  XFAIL ("Bad body",
         "", // ignored
         "return {}");

  XFAIL ("Bad body", "unexpected tag: unknown", "return {'unknown', 5}");

  // Success: Empty body.
  {
    Document *doc;
    PASS (&doc, "Empty body", "\
return {'context', 'hello'}\
");
    g_assert_nonnull (doc);

    MediaSettings *settings = doc->getSettings ();
    g_assert_nonnull (settings);

    Context *root = doc->getRoot ();
    g_assert_nonnull (root);
    g_assert (root->getId () == "__root__");
    g_assert (root->hasAlias ("hello"));
    g_assert (root->getPorts ()->size () == 0);
    g_assert (root->getChildren ()->size () == 1);
    g_assert (root->getLinks ()->size () == 0);

    g_assert (doc->getMedias ()->size () == 1);
    delete doc;
  }

  exit (EXIT_SUCCESS);
}
