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

#ifndef TESTS_H
#define TESTS_H

#include "aux-ginga.h"
#include "ginga.h"

#include "Context.h"
#include "Document.h"
#include "Event.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Parser.h"
#include "ParserLua.h"
#include "Switch.h"

const char *media_samples[] = { "",
                                ABS_TOP_SRCDIR "/tests-ncl/samples/arcade.mp3",
                                ABS_TOP_SRCDIR "/tests-ncl/samples/gnu.png",
                                ABS_TOP_SRCDIR "/tests-ncl/samples/fps.lua",
                                ABS_TOP_SRCDIR "/tests-ncl/samples/vector.svg",
                                ABS_TOP_SRCDIR "/tests-ncl/samples/text.txt",
                                ABS_TOP_SRCDIR "/tests-ncl/samples/clock.ogv",
                                ABS_TOP_SRCDIR "/tests-ncl/samples/page.html" };

static G_GNUC_UNUSED string
tests_write_tmp_file (const string &buf, const string &file_ext = "ncl")
{
  string path;
  gchar *filename;
  gint fd;
  GError *error = nullptr;

  string file_name = string ("ginga-tests-XXXXXX.") + file_ext;

  // g_file_open_tmp should follow the rules for mkdtemp() templates
  fd = g_file_open_tmp (file_name.c_str (), &filename, &error);
  if (unlikely (error != nullptr))
    {
      ERROR ("*** Unexpected error: %s", error->message);
      g_error_free (error);
    }
  g_close (fd, nullptr);

  path = filename;
  g_assert (g_file_set_contents (path.c_str (), buf.c_str (), -1, nullptr));

  g_free (filename);
  return path;
}

static G_GNUC_UNUSED void
tests_parse_and_start (Formatter **fmt, Document **doc, const string &buf,
                       const string &file_ext = "ncl")
{
  string errmsg;
  string file;

  tryset (fmt, new Formatter (nullptr));
  g_assert_nonnull (*fmt);

  file = tests_write_tmp_file (buf, file_ext);
  if (!(*fmt)->start (file, &errmsg))
    {
      g_printerr ("*** Unexpected error: %s", errmsg.c_str ());
      g_assert_not_reached ();
    }

  tryset (doc, (*fmt)->getDocument ());
  g_assert_nonnull (*doc);
  g_assert (g_remove (file.c_str ()) == 0);
}

static G_GNUC_UNUSED void
tests_create_document (Document **doc, Context **root,
                       MediaSettings **settings)
{
  tryset (doc, new Document ());
  g_assert_nonnull (doc);

  tryset (root, (*doc)->getRoot ());
  g_assert_nonnull (root);
  g_assert ((*doc)->getObjectById ("__root__") == *root);

  tryset (settings, (*doc)->getSettings ());
  g_assert_nonnull (settings);
  g_assert ((*doc)->getObjectById ("__settings__") == *settings);
}

#endif // TESTS_H
