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
#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia;

#include "ncl/layout/DeviceLayout.h"
#include "mb/LocalScreenManager.h"
#if HAVE_MULTIDEVICE
# include "gingancl/FormatterMultiDevice.h"
# include "multidevice/DeviceDomain.h"
# include "gingancl/FormatterPassiveDevice.h"
# include "gingancl/FormatterActiveDevice.h"
#endif
#include "lssm/CommonCoreManager.h"
#include "lssm/PresentationEngineManager.h"

using namespace ::br::pucrio::telemidia::ginga::core::mb;


#include "ncl/layout/IDeviceLayout.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "mb/ILocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#if HAVE_MULTIDEVICE
# include "multidevice/IDeviceDomain.h"
using namespace ::br::pucrio::telemidia::ginga::core::multidevice;
# include "gingancl/IFormatterMultiDevice.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::multidevice;
#endif

#include "lssm/ICommonCoreManager.h"
#include "lssm/IPresentationEngineManager.h"
using namespace ::br::pucrio::telemidia::ginga::lssm;

#include <string>
#include <iostream>
using namespace std;


void printHelp() {
	cout << endl << "Usage: ginga [OPTIONS]... [<--ncl> NCLFILE]" << endl;
	cout << "Example: ginga --ncl test.ncl" << endl;
	cout << endl << "OPTIONS are:" << endl;
	cout << "-h, --help                    Display this information." << endl;
	cout << "    --set-interfaceId         Set the interface that the ";
	cout << "document presentation" << endl;
	cout << "                                shall be started." << endl;
	cout << "-i, --insert-delay <value>    Insert a delay before application ";
	cout << "processing." << endl;
	cout << "-i, --insert-oc-delay <value> Insert a delay before tune main A/V";
	cout << " (to" << endl;
	cout << "                              exclusively process OC elementary ";
	cout << "streams)." << endl;
	cout << "-v, --version                 Display version." << endl;
	cout << "    --enable-log [mode]       Enable log mode:" << endl;
	cout << "                                Turn on verbosity, gather all";
	cout << " output" << endl;
	cout << "                                messages and write them into a";
	cout << " device" << endl;
	cout << "                                according to mode." << endl;
	cout << "                                  [mode] can be:" << endl;
	cout << "                                     'file'   write messages into";
	cout << " a file" << endl;
	cout << "                                              named logFile.txt.";
	cout << endl;
	cout << "                                     'stdout' write messages into";
	cout << " /dev/stdout" << endl;
	cout << "                                     'null'   write messages into";
	cout << " /dev/null" << endl;
	cout << "                                              (default mode).";
	cout << endl;
	cout << "    --context-dir <value>     Defines a new base directory to ";
	cout << " ginga files use it carefully.";
	cout << endl;
	cout << "    --x-offset <value>        Offset of left Ginga display coord.";
	cout << endl;
	cout << "    --y-offset <value>        Offset of top Ginga display coord.";
	cout << endl;
	cout << "    --set-width <value>       Force a width Ginga display value.";
	cout << endl;
	cout << "    --set-height <value>      Force a height Ginga display value.";
	cout << endl;
	cout << "    --device-class <value>    Define the NCL device class for Ginga";
	cout << endl;
	cout << "    --disable-multicast       Uses broadcast instead of multicast for communication with devices";
	cout << endl;
	cout << "    --device-srv-port <port>  Define the service port for Ginga as a secondary device";
	cout << endl;
	cout << "    --enable-automount        Enable trigger for main.ncl";
	cout << " applications received" << endl;
	cout << "                                via a transport protocol." << endl;
	cout << "    --enable-remove-oc-filter Enable processor economies after";
	cout << " first time that" << endl;
	cout << "                                the object carousel is mounted.";
	cout << endl;
	cout << "    --disable-interactivity   Disable NCL presentations" << endl;
	cout << "    --disable-oc              Disable OC filters" << endl;
	cout << "    --disable-mainav          Disable main AV decoders" << endl;
	cout << "    --enable-nptprinter       Ginga becomes nothing, but an NPT ";
	cout << "printer (debug purpose only)";
	cout << endl;
	cout << "    --set-tuner [ni:channel]  Force an interface and a channel to be tuned."<< endl;
	cout << "                              For instance:" << endl;
	cout << "                                 --set-tuner file:/tmp/test.ts"<< endl;
	cout << "                                 --set-tuner sbtvdt:635143"<< endl;
	cout << "                                 --set-tuner ip:224.0.0.1:1234"<< endl;
	cout << "                                             (...)" << endl;
	cout << "    --disable-unload          Disable unload components. Useful ";
	cout << "for debug. " << endl;
	cout << "    --vsystem <vsystem>       Specifies the video backend to use.";
	cout << endl;
	cout << "                              The default is to use DirectFB (dfb";
	cout << ") for Linux " << endl;
	cout << "                              and SDL (sdl) for other OS but you ";
	cout << "can also " << endl;
	cout << "                              run Ginga on Linux with SDL.";
	cout << endl;
	cout << "    --vsubsystem <vsubsystem> Specifies the graphics system to be";
	cout << " used by the " << endl;
	cout << "                              multimedia backend. For instance: ";
	cout << endl;
	cout << "                                 --vsystem dfb --vsubsystem fbdev";
	cout << endl;
	cout << "                                 --vsystem dfb --vsubsystem x11";
	cout << endl;
	cout << "                                 --vsystem dfb --vsubsystem sdl";
	cout << endl;
	cout << "                                 --vsystem sdl --vsubsystem x11";
	cout << endl;
	cout << "                                 --vsystem sdl --vsubsystem cocoa";
	cout << endl;
	cout << "                                             (...)" << endl;
	cout << "    --asystem <asystem>       Specifies the audio backend to use.";
	cout << endl;
	cout << "                              The default is to use xine (xine) ";
	cout << "for Linux " << endl;
	cout << "                              and SDL (sdlffmpeg) for other OS ";
	cout << "but you can also " << endl;
	cout << "                              run sdlffmpeg on Linux.";
	cout << endl;
	cout << "                              For instance:" << endl;
	cout << "                              With dfb vsystem" << endl;
	cout << "                                 --asystem fusionsound (default)";
	cout << endl;
	cout << "                                 --asystem xine";
	cout << endl;
	cout << "                              With sdl vsystem" << endl;
	cout << "                                 --asystem sdlffmpeg (default)";
	cout << endl;
	cout << "    --vmode <width>x<height>  Specifies the Video Window size.";
	cout << endl;
	cout << "    --wid <id>                Specifies an existent Window to be ";
	cout << "used as Ginga device Window.";
	cout << endl;
	cout << "    --poll-stdin              Poll for events using stdin.";
	cout << endl;
	cout << "    --enable-debug-window     Prints ginga windows hierarchy and ";
	cout << "dump their media contents.";
	cout << endl;
	cout << "    --disable-demuxer         Disables Ginga demuxer.";
	cout << endl;
	cout << endl << endl << endl;
}

void printVersion() {
	cout << endl;
	cout << "This is ginga (open source version):  a free " << endl;
	cout << " support for iTV middleware named Ginga." << endl;
	cout << "Ginga binary v" << VERSION << endl;
	cout << "(c) 2002-2015 The TeleMidia PUC-Rio Team." << endl << endl;
}

string updateFileUri(string file) {
	if (!SystemCompat::isAbsolutePath(file)) {
		return SystemCompat::getUserCurrentPath() + file;
	}

	return file;
}

int main(int argc, char *argv[]) {
	ICommonCoreManager* ccm = NULL;
	IPresentationEngineManager* pem = NULL;
	IScreenManager* dm = NULL;
#if HAVE_MULTIDEVICE
	IFormatterMultiDevice* fmd = NULL;
#endif
	GingaScreenID screenId;

	string nclFile      = "", param = "", bgUri = "", cmdFile = "", tSpec = "";
	string interfaceId  = "";

	string contextDir   = "";
	int i, devClass     = 0;
	int xOffset         = 0, yOffset = 0, w = 0, h = 0, maxTransp = 0;
	double delayTime    = 0;
	double ocDelay      = 0;
	int  deviceSrvPort  = 22222;
	bool isRemoteDoc    = false;
	bool removeOCFilter = false;
	bool forceQuit      = true;
	bool enableGfx      = true;
	bool autoMount      = false;
	bool hasInteract    = true;
	bool hasOCSupport   = true;
	bool disableUC      = false;
	bool exitOnEnd      = false;
	bool disableFKeys   = false;
	bool useMulticast   = true;
	bool debugWindow    = false;
	bool nptPrinter     = false;
	short logDest       = SystemCompat::LOG_NULL;

	SystemCompat::initMemCheck();
	SystemCompat::initializeSigpipeHandler();

	for (i = 1; i < argc; i++) {

		if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
			printHelp();
			return 0;

		} else if ((strcmp(argv[i], "-v") == 0) ||
			    (strcmp(argv[i], "--version")) == 0) {

			printVersion();
			return 0;

		} else if ((strcmp(argv[i], "--ncl") == 0) && ((i + 1) < argc)) {
			nclFile.assign(argv[i+1], strlen(argv[i+1]));
			clog << "argv = '" << argv[i+1] << "' nclFile = '";
			clog << nclFile << "'" << endl;

		} else if ((strcmp(argv[i], "--context-dir") == 0) && ((i + 1) < argc)) {
			contextDir.assign(argv[i+1], strlen(argv[i+1]));

		} else if ((strcmp(argv[i], "--enable-log") == 0) && ((i + 1) < argc)) {
			if (strcmp(argv[i+1], "file") == 0) {
				logDest = SystemCompat::LOG_FILE;

			} else if (strcmp(argv[i+1], "stdout") == 0) {
				logDest = SystemCompat::LOG_STDO;
			}

		} else if ((strcmp(argv[i], "--force-quit") == 0) && ((i + 1) < argc)) {
			if (strcmp(argv[i+1], "false") == 0) {
				forceQuit = false;
			}

		} else if ((strcmp(argv[i], "--x-offset") == 0) && ((i + 1) < argc)) {
			if (isNumeric(argv[i+1])) {
				param = argv[i+1];
				xOffset = util::stof(param);
			}

		} else if ((strcmp(argv[i], "--y-offset") == 0) && ((i + 1) < argc)) {
			if (isNumeric(argv[i+1])) {
				param = argv[i+1];
				yOffset = util::stof(param);
			}

		} else if ((strcmp(argv[i], "--set-width") == 0) && ((i + 1) < argc)) {
			if (isNumeric(argv[i+1])) {
				param = argv[i+1];
				w = util::stof(param);
			}

		} else if ((strcmp(argv[i], "--set-height") == 0) && ((i + 1) < argc)) {
			if (isNumeric(argv[i+1])) {
				param = argv[i+1];
				h = util::stof(param);
			}

		} else if ((strcmp(argv[i], "--set-max-transp") == 0) &&
				((i + 1) < argc)) {

			if (isNumeric(argv[i+1])) {
				param = argv[i+1];
				maxTransp = util::stof(param);
			}

		} else if ((strcmp(argv[i], "--set-bg-image") == 0) &&
				((i + 1) < argc)) {

			param = argv[i+1];
			if (fileExists(param)) {
				bgUri = param;
				clog << "main bgUri = '" << bgUri << "'";
				clog << " OK" << endl;

			} else {
				clog << "main Warning: can't set bgUri '" << param << "'";
				clog << " file does not exist" << endl;
			}

		} else if (((strcmp(argv[i], "--device-class") == 0) ||
				(strcmp(argv[i], "--dev-class") == 0)) && ((i + 1) < argc)) {

			if (strcmp(argv[i+1], "1") == 0 ||
					strcmp(argv[i+1], "passive") == 0) {

				devClass = 1;

			} else if (strcmp(argv[i+1], "2") == 0 ||
					strcmp(argv[i+1], "active") == 0) {

				devClass = 2;
			}

		} else if (strcmp(argv[i], "--disable-multicast") == 0) {
			useMulticast = false;

		} else if (strcmp(argv[i], "--device-srv-port") == 0) {
			if (isNumeric(argv[i+1])&&((i + 1) < argc)) {
				deviceSrvPort = util::stof(argv[i+1]);
				}

		} else if (((strcmp(argv[i], "-i") == 0) ||
				(strcmp(argv[i], "--insert-delay") == 0)) &&
				((i + 1) < argc)) {

			if (isNumeric(argv[i+1])) {
				param     = argv[i+1];
				delayTime = util::stof(param);
			}

		} else if (strcmp(argv[i], "--insert-oc-delay") == 0 &&
				((i + 1) < argc)) {

			if (isNumeric(argv[i+1])) {
				param   = argv[i+1];
				ocDelay = util::stof(param);
			}

		} else if (strcmp(argv[i], "--enable-automount") == 0) {
			autoMount = true;

		} else if (strcmp(argv[i], "--enable-remove-oc-filter") == 0) {
			removeOCFilter = true;

		} else if ((strcmp(argv[i], "--enable-cmdfile") == 0 ||
				strcmp(argv[i], "--set-cmdfile") == 0) && ((i + 1) < argc)) {

			cmdFile.assign(argv[i+1], strlen(argv[i+1]));
			clog << "argv = '" << argv[i+1] << "' cmdFile = '";
			clog << cmdFile << "'" << endl;

		} else if (strcmp(argv[i], "--disable-unload") == 0) {
			disableUC = true;

		} else if (strcmp(argv[i], "--disable-interactivity") == 0) {
			hasInteract = false;

		} else if (strcmp(argv[i], "--disable-oc") == 0) {
			hasOCSupport = false;

		} else if (strcmp(argv[i], "--enable-nptprinter") == 0) {
			nptPrinter   = true;
			hasOCSupport = false;
			hasInteract  = false;

		} else if ((strcmp(argv[i], "--set-tuner") == 0) && ((i + 1) < argc)) {
			tSpec.assign(argv[i+1], strlen(argv[i+1]));

		} else if (strcmp(argv[i], "--set-interfaceId") == 0) {
			interfaceId.assign(argv[i+1], strlen(argv[i+1]));

		} else if (strcmp(argv[i], "--set-exitonend") == 0) {
			exitOnEnd = true;

		} else if (strcmp(argv[i], "--disable-fkeys") == 0) {
			disableFKeys = true;

		} else if (strcmp(argv[i], "--enable-debug-window") == 0) {
			debugWindow = true;
		}
	}

	if (devClass == 0) {
		SystemCompat::setLogTo(logDest, "_base");

	} else if (devClass == 1) {
		SystemCompat::setLogTo(logDest, "_passive");

	} else if (devClass == 2) {
		SystemCompat::setLogTo(logDest, "_active");
	}

	if (contextDir != "") {
		SystemCompat::setGingaContextPrefix(contextDir);
	}

	if (delayTime > 0) {
		SystemCompat::uSleep(delayTime);
	}

	initTimeStamp();
	if (nclFile != "") {
		nclFile = SystemCompat::updatePath(updateFileUri(nclFile));

		if (argc > 1 &&
				nclFile.substr(0, 1) != SystemCompat::getIUriD() &&
				nclFile.substr(1, 2) != ":" + SystemCompat::getIUriD()) {

			clog << "ginga main() remote NCLFILE" << endl;
			isRemoteDoc = true;
		}
	}

	clog << "ginga main()";
	clog << "COMMAND = '" << argv[0] << "' ";
	clog << "NCLFILE = '" << nclFile.c_str() << "'";
	clog << endl;

	dm  = ScreenManagerFactory::getInstance();

	screenId = dm->createScreen(argc, argv);
	if (screenId < 0) {
		clog << "ginga main() Warning! Can't create Ginga screen";
		clog << endl;
		exit(-1);
	}

	if (devClass == 1) {

#if HAVE_MULTIDEVICE
		fmd = new FormatterPassiveDevice(
				screenId, NULL, xOffset, yOffset, w, h, useMulticast, deviceSrvPort);
#endif
#if HAVE_MULTIDEVICE
		if (bgUri != "") {
			fmd->setBackgroundImage(bgUri);
		}
		while(true) SystemCompat::uSleep(1000000); //getchar();
#endif

	} else if (devClass == 2) {
#if HAVE_MULTIDEVICE
		fmd = new FormatterActiveDevice(screenId, NULL, xOffset, yOffset, w, h, useMulticast, deviceSrvPort);
#endif

#if HAVE_MULTIDEVICE
		if (bgUri != "") {
			fmd->setBackgroundImage(bgUri);
		}
#endif

		getchar();

	} else {

		if (nclFile == "") {
			enableGfx = false;
		}

		pem = new PresentationEngineManager(
				devClass, xOffset, yOffset, w, h, enableGfx, useMulticast, screenId);

		if (pem == NULL) {
			clog << "ginga main() Warning! Can't create Presentation Engine";
			clog << endl;
			return -2;
		}

		pem->setEmbedApp(false);

		if (bgUri != "") {
			clog << endl << endl;
			clog << "main '" << bgUri << "'" << endl;
			clog << endl << endl;
			pem->setBackgroundImage(bgUri);
		}

		if (cmdFile != "") {
			cmdFile = updateFileUri(cmdFile);
			pem->setCmdFile(cmdFile);
		}

		pem->setExitOnEnd(exitOnEnd);
		pem->setDisableFKeys(disableFKeys);
		pem->setDebugWindow(debugWindow);

		pem->setInteractivityInfo(hasInteract);
		if (nclFile == "") {
			pem->setIsLocalNcl(false, NULL);
			pem->autoMountOC(autoMount);

			ccm = new CommonCoreManager();
			ccm->addPEM(pem, screenId);
			ccm->enableNPTPrinter(nptPrinter);
			ccm->setInteractivityInfo(hasOCSupport);
			ccm->removeOCFilterAfterMount(removeOCFilter);
			ccm->setOCDelay(ocDelay);
			ccm->setTunerSpec(tSpec);
			if (tSpec.find("scan") == std::string::npos) {
				ccm->startPresentation();

			} else {
				ccm->tune();
			}

		} else if (fileExists(nclFile) || isRemoteDoc) {
			clog << "ginga main() NCLFILE = " << nclFile.c_str() << endl;
			pem->setIsLocalNcl(forceQuit, NULL);
			if (pem->openNclFile(nclFile)) {
				pem->startPresentation(nclFile, interfaceId);
				pem->waitUnlockCondition();
			}

		} else {
			clog << "ginga main() nothing to do..." << endl;
		}

		if (pem != NULL) {
			clog << "ginga main() calling delete pem" << endl;
			delete pem;
			clog << "ginga main() delete pem all done" << endl;
		}

		if (ccm != NULL) {
			clog << "ginga main() calling delete ccm" << endl;
			delete ccm;
			clog << "ginga main() calling delete ccm all done" << endl;
		}
	}

	clog << "Ginga v" << VERSION << " all done!" << endl;
	cout << "Ginga v" << VERSION << " all done!" << endl;
	SystemCompat::uSleep(500000);

	SystemCompat::finishMemCheck();

	return 0;
}
