/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef __Process_h__
#define __Process_h__

#include "IProcessListener.h"

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <spawn.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <errno.h>
#include <string.h>

#include <iostream>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace process {
  class Process {
	private:
		static const short PST_NULL    = 0;
		static const short PST_RUNNING = 1;
		static const short PST_SDONE   = 2;
		static const short PST_UDONE   = 3;

		static const int SHM_SIZE      = 65536;

		pid_t pid;
		int comDesc;
		short processStatus;
		char** argv;
		char** envp;
		string processUri;
		posix_spawnattr_t spawnAttr;
		posix_spawn_file_actions_t fileActions;

		int shmDesc;

		string rCom;
		string wCom;
		FILE* wFd;
		FILE* rFd;

	protected:
		string objName;
		bool reader;

	private:
		bool isSpawnedReady;
		bool hasCom;

		bool isCheckingCom;
		pthread_mutex_t comMutex;
		pthread_cond_t comCond;

	protected:
		IProcessListener* sigListener;

	public:
		Process(char** argv);
		virtual ~Process();

	protected:
		void release();

	public:
		void setProcessInfo(string processUri, string objName);
		void checkCom();

	private:
		void tryCom();

	public:
		bool sendMsg(string msg);
		static bool sendMsg(FILE* fd, string msg);
		virtual void messageReceived(string msg);
		static string receiveMsg(FILE* fd);
		static FILE* openW(string wName);
		static FILE* openR(string rName);

		void setProcessListener(IProcessListener* listener);
		void run();
		void forceKill();

	private:
		void spawnedReady(bool ready);
		static void* createFiles(void* ptr);
		static void* detachWait(void* ptr);
		static void* detachReceive(void* ptr);
  };
}
}
}
}
}
}
}

#endif //__Process_h__
