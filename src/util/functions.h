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

#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <sstream>
#include <limits>
using namespace std;

extern "C" {
	#include <ctype.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#ifdef _WIN32
#define _WINSOCKAPI_    // stops windows.h including winsock.h
	#include <windows.h>
	#include <io.h>
	#include <time.h>
	#include <direct.h>
#endif
#ifndef _WIN32
	#include <sys/param.h>
	#include <unistd.h>
	#include <sys/time.h>
#endif
	#include <errno.h>
	#include <sys/timeb.h>
	#include <pthread.h>
}
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#ifndef PATH_MAX
# define PATH_MAX 512
#endif

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#endif

// Macros for namespace declaration.

#define NAMESPACE_BEGIN(t) \
  namespace t {

#define NAMESPACE_BEGIN2(t1, t2) \
  NAMESPACE_BEGIN (t1) NAMESPACE_BEGIN (t2)

#define NAMESPACE_BEGIN3(t1, t2, t3) \
  NAMESPACE_BEGIN2 (t1, t2) NAMESPACE_BEGIN (t3)

#define NAMESPACE_BEGIN4(t1, t2, t3, t4) \
  NAMESPACE_BEGIN3 (t1, t2, t3) NAMESPACE_BEGIN(t4)

#define NAMESPACE_BEGIN5(t1, t2, t3, t4, t5) \
  NAMESPACE_BEGIN4 (t1, t2, t3, t4) NAMESPACE_BEGIN (t5)

#define NAMESPACE_END  }
#define NAMESPACE_END2 }}
#define NAMESPACE_END3 }}}
#define NAMESPACE_END4 }}}}
#define NAMESPACE_END5 }}}}}

#define NAMESPACE_TELEMIDIA_BEGIN \
  NAMESPACE_BEGIN3 (br, pucrio, telemidia)
#define NAMESPACE_TELEMIDIA_END \
  NAMESPACE_END3

#define NAMESPACE_GINGA_BEGIN \
  NAMESPACE_TELEMIDIA_BEGIN NAMESPACE_BEGIN (ginga)
#define NAMESPACE_GINGA_END \
  NAMESPACE_TELEMIDIA_END NAMESPACE_END

#define NAMESPACE_GINGA_CORE_BEGIN \
  NAMESPACE_GINGA_BEGIN NAMESPACE_BEGIN (core)
#define NAMESPACE_GINGA_CORE_END \
  NAMESPACE_GINGA_END NAMESPACE_END

#define NAMESPACE_GINGA_CORE_PLAYER_BEGIN \
  NAMESPACE_GINGA_CORE_BEGIN NAMESPACE_BEGIN (player)
#define NAMESPACE_GINGA_CORE_PLAYER_END \
  NAMESPACE_GINGA_CORE_END NAMESPACE_END

#define TELEMIDIA_UTIL_BEGIN_DECLS \
  NAMESPACE_TELEMIDIA_BEGIN NAMESPACE_BEGIN (util)
#define TELEMIDIA_UTIL_END_DECLS \
  NAMESPACE_TELEMIDIA_END NAMESPACE_END

TELEMIDIA_UTIL_BEGIN_DECLS

string ultostr(unsigned long int value);
double strUTCToSec(string utcValue);
string cvtPercentual(string value, bool* isPercentual);
void initTimeStamp();
void printTimeStamp();
string intToStrHexa(int value);
int strHexaToInt(string value);
string upperCase(string s);
string lowerCase(string s);
bool isNumeric(void* value);
string itos(double i);
float stof(string s);
float itof(int i);
vector<string>* split(string str, string token, string pos_delimiter = "");
string trim(string str);
float getPercentualValue(string value);
bool isPercentualValue(string value);
bool fileExists(string filename);
bool isDirectory(const char* path);
double NaN();
double infinity();
bool isNaN(double value);
bool isInfinity(double value);
double getCurrentTimeMillis();

double getNextStepValue(
			double currentStepValue,
			double value,
			int factor,
			double time, double initTime, double dur, int stepSize);

bool readPPMFile(char *fn, int &X, int &Y, unsigned char* &result);
bool readBMPFile(char *fn, int &X, int &Y, unsigned char* &result);

bool ppmToJpeg(char *ppmfile, char *jpegfile, int quality);
bool bmpToJpeg(char *bmpfile, char *jpegfile, int quality);

void str_replace_all(string &str, const string &find_what, const string &replace_with);

TELEMIDIA_UTIL_END_DECLS

#endif //_FUNCTIONS_H_
