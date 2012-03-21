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

#include "system/compat/SystemCompat.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace compat {
	string SystemCompat::filesPref        = "";
	string SystemCompat::installPref      = "";
	string SystemCompat::userCurrentPath  = "";
	string SystemCompat::gingaCurrentPath = "";
	string SystemCompat::pathD            = "";
	string SystemCompat::iUriD            = "";
	string SystemCompat::fUriD            = "";
	bool SystemCompat::initialized        = false;

	void SystemCompat::checkValues() {
		if (!initialized) {
			initialized = true;
			initializeGingaPath();
			initializeUserCurrentPath();
			initializeGingaConfigFile();
		}
	}

	void SystemCompat::initializeGingaConfigFile() {
		ifstream fis;
		string line, key, value;
		string gingaini = gingaCurrentPath + "ginga.ini";

		fis.open(gingaini.c_str(), ifstream::in);

		if (!fis.is_open()) {
			clog << "SystemCompat::initializeGingaConfigFile ";
			clog << "Warning! Can't open input file: '" << gingaini;
			clog << "' loading default configuration!" << endl;

			pathD            = ":";
			iUriD            = "/";
			fUriD            = "\\";
			gingaCurrentPath = "/usr/local/sbin/";
			installPref      = "/usr/local";
			filesPref        = "/usr/local/etc/ginga/files/";

			return;
		}

		while (fis.good()) {
			fis >> line;
			if (line.find("#") == std::string::npos ||
					line.find("=") != std::string::npos) {

				key = line.substr(0, line.find_last_of("="));
				value = line.substr(
						(line.find_first_of("=") + 1),
						line.length() - (line.find_first_of("=") + 1));

				if (key == "system.internal.delimiter") {
					iUriD       = value;

				} else if (key == "system.foreign.delimiter") {
					fUriD       = value;

				} else if (key == "system.files.prefix") {
					filesPref   = value;

				} else if (key == "system.install.prefix") {
					installPref = value;
				}
			}
		}

		fis.close();
	}

	void SystemCompat::initializeGingaPath() {
		vector<string>* params;
		vector<string>::iterator i;
		string path, currentPath;

		path = getenv("PATH");
		if (path.find(";") != std::string::npos) {
			pathD = ";";
			iUriD = "\\";

		} else if (path.find(":") != std::string::npos) {
			pathD = ":";
			iUriD = "/";
		}

		params = split(path, pathD);
		if (params != NULL) {
			i = params->begin();
			while (i != params->end()) {
				currentPath = (*i) + iUriD + "ginga";
				if (access((*i).c_str(), (int)X_OK) == 0) {
					gingaCurrentPath = (*i);

					if (gingaCurrentPath.find_last_of(iUriD) !=
							gingaCurrentPath.length() - 1) {

						gingaCurrentPath = gingaCurrentPath + iUriD;
					}

					break;
				}
				++i;
			}
			delete params;
		}
	}

	void SystemCompat::initializeUserCurrentPath() {
		char path[PATH_MAX] = "";
		getcwd(path, PATH_MAX);

		userCurrentPath.assign(path, strlen(path));

		if (userCurrentPath.find_last_of(iUriD) !=
				userCurrentPath.length() - 1) {

			userCurrentPath = userCurrentPath + iUriD;
		}
	}

	void* SystemCompat::loadComponent(string libName, string symName) {
		void* comSym = NULL;

#ifndef WIN32
		if (libName.find(".so") == std::string::npos) {
			libName.append(".so");
		}
#else
		if (libName.find(".dll") == std::string::npos) {
			libName.append(".dll");
		}
#endif

#ifndef WIN32
		void* comp = dlopen(libName.c_str(), RTLD_LAZY);
		if (comp == NULL) {
			clog << "SystemCompat::loadComponent Warning: can't load ";
			clog << "component '" << libName << "' => ";
			clog << dlerror() << endl;
			return (NULL);
		}

		dlerror();

		comSym = dlsym(comp, symName.c_str());

		const char* dlsym_error = dlerror();
		if (dlsym_error != NULL) {
			clog << "ComponentManager warning: can't load symbol '";
			clog << symName << "' from library '" << libName;
			clog << "' => " << dlsym_error << endl;
			return (NULL);
		}

		dlerror();
#endif
		return comSym;
	}

	void SystemCompat::sigpipeHandler(int x) throw(const char*) {
#ifndef WIN32
		signal(SIGPIPE, sigpipeHandler);  //reset the signal handler
		fprintf(stderr,"throw: %s\n",strsignal(x));
		throw strsignal(x);  //throw the exeption
#endif //linux
	}


	string SystemCompat::updatePath(string dir) {
		bool found = false;
		string temp, newDir;
		vector<string>* params;
		vector<string>::iterator it;
		string::size_type pos;

		if (dir.find("<") != std::string::npos) {
			return dir;
		}

		while (true) {
			pos = dir.find_first_of(fUriD);
			if (pos == std::string::npos) {
				break;
			}
			dir.replace(pos, 1, iUriD);
		}

		params = split(dir, iUriD);
		newDir = "";
		it = params->begin();
		while (it != params->end()) {
			if ((it + 1) != params->end()) {
				temp = *(it + 1);
				if (temp != ".." || found) {
					newDir = newDir + iUriD + (*it);

				} else {
					++it;
					found = true;
				}

			} else if ((*it) != ".") {
				newDir = newDir + iUriD + (*it);

			}
			++it;
		}
		delete params;

		if (found) {
			return SystemCompat::updatePath(newDir);

		} else {
			return newDir;
		}
	}

	bool SystemCompat::isXmlStr(string location) {
		if (location.find("<") != std::string::npos ||
				location.find("?xml") != std::string::npos ||
				location.find("|") != std::string::npos) {

			return true;
		}

		return false;
	}

	bool SystemCompat::isAbsolutePath(string path) {
		string::size_type i, len;

		checkValues();

		if (isXmlStr(path)) {
			return true;
		}

		i = path.find_first_of(fUriD);
		while (i != string::npos) {
			path.replace(i, 1, iUriD);
			i = path.find_first_of(fUriD);
		}

		len = path.length();
		if ((len >= 1 && path.substr(0,1) == iUriD) ||
				(len >= 10 && path.substr(0,10) == "x-sbtvdts:") ||
				(len >= 9 && path.substr(0,9) == "sbtvd-ts:") ||
				(len >= 7 && path.substr(0,7) == "http://") ||
				(len >= 6 && path.substr(0,6) == "ftp://") ||
				(len >= 2 && path.substr(1,2) == ":" + iUriD) ||
				(len >= 7 && path.substr(0,7) == "file://") ||
				(len >= 6 && path.substr(0,6) == "tcp://") ||
				(len >= 6 && path.substr(0,6) == "udp://") ||
				(len >= 6 && path.substr(0,6) == "rtp://")) {

			return true;
		}

		return false;
	}

	string SystemCompat::getIUriD() {
		checkValues();
		return iUriD;
	}

	string SystemCompat::getFUriD() {
		checkValues();
		return fUriD;
	}

	string SystemCompat::getPath(string filename) {
		string path;
		string::size_type i;

		i = filename.find_last_of(iUriD);
		if (i != string::npos) {
			path = filename.substr(0, i);

		} else {
			path = "";
		}

		return path;
	}

	string SystemCompat::getUserCurrentPath() {
		checkValues();

		return userCurrentPath;
	}

	string SystemCompat::appendGingaFilesPrefix(string relUrl) {
		string absuri;

		checkValues();

		absuri = updatePath(filesPref + relUrl);
		/*cout << "SystemCompat::appendGingaFilesPrefix to relUrl = '";
		cout << relUrl << "' filesPref = '" << filesPref << "' ";
		cout << " updated path = '" << absuri << "' ";
		cout << endl;*/

		return absuri;
	}

	string SystemCompat::appendGingaInstallPrefix(string relUrl) {
		string absuri;

		checkValues();

		absuri = updatePath(installPref + relUrl);
		/*cout << "SystemCompat::appendGingaInstallPrefix to relUrl = '";
		cout << relUrl << "' installPref = '" << installPref << "' ";
		cout << " updated path = '" << absuri << "' ";
		cout << endl;*/

		return absuri;
	}

	void SystemCompat::initializeSigpipeHandler() {
#ifndef WIN32
		signal(SIGPIPE, sigpipeHandler);
#endif //linux
	}

	string SystemCompat::getOperatingSystem() {
#ifndef WIN32
		return "Linux";
#else
		return "Windows";
#endif
	}

	float SystemCompat::getClockSpeed() {
		float clockSpeed = 1000.0;

#ifndef WIN32
		ifstream fis;
		string line = "";

		fis.open("/proc/cpuinfo", ifstream::in);

		if (!fis.is_open()) {
			clog << "SystemInfo::initializeClockSpeed Warning: can't open ";
			clog << "file '/proc/cpuinfo'" << endl;
			return clockSpeed;
		}

		while (fis.good()) {
			fis >> line;
			if (line == "cpu") {
				fis >> line;
				if (line == "MHz") {
					fis >> line;
					if (line == ":") {
						fis >> line;
						clockSpeed = util::stof(line);
						break;
					}
				}
			}
		}
#endif
		return clockSpeed;
	}

	float SystemCompat::getMemorySize() {
#ifndef WIN32
		struct sysinfo info;
		sysinfo(&info);
		return info.totalram;
#else
		MEMORYSTATUS ms;
		GlobalMemoryStatus(&ms);
		return (float)ms.dwAvailPhys;
#endif
	}

	void SystemCompat::makeDir(const char* dirName, unsigned int mode) {
#ifdef WIN32
			_mkdir(dirName);
#else
			mkdir(dirName, mode);
#endif
	}

	void SystemCompat::uSleep(unsigned int sleepTime) {
#ifndef WIN32
		::usleep(sleepTime);
#elif WIN32
		Sleep(sleepTime/1000);
#endif
	}
}
}
}
}
}
}
}
