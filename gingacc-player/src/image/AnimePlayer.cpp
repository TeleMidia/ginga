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

#include "player/AnimePlayer.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "config.h"

#include "player/PlayersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	AnimePlayer::AnimePlayer(vector<string>* mrls) : Thread() {
		string mrl;
		vector<string>::iterator i;

		dur = 100000;
		running = false;
		win = NULL;
		anime = new vector<IImageProvider*>;
		i = mrls->begin();
		while (i != mrls->end()) {
			mrl = *i;
			if (fileExists(mrl)) {
#if HAVE_COMPSUPPORT
				anime->push_back(((ImageProviderCreator*)(cm->getObject(
						"ImageProvider")))(mrl.c_str()));
#else
#ifndef _WIN32
				anime->push_back(new DFBImageProvider(mrl.c_str()));
#else
				anime->push_back(new DXImageProvider(mrl.c_str()));
#endif
#endif
			}
			++i;
		}

	}

	AnimePlayer::~AnimePlayer() {
		vector<IImageProvider*>::iterator i;
		IImageProvider* provider;

		if (running) {
			running = false;
			waitForUnlockCondition();
		}

		if (win != NULL) {
			win->clearContent();
			win->hide();
			delete win;
			win = NULL;
		}

		if (anime != NULL) {
			i = anime->begin();
			while (i != anime->end()) {
				provider = *i;
				if (provider != NULL) {
					delete provider;
					provider = NULL;

#if HAVE_COMPSUPPORT
					cm->releaseComponentFromObject("ImageProvider");
#endif
				}
				++i;
			}

			delete anime;
			anime = NULL;
		}
	}

	void AnimePlayer::play() {
		running = true;
		Thread::start();
	}

	void AnimePlayer::stop() {
		if (running) {
			running = false;
			waitForUnlockCondition();
		}
	}

	void AnimePlayer::setVoutWindow(IWindow* win) {
		this->win = win;
	}

	void AnimePlayer::setDur(double dur) {
		this->dur = dur;
	}

	ISurface* AnimePlayer::prepareSurface(IImageProvider* provider) {
		ISurface* renderedSurface = NULL;
		renderedSurface = provider->prepare(false);
		return renderedSurface;
	}

	void AnimePlayer::run() {
		vector<IImageProvider*>::iterator i;
		IImageProvider* provider;
		ISurface* surface;

		if (anime != NULL) {
			while (running) {
				i = anime->begin();
				while (i != anime->end()) {
					provider = *i;
					surface = prepareSurface(provider);
					if (win != NULL) {
						win->renderFrom(surface);
					}
					delete surface;
					surface = NULL;
					Thread::usleep(dur);
					++i;
					if (!running) {
						unlockConditionSatisfied();
						return;
					}
				}
			}
		}
		unlockConditionSatisfied();
	}
}
}
}
}
}
}
