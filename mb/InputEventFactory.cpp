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

#include "config.h"

#include "InputEventFactory.h"
#include "IInputManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#if HAVE_KINECTSUPPORT
#include "CodeMap.h"

#include <XnCppWrapper.h>
#include <XnVSessionManager.h>
#include <XnVPushDetector.h>
#include <XnVBroadcaster.h>
#include <XnVSelectableSlider2D.h>

#define TRACKING_FILE SystemCompat::appendGingaFilesPrefix("mb/config/_tracking.xml")

xn::Context context;
xn::ScriptNode scriptNode;

XnVSessionManager* sessionmanager = NULL;
XnVSelectableSlider2D* sel2d;

//slider variables
float oldX = -1;
float oldY = -1;
bool leftc = false;
bool rightc = false;
bool upc = false;
bool downc = false;

//multiple hands
int handsNumber = 0;

//call backs
void XN_CALLBACK_TYPE MainSlider_OnValueChange(
		XnFloat fXValue, XnFloat fYValue, void* ctx) {

	float difX;
	float difY;
	float initialX = 0.5;
	float initialY = 0.5;
	float margin = 0.15;

	int keyCode = CodeMap::KEY_NULL;

	if (handsNumber == 1) {
		if (oldX == -1) {
			oldX = fXValue;
			//initialX = fXValue;
		}

		if (oldY == -1) {
			oldY = fYValue;
			//initialY = fYValue;
		}

		difX = fXValue - initialX;
		difY = fYValue - initialY;

		if ((fXValue > initialX - margin)&&(fXValue < initialX + margin)){
			leftc = false;
			rightc = false;
		}
		if ((fYValue > initialY - margin)&&(fYValue < initialY + margin)){
			upc = false;
			downc = false;
		}
		if ((!rightc) && (difX > 0) && (difX > margin)) {
			rightc = true;
			leftc = false;
			oldX = fXValue;
			oldY = fYValue;
			keyCode = CodeMap::KEY_CURSOR_RIGHT;

		} else if ((!leftc) && (difX < 0) && (difX < -margin)) {
			leftc = true;
			rightc = false;
			oldX = fXValue;
			oldY = fYValue;
			keyCode = CodeMap::KEY_CURSOR_LEFT;

		} else if ((!upc) && (difY > 0) && (difY > margin)) {
			upc = true;
			downc = false;
			oldX = fXValue;
			oldY = fYValue;
			keyCode = CodeMap::KEY_CURSOR_UP;

		} else if ((!downc) && (difY < 0) && (difY < -margin)) {
			downc = true;
			upc = false;
			oldX = fXValue;
			oldY = fYValue;
			keyCode = CodeMap::KEY_CURSOR_DOWN;
		}

		if (keyCode != CodeMap::KEY_NULL && ctx != NULL) {
			((IInputManager*)ctx)->postInputEvent(keyCode);
		}
	}
}

void XN_CALLBACK_TYPE handsUpdate(const XnVMultipleHands& mh, void* ctx) {
	int oldnhands = handsNumber;

	handsNumber = mh.ActiveEntries();

	if (handsNumber == 2 && oldnhands != 2 && ctx != NULL) {
		((IInputManager*)ctx)->postInputEvent(CodeMap::KEY_BACK);
	}
}

void XN_CALLBACK_TYPE pushDetector(
		XnFloat fVelocity, XnFloat fAngle, void* ctx) {
	
	if (handsNumber == 1 && ctx != NULL) {
		((IInputManager*)ctx)->postInputEvent(CodeMap::KEY_ENTER);
	}
}

void XN_CALLBACK_TYPE sessionStart(
		const XnPoint3D& ptFocusPoint, void* UserCtx) {

	printf("Sessao iniciou.\n");
}

void XN_CALLBACK_TYPE sessionEnd(void* UserCtx) {
	printf("Sessao finalizou. Clique para iniciar.\n");
}

#endif //HAVE_KINECTSUPPORT

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	InputEventFactory::InputEventFactory() : Thread() {
		clear();
	}

	InputEventFactory::~InputEventFactory() {
		clear();
	}

	void InputEventFactory::clear() {
		iem         = NULL;
		running     = false;
		currentType = -1;
	}

	bool InputEventFactory::createFactory(
			short factoryType, IInputManager* ctx) {

		if (factoryType == FT_KINECT) {
#if !HAVE_KINECTSUPPORT
			return false;

#else //HAVE_KINECTSUPPORT
			if (!running) {
				currentType = FT_KINECT;
				running     = true;
				iem         = ctx;
				start();
			}
			return true;
#endif //!HAVE_KINECTSUPPORT
		}

		return false;
	}

	void InputEventFactory::run() {
		if (currentType == FT_KINECT) {
#if HAVE_KINECTSUPPORT
			XnStatus rc = XN_STATUS_OK;

			XnVPushDetector pushdetector;
			XnVBroadcaster broadcaster;

			sessionmanager = new XnVSessionManager();
			sel2d = new XnVSelectableSlider2D(10, 10);

			rc = context.InitFromXmlFile(TRACKING_FILE, scriptNode);
			if (rc != XN_STATUS_OK) {
				clog << "InputEventFactory::createFactory Warning! ";
				clog << "Couldn't initialize: " << xnGetStatusString(rc);
				clog << endl;
				return;
			}

			rc = sessionmanager->Initialize(&context, "Click", "Click");
			if (rc != XN_STATUS_OK) {
				clog << "InputEventFactory::createFactory Warning! ";
				clog << "Couldn't initialize session: ";
				clog << xnGetStatusString(rc) << endl;
				return;
			}

			// call backs register
			sessionmanager->RegisterSession(
					NULL, &sessionStart, &sessionEnd, NULL);

			pushdetector.RegisterPush(iem, &pushDetector);
			sel2d->RegisterValueChange(iem, &MainSlider_OnValueChange);
			sel2d->RegisterHandsUpdate(iem, &handsUpdate);

			//Initializing gesture handling
			broadcaster.AddListener(&pushdetector);
			broadcaster.AddListener(sel2d);
			sessionmanager->AddListener(&broadcaster);

			//Start streams factory.
			context.StartGeneratingAll();

			while (running) {
				context.WaitAnyUpdateAll();
				sessionmanager->Update(&context);
			}
#endif //HAVE_KINECTSUPPORT
		}
	}
}
}
}
}
}
}
