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
	ShowButton::ShowButton() : Thread() {
		status         = NONE;
		previousStatus = NONE;
		win            = NULL;
	}

	ShowButton* ShowButton::_instance = NULL;

	ShowButton* ShowButton::getInstance() {
		if (ShowButton::_instance == NULL) {
			ShowButton::_instance = new ShowButton();
		}
		return ShowButton::_instance;
	}

	void ShowButton::initializeWindow() {
		int x = 0, y, w, h;
		ILocalDeviceManager* dm = NULL;
#if HAVE_COMPSUPPORT
		dm = ((LocalDeviceManagerCreator*)(
				cm->getObject("LocalDeviceManager")))();
#else
		dm = LocalDeviceManager::getInstance();
#endif

		if (dm != NULL) {
			x = (int)(dm->getDeviceWidth() - 70);
		}

		y = 10;
		w = 60;
		h = 60;

#if HAVE_COMPSUPPORT
		win = ((WindowCreator*)(cm->getObject("Window")))(-1, x, y, w, h);
#else
#ifndef _WIN32
		win = new DFBWindow(x, y, w, h);
#else
		win = new DXWindow(x, y, w, h);
#endif
#endif
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

		surface = ImagePlayer::renderImage(mrl);
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
		switch (status) {
			case PAUSE:
				cout << "ShowButton::run PAUSE" << endl;
#ifdef _WIN32		
				render(getUserDocAndSetPath().append("\\resource\\img\\button\\pauseButton.png"));
#else
				render("/usr/local/etc/ginga/files/img/button/pauseButton.png");
#endif
				break;

			case STOP:
				cout << "ShowButton::run STOP" << endl;
				if (previousStatus == PAUSE) {
					release();
				}

#ifdef _WIN32		
				render(getUserDocAndSetPath().append("\\resource\\img\\button\\stopButton.png"));
#else
				render("/usr/local/etc/ginga/files/img/button/stopButton.png");
#endif
//				Thread::usleep(1000000);
				release();
				break;

			case PLAY:
				cout << "ShowButton::run PLAY" << endl;
				if (previousStatus == PAUSE) {
					release();
				}

#ifdef _WIN32		
				render(getUserDocAndSetPath().append("\\resource\\img\\button\\playButton.png"));
#else
				render("/usr/local/etc/ginga/files/img/button/playButton.png");
#endif
//				Thread::usleep(1000000);
				release();
				break;

			default:
				cout << "ShowButton::run DEFAULT" << endl;
				break;
		}
		//unlock();
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IShowButton*
		createShowButton() {

	br::pucrio::telemidia::ginga::core::player::ShowButton* sb;
	sb = br::pucrio::telemidia::ginga::core::player::ShowButton::getInstance();
	return sb;
}

extern "C" void destroyShowButton(
		::br::pucrio::telemidia::ginga::core::player::IShowButton* sb) {

	delete sb;
}
