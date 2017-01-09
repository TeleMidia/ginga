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

#ifndef FORMATTERFOCUSMANAGER_H_
#define FORMATTERFOCUSMANAGER_H_

#include "util/functions.h"
#include "util/Color.h"

#include "mb/IMBDefs.h"
#include "mb/IInputManager.h"
#include "mb/interface/IInputEventListener.h"
#include "mb/interface/CodeMap.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "../model/CascadingDescriptor.h"
#include "../model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "../model/SelectionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "../model/ILinkActionListener.h"
#include "../model/LinkSimpleAction.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "../model/FormatterRegion.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "../adaptation/context/IPresentationContext.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adaptation::context;

#include "../adapters/FormatterPlayerAdapter.h"
#include "../adapters/PlayerAdapterManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "../multidevice/IFormatterMultiDevice.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::multidevice;

#include "IFormatterFocusManager.h"

#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace focus {
	class FormatterFocusManager :
			public IFormatterFocusManager,
			public IInputEventListener {

		private:
			IInputManager* im;

			GingaScreenID myScreen;
			IPresentationContext* presContext;
			map<string, set<ExecutionObject*>*>* focusTable;
			bool isHandler;
			string currentFocus;
			string objectToSelect;
			ExecutionObject* selectedObject;

			Color* defaultFocusBorderColor;
			int defaultFocusBorderWidth;
			Color* defaultSelBorderColor;
			PlayerAdapterManager* playerManager;
			FormatterFocusManager* parentManager;
			IFormatterMultiDevice* multiDevice;
			ILinkActionListener* settingActions;

			int xOffset;
			int yOffset;
			int width;
			int height;

			double focusHandlerTS;

			void* converter;
			pthread_mutex_t mutexFocus;
			pthread_mutex_t mutexTable;

			static bool init;
			static set<FormatterFocusManager*> instances;
			static pthread_mutex_t iMutex;

		public:
			FormatterFocusManager(
					PlayerAdapterManager* playerManager,
					IPresentationContext* presContext,
					IFormatterMultiDevice* multiDevice,
					ILinkActionListener* settingActions,
					void* converter);

			virtual ~FormatterFocusManager();

		private:
			static void checkInit();
			static bool hasInstance(
					FormatterFocusManager* instance, bool remove);

		public:
			void setParent(FormatterFocusManager* parent);
			bool isKeyHandler();
			bool setKeyHandler(bool isHandler);

		private:
			void setHandlingObjects(bool isHandling);
			void resetKeyMaster();

		public:
			void tapObject(void* executionObject);
			void setKeyMaster(string mediaId);
			//void setStandByState(bool standBy);
			void setFocus(string focusIndex);

		private:
			void setFocus(CascadingDescriptor* descriptor);
			void recoveryDefaultState(ExecutionObject* object);

		public:
			void showObject(ExecutionObject* object);
			void hideObject(ExecutionObject* object);

		private:
			ExecutionObject* getObjectFromFocusIndex(string focusIndex);
			void insertObject(ExecutionObject* object, string focusIndex);
			void removeObject(ExecutionObject* object, string focusIndex);

			bool keyCodeOk(ExecutionObject* currentObject);
			bool keyCodeBack();
			bool enterSelection(FormatterPlayerAdapter* player);
			void exitSelection(FormatterPlayerAdapter* player);
			void registerNavigationKeys();
			void registerBackKeys();
			void unregister();

		public:
			void setDefaultFocusBorderColor(Color* color);
			void setDefaultFocusBorderWidth(int width);
			void setDefaultSelBorderColor(Color* color);

			void setMotionBoundaries(int x, int y, int w, int h);

		private:
			void changeSettingState(string name, string act);

		public:
			bool userEventReceived(IInputEvent* ev);
			bool motionEventReceived(int x, int y, int z);
	};
}
}
}
}
}
}

#endif /*FORMATTERFOCUSMANAGER_H_*/
