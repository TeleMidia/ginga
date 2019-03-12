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

#ifdef __APPLE__
#include <AppKit/AppKit.h>
#endif

// Global formatter
Ginga *GINGA = nullptr;
gchar *gingaID = nullptr;

#define OPTION_LINE "FILE..."
#define OPTION_DESC                                                        \
  "Report bugs to: " PACKAGE_BUGREPORT "\n"                                \
  "Ginga home page: " PACKAGE_URL

static gboolean opt_bigpicture = FALSE; // toggle bigpicture mode

static GOptionEntry options[]
    = { { "bigpicture", 'b', 0, G_OPTION_ARG_NONE, &opt_bigpicture,
          "Enable bigpicture mode", NULL },
        { NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL } };

void
init_ginga_data ()
{
  if (gingaID == nullptr)
    gingaID = g_strdup_printf ("%li", g_get_real_time ());
}

#if defined(G_OS_WIN32)

static HMODULE libbase_dll = NULL;

BOOL WINAPI /* Avoid silly "no previous prototype" gcc warning */
DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

BOOL WINAPI
DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
      libbase_dll = hinstDLL;
      break;
    }

  return TRUE;
}
#endif

const gchar *
get_binary_dir (void)
{
  static const gchar *instalationdir = NULL;

  if (instalationdir)
    return instalationdir;

#ifdef __APPLE__
  NSAutoreleasePool *pool;
  NSString *resource_path;
  gchar *basename;
  gchar *dirname;

  pool = [[NSAutoreleasePool alloc] init];

  resource_path = [[NSBundle mainBundle] resourcePath];

  basename = g_path_get_basename ([resource_path UTF8String]);
  dirname = g_path_get_dirname ([resource_path UTF8String]);

  instalationdir = g_build_filename (dirname, basename, NULL);

  g_free (basename);
  g_free (dirname);

  [pool drain];

#elif defined G_OS_WIN32
  instalationdir
      = g_win32_get_package_installation_directory_of_module (libbase_dll);
  if (!instalationdir)
    g_error (
        "g_win32_get_package_installation_directory_of_module() failed");
#else
  instalationdir = (char *) GINGABINDIR;
#endif
  return instalationdir;
}

const gchar *
get_data_dir (void)
{
  static gchar *datadir = NULL;

  if (datadir)
    return datadir;

#if defined __APPLE__
  datadir = g_path_get_dirname (get_binary_dir ());

#elif defined G_OS_WIN32
  datadir = g_build_filename (get_binary_dir (), "share", "ginga", NULL);
#else
  datadir = (char *) GINGADATADIR;
#endif

  return datadir;
}

void
env_init ()
{
  static gboolean env_initialized = FALSE;

  if (env_initialized)
    g_error ("env_init() must only be called once!");
  env_initialized = TRUE;

#ifdef __APPLE__

  if(!strcmp(g_path_get_basename (get_binary_dir ()), ".libs"))
    return;

  const gchar *ldpath = g_getenv ("GST_PLUGIN_PATH");
  gchar *libdir = g_build_filename ("/usr", "local", "Cellar/", NULL);
  printf("GST: %s \n", libdir);
  if (ldpath && *ldpath)
    {
      gchar *tmp = g_strconcat (libdir, ":", ldpath, NULL);
      g_setenv ("GST_PLUGIN_PATH", tmp, TRUE);
      g_free (tmp);
    }
  else
    {
      g_setenv ("GST_PLUGIN_PATH", libdir, TRUE);
    }
  g_free (libdir);
  
#endif
}

int
main (int argc, char **argv)
{
  int saved_argc;
  char **saved_argv;

  GingaOptions opts;
  GOptionContext *ctx;
  GError *error = NULL;

  saved_argc = argc;
  saved_argv = g_strdupv (argv);

  // #ifdef G_OS_WIN32
  //   HWND var = GetConsoleWindow ();
  //   ShowWindow (var, SW_HIDE);
  // #endif

  opts.width = presentationAttributes.resolutionWidth;
  opts.height = presentationAttributes.resolutionHeight;
  opts.debug = false;

  GINGA = Ginga::create (&opts);
  g_assert_nonnull (GINGA);

  env_init ();
  gtk_init (&saved_argc, &saved_argv);

  // Parse command-line options.
  ctx = g_option_context_new (OPTION_LINE);
  g_assert_nonnull (ctx);
  g_option_context_add_main_entries (ctx, options, NULL);
  if (!g_option_context_parse (ctx, &saved_argc, &saved_argv, &error))
    {
      g_print ("option parsing failed: %s\n", error->message);
      exit (1);
    }
  g_option_context_free (ctx);

  init_ginga_data ();

  load_settings ();

  setlocale (LC_ALL, "C");

  gtk_window_set_default_icon_from_file (
      g_build_path (G_DIR_SEPARATOR_S, get_data_dir (), "icons", "common",
                    "ginga_icon.png", NULL),
      &error);

  printf ("PATH: %s",
          g_build_path (G_DIR_SEPARATOR_S, get_data_dir (), "icons",
                        "common", "ginga_icon.png", NULL));

  // send log message to server
  send_http_log_message (0, (gchar *) "Open Ginga");
  // check for ginga updates
  send_http_log_message (-1, (gchar *) "Check for Ginga updates");
  g_assert (g_setenv ("G_MESSAGES_DEBUG", "all", true));

  create_main_window ();

  if (opt_bigpicture)
    {
      create_bigpicture_window ();
    }
  else
    {
      if (saved_argc > 1)
        {
          gchar *filename = saved_argv[1];
          gchar *ext = strrchr (filename, '.');
          if (!g_strcmp0 (ext, ".ncl"))
            insert_historicbox (filename);
        }
    }

  gtk_main ();

  exit (EXIT_SUCCESS);
}
