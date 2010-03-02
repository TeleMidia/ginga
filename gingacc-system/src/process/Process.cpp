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

#include "../../include/process/Process.h"

#include <pthread.h>
#include <iostream>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace process {
	Process::Process(string processUri, char** argv, char** envp) {
		this->pid           = -1;
		this->argv          = argv;
		this->envp          = envp;
		this->processUri    = processUri;
		this->processStatus = PST_NULL;
		this->sigListener   = NULL;

		posix_spawnattr_init(&spawnAttr);
		posix_spawn_file_actions_init(&fileActions);
	}

	Process::~Process() {
		posix_spawnattr_destroy(&spawnAttr);
		posix_spawn_file_actions_destroy(&fileActions);
	}

	bool Process::sendProcessObject(string objName, void* obj, int objSize) {

	}

	void* Process::receiveProcessObject(string objectName) {

	}

	void Process::setSignalListener(IProcessListener* listener) {
		sigListener = listener;
	}

	void Process::run() {
		pthread_t threadId_;
		int rspawn;

		if (processStatus == PST_NULL) {
			rspawn = posix_spawn(
					&pid,
					processUri.c_str(),
					&fileActions,
					&spawnAttr,
					argv,
					envp);

			if (rspawn == 0) {
				processStatus = PST_RUNNING;
				pthread_create(&threadId_, 0, Process::detachWait, this);
				pthread_detach(threadId_);
			}
		}
	}

	void Process::forceKill() {
		int rkill;

		rkill = kill(pid, SIGKILL);
	}

	void* Process::detachWait(void* ptr) {
		Process* process = (Process*)ptr;
		int status;
		int wpid;

		wpid = waitpid(process->pid, &status, WUNTRACED | WCONTINUED);

        if (WIFEXITED(status)) {
        	cout << "Process::detachWait process '" << process->processUri;
        	cout << "' exited with exit status '" << WEXITSTATUS(status);
        	cout << "'" << endl;
        	status = IProcessListener::PST_EXIT_OK;

        } else {
        	cout << "Process::detachWait process '" << process->processUri;
        	cout << "' has not terminated correctly.";
        	cout << endl;

        	status = IProcessListener::PST_EXIT_ERROR;
        }

		if (process->sigListener != NULL) {
			process->sigListener->receiveProcessSignal(status, process);
		}
	}
}
}
}
}
}
}
}
