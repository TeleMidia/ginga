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

#include <config.h>
#include <string.h>

#include <cairo.h>
#include <glib.h>
#include <stdlib.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include "ginga.h"

#define deconst(t, x) ((t)(ptrdiff_t)(const void *)(x))
#define gpointerof(p) ((gpointer)((ptrdiff_t)(p)))


// Global formatter.
static Ginga *GINGA = nullptr;


// Options.

#define OPTION_LINE "FILE"
#define OPTION_DESC                             \
  "Report bugs to: " PACKAGE_BUGREPORT "\n"     \
  "Ginga home page: " PACKAGE_URL

static gboolean opt_fullscreen = FALSE; // true if --fullscreen was given
static gboolean opt_scale = FALSE;      // true if --scale was given
static gint opt_width = 800;            // initial window width
static gint opt_height = 600;           // initial window height
static gdouble opt_fps = 60;            // initial target frame-rate

static gboolean
opt_size (const gchar *opt, const gchar *arg,
          gpointer data, GError **err)
{
  gint64 width;
  gint64 height;
  gchar *end;

  (void) opt;
  (void) arg;
  (void) data;

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
   gpointerof (opt_version), "Print version information and exit", NULL},
  {NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL}
};


// Error handling.

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


// Callbacks.

static gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
  (void) widget;
  (void) data;

  cairo_set_source_rgb (cr, 1., 0., 1.);
  cairo_rectangle (cr, 0, 0, opt_width, opt_height);
  cairo_fill (cr);
  GINGA->redraw (cr);
  return TRUE;
}

static gboolean
resize_callback (GtkWidget *widget, GdkEventConfigure *e, gpointer data)
{
  (void) widget;
  (void) data;

  opt_width = e->width;
  opt_height = e->height;

  GINGA->resize (opt_width, opt_height);

  // We must return FALSE here, otherwise the new geometry is not propagated
  // to draw_callback().
  return FALSE;
}

static gboolean
keyboard_callback (GtkWidget *widget, GdkEventKey *e, gpointer type)
{
  const char *key;
  gboolean free_key = FALSE;

  switch (e->keyval)
    {
    case GDK_KEY_Escape:        /* quit */
      gtk_widget_destroy (widget);
      return TRUE;
    case GDK_KEY_F11:          /* toggle full-screen */
      if (g_strcmp0 ((const char *) type, "release") == 0)
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
          free_key = TRUE;
        }
      break;
    }

  GINGA->send_key (std::string (key),
                   g_strcmp0 ((const char *) type, "press") == 0);
  if (free_key)
    g_free (deconst (char *, key));

  return TRUE;
}

#if GTK_CHECK_VERSION(3,8,0)
static gboolean
tick_callback (GtkWidget *widget, GdkFrameClock *frame_clock,
               G_GNUC_UNUSED gpointer data)
#else
static gboolean
tick_callback (GtkWidget *widget)
#endif
{
  guint64 time;
  static guint64 frame = (guint64) -1;
  static guint64 last;
  static guint64 first;

#if GTK_CHECK_VERSION(3,8,0)
  time = (guint64)(gdk_frame_clock_get_frame_time (frame_clock) * 1000);
  frame = (guint64) gdk_frame_clock_get_frame_counter (frame_clock);
#else
  time = ginga_gettime ();
  frame++;
#endif

  if (frame == 0)
    {
      first = time;
      last = time;
    }
  GINGA->send_tick (time - first, time - last, frame);
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
  std::string file;

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
      exit (EXIT_FAILURE);
    }

  if (saved_argc < 2)
    {
      usage_error ("Missing file operand");
      exit (EXIT_FAILURE);
    }

  file = std::string (saved_argv[1]);
  g_strfreev (saved_argv);

  // Create Ginga handle width the original args.
  GINGA = new Ginga (argc, argv, opt_width, opt_height, opt_fullscreen);

  // Create application window.
  app = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (app);
  gtk_window_set_title (GTK_WINDOW (app), PACKAGE_STRING);
  gtk_window_set_default_size (GTK_WINDOW (app), opt_width, opt_height);
  gtk_widget_set_app_paintable (app, TRUE);
  if (opt_fullscreen)
    gtk_window_fullscreen (GTK_WINDOW (app));

  // Start Ginga.
  GINGA->start (file);

  // Setup GTK+ callbacks.
  g_signal_connect (app, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect (app, "draw", G_CALLBACK (draw_callback), NULL);
  g_signal_connect (app, "configure-event",
                    G_CALLBACK (resize_callback), NULL);
  g_signal_connect (app, "key-press-event",
                    G_CALLBACK (keyboard_callback),
                    deconst (void *, "press"));
  g_signal_connect (app, "key-release-event",
                    G_CALLBACK (keyboard_callback),
                    deconst (void *, "release"));
#if GTK_CHECK_VERSION(3,8,0)
  gtk_widget_add_tick_callback (app, (GtkTickCallback) tick_callback,
                                NULL, NULL);
#else
  g_timeout_add (1000 / opt_fps, (GSourceFunc) tick_callback, app);
#endif

  // Show window and enter event loop.
  gtk_widget_show_all (app);
  gtk_main ();

  // Cleanup.
  // delete GINGA;      FIXME!!!

  exit (EXIT_SUCCESS);
}
