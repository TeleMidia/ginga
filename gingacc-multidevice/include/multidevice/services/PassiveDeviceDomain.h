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

#ifndef _PassiveDeviceDomain_H_
#define _PassiveDeviceDomain_H_

#include "DeviceDomain.h"
#include "network/MulticastSocketService.h"
#include "network/BroadcastDualSocketService.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
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
		void setDeviceInfo(int width, int height);
		bool runControlTask();
		bool runDataTask();
		void checkDomainTasks();
  };
}
}
}
}
}
}

#endif /*_PassiveDeviceDomain_H_*/
