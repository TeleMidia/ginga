/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licen� Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribu�o na expectativa de que seja util, porem, SEM
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

#include "system/process/SpawnedProcess.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace process {
	SpawnedProcess::SpawnedProcess(string objName, string wCom, string rCom) {
		pthread_t threadId_;

		this->objectName = objName;
		this->wCom       = wCom;
		this->rCom       = rCom;

		wFd = Process::openW(wCom);
		if (wFd < 0) {
			clog << "SpawnedProcess::SpawnedProcess Warning! ";
			clog << "Can't openW '" << wCom << "'" << endl;
		}

		rFd = Process::openR(rCom);
		if (rFd < 0) {
			clog << "SpawnedProcess::SpawnedProcess Warning! ";
			clog << "Can't openR '" << rCom << "'" << endl;
		}

		pthread_mutex_init(&waitSig, NULL);

		reader = true;
		pthread_create(&threadId_, 0, SpawnedProcess::detachReceive, this);
		pthread_detach(threadId_);
	}

	SpawnedProcess::~SpawnedProcess() {
		reader = false;

		if (wFd > 0) {
			close(wFd);
		}

		if (rFd > 0) {
			close(rFd);
		}

		pthread_mutex_destroy(&waitSig);
	}

	void* SpawnedProcess::detachReceive(void* ptr) {
		string msg;
		SpawnedProcess* sp = (SpawnedProcess*)ptr;

		while (sp->reader) {
			msg = Process::receiveMsg(sp->rFd);
			if (msg != "") {
				sp->messageReceived(msg);
			}
		}

		return sp;
	}

	void SpawnedProcess::sendMessage(string msg) {
		Process::sendMsg(wFd, msg);
	}

	void SpawnedProcess::messageReceived(string msg) {
		clog << "SpawnedProcess::messageReceived '" << msg << "'" << endl;
	}

	void SpawnedProcess::waitSignal() {
		pthread_mutex_lock(&waitSig);
		pthread_mutex_lock(&waitSig);
	}
}
}
}
}
}
}
}
