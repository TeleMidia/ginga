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

#ifndef SystemCompat_H_
#define SystemCompat_H_

#include "ginga.h"

extern "C" {
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#ifdef _MSC_VER
#ifndef isnan
#define isnan(x) ((x) != (x))
#endif

#ifndef snprintf
#define snprintf sprintf_s
#endif

#include <sys/timeb.h>
#include <sys/types.h>
#include <time.h>
#include <direct.h>
#include <io.h>
#include <windows.h>
#include <winbase.h>
#include <winsock2.h>
#ifdef WINSTRUCTS
#include <Ws2tcpip.h>
#endif
#pragma comment(lib, "ws2_32.lib")

#else
#if HAVE_SYS_SYSINFO_H
#include <sys/sysinfo.h>
#endif

#if (defined HAVE_SYS_SOCKET_H || defined HAVE_IF_H)
#ifdef STDC_HEADERS
#include <stdlib.h>
#include <stddef.h>
#else
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#endif
#endif

#include <sys/param.h>
#include <unistd.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/utsname.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_IF_H
#include <net/if.h>
#endif
#endif
}

#include "util/functions.h"
using namespace ::ginga::util;

#ifndef INT64_C
#define INT64_C(c) (c##LL)
#endif // INT64_C

#ifndef UINT64_C
#define UINT64_C(c) (c##ULL)
#endif // UINT64_C

#ifndef __INT64_C
#define __INT64_C int64_t
#endif //__INT64_C

#ifndef PRIx64
#define PRIx64 "llx"
#endif

#if defined(_MSC_VER) && !defined(__MINGW32__)
struct timezone
{
  int tz_minuteswest; /* minutes W of Greenwich */
  int tz_dsttime;     /* type of dst correction */
};

typedef HANDLE PipeDescriptor;

#else
typedef int PipeDescriptor;
#endif

// If access modes (F_OK, X_OK, W_OK or R_OK) is not defined we must define
// then. This will usually occur on Windows.
#ifndef F_OK
#define F_OK 0
#endif

#ifndef X_OK
#define X_OK 1
#endif

#ifndef W_OK
#define W_OK 2
#endif

#ifndef R_OK
#define R_OK 4
#endif

GINGA_SYSTEM_BEGIN

class SystemCompat
{
private:
  static string filesPref;
  static string ctxFilesPref;
  static string installPref;
  static string userCurrentPath;
  static string gingaCurrentPath;
  static string pathD;
  static string iUriD;
  static string fUriD;
  static string gingaPrefix;
  static bool initialized;
  static void *cmInstance;

  static void checkValues ();
  static void initializeGingaPrefix ();
  static void initializeGingaPath ();
  static void initializeUserCurrentPath ();
  static void initializeGingaConfigFile ();

public:
  /********
   * URIs *
   ********/
  static string getGingaPrefix ();
  static string updatePath (string dir);
  static string updatePath (string dir, string separator);
  static bool isXmlStr (string location);
  static bool checkUriPrefix (string uri);
  static bool isAbsolutePath (string path);

  static string getIUriD ();
  static string getFUriD ();

  static string getPath (string filename);
  static string convertRelativePath (string relPath);

  static string getGingaBinPath ();
  static string getUserCurrentPath ();

  /*
   * defines a new base directory to ginga context files
   * use it carefully
   */
  static void setGingaContextPrefix (string newBaseDir);

  /*
   * get base directory to ginga context files
   */
  static string getGingaContextPrefix ();

  /*
   * updates the URL with specific system delimiter
   * and append with ginga config files prefix
   */
  static string appendGingaFilesPrefix (string relUrl);

  /*
   * updates the URL with specific system delimiter
   * and append with ginga config install prefix
   */
  static string appendGingaInstallPrefix (string relUrl);

  /**********************
   * Specific Functions *
   **********************/
  static const short LOG_NULL = 0;
  static const short LOG_STDO = 1;
  static const short LOG_FILE = 2;
  static void setLogTo (short logType, string sufix = "");

  /******************
   * Time functions *
   ******************/
  static int getUserClock (struct timeval *usrClk);

  /******************
   * Pipe Functions *
   ******************/
  static string checkPipeName (string pipeName);

private:
  static void checkPipeDescriptor (PipeDescriptor pd);

public:
  static bool createPipe (string pipeName, PipeDescriptor *pd);
  static bool openPipe (string pipeName, PipeDescriptor *pd);
  static void closePipe (PipeDescriptor pd);
  static int readPipe (PipeDescriptor pd, char *buffer, int buffSize);
  static int writePipe (PipeDescriptor pd, char *data, int dataSize);

  /*****************
   * Zip Functions *
   *****************/
  static int zip_directory (const string &zipfile_path,
                            const string &directory_path,
                            const string &iUriD); // REFACTORING
  static int unzip_file (const char *zipname, const char *filedir);
};

GINGA_SYSTEM_END

#endif /*SystemCompat_H_*/
