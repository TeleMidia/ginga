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

#include "../../include/system/SystemInfo.h"

#include "../../config.h"

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "../../../gingacc-system/include/io/LocalDeviceManager.h"
#endif

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "lua.h"

#include <fstream>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace contextmanager {
#if HAVE_COMPSUPPORT
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif
	static ILocalDeviceManager* dm = NULL;

	SystemInfo::SystemInfo() {
		sysinfo(&info);
		uname(&sn);

		initializeClockSpeed();
		sysTable = NULL;

		if (dm == NULL) {
#if HAVE_COMPSUPPORT
			dm = ((LocalDeviceManagerCreator*)(
					cm->getObject("LocalDeviceManager")))();
#else
			dm = LocalDeviceManager::getInstance();
#endif
		}
	}

	SystemInfo::~SystemInfo() {

	}

	void SystemInfo::initializeClockSpeed() {
		ifstream fis;
		string line = "";

		clockSpeed = 0;
		fis.open("/proc/cpuinfo", ifstream::in);

		if (!fis.is_open()) {
			cout << "SystemInfo::initializeClockSpeed Warning: can't open ";
			cout << "file '/proc/cpuinfo'" << endl;
			return;
		}

		while (fis.good()) {
			fis >> line;
			if (line == "cpu") {
				fis >> line;
				if (line == "MHz") {
					fis >> line;
					if (line == ":") {
						fis >> line;
						clockSpeed = stof(line);
					}
				}
			}
		}
	}

	string SystemInfo::getValue(string attribute) {
		map<string, string>::iterator i;

		if (sysTable != NULL) {
			i = sysTable->find(attribute);
			if (i != sysTable->end()) {
				return i->second;
			}
		}

		return "";
	}

	void SystemInfo::printSysTable() {
		map<string, string>::iterator i;

		cout << "SystemInfo::printSysTable " << endl;
		i = sysTable->begin();
		while (i != sysTable->end()) {
			cout << "'" << i->first << "' = '" << i->second << "'" << endl;
			++i;
		}
	}

	void SystemInfo::setSystemTable(map<string, string>* sysTable) {
		if (this->sysTable != NULL) {
			delete this->sysTable;
		}

		this->sysTable = sysTable;
		//printSysTable();
	}

	string SystemInfo::getSystemLanguage() {
		string value = getValue("system.language");
		if (value == "") {
			cout << "SystemInfo::getSystemLanguage can't find '";
			cout << "system.language, return por" << endl;
			return "por";
		}

		return value;
	}

	string SystemInfo::getCaptionLanguage() {
		string value = getValue("system.caption");
		if (value == "") {
			return "por";
		}

		return value;
	}

	string SystemInfo::getSubtitleLanguage() {
		string value = getValue("system.subtitle");
		if (value == "") {
			return "por";
		}

		return value;
	}

	float SystemInfo::getReturnBitRate() {
		string value = getValue("system.returnBitRate");
		if (value == "") {
			return 0;
		}

		return stof(value);
	}

	void SystemInfo::getScreenSize(int* width, int* height) {
		// TODO: correct retrieve
		*width = dm->getDeviceWidth();
		*height = dm->getDeviceHeight();
	}

	void SystemInfo::getScreenGraphicSize(int* width, int* height) {
		// TODO: correct retrieve
		*width = dm->getDeviceWidth();
		*height = dm->getDeviceHeight();
	}

	string SystemInfo::getAudioType() {
		string value = getValue("system.audioType");
		if (value == "") {
			return "stereo";
		}

		return value;
	}

	float SystemInfo::getCPUClock() {
		return clockSpeed;
	}

	float SystemInfo::getMemorySize() {
		return info.totalram;
	}

	string SystemInfo::getOperatingSystem() {
		return sn.sysname;
	}

	string SystemInfo::getJavaConfiguration() {
		string value = getValue("system.javaConfiguration");
		if (value == "") {
			return "0";
		}

		return value;
	}

	string SystemInfo::getJavaProfile() {
		string value = getValue("system.javaProfile");
		if (value == "") {
			return "0";
		}

		return value;
	}

	string SystemInfo::getLuaVersion() {
		return (string)(LUA_VERSION);
	}
}
}
}
}
}
}
