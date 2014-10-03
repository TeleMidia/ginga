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

This program is distributed in the hope that it WILL be useful, but WITHOUT ANY
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

#include "config.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "player/IPlayerListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "mb/ILocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "gingalssm/IPresentationEngineManager.h"
using namespace ::br::pucrio::telemidia::ginga::lssm;

#if HAVE_COMPONENTS
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#endif

bool verbose = false;

//GTK
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

//X11
#include <X11/Xlib.h>    /* fundamentals X datas structures */
#include <X11/Xutil.h>   /* datas definitions for various functions */
#include <X11/keysym.h>  /* for a perfect use of keyboard events */

Window createXWindow(
		Display* d, int screen, Window parent, int x, int y, int w, int h) {

	int blackColor = BlackPixel(d, screen);
	Window win;

	win = XCreateSimpleWindow(
    		d,           /* display */
    		parent,      /* parent */
    		x,           /* x */
    		y,           /* y */
    		w,           /* w */
    		h,           /* h */
    		0,           /* border_width */
    		blackColor,  /* border_color */
    		blackColor); /* background_color */

	XMapWindow(d, win);
    XClearWindow(d, win);
    XFlush(d);

    return win;
}

string updateFileUri(string file) {
	if (!SystemCompat::isAbsolutePath(file)) {
		return SystemCompat::getUserCurrentPath() + file;
	}

	return file;
}

class EmbeddingGinga : public IPlayerListener {
	private:
		IPresentationEngineManager* pem1;
		IPresentationEngineManager* pem2;
		string nclFile1;
		string nclFile2;
		string interfaceId1;
		string interfaceId2;
		string value1;
		string value2;

public:
	EmbeddingGinga(
			IPresentationEngineManager* pem1, IPresentationEngineManager* pem2,
			string f1, string f2,
			string id1, string id2) {

		this->pem1         = pem1;
		this->pem2         = pem2;
		this->nclFile1     = f1;
		this->nclFile2     = f2;
		this->value1       = "";
		this->value2       = "";

		this->interfaceId1 = id1;
		this->interfaceId2 = id2;
	}

	void resetValues() {
		this->value1       = "";
		this->value2       = "";
	}

	void updateStatus(
			short code, string parameter, short type, string value) {

		string oldValue;

		if (code == IPlayer::PL_NOTIFY_STOP &&
				type == IPlayer::TYPE_ATTRIBUTION) {

			if (pem2 != NULL) {
				if (parameter == interfaceId1) {
					oldValue = pem2->getPropertyValue(nclFile2, interfaceId2);

					cout << "EmbeddingGinga::updateStatus: ";
					cout << "setting port '" << interfaceId2 << "' of '";
					cout << nclFile2 << "' with value '";
					cout << value << "' (current value is '";
					cout << oldValue << "')";
					cout << endl;

					if (value1 != oldValue || value2 == "") {
						value2 = value;
						pem2->setPropertyValue(nclFile2, interfaceId2, value);
					}

				} else if (parameter == interfaceId2) {
					oldValue = pem1->getPropertyValue(nclFile1, interfaceId1);

					cout << "EmbeddingGinga::updateStatus: ";
					cout << "setting port '" << interfaceId1 << "' of '";
					cout << nclFile1 << "' with value '";
					cout << value << "' (current value is '";
					cout << oldValue << "')";
					cout << endl;

					value1 = value;
					pem1->setPropertyValue(nclFile1, interfaceId1, value);
				}

			} else {
				cout << "EmbeddingGinga::updateStatus: ";
				cout << "received end attribution of '" << interfaceId1;
				cout << "' from file '";
				cout << nclFile1 << "' with value '";
				cout << value << "'";
				cout << endl;
			}
		}

		if (verbose) {
			cout << "EmbeddingGinga::updateStatus: port '" << parameter << "'";

			if (type == IPlayer::TYPE_PRESENTATION) {
				cout << " is mapped into a presentation event that has just ";

			} else if (type == IPlayer::TYPE_ATTRIBUTION) {
				cout << " is mapped into an attribution event with ";
				cout << "current value = '" << value << "' that has just ";
			}

			switch(code) {
				case IPlayer::PL_NOTIFY_START:
					cout << "started!" << endl;
					break;

				case IPlayer::PL_NOTIFY_PAUSE:
					cout << "paused!" << endl;
					break;

				case IPlayer::PL_NOTIFY_RESUME:
					cout << "resumed!" << endl;
					break;

				case IPlayer::PL_NOTIFY_STOP:
					cout << "stopped!" << endl;
					break;

				case IPlayer::PL_NOTIFY_ABORT:
					cout << "aborted!" << endl;
					break;

				case IPlayer::PL_NOTIFY_NCLEDIT:
					cout << "edited!" << endl;
					break;

				default:
					cout << "updated for other listeners" << endl;
					break;
			}
		}
	}
};

void printHelp() {
	cout << "Help:";
	cout << "At least one NCL document is necessary." << endl;
	cout << "Here are the options: " << endl;
	cout << "--ncl <nclfile>                 This is the NCL document" << endl;
	cout << "--ncl2 <nclfile2>               This is the 2nd NCL document then";
	cout << "                                both can communicate!" << endl;
	cout << "--set-interface <interfaceId1>   Port id that is mapped to a ";
	cout << "property of nclfile" << endl;
	cout << "--set-interface2 <interfaceId2> Port id that is mapped to a ";
	cout << "property of nclfile2" << endl;
	cout << "--verbose                       Shows all testEmbed messages";
	cout << endl;
	exit(0);
}

IScreenManager* dm = NULL;
bool running = true;

void* callMain(void* ptr) {
	gtk_main();

	return NULL;
}

GingaScreenID screen1;
GingaScreenID screen2;

void rendererUpdate() {
	while (running) {
		dm->refreshScreen(screen1);
		dm->refreshScreen(screen2);
		SystemCompat::uSleep(1000000/25);
	}
}

int main(int argc, char** argv, char** envp) {
	EmbeddingGinga* embedding;
	IPresentationEngineManager* pem1 = NULL;
	IPresentationEngineManager* pem2 = NULL;
	string strParent;
	string strParent2;

	set<string>* portIds = NULL;
	set<string>::iterator it;

	int devClass   = 0;
	bool enableGfx = false;
	bool disableUC = false;
	int i;

	string nclFile1     = "";
	string interfaceId1 = "";

	string nclFile2     = "";
	string interfaceId2 = "";

	SystemCompat::setLogTo(SystemCompat::LOG_NULL);
	initTimeStamp();

	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--ncl") == 0) && ((i + 1) < argc)) {
			nclFile1.assign(argv[i+1], strlen(argv[i+1]));

		} else if (strcmp(argv[i], "--disable-unload") == 0) {
			disableUC = true;

		} else if (strcmp(argv[i], "--help") == 0 ||
				strcmp(argv[i], "-h") == 0) {

			printHelp();
			exit(0);

		} else if ((strcmp(argv[i], "--enable-log") == 0) && ((i + 1) < argc)) {
			if (strcmp(argv[i + 1], "stdout") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_STDO);

			} else if (strcmp(argv[i + 1], "file") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_FILE);
			}

		} else if ((strcmp(argv[i], "--set-interface") == 0) &&
				((i + 1) < argc)) {

			interfaceId1.assign(argv[i + 1]);

		} else if ((strcmp(argv[i], "--set-interface2") == 0) &&
				((i + 1) < argc)) {

			interfaceId2.assign(argv[i + 1]);

		} else if ((strcmp(argv[i], "--ncl2") == 0) && ((i + 1) < argc)) {
			nclFile2.assign(argv[i+1], strlen(argv[i+1]));

		} else if (strcmp(argv[i], "--help") == 0) {
			verbose = true;
		}
	}

	if (nclFile1 == "") {
		nclFile1 = "1.ncl";
	}

	if (interfaceId1 == "") {
		interfaceId1 = "doc1p3";
	}

	if (nclFile2 == "") {
		nclFile2 = "2.ncl";
	}

	if (interfaceId1 == "") {
		interfaceId1 = "doc2p1";
	}

	//GTK Structures
	GtkWidget* gtkWidget;
	GdkWindow* gdkWindow;

	gtk_init(NULL, NULL);

	gtkWidget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(gtkWidget, 640, 480);
	gtk_widget_show(gtkWidget);

	pthread_t id;
	pthread_create(&id, NULL, callMain, NULL);

	/*
	 * XXX: Attention!
	 * avoiding bad window error:
	 *   this occurs when gtk is not ready and we try to get a gtk window from
	 *   a gtk widget. To be sure that gtk is ready, we can sleep 1 second,
	 *   then the gtk_main can be performed by callMain thread function.
	 */
	SystemCompat::uSleep(2000000);
	gdkWindow = gtk_widget_get_window(gtkWidget);

	//X11 Structures
	Display* xDisplay;
	int xScreen;
	Window parentXId;

	xDisplay = XOpenDisplay(NULL);
	if (xDisplay == NULL) {
	    cout << "gingalssm-cpp testEmbedGtk (" << __LINE__ << "):";
	    cout << "Can't open display!" << endl;
		return (1);
	}

    xScreen   = DefaultScreen(xDisplay);
    parentXId = GDK_DRAWABLE_XID(gdkWindow);

#if HAVE_COMPONENTS
	IComponentManager* cm = IComponentManager::getCMInstance();
	cm->setUnloadComponents(!disableUC);

	dm = ((LocalScreenManagerCreator*)(cm->getObject("LocalScreenManager")))();

	char* fakeArgv1[5];
	char* fakeArgv2[5];

	/********
	 * NCL1 *
	 ********/
	/*
	 * Preparing parent data:
	 *     <display>,<parentId>,<childX>,<childY>,<childW>,<childH>
	 */
	strParent = "";

	strParent.assign(XDisplayName((char*)xDisplay));

	strParent    = strParent +
			"," + ultostr((unsigned long)parentXId) + ",0,0,320,480";

	fakeArgv1[0] = (char*)"testEmbedGtk";
	fakeArgv1[1] = (char*)"--vsystem";
	fakeArgv1[2] = (char*)"sdl";
	fakeArgv1[3] = (char*)"--parent";
	fakeArgv1[4] = (char*)strParent.c_str();
	fakeArgv1[5] = (char*)"--external-renderer";

	screen1 = dm->createScreen(6, fakeArgv1);

	pem1    = ((PEMCreator*)(cm->getObject("PresentationEngineManager")))(
			devClass, 0, 0, 0, 0, enableGfx, false, screen1);

	/********
	 * NCL2 *
	 ********/
	/*
	 * Preparing parent data:
	 *     <display>,<parentId>,<childX>,<childY>,<childW>,<childH>
	 */
	strParent2 = "";

	strParent2.assign(XDisplayName((char*)xDisplay));

	strParent2   = strParent2 + "," + ultostr(parentXId) + ",320,0,320,480";

	fakeArgv2[0] = (char*)"testEmbedGtk";
	fakeArgv2[1] = (char*)"--vsystem";
	fakeArgv2[2] = (char*)"sdl";
	fakeArgv2[3] = (char*)"--parent";
	fakeArgv2[4] = (char*)strParent2.c_str();
	fakeArgv2[5] = (char*)"--external-renderer";

	screen2 = dm->createScreen(6, fakeArgv2);

	pem2    = ((PEMCreator*)(cm->getObject("PresentationEngineManager")))(
			devClass, 0, 0, 0, 0, enableGfx, false, screen2);

#else
	cout << "ginga-lssm test works only when component manager support is ";
	cout << "enabled" << endl;
	exit(0);
#endif

	nclFile1 = updateFileUri(nclFile1);
	nclFile2 = updateFileUri(nclFile2);

	embedding = new EmbeddingGinga(
			pem1, pem2, nclFile1, nclFile2, interfaceId1, interfaceId2);

	pem1->openNclFile(nclFile1);
	pem1->addPlayerListener(nclFile1, embedding);

	pem2->openNclFile(nclFile2);
	pem2->addPlayerListener(nclFile2, embedding);

	pem1->startPresentation(nclFile1, "");
	pem2->startPresentation(nclFile2, "");

	cout << "ginga-lssm test calling renderer update!" << endl;
	rendererUpdate();

	delete pem1;

	delete dm;

#if HAVE_COMPONENTS
	delete cm;
#endif

	delete embedding;

	cout << "ginga-lssm test all done!" << endl;
	cout << "Please, press enter to exit!" << endl;
	getchar();

	return 0;
}
