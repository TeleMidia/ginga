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

#ifndef _IFormatterMultiDevice_H_
#define _IFormatterMultiDevice_H_

#include "ncl/layout/IDeviceLayout.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "player/IPlayer.h"
#include "player/IPlayerListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "gingancl/adaptation/context/IPresentationContext.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adaptation::context;

#include "gingancl/focus/IFormatterFocusManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::focus;

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace multidevice {
	class IFormatterMultiDevice : public IPlayerListener {
		public:
			virtual ~IFormatterMultiDevice(){};

			virtual void printGingaWindows()=0;
			virtual void listenPlayer(IPlayer* player)=0;
			virtual void stopListenPlayer(IPlayer* player)=0;

			virtual void setPresentationContex(
					IPresentationContext* presContext)=0;

			virtual void setFocusManager(
					IFormatterFocusManager* focusManager)=0;

			virtual void setParent(IFormatterMultiDevice* parent)=0;
			virtual void setBackgroundImage(string uri)=0;
			virtual void* getMainLayout()=0;
			virtual void* getFormatterLayout(int devClass)=0;
			virtual string getScreenShot()=0;
			virtual string serializeScreen(int devClass, IWindow* mapWindow)=0;

		protected:
			virtual bool newDeviceConnected(int newDevClass, int w, int h)=0;
			virtual void connectedToBaseDevice(unsigned int domainAddr)=0;

			virtual bool receiveRemoteEvent(
					int remoteDevClass,
					int eventType,
					string eventContent)=0;

			virtual bool receiveRemoteContent(
					int remoteDevClass,
					char *stream, int streamSize)=0;

			virtual bool receiveRemoteContent(
					int remoteDevClass,
					string contentUri)=0;

		public:
			virtual void addActiveUris(string baseUri, vector<string>* uris)=0;
			virtual void updatePassiveDevices()=0;

		protected:
			virtual void updateStatus(
					short code,
					string parameter,
					short type,
					string value)=0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::ncl::multidevice::
		IFormatterMultiDevice* FormatterMultiDeviceCreator(
				GingaScreenID screenId,
				IDeviceLayout* deviceLayout,
				int devClass,
				string playerId,
				int x,
				int y,
				int w,
				int h,
				bool deviceSearch, int srvPort);

typedef void FormatterMultiDeviceDescroyer(
		::br::pucrio::telemidia::ginga::ncl::multidevice::
				IFormatterMultiDevice* fmd);

#endif /* _IFormatterMultiDevice_H_ */
