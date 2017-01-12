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

#ifndef _IFormatterMultiDevice_H_
#define _IFormatterMultiDevice_H_

#include "ncl/layout/IDeviceLayout.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "player/IPlayer.h"
#include "player/IPlayerListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "adaptation/IPresentationContext.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adaptation::context;

#include "IFormatterFocusManager.h"
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
			virtual string serializeScreen(int devClass, GingaWindowID mapWindow)=0;

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
