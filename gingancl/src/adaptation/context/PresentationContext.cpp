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

#include "gingancl/adaptation/context/PresentationContext.h"

#include "config.h"

#if HAVE_COMPONENTS
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "contextmanager/ContextManager.h"
#include "contextmanager/system/SystemInfo.h"
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adaptation {
namespace context {
#if HAVE_COMPONENTS
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif

	IContextManager* PresentationContext::contextManager = NULL;

	PresentationContext::PresentationContext(GingaScreenID screenId) {
		createObserversVector();

		myScreen = screenId;

		if (contextManager == NULL) {
#if HAVE_COMPONENTS
			contextManager = ((ContextManagerCreator*)(cm->getObject(
					"ContextManager")))();
#else
			PresentationContext::contextManager = ContextManager::getInstance();
#endif
		}

		contextManager->addContextListener(this);

		initializeUserContext();
		initializeSystemValues();

		globalVarListener = NULL;
		devListener = NULL;

		Thread::mutexInit(&attrMutex, NULL);
	}

	PresentationContext::~PresentationContext() {
		contextManager->removeContextListener(this);

		contextTable.clear();
	}

	void PresentationContext::setPropertyValue(
		    string property, string value) {

		string oldValue = "";
		map<string, string>::iterator i;

		Thread::mutexLock(&attrMutex);
		if ((property.length() >= 7 && property.substr(0, 7) == "system.") ||
				(property.length() >= 5 && property.substr(0, 5) == "user.")) {

			Thread::mutexUnlock(&attrMutex);
			return;
		}

		clog << "PresentationContext::setPropertyValue propName = '";
		clog << property << "' propValue = '" << value << "'" << endl;
		i = contextTable.find(property);

		if (i != contextTable.end()) {
			oldValue = i->second;
		}

		contextTable[property] = value;
		Thread::mutexUnlock(&attrMutex);

		if ((value != "") && (value != oldValue)) {
			notifyObservers(&property);
		}

		if (devListener != NULL) {
#if HAVE_MULTIDEVICE
			//clog << "PresentationContext::set(devListener != NULL)" << endl;
			int myDevClass = -1;
			string evPayload = property + " = " + value;
			devListener->receiveRemoteEvent(myDevClass,
					IDeviceDomain::FT_ATTRIBUTIONEVENT,
					evPayload);
#endif //HAVE_MULTIDEVICE
		}

	}

	void PresentationContext::incPropertyValue(string propertyName) {
		string oldValue = "", newValue = "";
		map<string, string>::iterator i;

		i = contextTable.find(propertyName);
		if (i != contextTable.end()) {
			oldValue = i->second;
		}

		if (oldValue == "") {
			return;
		}

		newValue = itos(util::stof(oldValue) + 1);
		if ((newValue != "") && (newValue != oldValue)) {
			contextTable[propertyName] = newValue;
			notifyObservers(&propertyName);
		}
	}

	void PresentationContext::decPropertyValue(string propertyName) {
		string oldValue = "", newValue = "";
		map<string, string>::iterator i;

		i = contextTable.find(propertyName);
		if (i != contextTable.end()) {
			oldValue = i->second;
		}

		if (oldValue == "") {
			return;
		}

		newValue = itos(util::stof(oldValue) - 1);
		if ((newValue != "") && (newValue != oldValue)) {
			contextTable[propertyName] = newValue;
			notifyObservers(&propertyName);
		}
	}

	set<string>* PresentationContext::getPropertyNames() {
		set<string>* propertyNames = new set<string>;
		map<string, string>::iterator i;
		for (i = contextTable.begin(); i != contextTable.end(); ++i) {
			propertyNames->insert(i->first);
		}

		return propertyNames;
	}

	string PresentationContext::getPropertyValue(string attributeId) {
		string propValue;

		Thread::mutexLock(&attrMutex);
		if (contextTable.count(attributeId) == 0) {
			clog << "PresentationContext::getPropertyValue prop '";
			clog << attributeId << "' has a NULL value" << endl;
			Thread::mutexUnlock(&attrMutex);
			return "";
		}

		propValue = contextTable[attributeId];
		Thread::mutexUnlock(&attrMutex);

		clog << "PresentationContext::getPropertyValue prop '";
		clog << attributeId << "' == '" << propValue;
		clog << "'" << endl;

		return propValue;
	}

	void PresentationContext::initializeUserContext() {
		int currentUserId;
		map<string, string>* userProfile = NULL;

		currentUserId = PresentationContext::contextManager->getCurrentUserId();
		userProfile   = PresentationContext::contextManager->getUserProfile(
				currentUserId);

		if (userProfile == NULL) {
			PresentationContext::contextManager->addContextVar(
					currentUserId, "system.background-color", "000000");

			userProfile = PresentationContext::contextManager->getUserProfile(
					currentUserId);
		}

		contextTable.clear();
		contextTable.insert(userProfile->begin(), userProfile->end());

		delete userProfile;

		initializeUserInfo(currentUserId);
	}

	void PresentationContext::initializeUserInfo(int currentUserId) {
		IGingaUser* user;

		user = PresentationContext::contextManager->getUser(currentUserId);
		if (user != NULL) {
			contextTable[USER_AGE]      = itos(user->getUserAge());
			contextTable[USER_LOCATION] = user->getUserLocation();
			contextTable[USER_GENRE]    = user->getUserGenre();
		}
	}

	void PresentationContext::initializeSystemValues() {
		ISystemInfo* si;
		int w, h;

		//clog << "PresentationContext::initializeSystemValues " << endl;
		si = PresentationContext::contextManager->getSystemInfo();

		contextTable[SYSTEM_LANGUAGE]        = si->getSystemLanguage();
		contextTable[SYSTEM_CAPTION]         = si->getCaptionLanguage();
		contextTable[SYSTEM_SUBTITLE]        = si->getSubtitleLanguage();
		contextTable[SYSTEM_RETURN_BIT_RATE] = itos(si->getReturnBitRate());

		si->getScreenSize(myScreen, &w, &h);
		contextTable[SYSTEM_SCREEN_SIZE] = itos(w) + "," + itos(h);

		si->getScreenGraphicSize(myScreen, &w, &h);
		contextTable[SYSTEM_SCREEN_GRAPHIC_SIZE] = itos(w) + "," + itos(h);
		contextTable[SYSTEM_AUDIO_TYPE]          = si->getAudioType();
		contextTable[SYSTEM_CPU]                 = itos(si->getCPUClock());
		contextTable[SYSTEM_MEMORY]              = itos(si->getMemorySize());
		contextTable[SYSTEM_OPERATING_SYSTEM]    = si->getOperatingSystem();

		contextTable[SYSTEM_DEVNUMBER + "(0)"] = "0";
		contextTable[SYSTEM_DEVNUMBER + "(1)"] = "0";
		contextTable[SYSTEM_DEVNUMBER + "(2)"] = "0";

		contextTable[SYSTEM_CLASSTYPE + "(0)"] = "base";
		contextTable[SYSTEM_CLASSTYPE + "(1)"] = "passive";
		contextTable[SYSTEM_CLASSTYPE + "(2)"] = "active";

		contextTable[SYSTEM_INFO + "(0)"] = "0";
		contextTable[SYSTEM_INFO + "(1)"] = "1";
		contextTable[SYSTEM_INFO + "(2)"] = "2";

		contextTable[SYSTEM_CLASS_NUMBER] = "3";

		if (contextTable.count(DEFAULT_FOCUS_BORDER_COLOR) == 0) {
			contextTable[DEFAULT_FOCUS_BORDER_COLOR] = "blue";
		}

		if (contextTable.count(DEFAULT_SEL_BORDER_COLOR) == 0) {
			contextTable[DEFAULT_SEL_BORDER_COLOR] = "green";
		}

		if (contextTable.count(DEFAULT_FOCUS_BORDER_WIDTH) == 0) {
			contextTable[DEFAULT_FOCUS_BORDER_WIDTH] = "3";
		}

		if (contextTable.count(DEFAULT_FOCUS_BORDER_TRANSPARENCY) == 0) {
			contextTable[DEFAULT_FOCUS_BORDER_TRANSPARENCY] = "0";
		}
	}

	void PresentationContext::save() {
		string property;
		map<string, string>::iterator i;

		//contextManager->saveUsersAccounts();
		PresentationContext::contextManager->saveUsersProfiles();
	}

	void PresentationContext::setGlobalVarListener(IContextListener* listener) {
		globalVarListener = listener;
	}

	void PresentationContext::receiveGlobalAttribution(
			string propertyName, string value) {

		if (globalVarListener != NULL) {
			globalVarListener->receiveGlobalAttribution(propertyName, value);
		}
	}

#if HAVE_MULTIDEVICE
	void PresentationContext::setRemoteDeviceListener(IRemoteDeviceListener* rdl) {
		devListener = rdl;
	}
#endif
}
}
}
}
}
}
}
