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

#include "player/AwesomiumPlayer.h"

#include "player/PlayersComponentSupport.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {

	bool AwesomiumPlayer::init          = false;
	bool AwesomiumPlayer::isWaitingStop = false;
	bool AwesomiumPlayer::running       = false;
	AwesomiumPlayer* AwesomiumPlayer::p = NULL;

	pthread_cond_t AwesomiumPlayer::stopCond;
	pthread_mutex_t AwesomiumPlayer::stopCondMutex;

	AwesomiumPlayer::AwesomiumPlayer(GingaScreenID screenId, string mrl) :
			Player(screenId, mrl) {

		if (!init) {
			init    = true;
			running = false;
			pthread_mutex_init(&stopCondMutex, NULL);
			pthread_cond_init(&stopCond, NULL);
		}

		awesome = AwesomiumHandler::createAwesomium(screenId);
	}

	AwesomiumPlayer::~AwesomiumPlayer() {
		clog << "AwesomiumPlayer::~AwesomiumPlayer " << endl;

		if (p == this) {
			p = NULL;
		}
		AwesomiumHandler::destroyAwesomium(awesome);
	}

	ISurface* AwesomiumPlayer::getSurface() {
		clog << "AwesomiumPlayer::getSurface '" << mrl << "'" << endl;

		surface = AwesomiumHandler::getSurface(awesome);
		return (Player::getSurface());
	}

	void AwesomiumPlayer::setNotifyContentUpdate(bool notify) {
		Player::setNotifyContentUpdate(notify);
	}

	bool AwesomiumPlayer::setOutWindow(GingaWindowID windowId) {
		return (Player::setOutWindow(windowId));
	}

	void AwesomiumPlayer::setBounds(int x, int y, int w, int h) {
		clog << "AwesomiumPlayer::setBounds x = '" << x << "', y = ";
		clog << y << "', w = '" << w << "', h = '" << h << "'.";
		clog << endl;

		AwesomiumHandler::setAwesomiumBounds(awesome, x, y, w, h);
	}

	void AwesomiumPlayer::play() {
		pthread_t tId;
		pthread_attr_t tattr;

		if (p == NULL) {
			p = this;
		}

		if (!running) {
			running = true;
			pthread_attr_init(&tattr);
			pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
			pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM);

			pthread_create(&tId, &tattr, AwesomiumPlayer::_loadUrl, this);
			pthread_detach(tId);
		}

		Player::play();
	}

	void AwesomiumPlayer::stop() {
		AwesomiumHandler::stopUpdate(awesome);
		Player::stop();

		waitStop();
	}

	void AwesomiumPlayer::setPropertyValue(string name, string value) {
		double val;
		int x, y, w, h;
		vector<string>* params;

		//TODO: set transparency, scrollbar, support...
		if (name == "bounds") {
			if (value.find("%") != std::string::npos) {
				return;
			}

			params = split(value, ",");
			if (params->size() != 4) {
				delete params;
				return;
			}

			x = (int)stof((string)((*params)[0]));
			y = (int)stof((string)((*params)[1]));
			w = (int)stof((string)((*params)[2]));
			h = (int)stof((string)((*params)[3]));

			delete params;

			setBounds(x, y, w, h);
			return;

		} else if (name == "size") {
			if (value.find("%") != std::string::npos) {
				return;
			}

			params = split(value, ",");
			if (params->size() != 2) {
				delete params;
				return;
			}

			w = (int)stof((string)((*params)[2]));
			h = (int)stof((string)((*params)[3]));

			delete params;

			setBounds(0, 0, w, h);
			return;
		}

		Player::setPropertyValue(name, value);
	}

	bool AwesomiumPlayer::setKeyHandler(bool isHandler) {
		AwesomiumHandler::setFocus(awesome, isHandler);
		return (isHandler);
	}

	void* AwesomiumPlayer::_loadUrl(void* ptr) {
		/*struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 100000;*/

		while (running) {
			if (p != NULL && p->status == PLAY) {
				AwesomiumHandler::loadUrl(p->awesome, p->mrl);
				p->stopped();
				running = false;

			} else {
				SystemCompat::uSleep(300000);
			}

			clog << "AwesomiumPlayer::_loadUrl" << endl;
		}

		return (NULL);
	}

	void AwesomiumPlayer::waitStop() {
		isWaitingStop = true;
		pthread_mutex_lock(&stopCondMutex);
		pthread_cond_wait(&stopCond, &stopCondMutex);

		isWaitingStop = false;
		pthread_mutex_unlock(&stopCondMutex);
	}

	bool AwesomiumPlayer::stopped() {
		if (isWaitingStop) {
			pthread_cond_signal(&stopCond);
			return true;
		}
		return false;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
		createAwesomiumPlayer(
				GingaScreenID screenId, const char* mrl, bool hasVisual) {

	string strMrl = "";

	if (mrl == NULL) {
		return NULL;
	}

	strMrl.assign(mrl);

	return (new ::br::pucrio::telemidia::ginga::core::player::AwesomiumPlayer(
			screenId, strMrl));
}

extern "C" void destroyAwesomiumPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
