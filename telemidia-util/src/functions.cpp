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

#include <config.h>
#include <dirent.h>

#ifndef _WIN32
 #define IS_DIRECTORY(st_mode)  (((st_mode) & S_IFMT) == S_IFDIR)
 #define IS_LINK(st_mode)	(((st_mode) & S_IFMT) == S_IFLNK)
 #define IS_REGULAR(st_mode)	(((st_mode) & S_IFMT) == S_IFREG)
 #ifdef __APPLE__
  #include <sys/dir.h>
    typedef struct direct DIRENT;
 #else
  #include <dirent.h>
    typedef struct dirent DIRENT;
  #include <sys/time.h>
  #include <sys/resource.h>
 #endif
#else  // _WIN32
#define IS_DIRECTORY(st_mode)	(((st_mode) & S_IFMT) == S_IFDIR)
#define IS_LINK(st_mode)	0
#define IS_REGULAR(st_mode)	(((st_mode) & S_IFMT) == S_IFREG)
#define lstat stat
#include <windows.h>
#include <float.h>
#endif

#include <math.h>

#include "util/functions.h"

TELEMIDIA_UTIL_BEGIN_DECLS

	string userCurrentPath;
	static double timeStamp;

	string ultostr(unsigned long int value) {
		string strValue;

		char dst[32];
		char digits[32];
		unsigned long int i = 0, j = 0, n = 0;

		do {
			n = value % 10;
			digits[i++] = (n < 10 ? (char)n+'0' : (char)n-10+'a');
			value /= 10;

			if (i > 31) {
				break;
			}

		} while (value != 0);

		n = i;
		i--;

		while (i >= 0 && j < 32) {
			dst[j] = digits[i];
			i--;
			j++;
		}

		strValue.assign(dst, n);

		return strValue;
	}

	double strUTCToSec(string utcValue) {
		double secs = 0;
		vector<string>* params;

		params = split(utcValue, ":");
		switch (params->size()) {
			case 1:
				if (utcValue.find("s") != std::string::npos) {
					utcValue = utcValue.substr(0, utcValue.length() - 1);
				}

				secs = util::stof(utcValue);
				break;

			case 2:
				secs = 60 * util::stof((*params)[0]) + util::stof((*params)[1]);
				break;

			case 3:
				secs = 3600 * util::stof((*params)[0]) + 60 * util::stof((*params)[1]) +
						util::stof((*params)[2]);
				break;

			default:
				secs = infinity();
		}

		delete params;
		params = NULL;

		return secs;
	}

	string cvtPercentual(string value, bool* isPercentual) {
		string newValue;

		newValue      = value;
		*isPercentual = false;

		if (value.find("%") != std::string::npos) {
			*isPercentual = true;
			newValue      = value.substr(0, value.length() - 1);
		}

		return newValue;
	}

	void initTimeStamp() {
		timeStamp = getCurrentTimeMillis();
	}

	void printTimeStamp() {
		double printTS = getCurrentTimeMillis() - timeStamp;
		clog << "TimeStamp: " << (printTS / 1000) << endl;
	}

	int strHexaToInt(string value) {
		std::stringstream str1(value);
		int number;

		if (value == "") {
			number = -1;

		} else if (value.find("0x") != string::npos) {
			if (!(str1 >> std::hex >> number)) {
				number = -1;
			}

		} else {
			if (!(str1 >> number)) {
				number = -1;
			}
		}
		return number;
	}

	string intToStrHexa(int value) {
		stringstream str;
		str << std::hex << std::uppercase << value;
		return "0x" + str.str();
	}

	string lowerCase(string s) {
		for (unsigned int j=0; j<s.length(); ++j) {
			s[j]=tolower(s[j]);
		}
		return s;
	}

	string upperCase(string s) {
		if (s == "") {
			return "";
		}

		for (unsigned int j=0; j<s.length(); ++j) {
			s[j]=toupper(s[j]);
		}
		return s;
	}

	bool isNumeric(void* value) {
		bool r = true;
		char* str = (char*)value;

		for (unsigned int i = 0; i < strlen(str); i++) {
			if (i == 0)
				if ((str[i] < '0' || str[i] > '9') &&
					    (str[i] != '.') &&
					    (str[i] != '+' || str[i] != '-'))
					r = false;

			if ((str[i] < '0' || str[i] > '9') && (str[i] != '.'))
				r = false;
		}

		return r;
	}

	string itos(double i) {
		ostringstream os;
		os << i;
		return os.str();
	}

	float stof(string s) {
		float f = 0.0f;
		stringstream ss;
		ss << s;
		ss >> f;
		return f;
	}

	float itof(int i) {
		string s = itos(i);
		return util::stof(s);
	}

	string getUserCurrentPath() {
		return userCurrentPath;
	}

	vector<string>* localSplit(string str, string delimiter) {
		vector<string>* splited;
		string::size_type lastPos, curPos;

		splited = new vector<string>;

		if (str == "") {
			return splited;
		}

		if (str.find_first_of(delimiter) == std::string::npos) {
			splited->push_back(str);
			return splited;
		}

		lastPos = str.find_first_not_of(delimiter, 0);

		if (lastPos != string::npos) {
			curPos = str.find_first_of(delimiter, lastPos);
			while (string::npos != curPos) {
				splited->push_back(str.substr(lastPos, curPos - lastPos));
				lastPos = str.find_first_not_of(delimiter, curPos);
				if (lastPos == string::npos) {
					break;
				}
				curPos = str.find_first_of(delimiter, lastPos);
			}
		}
		return splited;
	}

	vector<string>* split(string str, string delimiter, string pos_delimiter) {
		vector<string>* splited;
		splited = new vector<string>;
		string::size_type pos = 0;
		string::size_type lastPos = 0;

		if (pos_delimiter == "") {
			return localSplit(str, delimiter);
		}

		while (string::npos != lastPos) {
			if (str.find_first_of(pos_delimiter, pos) == pos) {
				lastPos = str.find_last_of(pos_delimiter);
				if (string::npos != lastPos) {
					splited->push_back(str.substr(pos + 1, lastPos - pos - 1));
					lastPos = str.find_first_of(delimiter, lastPos);
					if (string::npos == lastPos)
						pos = lastPos;
					else
						pos = lastPos + 1;

				}
				else {
					return splited;

				}

		    }
		    else {
		    	lastPos = str.find_first_of(delimiter, pos);
		    	if (string::npos != lastPos) {
		    		splited->push_back(str.substr(pos, lastPos - pos));
		    		pos = lastPos+1;
		    	}

		    }

		}

		if (string::npos != pos)
			splited->push_back(str.substr(pos));

		return splited;

	}

	string trim(string str) {
		string strR = "";

		string::size_type lastPos = str.find_first_not_of(" ", 0);
		string::size_type pos = str.find_first_of(" ", lastPos);

		while (string::npos != pos || string::npos != lastPos) {
		    strR = strR + str.substr(lastPos, pos - lastPos);
		    lastPos = str.find_first_not_of(" ", pos);
		    pos = str.find_first_of(" ", lastPos);
		}
		return strR;
	}

	float getPercentualValue(string value) {
		string actualValue;
		float floatValue;

		//retirar o caracter percentual da string
		actualValue = value.substr(0, (value.length() - 1));
		//converter para float
		floatValue = util::stof(actualValue);

		//se menor que zero, retornar zero
		if (floatValue < 0)
			floatValue = 0;
		//else if (floatValue > 100)
			//se maior que 100, retornar 100
			//floatValue = 100;

		//retornar valor percentual
		return floatValue;
	}

	/**
	 * Testa se uma string indica um valor percentual
	 * @param value string com um valor
	 * @return true se o valor e' percentual; false caso contrario.
	 */
	bool isPercentualValue(string value) {
		if (value.find_last_of("%") == (value.length() - 1))
			return true;
		else
			return false;
	}

	bool fileExists(string fileName) {
		string currentPath;
		char path[PATH_MAX] = "";

		if (fileName == "") {
			clog << "fileExists is false: null entry" << endl;
			return false;
		}

		try {
			if (fileName.find("<") != std::string::npos) {
				return true;
			}

//TODO: This must be moved to SystemCompat?
#if defined(_WIN32) && !defined(__MINGW32__)
			if (_access(fileName.c_str(), 0) == 0) {
				return true;
			}
#else
			if (access(fileName.c_str(), (int)F_OK) == 0) {
				return true;
			}
#endif

			clog << "fileExists is false: can't access '";
			clog << fileName << "'" << endl;
			return false;

		} catch (...) {
			clog << "util::functions fileExits exception" << endl;
		}

		clog << "fileExists is false: exception for '";
		clog << fileName << "'" << endl;
		return false;
	}

	bool isDirectory(const char* path) {
		struct stat f_stat;

		stat(path, &f_stat);

		/* return IS_DIRECTORY(f_stat.st_mode); */

		if (f_stat.st_mode & S_IFREG) {
			return false;
		}

		if (f_stat.st_mode & S_IFDIR) {
			return true;
		}

		return false;
	}

	static numeric_limits<double> double_info;

	double NaN() {
		return double_info.quiet_NaN();
	}

	double infinity() {
		return double_info.infinity();
	}

	bool isNaN(double value) {
#ifdef _WIN32
		if (_isnan(value)) {
#else
		if (isnan(value)) {
#endif
			return true;
		}

		return false;
	}

	bool isInfinity(double value) {
#ifndef _WIN32
		if (isinf(value)) {
#else
		if (!_finite(value)) {
#endif
			return true;
		}

		/*clog << "isInfinity Warning! Value = '" << value << "', ";
		clog << "sval = " << sval << endl;*/
		return false;
	}

	static double startTimeMills;
	static bool firstCallTimeMills = true;

	double getCurrentTimeMillis() {
		struct timeb t;

		ftime(&t);

		if(firstCallTimeMills) {
			firstCallTimeMills = false;
			startTimeMills = (double)t.time*1000 + (double)t.millitm - 1;
			return 1;
		}

		return (double)t.time*1000 + (double)t.millitm - startTimeMills;
	}

	//factor is not in use. It will be removed.
	double getNextStepValue(
			double initValue,
			double target,
			int factor,
			double time, double initTime, double dur, int durStep) {

		int numSteps;
		double stepSize, nextStepValue;

		//rg
		if (durStep <= 0) { //durStep is negative or not defined
			nextStepValue = initValue + (
					(double)(target-initValue)/dur) * (time-initTime);

		} else if (dur <= 0) {
			return target;

		} else {
			numSteps = dur/(durStep*1000);
			stepSize = (target-initValue)/numSteps;

			//clog << floor((time-initTime)/(durStep*1000)) << endl;
			//clog << stepSize << endl << endl;
			nextStepValue = initValue + (int) (
					(int)((time-initTime)/(durStep*1000))*stepSize);
		}

		return nextStepValue;
	}

	// Replaces ALL occurences in <str> of the string <find_what> with the
	// string <replace_with>
	void str_replace_all(string &str, const string &find_what, const string &replace_with)
	{
		string::size_type pos=0;
		while((pos=str.find(find_what, pos))!=string::npos) {
			str.erase(pos, find_what.length());
			str.insert(pos, replace_with);
			pos+=replace_with.length();
		}
	}

TELEMIDIA_UTIL_END_DECLS
