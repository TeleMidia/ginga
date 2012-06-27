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

#include "player/ShowButton.h"

#include "player/PlayersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
#if HAVE_COMPSUPPORT
	static ILocalScreenManager* dm = ((LocalScreenManagerCreator*)(
			cm->getObject("LocalScreenManager")))();
#else
	static ILocalScreenManager* dm = LocalScreenManager::getInstance();
#endif

	ShowButton::ShowButton(GingaScreenID screenId) : Thread() {
		myScreen       = screenId;
		status         = NONE;
		previousStatus = NONE;
		win            = NULL;
		running        = false;
	}

	ShowButton::~ShowButton() {
		isDeleting = true;

		while (running) {
			SystemCompat::uSleep(50000);
		}
	}

	void ShowButton::initializeWindow() {
		int x = 0, y, w, h;

		if (dm != NULL) {
			x = (int)(dm->getDeviceWidth(myScreen) - 70);
		}

		y = 10;
		w = 60;
		h = 60;

		win = dm->createWindow(myScreen, x, y, w, h, 4.0);
		win->setCaps(win->getCap("ALPHACHANNEL"));
		win->draw();
	}

	void ShowButton::stop() {
		//lock();
		previousStatus = status;
		status = STOP;
		//Thread::start();
		//unlock();
		run();
	}

	void ShowButton::pause() {
		//lock();
		previousStatus = status;
		if (status != PAUSE) {
			status = PAUSE;

		} else {
			status = PLAY;
		}

		run();
		//Thread::start();
		//unlock();
	}

	void ShowButton::resume() {
		pause();
	}

	void ShowButton::release() {
		lock();
		if (win != NULL) {
			win->hide();
			delete win;
			win = NULL;
		}
		unlock();
	}

	void ShowButton::render(string mrl) {
		ISurface* surface;

		surface = dm->createRenderedSurfaceFromImageFile(myScreen, mrl.c_str());

		lock();
		if (win == NULL) {
			initializeWindow();
		}

		if (surface->setParent((void*)win)) {
			win->renderFrom(surface);
		}
		win->show();
		win->raiseToTop();
		unlock();
		delete surface;
		surface = NULL;
	}

	void ShowButton::run() {
		//lock();
		running = true;

		if (!isDeleting) {
			switch (status) {
				case PAUSE:
					clog << "ShowButton::run PAUSE" << endl;
					render(SystemCompat::appendGingaFilesPrefix(
							"img/button/pauseButton.png"));

					break;

				case STOP:
					clog << "ShowButton::run STOP" << endl;
					if (previousStatus == PAUSE) {
						release();
					}

					render(SystemCompat::appendGingaFilesPrefix(
							"img/button/stopButton.png"));

					SystemCompat::uSleep(1000000);
					release();
					break;

				case PLAY:
					clog << "ShowButton::run PLAY" << endl;
					if (previousStatus == PAUSE) {
						release();
					}

					render(SystemCompat::appendGingaFilesPrefix(
							"img/button/playButton.png"));

					SystemCompat::uSleep(1000000);
					release();
					break;

				default:
					clog << "ShowButton::run DEFAULT" << endl;
					break;
			}
		}

		running = false;
		//unlock();
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IShowButton*
		createShowButton(GingaScreenID screenId) {

	br::pucrio::telemidia::ginga::core::player::ShowButton* sb;
	sb = new br::pucrio::telemidia::ginga::core::player::ShowButton(screenId);

	return sb;
}

extern "C" void destroyShowButton(
		::br::pucrio::telemidia::ginga::core::player::IShowButton* sb) {

	delete sb;
}
