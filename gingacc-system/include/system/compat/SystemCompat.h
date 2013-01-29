/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#ifndef SystemCompat_H_
#define SystemCompat_H_

extern "C" {
// For Linux, Mac OS Snow Leopard (10.6) and Win32
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#ifdef _WIN32
	#include <sys/timeb.h>
	#include <sys/types.h>
	#include <time.h>
	#include <direct.h>
	#include <io.h>
	#include <windows.h>
	#include <winsock2.h>
  #ifdef WINSTRUCTS
	#include <Ws2tcpip.h>
  #endif
	#pragma comment(lib,"ws2_32.lib")
#else // For Linux and Mac OS Snow Leopard (10.6)
	#include <stdio.h>

  #if (defined __APPLE__ || defined HAVE_MACH_SL_H ) // For Mac OS Snow Leopard (10.6)
	#include <mach/mach.h>
	#include <mach/mach_host.h>
	#include <mach/mach_types.h>
	#include <mach/vm_statistics.h>
	#include <sys/sysctl.h>
	#include <sys/types.h>
  #elif (HAVE_SYS_SYSINFO_H) // For Linux only
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
using namespace ::br::pucrio::telemidia::util;

#include <string>
using namespace std;


#if defined(_WIN32) && !defined(__MINGW32__)
struct timezone
{
 int  tz_minuteswest; /* minutes W of Greenwich */
 int  tz_dsttime;     /* type of dst correction */
};
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
			static bool isXmlStr(string location);
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


			/**********************
			 * Time functions *
			 **********************/
			static int gettimeofday(struct timeval *tv, struct timezone *tz);
			static int getUserClock(struct timeval* usrClk);

			/**********************
			 * Math functions *
			 **********************/
			static int rint (double x);
	};
}
}
}
}
}
}
}

#endif /*SystemCompat_H_*/
