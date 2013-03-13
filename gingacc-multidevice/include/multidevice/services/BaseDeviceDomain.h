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

#ifndef _BaseDeviceDomain_H_
#define _BaseDeviceDomain_H_

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "network/BroadcastSocketService.h"
#include "network/BroadcastDualSocketService.h"

#include "device/BaseDeviceService.h"

#include "DeviceDomain.h"

#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {

typedef struct {
	char* data;
	int size;
	double timestamp;
} RemoteTask;

  class BaseDeviceDomain : public DeviceDomain {
	protected:
	  ISocketService* passiveSocket;

	  pthread_mutex_t pMutex;
	  vector<RemoteTask*> passiveTasks;
	  RemoteTask lastMediaContentTask;
	  bool hasNewPassiveTask;
	  int timerCount;
	  double passiveTimestamp;

	public:
		BaseDeviceDomain(bool useMulticast, int srvPort);
		virtual ~BaseDeviceDomain();

	protected:
		virtual bool taskRequest(int destDevClass, char* data, int taskSize);
		virtual bool passiveTaskRequest(char* data, int taskSize);
		virtual bool activeTaskRequest(char* data, int taskSize);

		virtual void postConnectionRequestTask(int width, int height){};
		virtual void receiveConnectionRequest(char* task);
		virtual void postAnswerTask(int reqDeviceClass, int answer);
		virtual void receiveAnswerTask(char* answerTask){};

	public:
		virtual bool postMediaContentTask(int destDevClass, string url);

	protected:
		virtual bool receiveMediaContentTask(char* task){return false;};
		virtual bool receiveEventTask(char* task);

	public:
		virtual void setDeviceInfo(int width, int height);

	protected:
		virtual bool runControlTask();
		virtual bool runDataTask();
		virtual void checkPassiveTasks();
		virtual void checkDomainTasks();
  };
}
}
}
}
}
}

#endif /*_BaseDeviceDomain_H_*/
