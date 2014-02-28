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

#include "gingancl/model/LinkAction.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkAction::LinkAction() : Thread() {
		initLinkAction(0);
	}

	LinkAction::LinkAction(double delay) : Thread() {
		initLinkAction(delay);
	}

	LinkAction::~LinkAction() {
		isDeleting = true;

		Thread::mutexLock(&plMutex);
		if (progressionListeners != NULL) {
			delete progressionListeners;
			progressionListeners = NULL;
		}
		Thread::mutexUnlock(&plMutex);
		Thread::mutexDestroy(&plMutex);
	}

	void LinkAction::initLinkAction(double delay) {
		satisfiedCondition = NULL;
		this->delay = delay;
		progressionListeners = new vector<LinkActionProgressionListener*>;
		Thread::mutexInit(&plMutex, NULL);
		typeSet.insert("LinkAction");
	}

	bool LinkAction::instanceOf(string s) {
		if (typeSet.empty()) {
			return false;

		} else {
			return (typeSet.find(s) != typeSet.end());
		}
	}

	double LinkAction::getWaitDelay() {
		return this->delay;
	}

	void LinkAction::setSatisfiedCondition(
			void* satisfiedCondition) {

		this->satisfiedCondition = satisfiedCondition;
	}

	void LinkAction::run(void* satisfiedCondition) {
		this->satisfiedCondition = satisfiedCondition;
		run();
	}

	void LinkAction::run() {
		if (hasDelay()) {
			clog << "LinkAction::run uSleeping '" << delay*1000 << "'" << endl;
			SystemCompat::uSleep((long)(delay*1000));
		}
	}

	void LinkAction::setWaitDelay(double delay) {
		this->delay = delay;
	}

	bool LinkAction::hasDelay() {
		if (this->delay > 0) {
			return true;
		}
		return false;
	}

	void LinkAction::addActionProgressionListener(
		    LinkActionProgressionListener* listener) {

		vector<LinkActionProgressionListener*>::iterator i;

		if (tryLock()) {
			if (progressionListeners != NULL && !isDeleting) {
				i = progressionListeners->begin();
				while (i != progressionListeners->end()) {
					if (listener == *i) {
						clog << "LinkAction::addActionProgressionListener ";
						clog << "Warning! Trying to add the same listener twice";
						clog << endl;
						Thread::mutexUnlock(&plMutex);
						return;
					}
					++i;
				}
				progressionListeners->push_back(listener);
			}
			Thread::mutexUnlock(&plMutex);
		}
	}

	void LinkAction::removeActionProgressionListener(
		    LinkActionProgressionListener* listener) {

		vector<LinkActionProgressionListener*>::iterator i;

		if (tryLock()) {
			if (progressionListeners != NULL && !isDeleting) {
				for (i = progressionListeners->begin();
						i != progressionListeners->end();
						++i) {

					if (*i == listener) {
						progressionListeners->erase(i);
						break;
					}
				}
			}
			Thread::mutexUnlock(&plMutex);
		}
	}

	void LinkAction::notifyProgressionListeners(bool start) {
		int i, size;
		LinkActionProgressionListener* listener;
		vector<LinkActionProgressionListener*>* notifyList;

		if (tryLock()) {
			if (progressionListeners != NULL && !isDeleting) {
				notifyList = new vector<LinkActionProgressionListener*>(
						*progressionListeners);

				Thread::mutexUnlock(&plMutex);

				size = notifyList->size();
				for (i = 0; i < size; i++) {
					listener = (*notifyList)[i];
					listener->actionProcessed(start);
					if (isDeleting) {
						break;
					}
				}
				delete notifyList;

			} else {
				Thread::mutexUnlock(&plMutex);
			}
		}
	}

	bool LinkAction::tryLock() {
		if (isDeleting) {
			return false;
		}

		Thread::mutexLock(&plMutex);
		return true;
	}
}
}
}
}
}
}
}
