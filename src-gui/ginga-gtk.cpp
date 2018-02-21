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

#include "ginga_gtk.h"
#include "aux-glib.h"
#include <locale.h>
#include <glib/gi18n.h>

#include "ginga.h"
using namespace ::std;

#ifdef G_OS_WIN32
#include <windows.h>
#endif

// Global formatter.
Ginga *GINGA = nullptr;
gchar *gingaID = nullptr;

#define OPTION_LINE "FILE..."
#define OPTION_DESC                                                        \
  "Report bugs to: " PACKAGE_BUGREPORT "\n"                                \
  "Ginga home page: " PACKAGE_URL

void
init_ginga_data ()
{
  if (gingaID == nullptr)
    gingaID = g_strdup_printf ("%li", g_get_real_time ());
}

int
main (int argc, char **argv)
{
  int saved_argc;
  char **saved_argv;

  GingaOptions opts;
  GtkWidget *app;
  GOptionContext *ctx;
  gboolean status;
  GError *error = NULL;

  saved_argc = argc;
  saved_argv = g_strdupv (argv);

#ifdef G_OS_WIN32
  HWND var = GetConsoleWindow ();
  ShowWindow (var, SW_HIDE);
#endif

  opts.width = presentationAttributes.resolutionWidth;
  opts.height = presentationAttributes.resolutionHeight;
  opts.debug = false;
  opts.opengl = false;
  opts.experimental = true;

  gtk_init (&saved_argc, &saved_argv);

  // Parse command-line options.
  ctx = g_option_context_new (OPTION_LINE);
  g_assert_nonnull (ctx);
  status = g_option_context_parse (ctx, &saved_argc, &saved_argv, &error);
  g_option_context_free (ctx);

  if (!status)
    {
      g_assert_nonnull (error);
      // usage_error ("%s", error->message);
      g_error_free (error);
      _exit (0);
    }

  init_ginga_data ();
  load_settings ();

  GINGA = Ginga::create (&opts);
  g_assert_nonnull (GINGA);

  setlocale (LC_ALL, "C");

  gtk_window_set_default_icon_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons", "common",
                    "ginga_icon.png", NULL),
      &error);

  // send log message to server
  send_http_log_message (0, (gchar *) "Open Ginga");
  // check for ginga updates
  send_http_log_message (-1, (gchar *) "Check for Ginga updates");
  g_assert (g_setenv ("G_MESSAGES_DEBUG", "all", true));
  create_main_window ();

  if (saved_argc > 1)
    {
      gchar *filename = saved_argv[1];
      gchar *ext = strrchr (filename, '.');
      if (!g_strcmp0 (ext, ".ncl"))
        {
          insert_historicbox (filename);
        }
    }

  gtk_main ();

  exit (EXIT_SUCCESS);
}
