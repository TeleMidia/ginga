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

#include <cairo.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "ginga.h"
#include "Document.h"
#include "Media.h"
#include "MediaSettings.h"
using namespace ::std;

GINGA_BEGIN_DECLS
#include "aux-glib.h"
#include "aux-lua.h"
GINGA_END_DECLS

PRAGMA_DIAG_IGNORE (-Wunused-function)
PRAGMA_DIAG_IGNORE (-Wunused-macros)

// Options.

// True means enable debugging mode.           -d, --debug
static gboolean debugging_on = FALSE;

// True means enable fullscreen mode.          -f, --fullscreen, <F11>
static gboolean fullscreen_on = FALSE;

// Name or RGB of the canvas background color. -b, --background
static gchar *background_color = NULL;

// Main window dimensions.                     -s, --size
static gint initial_width  = 800; // pixels
static gint initial_height = 600; // pixels

static gboolean
opt_parse_size (unused (const gchar *opt),
                const gchar *arg,
                unused (gpointer data),
                GError **err)
{
  gint64 width;
  gint64 height;
  gchar *end;

  g_return_val_if_fail (arg != NULL, FALSE);

  width = g_ascii_strtoll (arg, &end, 10);
  if (width == 0)
    goto syntax_error;
  initial_width = (gint) (CLAMP (width, 0, G_MAXINT));

  if (*end != 'x')
    goto syntax_error;

  height = g_ascii_strtoll (++end, NULL, 10);
  if (height == 0)
    goto syntax_error;
  initial_height = (gint) (CLAMP (height, 0, G_MAXINT));

  return TRUE;

syntax_error:
  g_set_error (err, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
               "Invalid size string '%s'", arg);
  return FALSE;
}

static void
version (void)
{
  puts (PACKAGE_STRING);
  _exit (0);
}

static GOptionEntry options[] =
{
 {"background", 'b', 0, G_OPTION_ARG_STRING, &background_color,
  "Set background color", "COLOR"},
 {"debug", 'd', 0, G_OPTION_ARG_NONE, &debugging_on,
  "Enable debugging", NULL},
 {"fullscreen", 'f', 0, G_OPTION_ARG_NONE, &fullscreen_on,
  "Enable fullscreen mode", NULL},
 {"size", 's', 0, G_OPTION_ARG_CALLBACK, pointerof (opt_parse_size),
  "Set initial window size", "WIDTHxHEIGHT"},
 {"version", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
  pointerof (version), "Print version information and exit", NULL},
 {NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL}
};

// Error handling.

#define usage_error(fmt, ...)\
  _error (TRUE, 0, fmt, ##__VA_ARGS__)

#define usage_die(fmt, ...)\
  _error (TRUE, EXIT_FAILURE, fmt, ##__VA_ARGS__)

#define error(fmt, ...)\
  _error (FALSE, 0, fmt, ##__VA_ARGS__)

#define die(fmt, ...)\
  _error (FALSE, 1, fmt, ##__VA_ARGS__)

static G_GNUC_PRINTF (3, 4) void
_error (gboolean try_help, gint die, const gchar *format, ...)
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

// Debugging.

static void
debugging_toggle (void)
{
  static string saved_G_MESSAGES_DEBUG = "";

  if (debugging_on)
    {
      const char *curr = g_getenv ("G_MESSAGES_DEBUG");
      if (curr != nullptr)
        saved_G_MESSAGES_DEBUG = string (curr);
      g_assert (g_setenv ("G_MESSAGES_DEBUG", "all", true));
    }
  else
    {
      g_assert (g_setenv ("G_MESSAGES_DEBUG",
                          saved_G_MESSAGES_DEBUG.c_str (), true));
    }
}

// GTK+ Widgets.

// Application window.
static GtkWidget *app_win = NULL;

// Vertical box container.
static GtkWidget *app_vbox = NULL;

// Header bar: shows document path, time, and controls.
static GtkWidget *app_header_bar = NULL;

// Info bar: shows error messages.
static GtkWidget *app_info_bar = NULL;
static GtkWidget *app_info_bar_label = NULL;

// Drawing area: renders Ginga's output.
static GtkWidget *app_canvas = NULL;

// Text entry: captures Lua code for on-the-fly evaluation.
static GtkWidget *app_cmd_buf = NULL;

// The application CSS provider.
static GtkCssProvider *app_css = NULL;

// The CSS code to load in the provider.
#define CSS                                     \
  .subtitle {                                   \
    font-family: monospace;                     \
  }                                             \
  #cmdbuf {                                     \
    font-family: monospace;                     \
  }

// GTK+ callbacks.

static gboolean
on_canvas_draw (unused (GtkWidget *canvas),
                cairo_t *cr,
                Ginga *ginga)
{
  gint64 ms;
  gchar *str;

  g_return_val_if_fail (cr != NULL, FALSE);
  g_return_val_if_fail (ginga != NULL, FALSE);

  Document *doc = (Document *) ginga->getDocument ();
  g_assert_nonnull (doc);
  doc->draw (cr);

  ms = doc->getTime () / 1000;
  str = g_strdup_printf
    ("%u:%02u:%02u.%03u",
     (guint) ((ms / (1000 * 60 * 60))),
     (guint) ((ms / (1000 * 60)) % 60),
     (guint) ((ms / 1000) % 60),
     (guint) ((ms % 1000)));

  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (app_header_bar), str);
  g_free (str);

  return FALSE;                 // propagate
}

static gboolean
on_canvas_key_event (unused (GtkWidget *canvas),
                     GdkEventKey *evt,
                     Ginga *ginga)
{
  const gchar *key;
  gboolean free_key = false;

  g_return_val_if_fail (evt != NULL, FALSE);
  g_return_val_if_fail (ginga != NULL, FALSE);

  switch (evt->keyval)
    {
    case GDK_KEY_Escape:        // <ESC> clears error messages
      {
        if (evt->type == GDK_KEY_RELEASE)
          goto done;

        gtk_info_bar_set_revealed (GTK_INFO_BAR (app_info_bar), FALSE);
        gtk_widget_grab_focus (GTK_WIDGET (app_canvas));
        goto done;
      }
    case GDK_KEY_F10:           // <F10> toggles debugging mode
      {
        if (evt->type == GDK_KEY_RELEASE)
          goto done;

        debugging_on = !debugging_on;
        debugging_toggle ();
        goto done;
      }
    case GDK_KEY_F11:           // <F11> toggles full-screen
      {
        if (evt->type == GDK_KEY_RELEASE)
          goto done;

        fullscreen_on = !fullscreen_on;
        if (fullscreen_on)
          gtk_window_fullscreen (GTK_WINDOW (app_win));
        else
          gtk_window_unfullscreen (GTK_WINDOW (app_win));
        goto done;
      }
    case GDK_KEY_colon:         // <:> focuses command-buffer
      {
        gtk_widget_grab_focus (GTK_WIDGET (app_cmd_buf));
        goto done;
      }
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
      key = gdk_keyval_name (evt->keyval);
      if (strlen (key) > 1)
        {
          key = g_utf8_strup (key, -1);
          free_key = true;
        }
      break;
    }

  // if (!ginga->sendKey (string (key), evt->type == GDK_KEY_PRESS))
  //   {
  //     g_assert (ginga->getState () == GINGA_STATE_STOPPED);
  //     gtk_main_quit ();         // all done
  //   }

  if (free_key)
    g_free (deconst (gchar *, key));

 done:
  return FALSE;                 // propagate
}

static void
on_canvas_size_allocate (unused (GtkWidget *canvas),
                         GdkRectangle *rect,
                         Ginga *ginga)
{
  g_return_if_fail (rect != NULL);
  g_return_if_fail (ginga != NULL);

  ginga->resize (rect->width, rect->height);
}

static void
on_cmd_buf_activate (GtkWidget *cmd_buf,
                     Ginga *ginga)
{
  Document *doc;
  lua_State *L;
  const gchar *str;
  const gchar *errmsg;

  g_return_if_fail (cmd_buf != NULL);
  g_return_if_fail (ginga != NULL);

  str = gtk_entry_get_text (GTK_ENTRY (cmd_buf));
  g_assert_nonnull (str);

  doc = (Document *) ginga->getDocument ();
  if (unlikely (doc == NULL))
    {
      errmsg = "Document is NULL";
      goto fail;
    }

  L = doc->getLuaState ();
  if (unlikely (L == NULL))
    {
      errmsg = "Lua state is null";
      goto fail;
    }

  if (unlikely (luaL_dostring (L, str) != LUA_OK))
    {
      errmsg = lua_tostring (L, -1);
      lua_pop (L, 1);
      goto fail;
    }

  gtk_widget_grab_focus (GTK_WIDGET (app_canvas));
  return;

 fail:
  gtk_label_set_text (GTK_LABEL (app_info_bar_label), errmsg);
  gtk_info_bar_set_message_type (GTK_INFO_BAR (app_info_bar),
                                 GTK_MESSAGE_ERROR);
  gtk_info_bar_set_revealed (GTK_INFO_BAR (app_info_bar), TRUE);
}

static gboolean
on_cmd_buf_key_press_event (unused (GtkWidget *cmd_buf),
                            GdkEventKey *evt)
{
  g_return_val_if_fail (evt != NULL, FALSE);

  g_assert (evt->type == GDK_KEY_PRESS);

  switch (evt->keyval)
    {
    case GDK_KEY_Escape:
      gtk_info_bar_set_revealed (GTK_INFO_BAR (app_info_bar), FALSE);
      gtk_widget_grab_focus (GTK_WIDGET (app_canvas));
      break;
    default:
      break;
    }

  return FALSE;                 // propagate
}

static void
on_info_bar_response (unused (GtkWidget *info_bar),
                      gint response)
{
  switch (response)
    {
    case GTK_RESPONSE_CLOSE:
      gtk_info_bar_set_revealed (GTK_INFO_BAR (app_info_bar), FALSE);
      return;
    default:
      return;
    }
}

static gboolean
on_win_tick (GtkWidget *window,
             GdkFrameClock *frame_clock,
             Ginga *ginga)
{
  guint64 time;
  static gint64 frame = -1;
  static gint64 last;

  g_return_val_if_fail (window != NULL, G_SOURCE_CONTINUE);
  g_return_val_if_fail (frame_clock != NULL, G_SOURCE_CONTINUE);
  g_return_val_if_fail (ginga != NULL, G_SOURCE_CONTINUE);

  time = gdk_frame_clock_get_frame_time (frame_clock);
  frame = gdk_frame_clock_get_frame_counter (frame_clock);

  if (frame == 0)
    last = time;

  Document *doc = (Document *) ginga->getDocument ();
  if (doc != NULL)
    doc->advanceTime (time - last);
    // {
    //   g_assert (ginga->getState () == GINGA_STATE_STOPPED);
    //   gtk_main_quit ();         // all done
    //   return G_SOURCE_REMOVE;
    // }

  last = time;
  gtk_widget_queue_draw (window);

  return G_SOURCE_CONTINUE;     // keep callback installed
}

static void
app_init (Ginga *ginga)
{
  GError *error = NULL;
  gulong id;

  g_assert_nonnull (ginga);

  // Window.
  app_win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (app_win), PACKAGE_STRING);
  if (fullscreen_on)
    gtk_window_fullscreen (GTK_WINDOW (app_win));

  id = g_signal_connect (app_win, "delete-event",
                         G_CALLBACK (gtk_main_quit), NULL);
  g_assert (id > 0);
  gtk_widget_add_tick_callback (GTK_WIDGET (app_win),
                                (GtkTickCallback) on_win_tick, ginga, NULL);

  app_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (app_win), app_vbox);

  // Header bar.
  app_header_bar = gtk_header_bar_new ();
  gtk_box_pack_start (GTK_BOX (app_vbox), app_header_bar, FALSE, TRUE, 0);
  gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (app_header_bar),
                                        TRUE);
  gtk_header_bar_set_title (GTK_HEADER_BAR (app_header_bar), "");
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (app_header_bar), "");

  // Info bar.
  app_info_bar = gtk_info_bar_new ();
  gtk_box_pack_start (GTK_BOX (app_vbox), app_info_bar, FALSE, TRUE, 0);
  gtk_info_bar_set_revealed (GTK_INFO_BAR (app_info_bar), FALSE);
  gtk_info_bar_set_message_type (GTK_INFO_BAR (app_info_bar),
                                 GTK_MESSAGE_ERROR);
  gtk_info_bar_set_show_close_button (GTK_INFO_BAR (app_info_bar), TRUE);

  g_signal_connect (app_info_bar, "response",
                    G_CALLBACK (on_info_bar_response), NULL);

  app_info_bar_label = gtk_label_new ("");
  gtk_container_add
    (GTK_CONTAINER
     (gtk_info_bar_get_content_area (GTK_INFO_BAR (app_info_bar))),
      app_info_bar_label);
  gtk_label_set_xalign (GTK_LABEL (app_info_bar_label), 0);

  // Canvas.
  app_canvas = gtk_drawing_area_new ();
  gtk_box_pack_start (GTK_BOX (app_vbox), app_canvas, TRUE, TRUE, 0);
  gtk_widget_set_size_request (app_canvas, initial_width, initial_height);
  gtk_widget_set_can_focus (app_canvas, TRUE);

  g_signal_connect (app_canvas, "draw",
                    G_CALLBACK (on_canvas_draw), ginga);
  g_signal_connect (app_canvas, "key-press-event",
                    G_CALLBACK (on_canvas_key_event), ginga);
  g_signal_connect (app_canvas, "key-release-event",
                    G_CALLBACK (on_canvas_key_event), ginga);
  g_signal_connect (app_canvas, "size-allocate",
                    G_CALLBACK (on_canvas_size_allocate), ginga);

  // Command buffer.
  app_cmd_buf = gtk_entry_new ();
  gtk_box_pack_end (GTK_BOX (app_vbox), app_cmd_buf, FALSE, TRUE, 0);

  gtk_widget_set_name (app_cmd_buf, "cmdbuf");
  gtk_entry_set_placeholder_text (GTK_ENTRY (app_cmd_buf),
                                  "Type Lua code here");

  g_signal_connect (app_cmd_buf, "activate",
                    G_CALLBACK (on_cmd_buf_activate), ginga);
  g_signal_connect (app_cmd_buf, "key-press-event",
                    G_CALLBACK (on_cmd_buf_key_press_event), ginga);

  // CSS.
  app_css = gtk_css_provider_new ();
  gtk_css_provider_load_from_data (app_css, G_STRINGIFY (CSS), -1, &error);
  if (unlikely (error != NULL))
    die ("%s", error->message);
  g_assert_null (error);

  gtk_style_context_add_provider_for_screen
    (gtk_widget_get_screen (app_win), GTK_STYLE_PROVIDER (app_css),
     GTK_STYLE_PROVIDER_PRIORITY_USER);
}

// Main.

gint
main (gint argc, gchar **argv)
{

  GOptionContext *ctx;
  gboolean status;
  GError *error = NULL;
  Ginga *ginga;

  gtk_init (&argc, &argv);

  // Parse command-line options.
  ctx = g_option_context_new ("[FILE]");
  g_assert_nonnull (ctx);
  g_option_context_set_description (ctx, "\
Report bugs to:  " PACKAGE_BUGREPORT "\n\
Ginga home page: " PACKAGE_URL "\n");
  g_option_context_add_main_entries (ctx, options, NULL);
  status = g_option_context_parse (ctx, &argc, &argv, &error);
  g_option_context_free (ctx);

  if (!status)
    {
      g_assert_nonnull (error);
      usage_error ("%s", error->message);
      g_error_free (error);
      _exit (0);
    }

  if (argc < 2)
    {
      usage_error ("Missing file operand");
      _exit (0);
    }

  debugging_toggle ();

  // Create Ginga handle.
  ginga = Ginga::create ();
  g_assert_nonnull (ginga);

  // Create application window.
  app_init (ginga);

  // Run each NCL file, one after another.
  string errmsg;
  if (unlikely (!ginga->start (string (argv[1]),
                               initial_width, initial_height, &errmsg)))
    {
      die ("%s", errmsg.c_str ());
    }
  gtk_header_bar_set_title (GTK_HEADER_BAR (app_header_bar), argv[1]);
  gtk_widget_show_all (app_win);
  gtk_main ();

  // Done.
  Document *doc = (Document *) ginga->getDocument ();
  if (doc != NULL)
    {
      lua_close (doc->getLuaState ()); // GC deletes doc
    }

  delete ginga;
  g_print ("DONE\n");
  _exit (0);
}
