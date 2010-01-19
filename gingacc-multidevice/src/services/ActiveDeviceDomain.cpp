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

#include "../../include/services/ActiveDeviceDomain.h"

#include "../../include/services/device/ActiveDeviceService.h"
#include "../../include/services/network/MulticastSocketService.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	ActiveDeviceDomain::ActiveDeviceDomain() : BaseDeviceDomain() {
		deviceClass     = CT_ACTIVE;
		deviceService   = new ActiveDeviceService();

		//activeMulticast = new BroadcastSocketService();

		activeMulticast =  new MulticastSocketService(
				(char*)(ACTIVE_MCAST_ADDR.c_str()),
				BROADCAST_PORT + CT_ACTIVE);
	}

	ActiveDeviceDomain::~ActiveDeviceDomain() {

	}

	void ActiveDeviceDomain::postConnectionRequestTask(int w, int h) {
		char* task;
		int connReqPayloadSize = 5;
		int taskSize;

		//TODO: offer a configure way in requests connection to CT_ACTIVE devs

		/*cout << "ActiveDeviceDomain::postConnectionRequestTask";
		cout << endl;*/

		//prepare frame
		task = mountFrame(
				myIP, CT_BASE, FT_CONNECTIONREQUEST, connReqPayloadSize);

		task[HEADER_SIZE] = deviceClass;

		task[HEADER_SIZE + 1] = w & 0xFF;
		task[HEADER_SIZE + 2] = (w & 0xFF00) >> 8;

		task[HEADER_SIZE + 3] = h & 0xFF;
		task[HEADER_SIZE + 4] = (h & 0xFF00) >> 8;

		taskSize = HEADER_SIZE + connReqPayloadSize;
		broadcastTaskRequest(task, taskSize);
	}

	void ActiveDeviceDomain::receiveAnswerTask(char* task) {
		unsigned int taskIP;

		taskIP = getUIntFromStream(task);
		if (taskIP != myIP) {
			return; //this is'nt a warning
		}

		deviceService->connectedToBaseDevice(sourceIp);

		if (connected) {
			cout << "ActiveDeviceDomain::receiveAnswerTask Warning! ";
			cout << "received an answer task in connected state" << endl;
		}

		//TODO: check if central domain IP + port received in task is correct
		//cout << "ActiveDeviceDomain::receiveAnswerTask Connected with ";
		//cout << "base multi-device domain" << endl;
		connected = true;
	}

	bool ActiveDeviceDomain::receiveMediaContentTask(char* task) {
		/*cout << "ActiveDeviceDomain::receiveMediaContentTask ";
		cout << "destcass = '" << destClass << "'" << endl;*/
		return deviceService->receiveMediaContent(
				sourceIp, task, this->frameSize);
	}
}
}
}
}
}
}
