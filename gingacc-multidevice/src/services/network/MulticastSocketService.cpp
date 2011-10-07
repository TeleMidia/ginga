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

#include "multidevice/services/network/MulticastSocketService.h"

#ifdef _WIN32
#include <io.h>
#include <WS2tcpip.h>
#define MSG_DONTWAIT 0
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	MulticastSocketService::MulticastSocketService(
			char* groupAddr, unsigned int portNumber) {

		outputBuffer = new vector<struct frame*>;
		port         = portNumber;
		gAddr        = groupAddr;

		memset(&mss, 0, sizeof(mss));
		mss.sin_family      = AF_INET;
		mss.sin_port        = htons(port);
		mss.sin_addr.s_addr = inet_addr(gAddr);

		pthread_mutex_init(&mutexBuffer, NULL);
		createMulticastGroup();
	}

	MulticastSocketService::~MulticastSocketService() {
		if (msdR > 0) {
			setsockopt(
					msdR,
					IPPROTO_IP,
					IP_DROP_MEMBERSHIP,
					(char*)&mss, sizeof(mss));

			close(msdR);
		}

		if (msdW > 0) {
			close(msdW);
		}

		if (outputBuffer != NULL) {
			delete outputBuffer;
			outputBuffer = NULL;
		}

		pthread_mutex_destroy(&mutexBuffer);
	}

	int MulticastSocketService::createMulticastGroup() {
		if (createSocket()) {
			if (setSocketOptions()) {
				if (tryToBind()) {
					if (addToGroup()) {
						return msdR;
					}
				}
			}
		}
		return -1;
	}

	bool MulticastSocketService::createSocket() {
		unsigned char trueVar = 1;

		msdW = socket(AF_INET, SOCK_DGRAM, 0);
		//msdW = socket(AF_INET, SOCK_STREAM, 0);
		if (msdW < 0){
			perror("MulticastSocketService::createSocket msdW");
			return false;
		}

		msdR = socket(AF_INET, SOCK_DGRAM, 0);
		//msdR = socket(AF_INET, SOCK_STREAM, 0);
		if (msdR < 0){
			perror("MulticastSocketService::createSocket msdR");
			return false;
		}

		#ifdef _WIN32

		#else
		#ifndef __DARWIN_UNIX03
		setsockopt(msdR, SOL_SOCKET, SO_BSDCOMPAT, &trueVar, sizeof(trueVar));
		setsockopt(msdW, SOL_SOCKET, SO_BSDCOMPAT, &trueVar, sizeof(trueVar));
		#endif

		#endif

		return true;
	}

	bool MulticastSocketService::addToGroup() {
		int ret;
		struct ip_mreq stIpMreq;

		stIpMreq.imr_multiaddr.s_addr = inet_addr(gAddr);
		stIpMreq.imr_interface.s_addr = htonl(INADDR_ANY);

		ret = setsockopt(
				msdR,
				IPPROTO_IP,
				IP_ADD_MEMBERSHIP,
				(char*)&stIpMreq, sizeof(struct ip_mreq));

		if (ret < 0) {
			perror("MulticastSocketService::addToGroup msdR");
			return false;
		}

		return true;
	}

	bool MulticastSocketService::setSocketOptions() {
		unsigned char loop = 0;
		unsigned char ttl  = MCAST_TTL;
	    int reuse = 1;
	    int ret = setsockopt(
	    		msdR,
	    		SOL_SOCKET,
	    		SO_REUSEADDR,
#ifndef _WIN32
	    		(int*)&reuse, sizeof(reuse));
#else
				(char*)&reuse, sizeof(reuse));
#endif

		if (ret < 0) {
			perror("MulticastSocketService::setSocketOptions reuse");
		}

		ret = setsockopt(
				msdW,
				IPPROTO_IP,
				IP_MULTICAST_LOOP,
#ifndef _WIN32
				&loop, sizeof(loop));
#else
				(const char*)&loop, sizeof(loop));
#endif

		if (ret < 0) {
			perror("MulticastSocketService::setSocketOptions loop");
		}

		ret = setsockopt(
				msdW,
				IPPROTO_IP,
				IP_MULTICAST_TTL,
#ifndef _WIN32
				&ttl, sizeof(ttl));
#else
				(const char*)&ttl, sizeof(ttl));
#endif


		if (ret < 0) {
			perror("MulticastSocketService::setSocketOptions TTL");
		}

		return true;
	}

	bool MulticastSocketService::tryToBind() {
		int ret = bind(msdR, (struct sockaddr*)&mss, sizeof(struct sockaddr));
		if (ret < 0) {
			perror ("MulticastSocketService::tryToBind bind");
			return false;
		}

		return true;
	}

	int MulticastSocketService::getServicePort() {
		return port;
	}

	void MulticastSocketService::dataRequest(
			char* data, int taskSize, bool repeat) {

		struct frame* f;

		pthread_mutex_lock(&mutexBuffer);
		f = new struct frame;
		f->data      = data;
		f->size      = taskSize;
		f->repeat    = repeat;
		outputBuffer->push_back(f);
		pthread_mutex_unlock(&mutexBuffer);
	}

	bool MulticastSocketService::sendData(struct frame* f) {
		char* data;
		int taskSize, result, i;

		data     = f->data;
		taskSize = f->size;

		/*clog << "MulticastSocketService::sendData Sending";
		clog << " taskSize = '" << taskSize  << "'" << endl;*/

		for (i = 0; i < NUM_OF_COPIES; i++) {
			result = sendto(
					msdW,
					data,
					taskSize,
					0,
					(struct sockaddr*)&mss,
					sizeof(mss));

			if (result == -1) {
				perror ("MulticastSocketService::sendData msdW sendTo");
				clog << " TASKSIZE = '" << taskSize << "'" << endl;
				return false;
			}

			/*if (!f->repeat) {
				return true;
			}*/
		}

		return true;
	}

	bool MulticastSocketService::checkOutputBuffer() {
		vector<struct frame*>::iterator i;
		bool sent = false;
		struct frame* f;

		pthread_mutex_lock(&mutexBuffer);
		i = outputBuffer->begin();
		if (i != outputBuffer->end()) {
			f = *i;
			sent = sendData(f);
			if (sent) {
				outputBuffer->erase(i);
				delete[] f->data;
				delete f;
			}

		} else {
			/*clog << "MulticastSocketService::checkOutputBuffer ";
			clog << "empty buffer" << endl;*/
		}
		pthread_mutex_unlock(&mutexBuffer);

		return sent;
	}

	bool MulticastSocketService::checkInputBuffer(char* data, int* size) {
		int nfds, res, recvFrom;
		fd_set fdset;
		struct timeval tv_timeout;

		FD_ZERO(&fdset);
		FD_SET(msdR, &fdset);

		nfds               = msdR + 1;
		tv_timeout.tv_sec  = 0;
		tv_timeout.tv_usec = 0;

		res = select(nfds, &fdset, NULL, NULL, &tv_timeout);
		switch (res) {
			case -1:
				clog << "MulticastSocketService::checkInputBuffer ";
				clog << "Warning! select ERRNO = " << errno << endl;
				memset(data, 0, MAX_FRAME_SIZE);
				return false;

			case 1:
				memset(data, 0, MAX_FRAME_SIZE);
				*size = recvfrom(
						msdR,
						data,
						MAX_FRAME_SIZE,
						MSG_DONTWAIT,
						(struct sockaddr*)NULL,
#ifndef _WIN32
						(socklen_t*)NULL);
#else
						(int *)NULL);
#endif

				if (*size == -1) {
					if (errno != EAGAIN) {
						clog << "MulticastSocketService::checkInputBuffer ";
#ifndef _WIN32
						herror("check domain error: ");
#endif
						clog << "Warning! receive data ERRNO = " << errno;
						clog << endl;
						memset(data, 0, MAX_FRAME_SIZE);
						return false;

					} else {
						memset(data, 0, MAX_FRAME_SIZE);
						return false;
					}
				}

				if (*size <= HEADER_SIZE) {
					clog << "MulticastSocketService::checkInputBuffer ";
					clog << "Warning! Received invalid frame: ";
					clog << "bytes received = '" << *size << "' ";
					clog << "HEADER_SIZE = '" << HEADER_SIZE << "' ";
					clog << endl;

					memset(data, 0, MAX_FRAME_SIZE);
					return false;
				}

				recvFrom = getUIntFromStream(data + 1);
				if (!isValidRecvFrame(recvFrom, data)) {
					memset(data, 0, MAX_FRAME_SIZE);
					return false;
				}
				break;

			default:
				memset(data, 0, MAX_FRAME_SIZE);
				return false;
		}

		return true;
	}
}
}
}
}
}
}
