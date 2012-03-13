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

extern "C" {
	#include <sys/param.h>
	#include <unistd.h>
}


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace compat {
	string SystemCompat::filesPref        = "";
	string SystemCompat::userCurrentPath  = "";
	bool SystemCompat::initialized        = false;

	void SystemCompat::checkValues() {
		if (!initialized) {
			initialized = true;
			filesPref   = "/usr/local/etc/ginga/files/";
			initializeUserCurrentPath();
		}
	}

	void SystemCompat::initializeUserCurrentPath() {
		char path[PATH_MAX] = "";
		getcwd(path, PATH_MAX);

		userCurrentPath.assign(path, strlen(path));

		if (userCurrentPath.find_last_of("/") !=
				userCurrentPath.length() - 1) {

			userCurrentPath = userCurrentPath + "/";
		}
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
			pos = dir.find_first_of("\\");
			if (pos == std::string::npos) {
				break;
			}
			dir.replace(pos, 1, "/");
		}

		params = split(dir, "/");
		newDir = "";
		it = params->begin();
		while (it != params->end()) {
			if ((it + 1) != params->end()) {
				temp = *(it + 1);
				if (temp != ".." || found) {
					newDir = newDir + "/" + (*it);

				} else {
					++it;
					found = true;
				}

			} else if ((*it) != ".") {
				newDir = newDir + "/" + (*it);

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

		if (isXmlStr(path)) {
			return true;
		}

		i = path.find_first_of("\\");
		while (i != string::npos) {
			path.replace(i,1,"/");
			i = path.find_first_of("\\");
		}

		len = path.length();
		if ((len >= 10 && path.substr(0,10) == "x-sbtvdts:")
				|| (len >= 9 && path.substr(0,9) == "sbtvd-ts:")
				|| (len >= 7 && path.substr(0,7) == "http://")
			    || (len >= 6 && path.substr(0,6) == "ftp://")
			    || (len >= 1 && path.substr(0,1) == "/")
			    || (len >= 2 && path.substr(1,1) == ":")
			    || (len >= 7 && path.substr(0,7) == "file://")
			    || (len >= 6 && path.substr(0,6) == "tcp://")
			    || (len >= 6 && path.substr(0,6) == "udp://")
			    || (len >= 6 && path.substr(0,6) == "rtp://")) {

			return true;
		}

		return false;
	}

	string SystemCompat::getPath(string filename) {
		string path;
		string::size_type i;

		i = filename.find_last_of('/');
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
		checkValues();

		return filesPref + relUrl;
	}
}
}
}
}
}
}
}
