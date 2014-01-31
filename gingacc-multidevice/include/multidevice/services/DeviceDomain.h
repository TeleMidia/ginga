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

#ifndef _DeviceDomain_H_
#define _DeviceDomain_H_

#include "network/ISocketService.h"
#include "network/BroadcastSocketService.h"
#include "network/MulticastSocketService.h"
#include "network/NetworkUtil.h"
#include "device/IDeviceService.h"
#include "RemoteEventService.h"
#include "IDeviceDomain.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
  class DeviceDomain : public IDeviceDomain {
	protected:
		static char headerStream[HEADER_SIZE];
		static char* mdFrame;

		static unsigned int myIP;
		static bool taskIndicationFlag;

		IDeviceService* deviceService;
		ISocketService* broadcastService;
		RemoteEventService* res;

		unsigned int sourceIp;
		int bytesRecv;
		int deviceClass;
		int deviceWidth;
		int deviceHeight;
		int destClass;
		int frameType;
		int schedulePost;
		int schedDevClass;
		double receivedTimeStamp;
		double sentTimeStamp;
		unsigned int frameSize;
		bool connected;
		bool newAnswerPosted;
		bool enableMulticast;
		int servicePort;
		string base_device_ncl_path;

	public:
		DeviceDomain(bool useMulticast, int srvPort);
		virtual ~DeviceDomain();

		bool isConnected();

	protected:
		void clearHeader();
		bool broadcastTaskRequest(char* data, int taskSize);
		virtual bool taskRequest(int destDevClass, char* data, int taskSize)=0;
		virtual bool passiveTaskRequest(char* data, int taskSize)=0;
		virtual bool activeTaskRequest(char* data, int taskSize)=0;
		char* taskReceive();
		void parseTaskHeader();
		void printTaskHeader();
		bool addDevice(int reqDeviceClass, int width, int height, int srvPort);

	public:
		void postConnectionRequestTask();

	protected:
		virtual void postConnectionRequestTask(int width, int height)=0;
		virtual void receiveConnectionRequest(char* task)=0;
		virtual void postAnswerTask(int reqDeviceClass, int answer)=0;
		virtual void receiveAnswerTask(char* answerTask)=0;

	public:
		virtual bool postMediaContentTask(int destDevClass, string url)=0;

	protected:
		virtual bool receiveMediaContentTask(char* task)=0;

	public:
		void postEventTask(
				int destDevClass,
				int frameType,
				char* payload,
				int payloadSize);

	protected:
		virtual bool receiveEventTask(char* task)=0;

	public:
		virtual void setDeviceInfo(int width, int height,string base_device_ncl_path);
		int getDeviceClass();

	protected:
		virtual bool runControlTask()=0;
		virtual bool runDataTask()=0;

	public:
		virtual void checkDomainTasks();
		virtual void addDeviceListener(IRemoteDeviceListener* listener);
		virtual void removeDeviceListener(IRemoteDeviceListener* listener);
  };
}
}
}
}
}
}

#endif /*_IDeviceDomain_H_*/
