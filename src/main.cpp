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

#include "lssm/CommonCoreManager.h"
#include "lssm/PresentationEngineManager.h"
using namespace ::ginga::lssm;

#include "ncl/DeviceLayout.h"
using namespace ::ginga::ncl;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "util/functions.h"
using namespace ::ginga::util;

#if WITH_MULTIDEVICE
#include "formatter/FormatterActiveDevice.h"
#include "formatter/FormatterMultiDevice.h"
#include "formatter/FormatterMultiDevice.h"
#include "multidev/DeviceDomain.h"
using namespace ::ginga::multidev;
#endif

#define usage_error(fmt, ...) _error (TRUE, fmt, ## __VA_ARGS__)
#define print_error(fmt, ...) _error (FALSE, fmt, ## __VA_ARGS__)

static void G_GNUC_PRINTF (2,3)
_error (gboolean try_help, const gchar *format, ...)
{
  va_list args;
  const gchar *me = g_get_application_name ();

  g_fprintf (stderr, "%s: ", me);
  va_start (args, format);
  g_vfprintf (stderr, format, args);
  va_end (args);
  g_fprintf (stderr, "\n");
  if (try_help)
    g_fprintf (stderr, "Try '%s --help' for more information.\n", me);
}

static void
printHelp ()
{
  cout << endl << "Usage: ginga [OPTIONS]... [<--ncl> NCLFILE]" << endl;
  cout << "Example: ginga --ncl test.ncl" << endl;
  cout << endl << "OPTIONS are:" << endl;
  cout << "-h, --help                    Display this information." << endl;
  cout << "    --set-interfaceId         Set the interface that the ";
  cout << "document presentation" << endl;
  cout << "                                shall be started." << endl;
  cout << "-v, --version                 Display version." << endl;
  cout << "    --enable-log [mode]       Enable log mode:" << endl;
  cout << "                                Turn on verbosity, gather all";
  cout << " output" << endl;
  cout << "                                messages and write them into a";
  cout << " device" << endl;
  cout << "                                according to mode." << endl;
  cout << "                                  [mode] can be:" << endl;
  cout << "                                     'file'   write messages "
          "into";
  cout << " a file" << endl;
  cout
      << "                                              named logFile.txt.";
  cout << endl;
  cout << "                                     'stdout' write messages "
          "into";
  cout << " /dev/stdout" << endl;
  cout << "                                     'null'   write messages "
          "into";
  cout << " /dev/null" << endl;
  cout << "                                              (default mode).";
  cout << endl;
  cout << "    --x-offset <value>        Offset of left Ginga display "
          "coord.";
  cout << endl;
  cout
      << "    --y-offset <value>        Offset of top Ginga display coord.";
  cout << endl;
  cout
      << "    --set-width <value>       Force a width Ginga display value.";
  cout << endl;
  cout << "    --set-height <value>      Force a height Ginga display "
          "value.";
  cout << endl;
  cout << "    --device-class <value>    Define the NCL device class for "
          "Ginga";
  cout << endl;
  cout << "    --disable-multicast       Uses broadcast instead of "
          "multicast "
          "for communication with devices";
  cout << endl;
  cout << "    --device-srv-port <port>  Define the service port for Ginga "
          "as "
          "a secondary device";
  cout << endl;
  cout << "    --enable-automount        Enable trigger for main.ncl";
  cout << " applications received" << endl;
  cout << "                                via a transport protocol."
       << endl;
  cout << "    --enable-remove-oc-filter Enable processor economies after";
  cout << " first time that" << endl;
  cout << "                                the object carousel is mounted.";
  cout << endl;
  cout << "    --disable-interactivity   Disable NCL presentations" << endl;
  cout << "    --disable-oc              Disable OC filters" << endl;
  cout << "    --disable-mainav          Disable main AV decoders" << endl;
  cout
      << "    --enable-nptprinter       Ginga becomes nothing, but an NPT ";
  cout << "printer (debug purpose only)";
  cout << endl;
  cout << "    --set-tuner [ni:channel]  Force an interface and a channel "
          "to "
          "be tuned."
       << endl;
  cout << "                              For instance:" << endl;
  cout << "                                 --set-tuner file:/tmp/test.ts"
       << endl;
  cout << "                                 --set-tuner sbtvdt:635143"
       << endl;
  cout << "                                 --set-tuner ip:224.0.0.1:1234"
       << endl;
  cout << "                                             (...)" << endl;
  cout << endl;
  cout << "    --vmode <width>x<height>  Specifies the Video Window size.";
  cout << endl;
  cout << "    --wid <id>                Specifies an existent Window to "
          "be ";
  cout << "used as Ginga device Window.";
  cout << endl;
  cout << "    --poll-stdin              Poll for events using stdin.";
  cout << endl;
  cout << "    --disable-demuxer         Disables Ginga demuxer.";
  cout << endl;
  cout << endl << endl << endl;
}

int
main (int argc, char *argv[])
{
  PresentationEngineManager *pem = NULL;
  string file = "", param = "", bgUri = "", cmdFile = "", tSpec = "";
  string interfaceId = "";

  for (int i = 1; i < argc; i++)
    {
      if ((strcmp (argv[i], "-h") == 0)
          || (strcmp (argv[i], "--help") == 0))
        {
          printHelp ();
          return 0;
        }
      else if ((strcmp (argv[i], "-v") == 0)
               || (strcmp (argv[i], "--version")) == 0)
        {
          printf ("%s\n", PACKAGE_VERSION);
          return 0;
        }
      else if ((strcmp (argv[i], "--ncl") == 0) && ((i + 1) < argc))
        {
          file.assign (argv[i + 1], strlen (argv[i + 1]));
          clog << "argv = '" << argv[i + 1] << "' file = '";
          clog << file << "'" << endl;
        }
      else if ((strcmp (argv[i], "--enable-log") == 0) && ((i + 1) < argc))
        {
        }
      else if ((strcmp (argv[i], "--force-quit") == 0) && ((i + 1) < argc))
        {
        }
      else if ((strcmp (argv[i], "--x-offset") == 0) && ((i + 1) < argc))
        {
        }
      else if ((strcmp (argv[i], "--y-offset") == 0) && ((i + 1) < argc))
        {
        }
      else if ((strcmp (argv[i], "--set-width") == 0) && ((i + 1) < argc))
        {
        }
      else if ((strcmp (argv[i], "--set-height") == 0) && ((i + 1) < argc))
        {
        }
      else if ((strcmp (argv[i], "--set-max-transp") == 0)
               && ((i + 1) < argc))
        {
        }
      else if ((strcmp (argv[i], "--set-bg-image") == 0)
               && ((i + 1) < argc))
        {
          param = argv[i + 1];
          if (fileExists (param))
            {
              bgUri = param;
              clog << "main bgUri = '" << bgUri << "'";
              clog << " OK" << endl;
            }
          else
            {
              clog << "main Warning: can't set bgUri '" << param << "'";
              clog << " file does not exist" << endl;
            }
        }
      else if (strcmp (argv[i], "--disable-multicast") == 0)
        {
        }
      else if (strcmp (argv[i], "--device-srv-port") == 0)
        {
        }
      else if (strcmp (argv[i], "--enable-automount") == 0)
        {
        }
      else if (strcmp (argv[i], "--enable-remove-oc-filter") == 0)
        {
        }
      else if ((strcmp (argv[i], "--enable-cmdfile") == 0
                || strcmp (argv[i], "--set-cmdfile") == 0)
               && ((i + 1) < argc))
        {
        }
      else if (strcmp (argv[i], "--disable-unload") == 0)
        {
        }
      else if (strcmp (argv[i], "--disable-interactivity") == 0)
        {
        }
      else if (strcmp (argv[i], "--disable-oc") == 0)
        {
        }
      else if (strcmp (argv[i], "--enable-nptprinter") == 0)
        {
        }
      else if ((strcmp (argv[i], "--set-tuner") == 0) && ((i + 1) < argc))
        {
        }
      else if (strcmp (argv[i], "--set-interfaceId") == 0)
        {
        }
      else if (strcmp (argv[i], "--set-exitonend") == 0)
        {
        }
      else if (strcmp (argv[i], "--disable-fkeys") == 0)
        {
        }
    }


  // TODO: Add support to multi-devices (old --devClass option).
  // TODO: Add support to remote NCL files.
  // TODO: Add ISDBT tuning support.

  int xoffset, yoffset, width, height;

  g_set_prgname ("ginga");
  if (file == "")
    {
      usage_error ("missing file operand");
      exit (EXIT_FAILURE);
    }

  _Ginga_Display = new ginga::mb::Display (800, 600, false);
  g_assert_nonnull (_Ginga_Display);

  xoffset = 0;
  yoffset = 0;
  width = 800;
  height = 600;
  pem = new PresentationEngineManager (0, xoffset, yoffset, width, height,
                                       true, false);
  g_assert_nonnull (pem);
  pem->setEmbedApp (false);
  pem->setExitOnEnd (false);
  pem->setDisableFKeys (false);
  pem->setInteractivityInfo (true);
  pem->setIsLocalNcl (false, NULL);
  if (!pem->openNclFile (file))
    {
      print_error ("cannot open NCL file: %s", file.c_str ());
      //
      // FIXME: The next instruction causes the process to freeze:
      //     delete pem;
      //
      exit (EXIT_FAILURE);
    }

  pem->startPresentation (file, "");
  pem->waitUnlockCondition ();
  //
  // FIXME: The next instruction causes the process to freeze:
  //     delete pem;
  //
  exit (EXIT_SUCCESS);
}
