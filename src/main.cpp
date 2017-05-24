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

#include "ginga.h"

#if defined WITH_CEF && WITH_CEF
GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wunused-parameter)
GINGA_PRAGMA_DIAG_IGNORE (-Wunused-const-variable)
#include "cef_app.h"
GINGA_PRAGMA_DIAG_POP ()
#endif

#include "formatter/FormatterMediator.h"
using namespace ::ginga::formatter;

#include "mb/Display.h"
using namespace ::ginga::mb;

/* Options: */
#define OPTION_LINE "FILE"
#define OPTION_DESC                             \
  "Report bugs to: " PACKAGE_BUGREPORT "\n"     \
  "Ginga home page: " PACKAGE_URL

static gboolean opt_fullscreen = FALSE; /* true if --fullscreen was given */
static gboolean opt_scale = FALSE;      /* true if --scale was given */
static gint opt_width = 800;            /* initial window width */
static gint opt_height = 600;           /* initial window height */
static gdouble opt_fps = 60;            /* initial FPS rate */

static gboolean
opt_size (arg_unused (const gchar *opt), const gchar *arg,
          arg_unused (gpointer data), GError **err)
{
  gint64 width;
  gint64 height;
  gchar *end;

  width = g_ascii_strtoll (arg, &end, 10);
  if (width == 0)
    goto syntax_error;
  opt_width = (gint) (CLAMP (width, 0, G_MAXINT));

  if (*end != 'x')
    goto syntax_error;

  height = g_ascii_strtoll (++end, NULL, 10);
  if (height == 0)
    goto syntax_error;
  opt_height = (gint) (CLAMP (height, 0, G_MAXINT));

  return TRUE;

 syntax_error:
  g_set_error (err, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
               "Invalid size string '%s'", arg);
  return FALSE;
}

static gboolean
opt_version (void)
{
  puts (PACKAGE_STRING);
  exit (EXIT_SUCCESS);
}

#define gpointerof(p) ((gpointer)((ptrdiff_t)(p)))
static GOptionEntry options[] = {
  {"size", 's', 0, G_OPTION_ARG_CALLBACK,
   gpointerof (opt_size), "Set initial window size", "WIDTHxHEIGHT"},
  {"fullscreen", 'S', 0, G_OPTION_ARG_NONE,
   &opt_fullscreen, "Enable full-screen mode", NULL},
  {"scale", 'x', 0, G_OPTION_ARG_NONE,
   &opt_scale, "Scale canvas to fit window", NULL},
   {"fps", 'f', 0, G_OPTION_ARG_DOUBLE,
   &opt_fps, "Set display FPS rate", NULL},
  {"version", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
   pointerof (opt_version), "Print version information and exit", NULL},
  {NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL}
};

/* Error handling: */
#define usage_error(format, ...) _error (TRUE, format, ## __VA_ARGS__)

static G_GNUC_PRINTF (2,3) void
_error (gboolean try_help, const gchar *format, ...)
{
  const gchar *me = g_get_application_name ();
  va_list args;

  va_start (args, format);
  g_fprintf (stderr, "%s: ", me);
  g_vfprintf (stderr, format, args);
  g_fprintf (stderr, "\n");
  va_end (args);

  if (try_help)
    g_fprintf (stderr, "Try '%s --help' for more information.\n", me);
}

int
main (int argc, char **argv)
{
  FormatterMediator *formatter;
  int ginga_argc = argc;
  char **ginga_argv = g_strdupv (argv);
  string file;

#if defined WITH_CEF && WITH_CEF
  CefMainArgs args (argc, argv);
  CefSettings settings;

  int pstatus = CefExecuteProcess (args, nullptr, nullptr);
  if (pstatus >= 0)
    return pstatus;

  if (unlikely (!CefInitialize (args, settings, nullptr, nullptr)))
    exit (EXIT_FAILURE);
#endif

  GOptionContext *ctx;
  gboolean status;
  GError *error = NULL;

  g_set_prgname ("ginga");
  ctx = g_option_context_new (OPTION_LINE);
  g_assert_nonnull (ctx);
  g_option_context_set_description (ctx, OPTION_DESC);
  g_option_context_add_main_entries (ctx, options, NULL);
  status = g_option_context_parse (ctx, &ginga_argc, &ginga_argv, &error);
  g_option_context_free (ctx);

  if (unlikely (!status))
    {
      g_assert (error != NULL);
      usage_error ("%s", error->message);
      g_error_free (error);
      exit (EXIT_FAILURE);
    }

  if (unlikely (argc != 2))
    {
      usage_error ("Missing file operand");
      exit (EXIT_FAILURE);
    }

  file = string (ginga_argv[1]);
  g_strfreev (ginga_argv);

  _Ginga_Display = new ginga::mb::Display (opt_width, opt_height,
                                           opt_fullscreen, opt_fps);
  formatter = new FormatterMediator ();

  formatter->play (file);
  _Ginga_Display->renderLoop ();

  delete Ginga_Display;
  // FIXME: This causes the program to crash!
  // delete formatter;

#if defined WITH_CEF && WITH_CEF
  CefShutdown ();
#endif

  exit (EXIT_SUCCESS);
}
