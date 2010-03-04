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

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <pthread.h>

extern char **environ;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace process {
	Process::Process(string processUri, char** argv) {
		pthread_t threadId_;

		this->pid           = -1;
		this->argv          = argv;
		this->envp          = environ;
		this->processUri    = processUri;
		this->processStatus = PST_NULL;
		this->sigListener   = NULL;
		this->reader        = true;

		rCom = processUri + itos((long int)(void*)this) + "_ctop";
		wCom = processUri + itos((long int)(void*)this) + "_ptoc";

		posix_spawnattr_init(&spawnAttr);
		posix_spawn_file_actions_init(&fileActions);

		pthread_create(&threadId_, 0, Process::createFiles, this);
		pthread_detach(threadId_);

		cout << "Process::Process created with rCom = '" << rCom << "'";
		cout << " and wCom = '" << wCom << "'";
		cout << endl;
	}

	Process::~Process() {
		reader = false;

		posix_spawnattr_destroy(&spawnAttr);
		posix_spawn_file_actions_destroy(&fileActions);
	}

	bool Process::sendMsg(string msg) {
		pthread_mutex_lock(&comMutex);
		sendMsg(0, msg);
		pthread_mutex_unlock(&comMutex);

		return true;
	}

	bool Process::sendMsg(int fd, string msg) {
		return true;
	}

	void Process::setProcessListener(IProcessListener* listener) {
		sigListener = listener;
	}

	void Process::run() {
		pthread_t threadId_;
		int rspawn;

		if (processStatus == PST_NULL) {
			argv[0] = (char*)rCom.c_str();
			argv[1] = (char*)wCom.c_str();

			rspawn  = posix_spawn(
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

		reader = false;
		rkill  = kill(pid, SIGKILL);
	}

	void* Process::createFiles(void* ptr) {
		Process* process = (Process*)ptr;

		rval = mkfifo(process->wCom.c_str(), S_IFIFO);
		rval = mkfifo(process->rCom.c_str(), S_IFIFO);

		process->wFd = open(process->wCom.c_str(), O_WRONLY);
	}

	void* Process::detachWait(void* ptr) {
		Process* process = (Process*)ptr;
		int status;
		int type;
		int wpid;

		wpid = waitpid(process->pid, &status, WUNTRACED | WCONTINUED);

        if (WIFEXITED(status)) {
        	cout << "Process::detachWait process '" << process->processUri;
        	cout << "' exited with exit status '" << WEXITSTATUS(status);
        	cout << "'" << endl;
        	type = IProcessListener::PST_EXIT_OK;

        } else if (WIFSTOPPED(status)) {
        	cout << "Process::detachWait process '" << process->processUri;
        	cout << "' has not terminated correctly.";
        	cout << endl;

        	type = IProcessListener::PST_EXIT_ERROR;

        } else {
        	type = IProcessListener::PST_EXEC_SIGNAL;
        }

		if (process->sigListener != NULL) {
			process->sigListener->receiveProcessSignal(type, status, process);
		}

		if (type == IProcessListener::PST_EXEC_SIGNAL) {
			detachWait(ptr);
		}
	}

	void* Process::detachReceive(void* ptr) {
		Process* process = (Process*)ptr;

		process->rFd = open(process->rCom.c_str(), O_RDONLY);
	}
}
}
}
}
}
}
}
