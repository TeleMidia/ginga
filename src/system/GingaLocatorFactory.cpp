/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#include "config.h"
#include "GingaLocatorFactory.h"

BR_PUCRIO_TELEMIDIA_GINGA_CORE_SYSTEM_FS_BEGIN

	GingaLocatorFactory* GingaLocatorFactory::_instance = 0;

	GingaLocatorFactory::GingaLocatorFactory() {
		iorClients = new map<string, string>;
		iorProviders = new map<string, string>;
		iorNames = new map<string, string>;

		isWaiting = false;
		Thread::condInit(&flagCondSignal, NULL);
		Thread::mutexInit(&flagMutexSignal, false);
	}

	GingaLocatorFactory::~GingaLocatorFactory() {
		if (iorClients != NULL) {
			delete iorClients;
			iorClients = NULL;
		}

		if (iorProviders != NULL) {
			delete iorProviders;
			iorProviders = NULL;
		}

		if (iorNames != NULL) {
			delete iorNames;
			iorNames = NULL;
		}

		Thread::condDestroy(&flagCondSignal);
		Thread::mutexDestroy(&flagMutexSignal);
	}

	void GingaLocatorFactory::release() {
		if (_instance != NULL) {
			delete _instance;
			_instance = NULL;
		}
	}

	GingaLocatorFactory* GingaLocatorFactory::getInstance() {
		if (GingaLocatorFactory::_instance == NULL) {
			GingaLocatorFactory::_instance = new GingaLocatorFactory();
		}

		return GingaLocatorFactory::_instance;
	}

	void GingaLocatorFactory::createLocator(string providerUri, string ior) {
		vector<string>* args;
		vector<string>::iterator i;
		string arg, newIor;

		if (ior.find(".") == std::string::npos) {
			newIor = ior;

		} else {
			newIor = "";
			args = split(ior, ".");
			i = args->begin();
			while(i != args->end()) {
				arg = *i;
				newIor = newIor + itos(strHexaToInt(arg));
				++i;
			}

			delete args;
			args = NULL;
		}

		string internal = SystemCompat::getIUriD();
		internal = internal + internal;

		string foreign = SystemCompat::getFUriD();
		foreign = foreign + foreign;

		while (providerUri.find(internal) != std::string::npos) {
			providerUri = providerUri.substr(0,
					providerUri.find_first_of(internal) - 1) +
							SystemCompat::getIUriD() +

					providerUri.substr(
							providerUri.find_first_of(internal) + 2,
							providerUri.length());
		}

		while (providerUri.find(foreign) != std::string::npos) {
			providerUri = providerUri.substr(0,
					providerUri.find_first_of(foreign) - 1) +
							SystemCompat::getFUriD() +

					providerUri.substr(
							providerUri.find_first_of(foreign) + 2,
							providerUri.length());
		}

		while (providerUri.substr(providerUri.length() - 1, 1) ==
				SystemCompat::getIUriD()) {

			providerUri = providerUri.substr(0, providerUri.length() - 1);
		}

		while (providerUri.substr(providerUri.length() - 1, 1) ==
				SystemCompat::getFUriD()) {

			providerUri = providerUri.substr(0, providerUri.length() - 1);
		}

		(*iorProviders)[newIor] = providerUri;
	}

	string GingaLocatorFactory::getLocation(string providerUri) {
		map<string, string>::iterator i;
		string clientUri = "";
		string ior = "-1";

		string internal = SystemCompat::getIUriD();
		internal = internal + internal;

		string foreign = SystemCompat::getFUriD();
		foreign = foreign + foreign;

		while (providerUri.find(internal) != std::string::npos) {
			providerUri = providerUri.substr(0,
					providerUri.find_first_of(internal) - 1) +
							SystemCompat::getIUriD() +

					providerUri.substr(
							providerUri.find_first_of(internal) + 2,
							providerUri.length());
		}

		while (providerUri.find(foreign) != std::string::npos) {
			providerUri = providerUri.substr(0,
					providerUri.find_first_of(foreign) - 1) +
							SystemCompat::getFUriD() +

					providerUri.substr(
							providerUri.find_first_of(foreign) + 2,
							providerUri.length());
		}

		while (providerUri.substr(providerUri.length() - 1, 1) ==
				SystemCompat::getIUriD()) {

			providerUri = providerUri.substr(0, providerUri.length() - 1);
		}

		while (providerUri.substr(providerUri.length() - 1, 1) ==
				SystemCompat::getFUriD()) {

			providerUri = providerUri.substr(0, providerUri.length() - 1);
		}

		i = iorProviders->begin();
		while (i != iorProviders->end()) {
			if (i->second == providerUri) {
				ior = i->first;
			}
			++i;
		}

		if (iorClients->count(ior) != 0) {
			clientUri = (*iorClients)[ior];
		}

		if (clientUri == "") {
			waitNewLocatorCondition();
			return getLocation(providerUri);
		}

		return clientUri;
	}

	string GingaLocatorFactory::getName(string ior) {
		vector<string>* args;
		vector<string>::iterator i;
		string arg, newIor;

		if (ior.find(".") == std::string::npos) {
			newIor = ior;

		} else {
			newIor = "";
			args = split(ior, ".");
			i = args->begin();
			while(i != args->end()) {
				arg = *i;
				newIor = newIor + itos(strHexaToInt(arg));
				++i;
			}

			delete args;
			args = NULL;
		}

		if (iorNames->count(newIor) != 0) {
			return (*iorNames)[newIor];
		}

		waitNewLocatorCondition();
		return getName(newIor);
	}

	void GingaLocatorFactory::addClientLocation(
			string ior, string clientUri, string name) {

		vector<string>* args;
		vector<string>::iterator i;
		string arg, newIor;

		if (ior.find(".") == std::string::npos) {
			newIor = ior;

		} else {
			newIor = "";
			args = split(ior, ".");
			i = args->begin();
			while(i != args->end()) {
				arg = *i;
				newIor = newIor + itos(strHexaToInt(arg));
				++i;
			}

			delete args;
			args = NULL;
		}

		(*iorClients)[newIor] = clientUri;
		(*iorNames)[newIor] = name;
		newLocatorConditionSatisfied();
	}

	void GingaLocatorFactory::waitNewLocatorCondition() {
		isWaiting = true;
		Thread::mutexLock(&flagMutexSignal);
		Thread::condWait(&flagCondSignal, &flagMutexSignal);
		isWaiting = false;
		Thread::mutexUnlock(&flagMutexSignal);
	}

	bool GingaLocatorFactory::newLocatorConditionSatisfied() {
		if (isWaiting) {
			Thread::condSignal(&flagCondSignal);
			return true;
		}
		return false;
	}

BR_PUCRIO_TELEMIDIA_GINGA_CORE_SYSTEM_FS_END
