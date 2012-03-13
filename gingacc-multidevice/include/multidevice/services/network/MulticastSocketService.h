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

#ifndef _MulticastSocketService_H_
#define _MulticastSocketService_H_

#include "ISocketService.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <net/if.h>

#ifdef __DARWIN_UNIX03
#include <ifaddrs.h>
#define inaddrr(x) (*(struct in_addr *) myAddr->x[sizeof sa.sin_port])
#endif

#include <pthread.h>
#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
  class MulticastSocketService : public ISocketService {
	private:
		unsigned int port;
		char* gAddr;

		int msdR, msdW;  // msd = multicast socket descriptor
		sockaddr_in mss; // mss = multicast socket struct

		pthread_mutex_t mutexBuffer;
		vector<struct frame*>* outputBuffer;

	public:
		MulticastSocketService(char*, unsigned int portNumber);
		~MulticastSocketService();

	private:
		int createMulticastGroup();
		bool createSocket();
		bool addToGroup();
		bool setSocketOptions();
		bool tryToBind();

	public:
		unsigned int getInterfaceIPAddress(){return 0;};
		int getServicePort();
		void dataRequest(char* data, int taskSize, bool repeat=true);

	private:
		bool sendData(struct frame* f);

	public:
		bool checkOutputBuffer();
		bool checkInputBuffer(char* data, int* size);
  };
}
}
}
}
}
}

#endif /*_MulticastSocketService_H_*/
