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

#include "lssm/PresentationEngineManager.h"
using namespace ::ginga::lssm;

/* Options: */
#define OPTION_LINE "FILE"
#define OPTION_DESC                             \
  "Report bugs to: " PACKAGE_BUGREPORT "\n"     \
  "Ginga home page: " PACKAGE_URL

static gboolean opt_fullscreen = FALSE; /* true if --fullscreen was given */
static gboolean opt_scale = FALSE;      /* true if --scale was given */
static gint opt_width = 800;            /* initial window width */
static gint opt_height = 600;           /* initial window height */

static gboolean
opt_size (arg_unused (const gchar *opt), const gchar *arg,
          arg_unused (gpointer data), GError **err)
{
  gint64 width;
  gint64 height;
  gchar *end;

  width = g_ascii_strtoull (arg, &end, 10);
  if (width == 0)
    goto syntax_error;
  opt_width = (gint) (CLAMP (width, 0, G_MAXINT));

  if (*end != 'x')
    goto syntax_error;

  height = g_ascii_strtoull (++end, NULL, 10);
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
  {"version", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
   pointerof (opt_version), "Print version information and exit", NULL},
  {NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL}
};

/* Error handling: */
#define usage_error(format, ...) _error (TRUE, format, ## __VA_ARGS__)
#define print_error(format, ...) _error (FALSE, format, ## __VA_ARGS__)

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
  GOptionContext *ctx;
  gboolean status;
  GError *error = NULL;

  string file;
  PresentationEngineManager *pem;

  g_set_prgname ("ginga");
  ctx = g_option_context_new (OPTION_LINE);
  g_assert_nonnull (ctx);
  g_option_context_set_description (ctx, OPTION_DESC);
  g_option_context_add_main_entries (ctx, options, NULL);
  status = g_option_context_parse (ctx, &argc, &argv, &error);
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

  _Ginga_Display = new ginga::mb::Display (opt_width, opt_height, false);
  g_assert_nonnull (_Ginga_Display);

  pem = new PresentationEngineManager (0, 0, 0, opt_width, opt_height,
                                       true, false);
  g_assert_nonnull (pem);
  pem->setEmbedApp (false);
  pem->setExitOnEnd (false);
  pem->setDisableFKeys (false);
  pem->setInteractivityInfo (true);
  pem->setIsLocalNcl (true, NULL);

  file = string (argv[1]);
  if (unlikely (!pem->openNclFile (file)))
    {
      print_error ("Cannot open NCL file: %s", file.c_str ());
      exit (EXIT_FAILURE);
    }

  pem->startPresentation (file, "");

  // Blocks main thread until PEM quits.
  // (FIXME: The main thread should do something useful.)
  g_mutex_lock (&pem->quit_mutex);
  while (!pem->quit)
    g_cond_wait (&pem->quit_cond, &pem->quit_mutex);
  g_mutex_unlock (&pem->quit_mutex);

  delete Ginga_Display;

  exit (EXIT_SUCCESS);
}
