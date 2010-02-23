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

#include "../include/Tuner.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	Tuner::Tuner() : Thread() {
		receiving        = false;
		listeners        = new set<ITunerListener*>;
		interfaces       = new map<int, INetworkInterface*>;
		currentInterface = -1;
		firstTune        = true;

		initializeInterfaces();
	}

	Tuner::~Tuner() {
		delete listeners;
		listeners = NULL;

		delete interfaces;
		interfaces = NULL;
	}

	void Tuner::initializeInterfaces() {
		ifstream fis;
		string addr;

		fis.open(
				"/usr/local/etc/ginga/files/tuner/tuner.ini",
				ifstream::in);

		if (!fis.is_open()) {
			cout << "can't open /usr/local/etc/ginga/files/tuner/tuner.ini";
			cout << endl;

		} else {
			currentInterface = 0;
			fis >> addr;
			while (fis.good()) {
				if (addr.length() > 3 && addr.substr(0, 1) != "#") {
					if (addr.substr(0, 3) >= "224" &&
							addr.substr(0, 3) <= "239") {

						createInterface("eth", "udp_multicast", addr);

					} else if ((addr.substr(0, 3) >= "001" &&
							addr.substr(0, 3) <= "223") ||
							(addr.substr(0, 3) >= "240" &&
							addr.substr(0, 3) <= "254")) {

						createInterface("eth", "udp_unicast", addr);

					} else if (addr == "isdbt" || addr == "sbtvdt") {
						createInterface("sbtvd", "terrestrial", addr);

					} else if (addr.substr(0, 2) == "fs") {
						createInterface(
								"fs",
								"local",
								addr.substr(3, addr.length() - 3));
					}
				}
				fis >> addr;
			}
			fis.close();
		}
	}

	void Tuner::createInterface(
			string network, string protocol, string address) {

		currentInterface++;
		INetworkInterface* newInterface = new NetworkInterface(
				currentInterface, network, protocol, address);

		(*interfaces)[currentInterface] = newInterface;
	}

	bool Tuner::listen(INetworkInterface* interface) {
		return interface->tune();
	}

	void Tuner::receive(INetworkInterface* interface) {
		int rval;
		char* buff;

		/*int debugStream = open(
				"debugStream.ts", O_CREAT | O_LARGEFILE | O_WRONLY, 0644);*/

		buff = new char[BUFFSIZE];
		receiving = true;

		do {
			rval = interface->receiveData(buff);
			if (rval > 0) {
				/*if (debugStream > 0) {
					write(debugStream, buff, rval);
				}*/

				notifyListeners(buff, (unsigned int)rval);

			} else if (rval < 0) {
				//cerr << "Tuner::receive minus rval" << endl;
			}

		} while (receiving);

		delete buff;

		//close(debugStream);

		interface->close();
		unlockConditionSatisfied();
		cout << "Tuner::receive ALL DONE!" << endl;
	}

	bool Tuner::hasSignal() {
		map<int, INetworkInterface*>::iterator i;
		INetworkInterface* curInt;

		i = interfaces->find(currentInterface);
		if (i != interfaces->end()) {
			curInt = i->second;
			return curInt->hasSignal();
		}

		return false;
	}

	void Tuner::tune() {
		start();
	}

	INetworkInterface* Tuner::getCurrentInterface() {
		map<int, INetworkInterface*>::iterator i;

		i = interfaces->find(currentInterface);
		if (i != interfaces->end()) {
			return i->second;
		}

		return NULL;
	}

	void Tuner::channelDown() {
		changeChannel(-1);
	}

	void Tuner::channelUp() {
		changeChannel(1);
	}

	void Tuner::changeChannel(int factor) {
		INetworkInterface* interface;
		IChannel* channel;

		if (receiving) {
			receiving = false;
			cout << "Tuner::changeChannel waiting" << endl;
			waitForUnlockCondition();
		}

		if (interfaces->count(currentInterface) != 0) {
			interface = (*interfaces)[currentInterface];
			channel   = interface->getCurrentChannel();
			updateListenersStatus(TS_SWITCHING_CHANNEL, channel);
			if (!interface->changeChannel(factor)) {
				cout << "Tuner::changeChannel can't find channel '";
				cout << factor << "'" << endl;

			} else {
				channel = interface->getCurrentChannel();
				updateListenersStatus(TS_NEW_CHANNEL_TUNED, channel);
			}
		}

		cout << "Tuner::changeChannel start new thread" << endl;
		start();
	}

	void Tuner::addListener(ITunerListener* listener) {
		listeners->insert(listener);
	}

	void Tuner::removeListener(ITunerListener* listener) {
		set<ITunerListener*>::iterator i;

		i = listeners->find(listener);
		if (i != listeners->end()) {
			listeners->erase(i);
		}
	}

	void Tuner::notifyListeners(char* buff, unsigned int val) {
		ITunerListener* listener;
		set<ITunerListener*>::iterator i;

		for (i = listeners->begin(); i != listeners->end(); ++i) {
			listener = *i;
			if (listener != NULL) {
				listener->receiveData(buff, val);
			}
		}
	}

	void Tuner::updateListenersStatus(short newStatus, IChannel* channel) {
		ITunerListener* listener;
		set<ITunerListener*>::iterator i;

		for (i = listeners->begin(); i != listeners->end(); ++i) {
			listener = *i;
			if (listener != NULL) {
				listener->updateStatus(newStatus, channel);
			}
		}
	}

	void Tuner::run() {
		INetworkInterface* interface = NULL, *curInt = NULL;
		map<int, INetworkInterface*>::iterator i;
		bool tuned = false;
		int intIx = -1;
		IChannel* channel;

		cout << "Tuner::run tuning... " << endl;

		if (!interfaces->empty()) {
			if (interfaces->count(currentInterface) != 0) {
				curInt = (*interfaces)[currentInterface];

				if (curInt != NULL) {
					tuned = listen(curInt);
					if (tuned) {
						interface = curInt;
					}
				}
			}

			i = interfaces->begin();
			while (!tuned) {
				intIx     = i->first;
				interface = i->second;

				if (curInt != interface) {
					tuned = listen(interface);
				}

				++i;
				if (i == interfaces->end()) {
					break;
				}
			}

			if (tuned && firstTune) {
				firstTune = false;
				channel = interface->getCurrentChannel();
				updateListenersStatus(TS_NEW_CHANNEL_TUNED, channel);
			}

			if (tuned && interface != NULL &&
					!(interface->getCaps() & DPC_CAN_DEMUXBYHW)) {

				cout << "Tuner::run() call receive" << endl;
				receive(interface);
			}

			cout << "Tuner::run done " << endl;

		} else {
			cout << "Tuner::run warning! interfaces.empty" << endl;
		}
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::tuning::ITuner* createTuner() {
	return new ::br::pucrio::telemidia::ginga::core::tuning::Tuner();
}

extern "C" void destroyTuner(
		::br::pucrio::telemidia::ginga::core::tuning::ITuner* t) {

	delete t;
}
