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
	Process::Process(char** argv) {
		this->pid           = -1;
		this->argv          = argv;
		this->envp          = environ;
		this->processUri    = processUri;
		this->processStatus = PST_NULL;
		this->sigListener   = NULL;
		this->reader        = false;
		this->wFd           = -1;
		this->rFd           = -1;
		this->hasCom        = false;

		posix_spawnattr_init(&spawnAttr);
		posix_spawn_file_actions_init(&fileActions);

		isCheckingCom = false;
		pthread_mutex_init(&comMutex, NULL);
		pthread_cond_init(&comCond, NULL);

		isSpawnedReady = false;
	}

	Process::~Process() {
		cout << "Process::~Process(" << this;
		cout << ")" << endl;

		sigListener = NULL;

		release();

		pthread_mutex_destroy(&comMutex);
		pthread_cond_destroy(&comCond);

		posix_spawnattr_destroy(&spawnAttr);
		posix_spawn_file_actions_destroy(&fileActions);
	}

	void Process::release() {
		tryCom();
		forceKill();

		hasCom         = false;
		isSpawnedReady = false;
		processStatus  = PST_NULL;

		close(wFd);
		close(rFd);

		unlink(wCom.c_str());
		unlink(rCom.c_str());
	}

	void Process::setProcessInfo(string processUri, string objName) {
		pthread_t threadId_;
		string processName;

		this->processUri = processUri;
		this->objName    = objName;

		if (processUri.find("/") != std::string::npos) {
			processName = processUri.substr(
					processUri.find_last_of("/") + 1,
					processUri.length() - processUri.find_last_of("/") + 1);

		} else {
			processName = processUri;
		}

		rCom = ("/tmp/ginga/_" + processName +
				itos((long int)(void*)this) + "_ctop");

		wCom = ("/tmp/ginga/_" + processName +
				itos((long int)(void*)this) + "_ptoc");

		pthread_create(&threadId_, 0, Process::createFiles, this);
		pthread_detach(threadId_);
	}

	int Process::createShm(string shmName, bool truncateFile, int shmSize) {
		int fd = shm_open(shmName.c_str(), O_CREAT | O_RDWR, S_IRWXU);
		if (fd == -1) {
			cout << "Process::createShm can't open shm file" << endl;
			return fd;
		}

		if (truncateFile && ftruncate(fd, shmSize) == -1) {
			cout << "Process::createShm can't truncate shm file" << endl;
			close(fd);
			shm_unlink(shmName.c_str());
			return -1;
		}

		return fd;
	}

	void Process::checkCom() {
		if (wFd > 0 && rFd > 0 && isSpawnedReady) {
			return;
		}

		isCheckingCom = true;
		pthread_mutex_lock(&comMutex);
		pthread_cond_wait(&comCond, &comMutex);
		isCheckingCom = false;
		pthread_mutex_unlock(&comMutex);
	}

	void Process::tryCom() {
		if (wFd > 0 && rFd > 0 && isSpawnedReady && isCheckingCom) {
			pthread_cond_signal(&comCond);
		}
	}

	bool Process::sendMsg(string msg) {
		return sendMsg(wFd, msg);
	}

	bool Process::sendMsg(int fd, string msg) {
		int rval;

		if (fd >= 0) {
			rval = write(fd, msg.c_str(), msg.length());
			if (rval == msg.length()) {
				return true;
			}
		}
		return false;
	}

	void Process::messageReceived(string msg) {
		cout << "Process::messageReceived '" << msg << "'" << endl;
	}

	string Process::receiveMsg(int fd) {
		int rval;
		char buff[512];
		string msg = "";

		rval = read(fd, buff, sizeof(buff));
		if (rval > 0) {
			msg.assign(buff, rval);
		}

		return msg;
	}

	int Process::openW(string wName) {
		int fd = open(wName.c_str(), O_WRONLY);
		if (fd < 0) {
			cout << "Process::openW Warning! ";
			cout << "can't open '" << wName << "'";
			cout << endl;
		}

		return fd;
	}

	int Process::openR(string rName) {
		int fd = open(rName.c_str(), O_RDONLY);
		if (fd < 0) {
			cout << "Process::openR Warning! ";
			cout << "can't open '" << rName << "'";
			cout << endl;
		}

		return fd;
	}

	void Process::setProcessListener(IProcessListener* listener) {
		sigListener = listener;
	}

	void Process::run() {
		pthread_t threadId_;
		pthread_t threadIdR_;
		int rspawn;

		while (!hasCom) {
			::usleep(10000);
		}

		if (processStatus == PST_NULL) {
			if (argv == NULL) {
				argv = envp;
			}
			argv[0] = (char*)objName.c_str();
			argv[1] = (char*)rCom.c_str();
			argv[2] = (char*)wCom.c_str();

			rspawn  = posix_spawn(
					&pid,
					processUri.c_str(),
					&fileActions,
					&spawnAttr,
					argv,
					envp);

			if (rspawn == 0) {
				cout << "Process::run process '";
				cout << processUri << "' successfully spawned" << endl;
				reader        = true;
				processStatus = PST_RUNNING;
				pthread_create(&threadId_, 0, Process::detachWait, this);
				pthread_detach(threadId_);

				pthread_create(&threadIdR_, 0, Process::detachReceive, this);
				pthread_detach(threadIdR_);

			} else {
				cout << "Process::run Warning! Can't spawn process '";
				cout << processUri << "'" << endl;
			}
		}
	}

	void Process::forceKill() {
		int rkill;

		sigListener = NULL;
		reader      = false;
		rkill       = kill(pid, SIGKILL);
		if (rkill != 0) {
			perror("forceKill");
		}
	}

	void Process::spawnedReady(bool ready) {
		isSpawnedReady = ready;

		tryCom();
	}

	void* Process::createFiles(void* ptr) {
		int rval;
		Process* process = (Process*)ptr;

		cout << "Process::createFiles(" << process << ")";
		cout << " creating wCom: '" << process->wCom << "'" << endl;
		if (fileExists(process->wCom)) {
			cout << "Process::createFiles(" << process << ") File '";
			cout << process->wCom << "' already exists" << endl;
			unlink(process->wCom.c_str());
		}

		rval = mkfifo(process->wCom.c_str(), S_IFIFO);
		if (rval < 0 && !fileExists(process->wCom)) {
			cout << "Process::createFiles Warning! ";
			perror("wCom");
			cout << "can't create wCom pipe '" << process->wCom << "'";
			cout << endl;
			return NULL;
		}

		cout << "Process::createFiles(" << process << ")";
		cout << " creating rCom: '" << process->rCom << "'" << endl;
		if (fileExists(process->rCom)) {
			cout << "Process::createFiles(" << process << ") File '";
			cout << process->rCom << "' already exists" << endl;
			unlink(process->rCom.c_str());

		}

		rval = mkfifo(process->rCom.c_str(), S_IFIFO);
		if (rval < 0) {
			cout << "Process::createFiles Warning! ";
			perror("rCom");
			cout << "can't create rCom pipe '" << process->rCom << "'";
			cout << endl;
			return NULL;
		}

		process->hasCom = true;
		process->wFd = openW(process->wCom);
		if (process->wFd < 0) {
			cout << "Process::createFiles Warning! ";
			cout << "can't open '" << process->wCom << "'";
			cout << endl;
		}

		process->tryCom();
		return NULL;
	}

	void* Process::detachWait(void* ptr) {
		Process* process = (Process*)ptr;
		int status;
		int type;
		int wpid;
		int ppid;
		IProcessListener* listener;

		ppid = process->pid;
		while (true) {
			wpid = waitpid(ppid, &status, WUNTRACED);
			if (wpid == ppid) {
				break;
			}
		}
		listener = process->sigListener;

        if (WIFEXITED(status)) {
        	cout << "Process::detachWait process '" << process->processUri;
        	cout << "' exited with exit status '" << WEXITSTATUS(status);
        	cout << "'" << endl;
        	type = IProcessListener::PST_EXIT_OK;

        } else if (WIFSTOPPED(status)) {
        	cout << "Process::detachWait process '" << process->processUri;
        	cout << "' has not terminated correctly: '" << WSTOPSIG(status);
        	cout << endl;

        	type = IProcessListener::PST_EXIT_ERROR;

        } else {
        	type = IProcessListener::PST_EXEC_SIGNAL;
        }

		if (listener != NULL) {
			listener->receiveProcessSignal(type, status, ppid);
		}

		return NULL;
	}

	void* Process::detachReceive(void* ptr) {
		string msg;
		Process* process = (Process*)ptr;

		process->rFd = openR(process->rCom);
		if (process->rFd < 0) {
			cout << "Process::detachReceive Warning! ";
			cout << "can't open '" << process->rCom << "'";
			cout << endl;
			return NULL;
		}

		process->tryCom();

		while (process->reader) {
			msg = receiveMsg(process->rFd);
			if (msg == "ready") {
				process->spawnedReady(true);

			} else if (msg != "") {
				process->messageReceived(msg);
			}
		}

		return NULL;
	}
}
}
}
}
}
}
}
