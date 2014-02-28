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
