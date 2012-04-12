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

#include "player/SmilPlayer.h"

#include "player/PlayersComponentSupport.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	SmilPlayer::SmilPlayer(GingaScreenID screenId, string mrl) :
			Player(screenId, mrl) {

		clog << "SmilPlayer::SmilPlayer(" << this << ") '";
		clog << mrl << "'" << endl;

		initPrefs();

		this->x        = 0;
		this->y        = 0;
		this->w        = 0;
		this->h        = 0;
		this->im       = NULL;
		ambulantPlayer = NULL;
		mainLoop       = NULL;
	}

	SmilPlayer::~SmilPlayer() {
		clog << "SmilPlayer::~SmilPlayer " << endl;

		if (im != NULL) {
			im->removeInputEventListener(this);
			im = NULL;
		}

		if (ambulantPlayer != NULL) {
			if (ambulantPlayer->is_playing() || ambulantPlayer->is_pausing()) {
				ambulantPlayer->stop();
				while (!ambulantPlayer->is_done()) {
					sleep(3);
				}
			}
		}

		if (mainLoop != NULL) {
			delete mainLoop;
			mainLoop = NULL;

			ambulantPlayer = NULL; // deleted by mainLoop
		}
	}

	/*
	 * Initializing ambulant preferences, including setting up for loading
	 * Ambulant plugins (which are needed for SMIL State.
	 */
	void SmilPlayer::initPrefs() {
		ambulant::common::preferences* prefs;

		ambulant::lib::logger::get_logger()->set_level(
				ambulant::lib::logger::LEVEL_SHOW);

		prefs = ambulant::common::preferences::get_preferences();

		prefs->m_prefer_ffmpeg = true;
		prefs->m_use_plugins   = true;
		prefs->m_log_level     = ambulant::lib::logger::LEVEL_DEBUG;
		prefs->m_parser_id     = "expat";
	}

	void SmilPlayer::initGui() {
		cout << "SmilPlayer::initGui" << endl;

		GingaWindowID uWin = dm->createUnderlyingSubWindow(
				myScreen, x, y, w, h, 0);

		if (uWin == NULL) {
			clog << "SmilPlayer::initGui Warning! Can't create ";
			clog << "underlying window! With the following coords: ";
			clog << " '" << x << "," << y << "," << w << "," << h << "'";
			clog << endl;

			return;
		}

		cout << "SmilPlayer::initGui underlying window id = '" << uWin << "'";
		cout << " With the following coords: ";
		cout << " '" << x << "," << y << "," << w << "," << h << "'";
		cout << endl;

		long long ll_winid = reinterpret_cast<long long>(uWin);
		int i_winid        = static_cast<int>(ll_winid);

		gtk_init(0, NULL);

		gtkwidget = GTK_WIDGET(gtk_plug_new((GdkNativeWindow)uWin));

		gtk_widget_set_visible(gtkwidget, true);
		gtk_widget_map(gtkwidget);

		gtk_gui* m_gui = new gtk_gui((char*) gtkwidget, mrl.c_str());
		mainLoop       = new gtk_mainloop(m_gui);

		ambulantPlayer = mainLoop->get_player();
		cout << "SmilPlayer::initGui all done" << endl;
	}

	ISurface* SmilPlayer::getSurface() {
		void* s;

		clog << "SmilPlayer::getSurface '" << mrl << "'" << endl;

		if (this->surface == NULL) {

		}

		return Player::getSurface();
	}

	void SmilPlayer::setNotifyContentUpdate(bool notify) {
		clog << "SmilPlayer::setNotifyContentUpdate '" << mrl << "'" << endl;

		Player::setNotifyContentUpdate(notify);
	}

	bool SmilPlayer::setOutWindow(GingaWindowID windowId) {
		clog << "SmilPlayer::setOutWindow '" << mrl << "'" << endl;
		return Player::setOutWindow(windowId);
	}

	void SmilPlayer::setBounds(int x, int y, int w, int h) {
		bool init = false;

		clog << "SmilPlayer::setBounds '" << x << ", ";
		clog << y << "', " << w << "', " << h << "'.";
		clog << endl;

		if ((this->w == 0 && w != 0) || (this->h == 0 && h != 0)) {
			init = true;
		}

		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;

		if (init) {
			initGui();
		}
	}

	void SmilPlayer::play() {
		clog << "SmilPlayer::play(" << mrl << ")" << endl;

		if (ambulantPlayer != NULL) {
			ambulantPlayer->start();
			gtk_widget_show_all(gtkwidget);
			gtk_widget_realize(gtkwidget);
		}

		Player::play();
	}

	void SmilPlayer::stop() {
		clog << "SmilPlayer::stop '" << mrl << "'" << endl;

		if (ambulantPlayer != NULL) {
			ambulantPlayer->stop();
		}
		Player::stop();
	}

	void SmilPlayer::pause() {
		clog << "SmilPlayer::pause '" << mrl << "'" << endl;

		if (ambulantPlayer != NULL) {
			ambulantPlayer->pause();
		}
		Player::pause();
	}

	void SmilPlayer::resume() {
		clog << "SmilPlayer::resume '" << mrl << "'" << endl;

		if (ambulantPlayer != NULL) {
			ambulantPlayer->resume();
		}
		Player::resume();
	}

	void SmilPlayer::setPropertyValue(string name, string value) {
		clog << "SmilPlayer::setProperty '" << name << "' value '";
		clog << value << "'" << endl;

		//TODO: set scrollbar, support...
		if (name == "transparency") {
			double val;

			val = stof(value);
			if (val >= 0.0 && val <= 1.0) {
				val = 1 - val;

			}

		} else if (name == "bounds") {
			int x, y, w, h;
			vector<string>* params;

			if (value.find("%") != std::string::npos) {
				return;
			}

			params = split(value, ",");
			if (params->size() != 4) {
				delete params;
				return;
			}

			x = (int)stof((*params)[0]);
			y = (int)stof((*params)[1]);
			w = (int)stof((*params)[2]);
			h = (int)stof((*params)[3]);

			delete params;

			setBounds(x, y, w, h);
			return;
		}

		Player::setPropertyValue(name, value);
	}

	bool SmilPlayer::setKeyHandler(bool isHandler) {
		clog << "SmilPlayer::setKeyHandler '" << mrl << "': isHandler = '";
		clog << isHandler << "', notifyContentUpdate = '";
		clog << notifyContentUpdate << "'" << endl;

		if (isHandler && notifyContentUpdate) {
			im->addInputEventListener(this, NULL);

		} else {
			im->removeInputEventListener(this);
		}

		return isHandler;
	}

	bool SmilPlayer::userEventReceived(IInputEvent* userEvent) {
		/*clog << "SmilPlayer::userEventReceived(" << mrl << "'): ";
		if (userEvent != NULL) {
			clog << userEvent->getKeyCode() << "'";
		}
		clog << endl;*/

		return true;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
		createSmilPlayer(
				GingaScreenID screenId, const char* mrl, bool hasVisual) {

	return new ::br::pucrio::telemidia::ginga::core::player::SmilPlayer(
			screenId, (string)mrl);
}

extern "C" void destroySmilPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
