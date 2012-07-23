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

#ifndef _DeviceService_H_
#define _DeviceService_H_

#include <pthread.h>

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "../../RemoteDevice.h"
#include "IDeviceService.h"

#include <set>
#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
  class DeviceService : public IDeviceService {
	protected:
		set<IRemoteDeviceListener*>* listeners;
		map<unsigned int, IRemoteDevice*>* devices;
		int serviceClass;
		pthread_mutex_t lMutex;
		pthread_mutex_t dMutex;

	public:
		DeviceService();
		virtual ~DeviceService();

	protected:
		IRemoteDevice* getDevice(unsigned int devAddr);

	public:
		void addListener(IRemoteDeviceListener* listener);
		void removeListener(IRemoteDeviceListener* listener);

		bool addDevice(
				unsigned int deviceAddress,
				int newDevClass,
				int width,
				int height);

		bool hasDevices();

		virtual void newDeviceConnected(unsigned int devAddr)=0;
		virtual void connectedToBaseDevice(unsigned int domainAddr)=0;

		virtual bool receiveEvent(
				unsigned int devAddr,
				int eventType,
				char* stream,
				int streamSize)=0;

		virtual bool receiveMediaContent(
				unsigned int devAddr,
				char* stream,
				int streamSize)=0;
  };
}
}
}
}
}
}

#endif /*_BaseDeviceService_H_*/
