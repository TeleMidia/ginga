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

#ifndef _PassiveDeviceDomain_H_
#define _PassiveDeviceDomain_H_

#include "DeviceDomain.h"
#include "MulticastSocketService.h"
#include "BroadcastDualSocketService.h"

GINGA_MULTIDEVICE_BEGIN

  class PassiveDeviceDomain : public DeviceDomain {
	private:
		ISocketService* passiveSocket;

	public:
		PassiveDeviceDomain(bool useMulticast, int srvPort);
		virtual ~PassiveDeviceDomain();

	protected:
		bool taskRequest(int destDevClass, char* data, int taskSize);
		bool passiveTaskRequest(char* data, int taskSize);
		bool activeTaskRequest(char* data, int taskSize){return false;};
		void postConnectionRequestTask(int width, int height);
		void receiveConnectionRequest(char* task){};
		void postAnswerTask(int reqDeviceClass, int answer){};
		void receiveAnswerTask(char* answerTask);
		bool postMediaContentTask(int destDevClass, string url){return false;};
		bool receiveMediaContentTask(char* task);
		bool receiveEventTask(char* task){return false;};
		void setDeviceInfo(int width, int height, string base_device_ncl_path);
		bool runControlTask();
		bool runDataTask();
		void checkDomainTasks();
  };

GINGA_MULTIDEVICE_END
#endif /*_PassiveDeviceDomain_H_*/
