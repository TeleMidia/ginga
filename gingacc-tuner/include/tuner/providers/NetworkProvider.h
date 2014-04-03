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

#ifndef NETWORKPROVIDER_H_
#define NETWORKPROVIDER_H_

#include "system/compat/SystemCompat.h"
#include "system/compat/PracticalSocket.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include <iostream>
#include <fstream>
using namespace std;

#include "tuner/providers/IDataProvider.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	class NetworkProvider : public IDataProvider {
		protected:
			string addr;
			string protocol;
			int portNumber;
			short capabilities;
			UDPSocket *udpSocket;

		public:
			NetworkProvider(string address, int port, string protocol);
			~NetworkProvider();

			virtual void setListener(ITProviderListener* listener){};
			virtual void attachFilter(IFrontendFilter* filter){};
			virtual void removeFilter(IFrontendFilter* filter){};

			virtual short getCaps() {
				return capabilities;
			};

			virtual bool tune() {
				if (callServer() > 0) {
					return true;
				}

				return false;
			};

			virtual IChannel* getCurrentChannel() {
				return NULL;
			}

			virtual bool getSTCValue(uint64_t* stc, int* valueType) {
				return false;
			}

			virtual bool changeChannel(int factor) {
				return false;
			}

			bool setChannel(string channelValue) {
				return false;
			}

			virtual int createPesFilter(
					int pid, int pesType, bool compositeFiler) {

				return -1;
			}

			virtual string getPesFilterOutput() {
				return "";
			}

			virtual void close() {};

			virtual int callServer();
			virtual int receiveData(char* buff, int skipSize, unsigned char packetSize);
	};
}
}
}
}
}
}

#endif /*NETWORKPROVIDER_H_*/
