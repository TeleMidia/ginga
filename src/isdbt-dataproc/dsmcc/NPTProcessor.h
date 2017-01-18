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

#ifndef NPTPROCESSOR_H_
#define NPTPROCESSOR_H_

#include "system/Thread.h"
using namespace br::pucrio::telemidia::ginga::core::system::thread;

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/ITimeBaseProvider.h"
using namespace br::pucrio::telemidia::ginga::core::system::time;

#include "isdbt-tuner/ISTCProvider.h"
using namespace br::pucrio::telemidia::ginga::core::tuning;

#include "DSMCCSectionPayload.h"
#include "MpegDescriptor.h"
#include "NPTReference.h"
#include "TimeBaseClock.h"


BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_DSMCC_NPT_BEGIN


class NPTProcessor : public Thread, public ITimeBaseProvider {
	
	struct TimeControl {
		double time;
		bool notified;
	};

	private:
		static const unsigned short MAX_NPT_VALUE       = 47721;
		static const char INVALID_CID		   = -1;
		static const short NPT_ST_OCCURRING    = 0;
		static const short NPT_ST_PAUSED       = 1;

		ISTCProvider* stcProvider;
		bool running;
		bool loopControlMin;
		bool loopControlMax;
		unsigned char currentCid;
		unsigned char occurringTimeBaseId;

		pthread_mutex_t loopMutex;
		pthread_mutex_t schedMutex;
		pthread_mutex_t lifeMutex;

		map<unsigned char, NPTReference*> scheduledNpts;
		map<unsigned char, TimeBaseClock*> timeBaseClock;
		map<unsigned char, Stc*> timeBaseLife;
		map<unsigned char, set<ITimeBaseProvider*>*> loopListeners;
		map<unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*> timeListeners;
		set<ITimeBaseProvider*> cidListeners;
		bool reScheduleIt;
		uint64_t firstStc;
		bool isFirstStc;
		bool nptPrinter;
		map<char,NPTReference*> lastNptList;

	public:
		NPTProcessor(ISTCProvider* stcProvider);
		virtual ~NPTProcessor();

		void setNptPrinter(bool nptPrinter);

	private:
		uint64_t getSTCValue();
		void clearUnusedTimebase();
		void clearTables();
		void detectLoop();

	public:
		bool addLoopListener(unsigned char cid, ITimeBaseListener* ltn);
		bool removeLoopListener(unsigned char cid, ITimeBaseListener* ltn);

		bool addTimeListener(
				unsigned char cid, double nptValue, ITimeBaseListener* ltn);

		bool removeTimeListener(unsigned char cid, ITimeBaseListener* ltn);

		bool addIdListener(ITimeBaseListener* ltn);
		bool removeIdListener(ITimeBaseListener* ltn);

		unsigned char getOccurringTimeBaseId();

	private:
		unsigned char getCurrentTimeBaseId();
		void notifyLoopToTimeListeners();
		void notifyTimeListeners(unsigned char cid, double nptValue);
		void notifyNaturalEndListeners(unsigned char cid, double nptValue);
		void notifyIdListeners(unsigned char oldCid, unsigned char newCid);
		TimeBaseClock* getTimeBaseClock(unsigned char cid);
		int updateTimeBase(TimeBaseClock* clk, NPTReference* npt);
		TimeBaseClock* getCurrentTimebase();
		double getCurrentTimeValue(unsigned char timeBaseId);

	public:
		int decodeDescriptors(vector<MpegDescriptor*>* list);
		double getNPTValue(unsigned char contentId);

	private:
		char getNextNptValue(char cid, double *nextNptValue, double* sleepTime);

		bool processNptValues();
		void run();
};


BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_DSMCC_NPT_END
#endif /* NPTPROCESSOR_H_ */
