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

#include "../../../include/multidevice/services/network/TcpClientConnection.h"

#ifdef _WIN32
#include <ws2tcpip.h>
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	TCPClientConnection::TCPClientConnection(unsigned int devid,
						 char* hostname,
						 char *port_str,
						 IRemoteDeviceListener* srv) {

		struct addrinfo hints, *res;
		int set;

		deviceId     = devid;
		srv_hostname = hostname;
		portno       = port_str;
		resrv        = srv;
		counter      = 0;
		running      = true;

		memset(&hints, 0, sizeof hints);

		hints.ai_family   = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		getaddrinfo(srv_hostname, port_str, &hints, &res);

		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0) {
			cout << "TCPClientConnection::ERROR opening socket";

		} else {
			set = 1;
#ifndef _WIN32
			setsockopt(
					sockfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&set, sizeof(int));
#else
			setsockopt(
					sockfd, SOL_SOCKET, SO_KEEPALIVE, (char*)&set, sizeof(int));
#endif

			connect(sockfd, res->ai_addr, res->ai_addrlen);
		}
	}

	TCPClientConnection::~TCPClientConnection() {
		running = false;
		close(sockfd);
	}

	/**
	 * Posts a command through the TCP connection.
	 * Returns true or false, whether the command is successful or not.
	 *
	 */
	bool TCPClientConnection::post(char* str) {
		char* com;
		char buf[5] = "";
		int nr;
		int nw;

#ifndef _WIN32
		asprintf(&com, "%d %s", counter, str);
#else
		com = new char(32);
		sprintf_s(com, 32,"%d %s", counter, str);
#endif
		counter++;

		if (sockfd < 0) {
			return false;
		}

#ifndef _WIN32
		nw = send(sockfd,com,strlen(com), MSG_NOSIGNAL);
#else
		nw = send(sockfd,com,strlen(com), 0 /*MSG_NOSIGNAL*/);
#endif
		if (nw != strlen(com)) {
			perror("TCPClientConnection::post send error");
			this->end();

		} else {
			return true;
			/*
			nr = recv(sockfd,buf,5,0);
			if (nr > 0) {
				if (strcmp(buf,"OK\n")==0) {
					return true;

				} else {
					return false;
				}
			}
			*/
		}

		return false;
	}

	void TCPClientConnection::run() {
		char buf[100]; //max event string size
		char msgType[4];
		char evtType[5];
		int nr;

		while (running) {
			memset(buf, 0, 100); //max event string size
			memset(msgType, 0, 4);
			memset(evtType, 0, 5);

			nr = recv(sockfd, buf, 100, 0);

			if (nr > 3) {
				if (nr > 100) {
					buf[99] = '\0';

				} else {
					buf[nr] = '\0';
				}

				//cout << "TCPClientConnection:run buf= " << buf << endl;

				strncpy(msgType,buf,3);
				msgType[3] = '\0';

				strncpy(buf,buf+4,nr);

				if ((strcmp(msgType,"EVT")) == 0) {
					strncpy(evtType,buf,4);
					evtType[4] = '\0';
					strncpy(buf,buf+5,nr);

					if ((strcmp(evtType,"ATTR")) == 0) {
						//cout << " new buf: " << buf << endl;
						resrv->receiveRemoteEvent(
								2,IDeviceDomain::FT_ATTRIBUTIONEVENT,buf);
					}
				}

			} else {
				if (nr < 0) {
					cout << "TCPClientConnection::run end()!";
					cout << " reason: nr=" << nr;
					cout << " buf=" << buf << endl;
					this->end();
				}
			}
		}
	}

	void TCPClientConnection::release() {
		running = false;
	}

	void TCPClientConnection::end() {
		release();
		close(sockfd);
	}
}
}
}
}
}
}
