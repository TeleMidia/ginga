/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _DeviceDomain_H_
#define _DeviceDomain_H_

#include "BroadcastSocketService.h"
#include "IRemoteDeviceListener.h"
#include "DeviceService.h"
#include "ISocketService.h"
#include "MulticastSocketService.h"
#include "NetworkUtil.h"
#include "RemoteEventService.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
  class DeviceDomain {
	public:
		//CT: class types
		static const int CT_BASE              = 0;
		static const int CT_PASSIVE           = 1;
		static const int CT_ACTIVE            = 2;

		//FT: frame types
		static const int FT_CONNECTIONREQUEST = 0;
		static const int FT_ANSWERTOREQUEST   = 1;
		static const int FT_KEEPALIVE         = 2;
		static const int FT_MEDIACONTENT      = 3;
		static const int FT_SELECTIONEVENT    = 4;
		static const int FT_ATTRIBUTIONEVENT  = 5;
		static const int FT_PRESENTATIONEVENT = 6;

	protected:
		static char headerStream[HEADER_SIZE];
		static char* mdFrame;

		static unsigned int myIP;
		static bool taskIndicationFlag;

		DeviceService* deviceService;
		ISocketService* broadcastService;
		void* res;

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
