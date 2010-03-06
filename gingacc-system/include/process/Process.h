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

#ifndef __Process_h__
#define __Process_h__

#include "IProcessListener.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
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
		string objName;
		posix_spawnattr_t spawnAttr;
		posix_spawn_file_actions_t fileActions;

		int shmDesc;

		string rCom;
		string wCom;
		int wFd;
		int rFd;
		bool reader;

		bool isSpawnedReady;

		bool isCheckingCom;
		pthread_mutex_t comMutex;
		pthread_cond_t comCond;

		IProcessListener* sigListener;

	public:
		Process(string processUri, string objName, char** argv);
		virtual ~Process();

		static int createShm(string shmName, bool truncateFile, int shmSize);
		void checkCom();

	private:
		void tryCom();

	public:
		bool sendMsg(string msg);
		static bool sendMsg(int fd, string msg);
		virtual void messageReceived(string msg);
		static string receiveMsg(int fd);
		static int openW(string wName);
		static int openR(string rName);

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
