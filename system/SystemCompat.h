/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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

#include "config.h"

extern "C" {
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#ifdef _WIN32
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
	#pragma comment(lib,"ws2_32.lib")

	#ifdef BUILD_DLL
		#define COMP_API __declspec(dllexport)
	#else
		#define COMP_API __declspec(dllimport)
	#endif //BUILD_DLL

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

	#define COMP_API
#endif
}

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <iostream>
#include <string>
using namespace std;

#include <stdint.h>

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#endif //INT64_C

#ifndef UINT64_C
#define UINT64_C(c) (c ## ULL)
#endif //UINT64_C

#ifndef __INT64_C
#define __INT64_C int64_t
#endif //__INT64_C

#ifndef PRIx64
#define PRIx64 "llx"
#endif

#if defined(_WIN32) && !defined(__MINGW32__)

/* clock_gettime() deps begin */
static const int CLOCK_REALTIME           = 0;
static const int CLOCK_MONOTONIC          = 1;
static const int CLOCK_PROCESS_CPUTIME_ID = 2;
static const int CLOCK_THREAD_CPUTIME_ID  = 3;

#define DELTA_EPOCH_IN_SEC      INT64_C(11644473600)
#define DELTA_EPOCH_IN_USEC     INT64_C(11644473600000000)
#define DELTA_EPOCH_IN_100NS    INT64_C(116444736000000000)
#define DELTA_EPOCH_IN_NS       INT64_C(11644473600000000000)

#define POW10_2     INT64_C(100)
#define POW10_3     INT64_C(1000)
#define POW10_4     INT64_C(10000)
#define POW10_6     INT64_C(1000000)
#define POW10_7     INT64_C(10000000)
#define POW10_9     INT64_C(1000000000)
/* clock_gettime() deps end */

struct timezone
{
 int  tz_minuteswest; /* minutes W of Greenwich */
 int  tz_dsttime;     /* type of dst correction */
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

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace compat {
	class SystemCompat {
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
			static void* cmInstance;

			static void checkValues();
			static void initializeGingaPrefix();
			static void initializeGingaPath();
			static void initializeUserCurrentPath();
			static void initializeGingaConfigFile();

			static void sigpipeHandler(int x) throw(const char*);

		public:
			/********
			 * URIs *
			 ********/
			static string getGingaPrefix();
            static string updatePath(string dir);
            static string updatePath(string dir, string separator);
            static bool isXmlStr(string location);
			static bool checkUriPrefix(string uri);
			static bool isAbsolutePath(string path);

			static string getIUriD();
			static string getFUriD();

			static string getPath(string filename);
			static string convertRelativePath(string relPath);

			static string getGingaBinPath();
			static string getUserCurrentPath();

			/*
			 * defines a new base directory to ginga context files
			 * use it carefully
			 */
			static void setGingaContextPrefix(string newBaseDir);

			/*
			 * get base directory to ginga context files
			 */
			static string getGingaContextPrefix();

			/*
			 * updates the URL with specific system delimiter
			 * and append with ginga config files prefix
			 */
			static string appendGingaFilesPrefix(string relUrl);

			/*
			 * updates the URL with specific system delimiter
			 * and append with ginga config install prefix
			 */
			static string appendGingaInstallPrefix(string relUrl);


			/******************************
			 * Handling Dynamic libraries *
			 ******************************/
			static void* getComponentManagerInstance();
			static void setComponentManagerInstance(void* cmInstance);

			static string appendLibExt(string libName);

			static void* loadComponent(
					string libName, void** llib, string symName);

			static bool releaseComponent(void* component);

			/****************
			 * SIG Handlers *
			 ****************/
			static void initializeSigpipeHandler();


			/*****************
			 * Embedded Info *
			 *****************/
			static string getOperatingSystem();
			static float getClockSpeed();
			static float getMemorySize();


			/**********************
			 * Specific Functions *
			 **********************/
			static void strError (int err, char *buf, size_t size);
			static int changeDir (const char *path);
			static void makeDir(const char* dirName, unsigned int mode);
			static void uSleep(unsigned int microseconds);
			static string getTemporaryDir();

			static void gingaProcessExit(short status);

			static const short LOG_NULL = 0;
			static const short LOG_STDO = 1;
			static const short LOG_FILE = 2;
			static void setLogTo(short logType, string sufix="");

			/******************
			 * Time functions *
			 ******************/
			static int gettimeofday(struct timeval *tv, struct timezone *tz);
			static int getUserClock(struct timeval* usrClk);
			static int clockGetTime(int clockType, struct timespec* tv);


			/******************
			 * Math functions *
			 ******************/
			static int rint (double x);


			/******************
			 * Pipe Functions *
			 ******************/
			static string checkPipeName(string pipeName);

		private:
			static void checkPipeDescriptor(PipeDescriptor pd);

		public:
			static bool createPipe(string pipeName, PipeDescriptor* pd);
			static bool openPipe(string pipeName, PipeDescriptor* pd);
			static void closePipe(PipeDescriptor pd);
			static int readPipe(PipeDescriptor pd, char* buffer, int buffSize);
			static int writePipe(PipeDescriptor pd, char* data, int dataSize);

			/*****************
			 * Zip Functions *
			 *****************/
			static int zip_directory(const string &zipfile_path, const string &directory_path, const string &iUriD); // REFACTORING
			static int unzip_file(const char *zipname, const char *filedir);

			/**********************
			 * MemCheck Functions *
			 **********************/
			static bool initMemCheck();
			static void memCheckPoint();
			static bool finishMemCheck();
	};
}
}
}
}
}
}
}

#endif /*SystemCompat_H_*/
