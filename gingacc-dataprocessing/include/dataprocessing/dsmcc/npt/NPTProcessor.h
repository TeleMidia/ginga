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

#ifndef NPTPROCESSOR_H_
#define NPTPROCESSOR_H_

#include "system/thread/Thread.h"
using namespace br::pucrio::telemidia::ginga::core::system::thread;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/time/ITimeBaseProvider.h"
using namespace br::pucrio::telemidia::ginga::core::system::time;

#include "tuner/providers/ISTCProvider.h"
using namespace br::pucrio::telemidia::ginga::core::tuning;

#include "DSMCCSectionPayload.h"
#include "Descriptor.h"
#include "NPTReference.h"
#include "TimeBaseClock.h"
#include "INPTListener.h"

#include <set>
#include <map>
#include <iostream>
#include <string>
#include <cmath>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace dsmcc {
namespace npt {

class NPTProcessor : public Thread, public ITimeBaseProvider {
	private:
		ISTCProvider* stcProvider;
		bool running;
		bool loopControlMin;
		bool loopControlMax;
		unsigned char currentCid;
		pthread_mutex_t loopMutex;

		map<unsigned char, NPTReference*>* scheduledNpts;
		map<unsigned char, TimeBaseClock*>* timeBaseClock;
		map<unsigned char, set<INPTListener*>*>* loopListeners;
		map<unsigned char, map<double, set<INPTListener*>*>*>* timeListeners;
		set<INPTListener*>* cidListeners;

	public:
		NPTProcessor(ISTCProvider* stcProvider);
		virtual ~NPTProcessor();

	private:
		uint64_t getSTCValue();

	public:
		bool addLoopListener(unsigned char cid, ITimeBaseListener* ltn);

		bool addTimeListener(
				unsigned char cid, double nptValue, ITimeBaseListener* ltn);

		bool removeTimeListener(unsigned char cid, ITimeBaseListener* ltn);

		bool addIdListener(ITimeBaseListener* ltn);
		bool removeIdListener(ITimeBaseListener* ltn);

		unsigned char getCurrentTimeBaseId();
		double getCurrentTimeValue(unsigned char timeBaseId);

	private:
		void notifyLoopToTimeListeners(unsigned char cid);
		void notifyTimeListeners(unsigned char cid, double nptValue);
		void notifyIdListeners(unsigned char oldCid, unsigned char newCid);
		TimeBaseClock* getTimeBaseClock(unsigned char cid);
		int updateTimeBase(TimeBaseClock* clk, NPTReference* npt);
		int scheduleTimeBase(NPTReference* npt);
		bool checkTimeBaseArgs(
				string function, TimeBaseClock* clk, NPTReference* npt);

	public:
		int decodeNPT(vector<Descriptor*>* list);
		double getNPTValue(unsigned char contentId);

	private:
		bool getNextNptValue(
				double* nextNptValue,
				NPTReference* npt,
				unsigned char* cid,
				double* sleepTime);

		bool processNptValues(NPTReference* npt, bool* isNotify);
		void run();
};

}
}
}
}
}
}
}
}

#endif /* NPTPROCESSOR_H_ */
