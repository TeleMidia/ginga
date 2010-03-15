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

#include "../../include/ChannelPlayer.h"

#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	ChannelPlayer::ChannelPlayer(bool hasVisual) : Player("") {
		this->objectMap = NULL;
		this->selectedPlayer = NULL;
		this->hasVisual = hasVisual;
		this->hasParent = false;
	}

	ChannelPlayer::~ChannelPlayer() {
		map<string, IPlayer*>::iterator players;
		IPlayer* objectPlayer;

		if (objectMap != NULL) {
			players = objectMap->begin();
			while (players != objectMap->end()) {
				objectPlayer = players->second;
				delete objectPlayer;
				objectPlayer = NULL;
				++players;
			}

			objectMap->clear();
			delete objectMap;
			objectMap = NULL;
		}
	}

	void ChannelPlayer::setPlayerMap(map<string, IPlayer*>* objs) {
		objectMap = objs;
	}

	IPlayer* ChannelPlayer::getSelectedPlayer() {
		if (selectedPlayer == NULL) {
			wclog << "ChannelPlayer::getSelectedPlayerAdd == NULL";
			wclog << endl;
			return NULL;
		}
		return selectedPlayer;
	}

	map<string, IPlayer*>* ChannelPlayer::getPlayerMap() {
		if (objectMap == NULL || objectMap->empty()) {
			return NULL;
		}

		return objectMap;
	}

	IPlayer* ChannelPlayer::getPlayer(string objectId) {
		IPlayer* newSelected;

		if (objectMap->count(objectId) != 0) {
			newSelected = (*objectMap)[objectId];
			if (newSelected != NULL && newSelected != selectedPlayer) {
				return newSelected;
			}
		}

		return NULL;
	}

	void ChannelPlayer::select(IPlayer* selObject) {
		if (selectedPlayer != NULL) {
			selectedPlayer->removeListener(this);
		}

		this->selectedPlayer = selObject;
		if (selectedPlayer != NULL) {
			selectedPlayer->addListener(this);
		}
	}

	double ChannelPlayer::getMediaTime() {
		return selectedPlayer->getMediaTime();
	}

	void ChannelPlayer::setSurfacesParent(void* parent) {
		map<string, IPlayer*>::iterator players;
		IPlayer* avPlayer;

		players = objectMap->begin();
		while (players != objectMap->end()) {
			avPlayer = players->second;
			if (avPlayer->getSurface() != NULL &&
					avPlayer->getSurface()->getParent() != parent) {

				avPlayer->getSurface()->setParent(parent);
			}
			++players;
		}
		hasParent = true;
	}

	ISurface* ChannelPlayer::getSurface() {
		if (selectedPlayer != NULL) {
			return selectedPlayer->getSurface();
		}
		return NULL;
	}

	void ChannelPlayer::play() {
		if (selectedPlayer != NULL) {
			if (!hasParent && selectedPlayer->getSurface() != NULL &&
					selectedPlayer->getSurface()->getParent() != NULL) {

				setSurfacesParent(selectedPlayer->getSurface()->getParent());
				selectedPlayer->play();

			} else if (!hasVisual) {
				selectedPlayer->play();
			}
		}
		Player::play();
	}

	void ChannelPlayer::pause() {
		if (selectedPlayer != NULL) {
			selectedPlayer->pause();
		}
		Player::pause();
	}

	void ChannelPlayer::resume() {
		if (selectedPlayer != NULL) {
			selectedPlayer->resume();
		}
		Player::resume();
	}

	void ChannelPlayer::stop() {
		IPlayer* objectPlayer;
		map<string, IPlayer*>::iterator players;

		players = objectMap->begin();
		while (players != objectMap->end()) {
			objectPlayer = players->second;
			objectPlayer->stop();
			++players;
		}
		hasParent = false;
		Player::stop();
	}

	void ChannelPlayer::updateStatus(
			short code, string parameter, short type) {

		notifyListeners(code, parameter, type);
	}

	void ChannelPlayer::setPropertyValue(
			string name, string value, double duration, double by) {

		IPlayer* objectPlayer;
		map<string, IPlayer*>::iterator players;

		//TODO: set brightness, ...
		if (name == "soundLevel") {
			players = objectMap->begin();
			while (players != objectMap->end()) {
				objectPlayer = players->second;
				objectPlayer->setPropertyValue(name, value, duration, by);
				++players;
			}
		}

		Player::setPropertyValue(name, value, duration, by);
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::
		IPlayer* createChannelPlayer(const char* mrl, bool hasVisual) {

	return (new ::br::pucrio::telemidia::ginga::core::player::ChannelPlayer(
			hasVisual));
}

extern "C" void destroyChannelPlayer(::br::pucrio::telemidia::ginga::core::
		player::IPlayer* p) {

	//TODO: static release method
	return delete p;
}
