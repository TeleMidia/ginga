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

#ifndef _FormatterActiveDevice_H_
#define _FormatterActiveDevice_H_
#include "FormatterMultiDevice.h"

#include "util/functions.h"
#include "util/Base64.h"
using namespace ::br::pucrio::telemidia::util;


#include "player/INCLPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "system/compat/SystemCompat.h"
#include "system/compat/PracticalSocket.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "gingancl/privatebase/IPrivateBaseManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <errno.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace multidevice {
	class FormatterActiveDevice : public FormatterMultiDevice {
		public:
			//TODO: here?
			static const unsigned int ADD_DOCUMENT        = 10081;
			static const unsigned int REMOVE_DOCUMENT     = 10082;
			static const unsigned int START_DOCUMENT      = 10083;
			static const unsigned int STOP_DOCUMENT       = 10084;
			static const unsigned int PAUSE_DOCUMENT      = 10085;
			static const unsigned int RESUME_DOCUMENT     = 10086;
			static const unsigned int SET_VAR             = 10087;
			static const unsigned int SELECTION           = 10088;

			//static const unsigned int COMMAND_SERVER_PORT = 22222;
			static const unsigned int RCVBUFSIZE          = 100;

			static const string IMG_DIR;
			static const string IMG_DEV;
			static const string IMG_RESET;
			static string TMP_DIR;

		private:
			TCPSocket* tcpSocket;
			int deviceServicePort;

			ISurface* s;

			map<string, string>* contentsInfo;
			map<string, string> initVars;

			string currentDocUri;
			IPrivateBaseManager* privateBaseManager;
			INCLPlayer* formatter;

			bool listening;

		public:
			FormatterActiveDevice(
					GingaScreenID screenId,
					IDeviceLayout* deviceLayout,
					int x, int y, int w, int h, bool useMulticast, int srvPort);

			virtual ~FormatterActiveDevice();

		protected:
			bool newDeviceConnected(int newDevClass, int w, int h) {
				return false;
			};

			bool socketSend(TCPSocket* sock, string payload);
			void connectedToBaseDevice(unsigned int domainAddr);
			bool receiveRemoteEvent(
					int remoteDevClass,
					int eventType,
					string eventContent);

			bool receiveRemoteContent(
					int remoteDevClass,
					char *stream, int streamSize) {

				return false;
			};

			bool receiveRemoteContent(int remoteDevClass, string contentUri);
			bool receiveRemoteContentInfo(string contentId, string contentUri);
			bool userEventReceived(IInputEvent* ev);

			bool openDocument(string contentUri);
			INCLPlayer* createNCLPlayer();
			NclPlayerData* createNclPlayerData();

			int getCommandCode(string *com);
			void handleTCPClient(TCPSocket *sock);
			bool handleTCPCommand(
					string sid,
					string snpt,
					string scommand,
					string spayload_desc,
					string payload);
	};
}
}
}
}
}
}

#endif /* _FormatterActiveDevice_H_ */
