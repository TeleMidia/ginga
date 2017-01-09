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

#ifndef _COMMONCOREMANAGER_H_
#define _COMMONCOREMANAGER_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "mb/IMBDefs.h"

#include "IPresentationEngineManager.h"
#include "ICommonCoreManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace lssm {
  class CommonCoreManager : public ICommonCoreManager {
	private:
		void* tuner;
		GingaWindowID tuningWindow;
		void* demuxer;
		void* dataProcessor;
		void* nptProvider;
		void* ccUser;
		double ocDelay;
		bool hasOCSupport;
		bool nptPrinter;
		GingaScreenID myScreen;
		IPresentationEngineManager* pem;

	public:
		CommonCoreManager();
		~CommonCoreManager();

		void addPEM(
				IPresentationEngineManager* pem, GingaScreenID screenId);

		void initializeInstance(std::string& data, short scenario);
		void testInstance(std::string& data, short scenario);

		void enableNPTPrinter(bool enableNPTPrinter);
		void setOCDelay(double ocDelay);
		void setInteractivityInfo(bool hasOCSupport);
		void removeOCFilterAfterMount(bool removeIt);
		void setTunerSpec(string tunerSpec);

	private:
		void showTunningWindow(
				GingaScreenID screenId, int x, int y, int w, int h);

		void releaseTunningWindow();

		IPlayer* createMainAVPlayer(
				string dstUri,
				GingaScreenID screenId,
				int x, int y, int w, int h);

	public:
		void tune();
		void startPresentation();
  };
}
}
}
}
}

#endif /*_COMMONCOREMANAGER_H_*/
