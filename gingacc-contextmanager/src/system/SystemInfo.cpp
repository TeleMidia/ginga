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

#include "contextmanager/system/SystemInfo.h"

#include "config.h"

#include "mb/LocalScreenManager.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include <fstream>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace contextmanager {
	static IScreenManager* dm = NULL;

	SystemInfo::SystemInfo() {
		initializeClockSpeed();
		sysTable = NULL;

		if (dm == NULL) {
			dm = ScreenManagerFactory::getInstance();
		}
	}

	SystemInfo::~SystemInfo() {

	}

	void SystemInfo::initializeClockSpeed() {
		ifstream fis;
		string line = "";

		clockSpeed = SystemCompat::getClockSpeed();
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

		clog << "SystemInfo::printSysTable " << endl;
		i = sysTable->begin();
		while (i != sysTable->end()) {
			clog << "'" << i->first << "' = '" << i->second << "'" << endl;
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
			clog << "SystemInfo::getSystemLanguage can't find '";
			clog << "system.language, return por" << endl;
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

		return util::stof(value);
	}

	void SystemInfo::getScreenSize(
			GingaScreenID screenId, int* width, int* height) {

		*width = dm->getDeviceWidth(screenId);
		*height = dm->getDeviceHeight(screenId);
	}

	void SystemInfo::getScreenGraphicSize(
			GingaScreenID screenId, int* width, int* height) {

		*width = dm->getDeviceWidth(screenId);
		*height = dm->getDeviceHeight(screenId);
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
		return SystemCompat::getMemorySize();
	}

	string SystemInfo::getOperatingSystem() {
		return SystemCompat::getOperatingSystem();
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
		return (string)("5.1");
	}
}
}
}
}
}
}
