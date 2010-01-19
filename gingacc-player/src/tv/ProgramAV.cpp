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

#include "../../include/ProgramAV.h"
#include "../../include/AVPlayer.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	ProgramAV::ProgramAV() {
		vp               = new map<int, IPlayer*>;
		currentPlayer    = NULL;
		fullScreenBounds = "";
	}

	ProgramAV::~ProgramAV() {
		map<int, IPlayer*>::iterator i;

		if (vp != NULL) {
			i = vp->begin();
			while (i != vp->end()) {
				i->second->stop();
				if (i->second == currentPlayer) {
					currentPlayer = NULL;
				}
				delete i->second;
				++i;
			}

			delete vp;
		}

		if (currentPlayer != NULL) {
			delete currentPlayer;
			currentPlayer = NULL;
		}
	}

	ProgramAV* ProgramAV::_instance = 0;

	ProgramAV* ProgramAV::getInstance() {
		if (ProgramAV::_instance == NULL) {
			ProgramAV::_instance = new ProgramAV();
		}
		return ProgramAV::_instance;
	}

	void ProgramAV::release() {
		if (currentPlayer != NULL) {
			currentPlayer->stop();
		}

		if (_instance != NULL) {
			delete _instance;
			_instance = NULL;
		}
	}

	void ProgramAV::setAVPid(int programPid, int aPid, int vPid) {
		IPlayer* p;

		p = getPlayer(programPid);

		if (p != NULL) {
			cout << "ProgramAV::setAVPid";
			cout << " aPid = '" << aPid << "'";
			cout << " vPid = '" << vPid << "'";
			cout << endl;

			((AVPlayer*)p)->setAVPid(aPid, vPid);

		} else {
			cout << "ProgramAV::setAVPid Warning! Can't find programPid '";
			cout << programPid << "'";
			cout << endl;
		}
	}

	IPlayer* ProgramAV::getPlayer(int pid) {
		map<int, IPlayer*>::iterator i;
		i = vp->find(pid);

		if (i == vp->end())
			return NULL;

		return i->second;
	}

	void ProgramAV::setPlayer(int pid, IPlayer* player) {
		map<int, IPlayer*>::iterator i;
		IPlayer* ePlayer;

		i = vp->find(pid);
		if (i == vp->end()) {
			(*vp)[pid]       = player;
			currentPlayer    = player;
			fullScreenBounds = getBounds(currentPlayer);

		} else {
			ePlayer    = (*vp)[pid];
			(*vp)[pid] = player;
			delete ePlayer;
			ePlayer = NULL;
		}
	}

	void ProgramAV::setPropertyValue(string pName, string pValue) {
		if (currentPlayer != NULL) {
			if (pName == "bounds") {
				if (pValue == "") {
					setBounds(currentPlayer, fullScreenBounds);
				} else {
					setBounds(currentPlayer, pValue);
				}

			} else {
				currentPlayer->setPropertyValue(pName, pValue);
			}
		}
	}

	string ProgramAV::intToStrBounds(int x, int y, int w, int h) {
		return itos(x) + "," + itos(y) + "," + itos(w) + "," + itos(h);
	}

	string ProgramAV::getBounds(IPlayer* player) {
		ISurface* s;
		IWindow* w;
		string bounds = "";

		s = player->getSurface();
		if (s == NULL) {
			cout << "ProgramAV::getBounds player surface is NULL" << endl;
			return bounds;
		}

		w = (IWindow*)(s->getParent());
		if (w != NULL) {
			bounds = intToStrBounds(w->getX(), w->getY(), w->getW(), w->getH());
		}

		return bounds;
	}

	void ProgramAV::setBounds(IPlayer* player, string bounds) {
		int x, y, w, h;
		vector<string>* args;
		ISurface* s  = player->getSurface();
		IWindow* win = (IWindow*)(s->getParent());

		if (win == NULL) {
			cout << "ProgramAV::setBounds Warning! PAV Window is NULL";
			cout << endl;
			return;
		}

		args = split(bounds, ",");
		if (args->size() == 4) {
			x = stof((*args)[0]);
			y = stof((*args)[1]);
			w = stof((*args)[2]);
			h = stof((*args)[3]);

			win->setBounds(x, y, w, h);
		}

		delete args;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::
		IProgramAV* createProgramAV() {

	return ::br::pucrio::telemidia::ginga::core::player::ProgramAV::
			getInstance();
}

extern "C" void destroyProgramAV(::br::pucrio::telemidia::ginga::core::player::
		IProgramAV* pav) {

	//TODO: static release method
	return delete pav;
}
