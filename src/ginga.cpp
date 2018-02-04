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

#include <config.h>
#include <stdio.h>
#include <string.h>

#include "aux-glib.h"
#include <cairo.h>
#include <gtk/gtk.h>

#if defined WITH_OPENGL && WITH_OPENGL
// clang-format off
PRAGMA_DIAG_IGNORE (-Wunused-macros)
PRAGMA_DIAG_IGNORE (-Wvariadic-macros)
// clang-format on
#endif

#include "ginga.h"
using namespace ::std;

// Global formatter.
static Ginga *GINGA = nullptr;

// Options.

#define OPTION_LINE "FILE..."
#define OPTION_DESC                                                        \
  "Report bugs to: " PACKAGE_BUGREPORT "\n"                                \
  "Ginga home page: " PACKAGE_URL

static gboolean opt_debug = FALSE;        // toggle debug
static gboolean opt_experimental = FALSE; // toggle experimental stuff
static gboolean opt_fullscreen = FALSE;   // toggle fullscreen-mode
static gboolean opt_opengl = FALSE;       // toggle OpenGL backend
static string opt_background = "";        // background color
static gint opt_width = 800;              // initial window width
static gint opt_height = 600;             // initial window height

static gboolean
opt_background_cb (unused (const gchar *opt), const gchar *arg,
                   unused (gpointer data), unused (GError **err))
{
  g_assert_nonnull (arg);
  opt_background = string (arg);
  return TRUE;
}

static gboolean
opt_size_cb (unused (const gchar *opt), const gchar *arg,
             unused (gpointer data), GError **err)
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

static void
opt_version_cb (void)
{
  puts (PACKAGE_STRING);
  _exit (0);
}

static GOptionEntry options[]
    = { {"background", 'b', 0, G_OPTION_ARG_CALLBACK,
          pointerof (opt_background_cb), "Set background color", "COLOR"},
        {"debug", 'd', 0, G_OPTION_ARG_NONE, &opt_debug,
          "Enable debugging", NULL},
        {"fullscreen", 'f', 0, G_OPTION_ARG_NONE, &opt_fullscreen,
          "Enable full-screen mode", NULL},
        {"opengl", 'g', 0, G_OPTION_ARG_NONE, &opt_opengl,
          "Use OpenGL backend", NULL},
        {"size", 's', 0, G_OPTION_ARG_CALLBACK, pointerof (opt_size_cb),
          "Set initial window size", "WIDTHxHEIGHT"},
        {"experimental", 'x', 0, G_OPTION_ARG_NONE, &opt_experimental,
          "Enable experimental stuff", NULL},
        {"version", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
          pointerof (opt_version_cb), "Print version information and exit",
          NULL},
        {NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL} };

// Error handling.

#define usage_error(fmt, ...) _error (TRUE, 0, fmt, ##__VA_ARGS__)

#define usage_die(fmt, ...) _error (TRUE, EXIT_FAILURE, fmt, ##__VA_ARGS__)

#define error(fmt, ...) _error (FALSE, 0, fmt, ##__VA_ARGS__)

#define die(fmt, ...) _error (FALSE, 1, fmt, ##__VA_ARGS__)

static G_GNUC_PRINTF (3, 4) void _error (gboolean try_help, int die,
                                         const gchar *format, ...)
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
  if (die > 0)
    _exit (die);
}

  // Callbacks.

#if GTK_CHECK_VERSION(3, 16, 0)
static gboolean render_gl_callback (unused (GtkGLArea *area),
                                    unused (GdkGLContext *ctx))
{
  GINGA->redraw (nullptr);
  return TRUE;
}
#endif

static gboolean
draw_callback (unused (GtkWidget *widget), cairo_t *cr,
               unused (gpointer data))
{
  GINGA->redraw (cr);
  return TRUE;
}

static void
exit_callback (void)
{
  _exit (0);
}

static gboolean
resize_callback (unused (GtkWidget *widget), GdkEventConfigure *e,
                 unused (gpointer data))
{
  opt_width = e->width;
  opt_height = e->height;
  GINGA->resize (opt_width, opt_height);

  // We must return FALSE here, otherwise the new geometry is not propagated
  // to the draw_callback().
  return FALSE;
}

static gboolean
keyboard_callback (GtkWidget *widget, GdkEventKey *e, gpointer type)
{
  const char *key;
  bool free_key = false;

  switch (e->keyval)
    {
    case GDK_KEY_Escape: // quit
      if (g_str_equal ((const char *) type, "release"))
        return TRUE;
      gtk_main_quit ();
      return TRUE;
    case GDK_KEY_F10: // toggle debugging
      if (g_str_equal ((const char *) type, "release"))
        return TRUE;
      opt_debug = !opt_debug;
      GINGA->setOptionBool ("debug", opt_debug);
      return TRUE;
    case GDK_KEY_F11: // toggle full-screen
      if (g_str_equal ((const char *) type, "release"))
        return TRUE;
      opt_fullscreen = !opt_fullscreen;
      if (opt_fullscreen)
        gtk_window_fullscreen (GTK_WINDOW (widget));
      else
        gtk_window_unfullscreen (GTK_WINDOW (widget));
      return TRUE;
    case GDK_KEY_asterisk:
      key = "*";
      break;
    case GDK_KEY_numbersign:
      key = "#";
      break;
    case GDK_KEY_Return:
      key = "ENTER";
      break;
    case GDK_KEY_F1:
      key = "RED";
      break;
    case GDK_KEY_F2:
      key = "GREEN";
      break;
    case GDK_KEY_F3:
      key = "YELLOW";
      break;
    case GDK_KEY_F4:
      key = "BLUE";
      break;
    case GDK_KEY_F5:
      key = "INFO";
      break;
    case GDK_KEY_Down:
      key = "CURSOR_DOWN";
      break;
    case GDK_KEY_Left:
      key = "CURSOR_LEFT";
      break;
    case GDK_KEY_Right:
      key = "CURSOR_RIGHT";
      break;
    case GDK_KEY_Up:
      key = "CURSOR_UP";
      break;
    default:
      key = gdk_keyval_name (e->keyval);
      if (strlen (key) > 1)
        {
          key = g_utf8_strup (key, -1);
          free_key = true;
        }
      break;
    }

  bool status = GINGA->sendKey (
      string (key), g_str_equal ((const char *) type, "press") == 0);

  if (free_key)
    g_free (deconst (char *, key));

  if (!status)
    {
      g_assert (GINGA->getState () == GINGA_STATE_STOPPED);
      gtk_main_quit (); // all done
    }

  return status;
}

#if GTK_CHECK_VERSION(3, 8, 0)
static gboolean
tick_callback (GtkWidget *widget, GdkFrameClock *frame_clock,
               unused (gpointer data))
#else
static gboolean
tick_callback (GtkWidget *widget)
#endif
{
  guint64 time;
  static guint64 frame = (guint64) -1;
  static guint64 last;
  static guint64 first;

#if GTK_CHECK_VERSION(3, 8, 0)
  time = (guint64) (gdk_frame_clock_get_frame_time (frame_clock) * 1000);
  frame = (guint64) gdk_frame_clock_get_frame_counter (frame_clock);
#else
  time = (guint64) g_get_monotonic_time ();
  frame++;
#endif

  if (frame == 0)
    {
      first = time;
      last = time;
    }

  if (!GINGA->sendTick (time - first, time - last, frame))
    {
      g_assert (GINGA->getState () == GINGA_STATE_STOPPED);
      gtk_main_quit (); // all done
      return G_SOURCE_REMOVE;
    }

  last = time;
  gtk_widget_queue_draw (widget);
  return G_SOURCE_CONTINUE;
}

// Main.

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
  gtk_init (&saved_argc, &saved_argv);

  // Parse command-line options.
  ctx = g_option_context_new (OPTION_LINE);
  g_assert_nonnull (ctx);
  g_option_context_set_description (ctx, OPTION_DESC);
  g_option_context_add_main_entries (ctx, options, NULL);
  status = g_option_context_parse (ctx, &saved_argc, &saved_argv, &error);
  g_option_context_free (ctx);

  if (!status)
    {
      g_assert_nonnull (error);
      usage_error ("%s", error->message);
      g_error_free (error);
      _exit (0);
    }

  if (saved_argc < 2)
    {
      usage_error ("Missing file operand");
      _exit (0);
    }

  if (opt_opengl)
    {
#if !(defined WITH_OPENGL && WITH_OPENGL)
      die ("Option -g requires OpenGL support");
#endif
#if !(GTK_CHECK_VERSION(3, 16, 0))
      die ("Option -g requires gtk+ >= 3.16");
#endif
    }

  // Create application window.
  app = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (app);
  gtk_window_set_title (GTK_WINDOW (app), PACKAGE_STRING);
  gtk_window_set_default_size (GTK_WINDOW (app), opt_width, opt_height);
  if (opt_fullscreen)
    gtk_window_fullscreen (GTK_WINDOW (app));

  // Setup draw area.
  if (opt_opengl)
    {
#if GTK_CHECK_VERSION(3, 16, 0)
      GtkWidget *area = gtk_gl_area_new ();
      g_assert_nonnull (area);
      gtk_container_add (GTK_CONTAINER (app), area);
      g_signal_connect (area, "render", G_CALLBACK (render_gl_callback),
                        NULL);
#else
      g_assert_not_reached ();
#endif
    }
  else
    {
      gtk_widget_set_app_paintable (app, TRUE);
      g_signal_connect (app, "draw", G_CALLBACK (draw_callback), NULL);
    }

  // Setup GTK+ callbacks.
  g_signal_connect (app, "destroy", G_CALLBACK (exit_callback), NULL);
  g_signal_connect (app, "configure-event", G_CALLBACK (resize_callback),
                    NULL);
  g_signal_connect (app, "key-press-event", G_CALLBACK (keyboard_callback),
                    deconst (void *, "press"));
  g_signal_connect (app, "key-release-event",
                    G_CALLBACK (keyboard_callback),
                    deconst (void *, "release"));
#if GTK_CHECK_VERSION(3, 8, 0)
  gtk_widget_add_tick_callback (app, (GtkTickCallback) tick_callback, NULL,
                                NULL);
#else
  g_timeout_add (1000 / 60, (GSourceFunc) tick_callback, app);
#endif

  // Create Ginga handle.
  opts.width = opt_width;
  opts.height = opt_height;
  opts.debug = opt_debug;
  opts.experimental = opt_experimental;
  opts.opengl = opt_opengl;
  opts.background = string (opt_background);
  GINGA = Ginga::create (&opts);
  g_assert_nonnull (GINGA);

  // Run each NCL file, one after another.
  int fail_count = 0;
  for (int i = 1; i < saved_argc; i++)
    {
      string errmsg;
      if (unlikely (!GINGA->start (string (saved_argv[i]), &errmsg)))
        {
          if (saved_argc > 2)
            error ("%s: %s", saved_argv[i], errmsg.c_str ());
          else
            error ("%s", errmsg.c_str ());
          fail_count++;
          continue;
        }
      gtk_widget_show_all (app);
      gtk_main ();
      GINGA->stop ();
    }

  // Done.
  delete GINGA;
  g_strfreev (saved_argv);

  _exit (fail_count);
}
