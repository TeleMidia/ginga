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

#include "tuner/Tuner.h"

#if HAVE_COMPONENTS
#include "mb/ILocalScreenManager.h"
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#include "mb/LocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
#if HAVE_COMPONENTS
	static IComponentManager* cm = IComponentManager::getCMInstance();
	static ILocalScreenManager* dm = ((LocalScreenManagerCreator*)(
			cm->getObject("LocalScreenManager")))();
#else
	static ILocalScreenManager* dm = LocalScreenManager::getInstance();
#endif

	Tuner::Tuner(
			GingaScreenID screenId,
			string network,
			string protocol,
			string address) : Thread() {

		receiving        = false;
		currentInterface = -1;
		firstTune        = true;
		listener         = NULL;
		loopListener     = NULL;
		skipSize         = 0;
		packetSize       = 188;
		currentSpec      = "";
		this->screenId   = screenId;

		dm->getInputManager(screenId)->addInputEventListener(this, NULL);

		interfaces.clear();

		if (network == "" && protocol == "" && address == "") {
			initializeInterfaces();

		} else {
			createInterface(network, protocol, address);
		}

		clog << "Tuner::Tuner all done" << endl;
	}

	Tuner::~Tuner() {
		clog << "Tuner::~Tuner" << endl;

		dm->getInputManager(screenId)->removeInputEventListener(this);
		listener     = NULL;
		loopListener = NULL;

		clearInterfaces();
		clog << "Tuner::~Tuner all done" << endl;
	}

	void Tuner::setLoopListener(ITunerListener* loopListener) {
		this->loopListener = loopListener;
	}

	bool Tuner::userEventReceived(IInputEvent* ev) {
		map<int, INetworkInterface*>::iterator i;

		clog << "Tuner::userEventReceived" << endl;
		if (ev->getKeyCode(screenId) == CodeMap::KEY_QUIT) {
			//CLOSE ALL TUNER INTERFACE/PROVIDER
			lock();
			notifyStatus(TS_TUNER_POWEROFF, NULL);
			i = interfaces.begin();
			while (i != interfaces.end()) {
				i->second->close();
				++i;
			}
			unlock();
		}

		return true;
	}

	void Tuner::clearInterfaces() {
		map<int, INetworkInterface*>::iterator i;
		INetworkInterface* ni;

		lock();
		i = interfaces.begin();
		while (i != interfaces.end()) {
			ni = i->second;
			delete ni;
			interfaces.erase(i);
			i = interfaces.begin();
		}
		interfaces.clear();
		currentInterface = -1;
		unlock();
	}

	void Tuner::receiveSignal(short signalCode) {
		switch (signalCode) {
			case PST_LOOP:
				if (loopListener != NULL) {
					loopListener->updateChannelStatus(
							TS_LOOP_DETECTED, NULL);
				}
				break;

			default:
				break;
		}
	}

	void Tuner::initializeInterface(string niSpec) {
		size_t pos;
		int fo;

		if (niSpec.length() > 3 && niSpec.substr(0, 1) != "#") {

			if (niSpec.substr(0, 5) == "file:") {
				createInterface(
						"file",
						"local",
						niSpec.substr(5, niSpec.length() - 5));

				return;

			} else if (niSpec.find("ip:") != string::npos) {
				niSpec = niSpec.substr(3, niSpec.length() - 3);

			} else {
				pos = niSpec.find_first_of("//");
				if (pos != string::npos) {
					niSpec = niSpec.substr(
							pos + 2, niSpec.length() - (pos + 2));
				}
			}

			if (sscanf(niSpec.c_str(), "%d", &fo) == 0) return;

			if (fo >= 224 && fo <= 239) {

				createInterface("ip", "udp_multicast", niSpec);

			} else if ((fo >= 001 && fo <= 223) || (fo >= 240 && fo <= 254)) {

				createInterface("ip", "udp_unicast", niSpec);

			} else if (niSpec.substr(0, 6) == "isdbt:") {
				createInterface("sbtvd", "terrestrial",
								niSpec.substr(6, niSpec.length() - 6));

			} else if (niSpec.substr(0, 7) == "sbtvdt:") {
				createInterface("sbtvd", "terrestrial",
								niSpec.substr(7, niSpec.length() - 7));

			} else if (niSpec == "isdbt" || niSpec == "sbtvdt") {
				createInterface("sbtvd", "terrestrial",  niSpec);

			} else {
				clog << "Tuner::initializeInterface can't initialize";
				clog << niSpec << "': unknown specification" << endl;
			}

		} else if (niSpec.substr(0, 1) != "#") {

			clog << "Tuner::initializeInterface can't initialize";
			clog << niSpec << "': invalid specification" << endl;
		}
	}

	void Tuner::initializeInterfaces() {
		ifstream fis;
		string addr;

		string tunerpath = SystemCompat::getGingaBinPath() + "/" + GINGA_TUNER_PATH;

		fis.open(
				tunerpath.c_str(),
				ifstream::in);

		if (!fis.is_open()) {
		        clog << "can't open " << tunerpath;
			clog << endl;

		} else {
			currentInterface = 0;
			fis >> addr;
			while (fis.good()) {
				initializeInterface(addr);
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

		lock();
		interfaces[currentInterface] = newInterface;
		unlock();
	}

	bool Tuner::listenInterface(INetworkInterface* nInterface) {
		IDataProvider* provider;
		bool tuned = false;

		provider = nInterface->tune();
		if (provider != NULL) {
			tuned = true;
			provider->setListener(this);
		}

		return tuned;
	}

	void Tuner::receiveInterface(INetworkInterface* nInterface) {
		int rval;
		char* buff;

		/*int debugStream = fopen(
				"debugStream.ts", "w+b");*/

		buff = new char[BUFFSIZE];
		receiving = true;

		do {
			rval = nInterface->receiveData(buff, skipSize, packetSize);
			skipSize = 0;
			if (rval > 0) {
				/*if (debugStream > 0) {
					write(debugStream, buff, rval);
				}*/

				notifyData(buff, (unsigned int)rval);

			} else if (rval < 0) {
				//cerr << "Tuner::receive minus rval" << endl;
			}

		} while (receiving);

		delete[] buff;

		//close(debugStream);

		nInterface->close();
		unlockConditionSatisfied();
		clog << "Tuner::receive ALL DONE!" << endl;
	}

	bool Tuner::hasSignal() {
		map<int, INetworkInterface*>::iterator i;
		INetworkInterface* ni;

		ni = getCurrentInterface();
		if (ni != NULL) {
			return ni->hasSignal();
		}

		return false;
	}

	void Tuner::setSpec(string niName, string ch) {
		clearInterfaces();

		currentSpec = niName + ":" + ch;
		initializeInterface(currentSpec);

		clog << "Tuner::setSpec NI = '" << niName << "'";
		clog << ", channel = '" << ch << "' all done!" << endl;
	}

	void Tuner::tune() {
		if (currentSpec.find("scan") == std::string::npos) {
			startThread();

		} else {
			run();
		}
	}

	INetworkInterface* Tuner::getCurrentInterface() {
		map<int, INetworkInterface*>::iterator i;
		INetworkInterface* ni;

		lock();
		i = interfaces.find(currentInterface);
		if (i != interfaces.end()) {
			ni = i->second;
			unlock();
			return ni;
		}
		unlock();

		return NULL;
	}

	void Tuner::channelDown() {
		changeChannel(-1);
	}

	void Tuner::channelUp() {
		changeChannel(1);
	}

	void Tuner::changeChannel(int factor) {
		INetworkInterface* nInterface;
		IChannel* channel;

		if (receiving) {
			receiving = false;
			clog << "Tuner::changeChannel waiting" << endl;
			waitForUnlockCondition();
		}

		nInterface = getCurrentInterface();
		if (nInterface != NULL) {
			channel = nInterface->getCurrentChannel();
			notifyStatus(TS_SWITCHING_CHANNEL, channel);
			if (!nInterface->changeChannel(factor)) {
				clog << "Tuner::changeChannel can't find channel '";
				clog << factor << "'" << endl;

			} else {
				channel = nInterface->getCurrentChannel();
				notifyStatus(TS_NEW_CHANNEL_TUNED, channel);
			}
		}

		clog << "Tuner::changeChannel start new thread" << endl;
		startThread();
	}

	void Tuner::setTunerListener(ITunerListener* listener) {
		this->listener = listener;
	}

	void Tuner::setSkipSize(int size) {
		skipSize = size;
	}

	void Tuner::setPacketSize(unsigned char size) {
		packetSize = size;
	}

	void Tuner::notifyData(char* buff, unsigned int val) {
		if (listener != NULL) {
			listener->receiveData(buff, val);
		}
	}

	void Tuner::notifyStatus(short newStatus, IChannel* channel) {
		if (listener != NULL) {
			listener->updateChannelStatus(newStatus, channel);
		}
	}

	void Tuner::run() {
		INetworkInterface* nInterface = NULL, *curInt = NULL;
		map<int, INetworkInterface*>::iterator i;
		bool tuned = false;
		int intIx = -1;
		IChannel* channel;

		clog << "Tuner::run tuning... " << endl;

		curInt = getCurrentInterface();
		if (curInt == NULL) {
			clog << "Tuner::run current interface not found" << endl;

		} else {
			tuned = listenInterface(curInt);
			if (tuned) {
				nInterface = curInt;
			}
		}

		if (!tuned) {
			clog << "Tuner::run current interface can't tune...";
			clog << " trying other interfaces" << endl;

			lock();
			i = interfaces.begin();
			while (i != interfaces.end()) {
				intIx     = i->first;
				nInterface = i->second;

				if (curInt != nInterface) {
					tuned = listenInterface(nInterface);
					if (tuned) {
						break;
					}
				}

				++i;
			}
			unlock();
		}

		if (tuned && firstTune) {
			firstTune = false;
			channel = nInterface->getCurrentChannel();
			notifyStatus(TS_NEW_CHANNEL_TUNED, channel);
		}

		if (tuned && nInterface != NULL &&
				!(nInterface->getCaps() & DPC_CAN_DEMUXBYHW)) {

			clog << "Tuner::run() call receive" << endl;
			receiveInterface(nInterface);
		}

		if (!tuned && listener != NULL) {
			notifyStatus(TS_TUNER_POWEROFF, NULL);
		}
		clog << "Tuner::run done " << endl;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::tuning::ITuner* createTuner(GingaScreenID screenId) {
	return new ::br::pucrio::telemidia::ginga::core::tuning::Tuner(screenId);
}

extern "C" void destroyTuner(
		::br::pucrio::telemidia::ginga::core::tuning::ITuner* t) {

	delete t;
}
