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
#ifdef _MSC_VER
extern "C" {
#include "asprintf.h"
}
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	const static int MAX_MSG_SIZE = 1024;

	TCPClientConnection::TCPClientConnection(
			unsigned int devid,
			unsigned int index,
			char* hostname,
			char *port_str,
			IRemoteDeviceListener* srv) {

		try {
			deviceId = devid;
			orderId = index;
			counter = 0;
			tcpSocket = new TCPSocket(string(hostname),atoi(port_str));
			////
			//TODO: improve (create setIndex e getIndex methods) so index does not change

			char* set_index;
			asprintf(&set_index,"%d %s %s=%d\n",0,"SET","child.index",orderId);
			this->post(set_index);
			////
			running = true;
			resrv = srv;
		}
		catch (SocketException &e) {
			clog << "TCPClientConnection:: creation error" << endl;
			clog << e.what() << endl;

		}

	}

	TCPClientConnection::~TCPClientConnection() {
		this->release();
		if (tcpSocket != NULL) {
			delete tcpSocket;
		}
	}

	bool TCPClientConnection::post(char* str) {
		char* com;

		if (tcpSocket == NULL) {
			return false;
		}
		asprintf(&com, "%d", counter);
		string s_com = string(com) + " " + string(str);

		counter++;

		try {
			//tcpSocket->send(com, strlen(com));
			tcpSocket->send((char*)s_com.c_str(),(int)s_com.size());
			return true;
		}
		catch (SocketException &e) {
			clog << "TCPClientConnection::post send error" << endl;
			clog << e.what() << endl;
			this->release();
			return false;

		}
		return false;
	}

	void TCPClientConnection::run() {
		//TODO: verify memcpy for windows (windows has memcpy_s)
		char buf[MAX_MSG_SIZE]; //max event string size
		char msgType[4];
		char evtType[5];
		int nr;
		while (running) {

			memset(buf, 0, MAX_MSG_SIZE);
			memset(msgType, 0, 4);
			memset(evtType, 0, 5);
			try {
				nr = tcpSocket->recv(buf,MAX_MSG_SIZE);

			}
			catch (SocketException &e) {
				clog << e.what() << endl;
				this->release();
			}

			if (nr > 3) {
					buf[nr] = '\0';
			}
			else {
				break;
			}

			//strncpy(msgType,buf,3);
			memcpy(msgType,buf,3);
			msgType[3] = '\0';

			//strncpy(buf,buf+4,nr);
			memcpy(buf,buf+4,nr);
			if ((strcmp(msgType,"EVT")) == 0) {
				//strncpy(evtType,buf,4);
				memcpy(evtType,buf,4);
				evtType[4] = '\0';
				//strncpy(buf,buf+5,nr);
				memcpy(buf,buf+5,nr);

				if ((strcmp(evtType,"ATTR")) == 0) {
					if (resrv != NULL) {
					resrv->receiveRemoteEvent(
							2,IDeviceDomain::FT_ATTRIBUTIONEVENT,buf);
					}
				}
			}//end if strcmp(msgtype)

		}//end while running


	}

	void TCPClientConnection::release() {
		running = false;
	}

}
}
}
}
}
}
