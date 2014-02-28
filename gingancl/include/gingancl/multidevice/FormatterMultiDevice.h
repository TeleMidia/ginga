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

#ifndef _FormatterMultiDevice_H_
#define _FormatterMultiDevice_H_

#include "config.h"

#include "mb/ILocalScreenManager.h"
#include "mb/interface/IInputEventListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#if HAVE_MULTIDEVICE
#include "multidevice/services/IDeviceDomain.h"
#include "multidevice/IRemoteDeviceManager.h"
#include "multidevice/IRemoteDeviceListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::multidevice;
#else
class IDeviceDomain {
	public:
		//CT: class types
		static const int CT_BASE              = 0;
		static const int CT_PASSIVE           = 1;
		static const int CT_ACTIVE            = 2;

		//FT: frame types
		static const int FT_CONNECTIONREQUEST = 0;
		static const int FT_ANSWERTOREQUEST   = 1;
		static const int FT_KEEPALIVE         = 2;
		static const int FT_MEDIACONTENT      = 3;
		static const int FT_SELECTIONEVENT    = 4;
		static const int FT_ATTRIBUTIONEVENT  = 5;
		static const int FT_PRESENTATIONEVENT = 6;
};
#endif //HAVE_MULTIDEVICE

#include "ncl/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl/components/Content.h"
#include "ncl/components/NodeEntity.h"
#include "ncl/components/ReferenceContent.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "gingancl/model/CascadingDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "gingancl/model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "gingancl/model/FormatterLayout.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "gingancl/adaptation/context/ContextBase.h"

#include "IFormatterMultiDevice.h"

#include <pthread.h>

#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace multidevice {
#if HAVE_MULTIDEVICE
	class FormatterMultiDevice :
			public IFormatterMultiDevice,
			public IInputEventListener,
			public IRemoteDeviceListener {
#else //!HAVE_MULTIDEVICE
	class FormatterMultiDevice :
			public IFormatterMultiDevice,
			public IInputEventListener {
#endif //HAVE_MULTIDEVICE
		protected:
			static ILocalScreenManager* dm;
			IInputManager* im;
			pthread_mutex_t mutex;

#if HAVE_MULTIDEVICE
			static IRemoteDeviceManager* rdm;
#else
			static void* rdm;
#endif //HAVE_MULTIDEVICE

			IDeviceLayout* deviceLayout;
			map<int, FormatterLayout*> layoutManager;
			vector<string>* activeUris;
			string activeBaseUri;
			FormatterLayout* mainLayout;
			IWindow* serialized;
			IWindow* printScreen;
			IWindow* bitMapScreen;
			int xOffset;
			int yOffset;
			int defaultWidth;
			int defaultHeight;
			int deviceClass;
			bool hasRemoteDevices;
			bool enableMulticast;
			IPresentationContext* presContext;
			IFormatterFocusManager* focusManager;
			FormatterMultiDevice* parent;
			GingaScreenID myScreen;

			set<IPlayer*> listening;
			pthread_mutex_t lMutex;

			static const int DV_QVGA_WIDTH  = 480;
			static const int DV_QVGA_HEIGHT = 320;

		public:
			FormatterMultiDevice(
					GingaScreenID screenId,
					IDeviceLayout* deviceLayout,
					int x, int y, int w, int h,
					bool useMulticast, int srvPort);

			virtual ~FormatterMultiDevice();

			void printGingaWindows();
			void listenPlayer(IPlayer* player);
			void stopListenPlayer(IPlayer* player);
			void setParent(IFormatterMultiDevice* parent);
			void setPresentationContex(IPresentationContext* presContext);
			void setFocusManager(IFormatterFocusManager* focusManager);
			void setBackgroundImage(string uri);
			void* getMainLayout();
			void* getFormatterLayout(int devClass);
			string getScreenShot();
			string serializeScreen(int devClass, IWindow* mapWindow);

		protected:
			virtual void postMediaContent(int destDevClass);

		public:
			FormatterLayout* getFormatterLayout(
					CascadingDescriptor* descriptor, ExecutionObject* object);

			GingaWindowID prepareFormatterRegion(
					ExecutionObject* object, ISurface* renderedSurface);

			void showObject(ExecutionObject* executionObject);
			void hideObject(ExecutionObject* executionObject);

		protected:
			virtual bool newDeviceConnected(int newDevClass, int w, int h);
			virtual void connectedToBaseDevice(unsigned int domainAddr)=0;

			virtual bool receiveRemoteEvent(
					int remoteDevClass,
					int eventType,
					string eventContent);

			virtual bool receiveRemoteContent(
					int remoteDevClass,
					char *stream, int streamSize) {

				return false;
			};

			virtual bool receiveRemoteContentInfo(
					string contentId, string contentUri) {

				return false;
			};

			void renderFromUri(IWindow* win, string uri);
			void tapObject(int devClass, int x, int y);

			virtual bool receiveRemoteContent(
					int remoteDevClass,
					string contentUri) {

				return false;
			};

		public:
			void addActiveUris(string baseUri, vector<string>* uris);
			virtual void updatePassiveDevices();

		protected:
			void updateStatus(
					short code, string parameter, short type, string value);

			virtual bool userEventReceived(IInputEvent* ev)=0;
	};
}
}
}
}
}
}

#endif /* _FormatterMultiDevice_H_ */
