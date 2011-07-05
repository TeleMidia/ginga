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

#include "multidevice/services/BaseDeviceDomain.h"
#include "multidevice/services/device/ActiveDeviceService.h"
#include "multidevice/services/device/PassiveDeviceService.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#ifndef _WIN32
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include <fcntl.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	BaseDeviceDomain::BaseDeviceDomain() : DeviceDomain() {
		timerCount    = 0;

		pthread_mutex_init(&pMutex, NULL);
		hasNewPassiveTask = false;
		passiveTimestamp  = 0;
		lastMediaContentTask.size = 0;

		deviceClass   = CT_BASE;
		deviceService = new BaseDeviceService();

		passiveMulticast  = new MulticastSocketService(
				(char*)(PASSIVE_MCAST_ADDR.c_str()),
				BROADCAST_PORT + CT_PASSIVE);

		activeMulticast = new MulticastSocketService(
				(char*)(ACTIVE_MCAST_ADDR.c_str()),
				BROADCAST_PORT + CT_ACTIVE);
	}

	BaseDeviceDomain::~BaseDeviceDomain() {
		pthread_mutex_destroy(&pMutex);
	}

	bool BaseDeviceDomain::taskRequest(
			int destDevClass,
			char* data,
			int taskSize) {

		RemoteTask* task;

		switch (destDevClass) {
			case CT_PASSIVE:
				task = new RemoteTask;
				task->data = data;
				task->size = taskSize;
				task->timestamp = getCurrentTimeMillis();
				//hasNewPassiveTask = true;
				pthread_mutex_lock(&pMutex);
				passiveTasks.push_back(task);
				//hasNewPassiveTask = false;
				pthread_mutex_unlock(&pMutex);
				return true;

			case CT_ACTIVE:
				return activeTaskRequest(data, taskSize);

			default:
				return false;
		}
	}

	bool BaseDeviceDomain::passiveTaskRequest(char* data, int taskSize) {
		bool repeat = true;

		if (!passiveTasks.empty() || hasNewPassiveTask) {
			//repeat = false;
		}

		passiveMulticast->dataRequest(data, taskSize, repeat);
		return true;
	}

	bool BaseDeviceDomain::activeTaskRequest(char* data, int taskSize) {
		/*cout << "BaseDeviceDomain::activeTaskRequest ";
		cout << endl;*/

		activeMulticast->dataRequest(data, taskSize);
		return true;
	}

	void BaseDeviceDomain::receiveConnectionRequest(char* task) {
		int reqDevClass, width, height;

		cout << "BaseDeviceDomain::receiveConnectionRequest " << endl;

		reqDevClass = (int)(unsigned char)task[0];
		width = ((((unsigned char)task[1]) & 0xFF) |
				((((unsigned char)task[2]) << 8) & 0xFF00));

		height = ((((unsigned char)task[3]) & 0xFF) |
				((((unsigned char)task[4]) << 8) & 0xFF00));

		if (addDevice(reqDevClass, width, height)) {
			schedDevClass = reqDevClass;
			schedulePost  = FT_ANSWERTOREQUEST;
		}
	}

	void BaseDeviceDomain::postAnswerTask(
			int reqDeviceClass, int answer) {

		int taskSize, pos, broadcastPort;
		int answerPayloadSize = 11;
		char* task;
		char* streamSourceIP;

		cout << "BaseDeviceDomain::postAnswerTask answer '";
		cout << answer << "' for device '";
		cout << sourceIp << "', which means '" << getStrIP(sourceIp);
		cout << "', of class '" << reqDeviceClass << endl;

		//prepare frame
		task = mountFrame(
				myIP, reqDeviceClass, FT_ANSWERTOREQUEST, answerPayloadSize);

		//fill prepared frame with answer payload
		pos = HEADER_SIZE;

		streamSourceIP = getStreamFromUInt(sourceIp);
		memcpy(task + pos, streamSourceIP, 4);
		delete[] streamSourceIP;

		pos = pos + 4;
		task[pos] = answer;

		pos++;
		memcpy(task + pos, task, 4);

		broadcastPort = broadcastService->getServicePort();
		pos = pos + 4;
		task[pos] = broadcastPort & 0xFF;
		task[pos+1] = (broadcastPort & 0xFF00) >> 8;

		taskSize = HEADER_SIZE + answerPayloadSize;
		broadcastTaskRequest(task, taskSize);
	}

	void BaseDeviceDomain::postNclMetadata(
			int devClass, vector<StreamData*>* streams) {

		char* task;
		StreamData* streamData;
		vector<StreamData*>::iterator i;

		if (devClass != IDeviceDomain::CT_BASE &&
				devClass != IDeviceDomain::CT_ACTIVE) {

			return;
		}

		i = streams->begin();
		while (i != streams->end()) {
			streamData = *i;

			//prepare frame
			task = mountFrame(
					myIP, devClass, FT_MEDIACONTENT, streamData->size);

			memcpy(task + HEADER_SIZE, streamData->stream, streamData->size);
			taskRequest(devClass, task, streamData->size + HEADER_SIZE);

			delete streamData;
			streams->erase(i);
			i = streams->begin();
		}
	}

	bool BaseDeviceDomain::postMediaContentTask(
			int destDevClass,
			string url) {

		int fd, fileSize, bytesRead, tSize;
		char* task;

		cout << "BaseDeviceDomain::postMediaContentTask file '";
		cout << url << "' to devices of class '" << destDevClass << "'";
		cout << endl;

		if (destDevClass == 0) {
			return false;
		}

		if (!deviceService->hasDevices()) {
			cout << "BaseDeviceDomain::postMediaContentTask no devs found!";
			cout << endl;
			return false;
		}

		fd = open(url.c_str(), O_RDONLY);
		if (fd > 0) {
			fileSize = lseek(fd, 0, SEEK_END);
			if (fileSize > 0) {
				close(fd);

				if (fileSize > MAX_FRAME_SIZE) {
					//TODO: frame segmentation support
					cout << "BaseDeviceDomain::postMediaContentTask ";
					cout << "Warning! Can't post a frame that the ";
					cout << "network doesn't support (" << fileSize;
					cout << ")" << endl;
					return false;
				}

				fd = open(url.c_str(), O_RDONLY);
				if (fd < 0) {
					cout << "BaseDeviceDomain::postMediaContentTask ";
					cout << "Warning! Can't re-open file '" << url;
					cout << "'" << endl;
					return false;
				}

				if (destDevClass == IDeviceDomain::CT_PASSIVE) {
					//prepare frame
					task = mountFrame(
							myIP, destDevClass, FT_MEDIACONTENT, fileSize);

					bytesRead = read(fd, task + HEADER_SIZE, fileSize);
					if (bytesRead == fileSize) {
						tSize = fileSize + HEADER_SIZE;
						if (lastMediaContentTask.size != 0) {
							delete[] lastMediaContentTask.data;
						}
						lastMediaContentTask.data = new char[tSize];
						lastMediaContentTask.size = tSize;
						memcpy(lastMediaContentTask.data, task, tSize);

						taskRequest(destDevClass, task, tSize);

					} else {
						cout << "BaseDeviceDomain::";
						cout << "postMediaContentTask ";
						cout << "Warning! Can't read '" << fileSize;
						cout << "' bytes from file '" << url << "' (";
						cout << bytesRead << " bytes read)" << endl;
						delete[] task;
					}
				}

			} else {
				cout << "BaseDeviceDomain::postMediaContentTask ";
				cout << "Warning! Can't seek file '" << url << "'" << endl;
			}

			close(fd);

		} else {
			cout << "BaseDeviceDomain::postMediaContentTask ";
			cout << "Warning! Can't open file '" << url << "'" << endl;
		}

		return true;
	}

	bool BaseDeviceDomain::receiveEventTask(char* task) {
		cout << "BaseDeviceDomain::receiveEventTask destClass '";
		cout << destClass << "'" << endl;
		return deviceService->receiveEvent(
				sourceIp, frameType, task, this->frameSize);
	}

	void BaseDeviceDomain::setDeviceInfo(int width, int height) {
		DeviceDomain::setDeviceInfo(width, height);
		connected = true;
	}

	bool BaseDeviceDomain::runControlTask() {
		char* task;
		cout << "BaseDeviceDomain::runControlTask :: " << endl;

		if (taskIndicationFlag) {
			task = taskReceive();
			if (task == NULL) {
				taskIndicationFlag = false;
				return false;
			}

			if (myIP == sourceIp) {
				/*cout << "DeviceDomain::runControlTask got my own task ";
				cout << "(size = '" << frameSize << "')" << endl;*/

				delete[] task;
				taskIndicationFlag = false;
				return false;
			}

			if (destClass != deviceClass) {
				cout << "DeviceDomain::runControlTask Task isn't for me!";
				cout << endl;

				delete[] task;
				taskIndicationFlag = false;
				return false;
			}

			if (frameSize + HEADER_SIZE != bytesRecv) {
				delete[] task;
				taskIndicationFlag = false;
				return false;
			}
			cout << "BaseDeviceDomain::runControlTask frame type '";
			cout << frameType << "'" << endl;

			switch (frameType) {
				case FT_CONNECTIONREQUEST:
					if (frameSize != 5) {
						cout << "25BaseDeviceDomain::runControlTask Warning!";
						cout << "received a connection request frame with";
						cout << " wrong size: '" << frameSize << "'" << endl;

					} else {
						cout << "calling receiveConnectionRequest" << endl;
						receiveConnectionRequest(task);
					}
					break;

				case FT_KEEPALIVE:
					cout << "BaseDeviceDomain::runControlTask KEEPALIVE";
					cout << endl;
					break;

				//what?
				default:
					cout << "DeviceDomain::runControlTask frame type ";
					cout << "WHAT?" << endl;
					delete[] task;
					taskIndicationFlag = false;
					return false;
			}

			delete[] task;

		} else {
			cout << "DeviceDomain::runControlTask Warning! Trying to control";
			cout << "a non indicated task." << endl;
		}

		taskIndicationFlag = false;
		return true;
	}

	bool BaseDeviceDomain::runDataTask() {
		char* task;

		task = taskReceive();
		if (task == NULL) {
			return false;
		}

		if (myIP == sourceIp) {
			/*cout << "BaseDeviceDomain::runDataTask receiving my own task";
			cout << endl;*/

			delete[] task;
			return false;
		}

		if (destClass != deviceClass) {
			cout << "BaseDeviceDomain::runDataTask";
			cout << " should never reaches here (receiving wrong destination";
			cout << " class '" << destClass << "')";
			cout << endl;

			delete[] task;
			return false;
		}

		if (frameSize + HEADER_SIZE != bytesRecv) {
			delete[] task;
			cout << "BaseDeviceDomain::runDataTask Warning! wrong ";
			cout << "frameSize '" << bytesRecv << "'" << endl;
			return false;
		}

		switch (frameType) {
			case FT_SELECTIONEVENT:
			case FT_ATTRIBUTIONEVENT:
			case FT_PRESENTATIONEVENT:
				receiveEventTask(task);
				break;

			//what?
			default:
				cout << "BaseDeviceDomain::runDataTask frame type ";
				cout << "WHAT?" << endl;
				delete[] task;
				return false;
		}

		delete[] task;
		return true;
	}

	void BaseDeviceDomain::checkPassiveTasks() {
		vector<RemoteTask*>::iterator i;
		RemoteTask* remoteTask;
		char* data;
		int taskSize;

		pthread_mutex_lock(&pMutex);
		i = passiveTasks.begin();
		if (i != passiveTasks.end()) {
			remoteTask = *i;
			data = remoteTask->data;
			taskSize = remoteTask->size;
			passiveTasks.erase(i);

			if (((remoteTask->timestamp - passiveTimestamp) >
					(1000 / PASSIVE_FPS)) || passiveTimestamp == 0) {

				passiveTimestamp = getCurrentTimeMillis();
				passiveTaskRequest(data, taskSize);

			} else {
				//cout << "CONTIGUOUS FRAME" << endl;
				/*if (passiveTasks.size() > 1) {
					cout << "DISCARDING FRAME" << endl;
					delete[] data;

				} else {*/
					passiveTimestamp = getCurrentTimeMillis();
					passiveTaskRequest(data, taskSize);
				//}
			}

			delete remoteTask;
		}
		pthread_mutex_unlock(&pMutex);
	}

	void BaseDeviceDomain::checkDomainTasks() {
		char* data;

		DeviceDomain::checkDomainTasks();
		if (newAnswerPosted) {
			newAnswerPosted = false;
			deviceService->newDeviceConnected(sourceIp);
		}

		if (passiveMulticast->checkInputBuffer(mdFrame, &bytesRecv)) {
 			runDataTask();
		}

		if (activeMulticast->checkInputBuffer(mdFrame, &bytesRecv)) {
 			runDataTask();
		}

		checkPassiveTasks();
		if (!passiveMulticast->checkOutputBuffer()) {
			if (lastMediaContentTask.size != 0) {
				timerCount++;
				if (timerCount > 5) {
					data = new char[lastMediaContentTask.size];
					memcpy(
							data,
							lastMediaContentTask.data,
							lastMediaContentTask.size);

					taskRequest(
							IDeviceDomain::CT_PASSIVE,
							data,
							lastMediaContentTask.size);

					timerCount = 0;
				}
			}
		}

		activeMulticast->checkOutputBuffer();
	}
}
}
}
}
}
}
