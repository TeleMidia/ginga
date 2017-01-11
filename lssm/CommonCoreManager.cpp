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

#include "lssm/CommonCoreManager.h"
#include "config.h"

#if HAVE_DSMCC
#include "tuner/Tuner.h"
#include "tsparser/Demuxer.h"
#include "tsparser/PipeFilter.h"
#include "dataprocessing/DataProcessor.h"
#endif
#include "player/ImagePlayer.h"
#include "player/AVPlayer.h"
#include "player/ProgramAV.h"

#if HAVE_TUNER
#include "tuner/ITuner.h"
using namespace ::br::pucrio::telemidia::ginga::core::tuning;
#endif

#if HAVE_TSPARSER
#include "tsparser/IDemuxer.h"
#include "tsparser/ITSFilter.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;
#endif

#if HAVE_DSMCC
#include "dataprocessing/IDataProcessor.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing;

#include "dataprocessing/dsmcc/carousel/object/IObjectListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::carousel;
#endif

#include "mb/ILocalScreenManager.h"
#include "mb/IWindow.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "player/IPlayer.h"
#include "player/IProgramAV.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#if HAVE_TUNER && HAVE_TSPARSER && HAVE_DSMCC
#include "lssm/DataWrapperListener.h"
#endif

#include "lssm/StcWrapper.h"

#include <sys/types.h>
#include <stdio.h>

#include "pthread.h"
#include <string>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace lssm {
	static IScreenManager* dm = ScreenManagerFactory::getInstance();

	CommonCoreManager::CommonCoreManager() {
		tuningWindow  = 0;
		tuner         = NULL;
		demuxer       = NULL;
		dataProcessor = NULL;
		ccUser        = NULL;
		nptProvider   = NULL;
		ocDelay       = 0;
		hasOCSupport  = true;
		pem           = NULL;
		nptPrinter    = false;
		myScreen      = 0;
	}

	CommonCoreManager::~CommonCoreManager() {
		//TODO: release attributes
		clog << "CommonCoreManager::~CommonCoreManager all done" << endl;
	}

	void CommonCoreManager::addPEM(
			IPresentationEngineManager* pem, GingaScreenID screenId) {

		this->myScreen = screenId;
		this->pem = pem;

#if HAVE_TUNER && HAVE_TSPARSER && HAVE_DSMCC
		tuner = new Tuner(myScreen);

		pem->setIsLocalNcl(false, tuner);

		clog << "CommonCoreManager::CommonCoreManager ";
		clog << "creating demuxer" << endl;
		demuxer = new Demuxer((ITuner*)tuner);

		clog << "CommonCoreManager::CommonCoreManager ";
		clog << "creating data processor" << endl;

		dataProcessor = new DataProcessor();

		ccUser = pem->getDsmccListener();

		// Add PEM as a listener of SEs and OCs
		((IDataProcessor*)dataProcessor)->addSEListener(
				"gingaEditingCommands", (IStreamEventListener*)(
						(DataWrapperListener*)ccUser));

		((IDataProcessor*)dataProcessor)->addObjectListener(
				(IObjectListener*)((DataWrapperListener*)ccUser));

		((IDataProcessor*)dataProcessor)->setServiceDomainListener(
				(IServiceDomainListener*)((DataWrapperListener*)ccUser));

		((IDataProcessor*)dataProcessor)->setDemuxer((IDemuxer*)demuxer);

		((ITuner*)tuner)->setLoopListener((IDataProcessor*)dataProcessor);

#endif //HAVE_TUNER && HAVE_TSPARSER && HAVE_DSMCC
	}

	void CommonCoreManager::initializeInstance(std::string& data, short scenario) {

	}

	void CommonCoreManager::testInstance(std::string& data, short scenario) {

	}

	void CommonCoreManager::enableNPTPrinter(bool enableNPTPrinter) {
		nptPrinter = enableNPTPrinter;
	}

	void CommonCoreManager::setOCDelay(double ocDelay) {
		this->ocDelay = ocDelay;
	}

	void CommonCoreManager::setInteractivityInfo(bool hasOCSupport) {
		this->hasOCSupport = hasOCSupport;
	}

	void CommonCoreManager::removeOCFilterAfterMount(bool removeIt) {
#if HAVE_TUNER && HAVE_TSPARSER && HAVE_DSMCC
		if (dataProcessor != NULL) {
			((IDataProcessor*)dataProcessor)->removeOCFilterAfterMount(removeIt);
		}
#endif
	}

	void CommonCoreManager::setTunerSpec(string tunerSpec) {
#if HAVE_TUNER
		string ni, ch;
		size_t pos;

		pos = tunerSpec.find_first_of(":");
		if (pos != std::string::npos) {
			ni = tunerSpec.substr(0, pos);
			ch = tunerSpec.substr(pos + 1, tunerSpec.length() - pos + 1);
			((ITuner*)tuner)->setSpec(ni, ch);
		}
#endif
	}

	void CommonCoreManager::showTunningWindow(
			GingaScreenID screenId, int x, int y, int w, int h) {

		GingaSurfaceID s     = 0;
		string tunerImg = "";

		tunerImg = SystemCompat::appendGingaFilesPrefix("tuner/tuning.png");
		if (fileExists(tunerImg)) {
			tuningWindow = dm->createWindow(
					screenId, x, y, w, h, -10.0);

			s = dm->createRenderedSurfaceFromImageFile(
					screenId, tunerImg.c_str());

			int cap = dm->getWindowCap(myScreen, tuningWindow, "ALPHACHANNEL");
			dm->setWindowCaps(myScreen, tuningWindow, cap);

			dm->drawWindow(myScreen, tuningWindow);
			dm->showWindow(myScreen, tuningWindow);
			dm->renderWindowFrom(myScreen, tuningWindow, s);
			dm->lowerWindowToBottom(myScreen, tuningWindow);

			dm->deleteSurface(s);
			s = 0;
		}
	}

	void CommonCoreManager::releaseTunningWindow() {
		if (tuningWindow != 0) {
			dm->clearWindowContent(myScreen, tuningWindow);
			dm->hideWindow(myScreen, tuningWindow);

			dm->deleteWindow(myScreen, tuningWindow);
			tuningWindow = 0;
		}
	}

	IPlayer* CommonCoreManager::createMainAVPlayer(
			string dstUri, GingaScreenID screenId, int x, int y, int w, int h) {

		IPlayer* ipav;

		clog << "lssm-ccm::cmavp creating player" << endl;

		ipav = ProgramAV::getInstance(screenId);

		//vPid = ((IDemuxer*)demuxer)->getDefaultMainVideoPid();
		//aPid = ((IDemuxer*)demuxer)->getDefaultMainAudioPid();

		//ipav->setPropertyValue("sbtvd-ts://audio", itos(aPid));
		//ipav->setPropertyValue("sbtvd-ts://video", itos(vPid));
		ipav->setPropertyValue(
				"setBoundaries",
				itos(x) + "," + itos(y) + "," +
				itos(w) + "," + itos(h));

		ipav->setPropertyValue("createPlayer", "sbtvd-ts://" + dstUri);
		ipav->setPropertyValue("showPlayer", "sbtvd-ts://" + dstUri);

		return ipav;
	}

	void CommonCoreManager::tune() {
#if HAVE_TUNER
		clog << "lssm-ccm::cpi tunning..." << endl;
		((ITuner*)tuner)->tune();
#endif
	}

	void CommonCoreManager::startPresentation() {
		//int aPid = -1, vPid = -1;
		int cpid;

#if HAVE_TUNER && HAVE_TSPARSER && HAVE_DSMCC

		ITSFilter* mavFilter  = NULL;
		IPlayer* ipav         = NULL;
		NclPlayerData* data   = NULL;
		StcWrapper* sw        = NULL;
		INetworkInterface* ni = NULL;
		string dstUri         = "dtv_channel.ts";

		data = pem->createNclPlayerData();

		showTunningWindow(data->screenId, data->x, data->y, data->w, data->h);
		tune();
		dstUri = ((IDemuxer*)demuxer)->createTSUri(dstUri);

		// Create Main AV
		ipav = createMainAVPlayer(
				dstUri,
				data->screenId,
				data->x, data->y, data->w, data->h);

		delete data;
		clog << "lssm-ccm::sp create av ok" << endl;

		if (dataProcessor != NULL) {
			ni = ((ITuner*)tuner)->getCurrentInterface();
			if (ni != NULL && (ni->getCaps() & DPC_CAN_DECODESTC)) {
				clog << "lssm-ccm::sp using stc hardware!" << endl;
				((IDataProcessor*)dataProcessor)->setSTCProvider(ni);

			} else {
				clog << "lssm-ccm::sp using stc wrapper!" << endl;
				sw = new StcWrapper(ipav);
				((IDataProcessor*)dataProcessor)->setSTCProvider(sw);
			}

			nptProvider = ((IDataProcessor*)dataProcessor)->getNPTProvider();
			if (nptProvider != NULL) {
				pem->setTimeBaseProvider((ITimeBaseProvider*)nptProvider);

			} else {
				clog << "lssm-ccm::sp warning! can't use npt provider" << endl;
			}

			((IDataProcessor*)dataProcessor)->setNptPrinter(nptPrinter);
			if (nptPrinter) {
				if (((IDemuxer*)demuxer)->hasStreamType(STREAM_TYPE_DSMCC_TYPE_C)) {
					((IDemuxer*)demuxer)->setNptPrinter(nptPrinter);
					cout << "TS HAS AN NPT STREAM" << endl;

				} else {
					cout << "NPTPRINTER WARNING!" << endl;
					cout << "TS DOESNT HAVE A STREAM WITH NPT STREAM TYPE" << endl;
				}
				((IDemuxer*)demuxer)->printPat();
			}

			((IDataProcessor*)dataProcessor)->createStreamTypeSectionFilter(
					STREAM_TYPE_DSMCC_TYPE_D); //DSM-CC descriptors

			if (hasOCSupport) {
				((IDataProcessor*)dataProcessor)->createStreamTypeSectionFilter(
						STREAM_TYPE_DSMCC_TYPE_B);

				((IDataProcessor*)dataProcessor)->createStreamTypeSectionFilter(
						STREAM_TYPE_DSMCC_TYPE_C);

				//AIT
				((IDataProcessor*)dataProcessor)->createStreamTypeSectionFilter(
						STREAM_TYPE_PRIVATE_SECTION);

				clog << "lssm ccm::sp OC support enabled" << endl;

			} else if (nptPrinter) {
				((IDataProcessor*)dataProcessor)->createStreamTypeSectionFilter(
						STREAM_TYPE_DSMCC_TYPE_C);
			}
		}

		releaseTunningWindow();

		((IDemuxer*)demuxer)->processDemuxData();

		clog << "lssm ccm::sp all done!" << endl;
#endif //TUNER...
	}
}
}
}
}
}
