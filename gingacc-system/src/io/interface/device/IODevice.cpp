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

#include "../../../../include/io/interface/device/IODevice.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	IODevice::IODevice() {
		audioDevices     = new map<unsigned int, IDeviceAudio*>;
		screenDevices    = new map<unsigned int, IDeviceScreen*>;
		connectorDevices = new map<unsigned int, IDeviceCommunication*>;

		pthread_mutex_init(&mutexList, NULL);
	}

	IODevice::~IODevice() {
		map<unsigned int, IDeviceScreen*>::iterator i;
		map<unsigned int, IDeviceAudio*>::iterator j;
		map<unsigned int, IDeviceCommunication*>::iterator k;

		pthread_mutex_lock(&mutexList);
		if (screenDevices != NULL) {
			i = screenDevices->begin();
			while (i != screenDevices->end()) {
				delete i->second;
				++i;
			}
			delete screenDevices;
			screenDevices = NULL;
		}

		if (audioDevices != NULL) {
			j = audioDevices->begin();
			while (j != audioDevices->end()) {
				delete j->second;
				++j;
			}
			delete audioDevices;
			audioDevices = NULL;
		}

		if (connectorDevices != NULL) {
			k = connectorDevices->begin();
			while (k != connectorDevices->end()) {
				delete k->second;
				++k;
			}
			delete connectorDevices;
			connectorDevices = NULL;
		}

		pthread_mutex_unlock(&mutexList);
		pthread_mutex_destroy(&mutexList);
	}

	unsigned int IODevice::addAudio(IDeviceAudio* audioResource) {
		unsigned int i;

		pthread_mutex_lock(&mutexList);
		if (audioDevices != NULL) {
			i = audioDevices->size();
			(*audioDevices)[i] = audioResource;
		}
		pthread_mutex_unlock(&mutexList);

		return i;
	}

	unsigned int IODevice::addScreen(IDeviceScreen* screenResource) {
		unsigned int i;

		pthread_mutex_lock(&mutexList);
		if (screenDevices != NULL) {
			i = screenDevices->size();
			(*screenDevices)[i] = screenResource;
		}
		pthread_mutex_unlock(&mutexList);

		return i;
	}

	unsigned int IODevice::addChannel(IDeviceCommunication* channelResource) {
		unsigned int i;

		pthread_mutex_lock(&mutexList);
		if (connectorDevices != NULL) {
			i = connectorDevices->size();
			(*connectorDevices)[i] = channelResource;
		}
		pthread_mutex_unlock(&mutexList);

		return i;
	}

	unsigned int IODevice::getScreenWidthRes(unsigned int screenNumber) {
		IDeviceScreen* scr;
		unsigned int wRes = 0;

		pthread_mutex_lock(&mutexList);
		if (screenDevices != NULL && screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			wRes = scr->getWidthResolution();
		}
		pthread_mutex_unlock(&mutexList);

		return wRes;
	}

	unsigned int IODevice::getScreenHeightRes(unsigned int screenNumber) {
		IDeviceScreen* scr;
		unsigned int hRes = 0;

		pthread_mutex_lock(&mutexList);
		if (screenDevices != NULL && screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			hRes = scr->getHeightResolution();
		}
		pthread_mutex_unlock(&mutexList);

		return hRes;
	}

	void* IODevice::getGfxRoot() {
		IDeviceScreen* scr;
		void* gfxRoot = NULL;

		pthread_mutex_lock(&mutexList);
		if (screenDevices != NULL && screenDevices->size() > 0) {
			scr = screenDevices->begin()->second;
			gfxRoot = scr->getGfxRoot();
		}
		pthread_mutex_unlock(&mutexList);

		return gfxRoot;
	}

	void IODevice::setBackgroundImage(string uri) {
		IDeviceScreen* scr;
		map<unsigned int, IDeviceScreen*>::iterator i;

		pthread_mutex_lock(&mutexList);
		if (screenDevices != NULL) {
			i = screenDevices->find(0);
			if (i != screenDevices->end()) {
				scr = i->second;
				scr->setBackgroundImage(uri);
			}
		}
		pthread_mutex_unlock(&mutexList);
	}

	void IODevice::mergeIds(
			int destId, vector<int>* srcIds, unsigned int screenNumber) {

		IDeviceScreen* scr;

		pthread_mutex_lock(&mutexList);
		if (screenDevices != NULL && screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			scr->mergeIds(destId, srcIds);
		}
		pthread_mutex_unlock(&mutexList);
	}

	void* IODevice::getWindow(int winId, unsigned int screenNumber) {
		IDeviceScreen* scr;
		void* win = NULL;

		pthread_mutex_lock(&mutexList);
		if (screenDevices != NULL && screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			win = scr->getWindow(winId);
		}
		pthread_mutex_unlock(&mutexList);

		return win;
	}

	void* IODevice::createWindow(void* winDesc, unsigned int screenNumber) {
		IDeviceScreen* scr;
		void* win = NULL;

		pthread_mutex_lock(&mutexList);
		if (screenDevices != NULL && screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			win = scr->createWindow(winDesc);
		}
		pthread_mutex_unlock(&mutexList);

		return win;
	}

	void IODevice::releaseWindow(void* win, unsigned int screenNumber) {
		IDeviceScreen* scr;

		pthread_mutex_lock(&mutexList);
		if (screenDevices != NULL && screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			scr->releaseWindow(win);
		}
		pthread_mutex_unlock(&mutexList);
	}

	void* IODevice::createSurface(void* surDesc, unsigned int screenNumber) {
		IDeviceScreen* scr;
		void* sur = NULL;

		pthread_mutex_lock(&mutexList);
		if (screenDevices != NULL && screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			sur = scr->createSurface(surDesc);
		}
		pthread_mutex_unlock(&mutexList);

		return sur;
	}

	void IODevice::releaseSurface(void* sur, unsigned int screenNumber) {
		IDeviceScreen* scr;

		pthread_mutex_lock(&mutexList);
		if (screenDevices != NULL && screenDevices->count(screenNumber) != 0) {
			scr = (*screenDevices)[screenNumber];
			scr->releaseSurface(sur);
		}
		pthread_mutex_unlock(&mutexList);
	}
}
}
}
}
}
}
}
