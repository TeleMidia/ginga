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

#include "../../../../config.h"

#include "system/io/interface/input/InputEventFactory.h"
#include "system/io/interface/input/CodeMap.h"
#include "system/io/IInputManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#if HAVE_KINECTSUPPORT
#include <XnCppWrapper.h>
#include <XnVSessionManager.h>
#include <XnVPushDetector.h>
#include <XnVBroadcaster.h>
#include <XnVSelectableSlider2D.h>

#define TRACKING_FILE "/usr/local/etc/ginga/files/system/config/_tracking.xml"

xn::Context context;
xn::ScriptNode scriptNode;

XnVSessionManager* sessionmanager = NULL;
XnVSelectableSlider2D* sel2d;

//slider variables
float oldX = -1;
float oldY = -1;

//multiple hands
int handsNumber = 0;

//call backs
void XN_CALLBACK_TYPE MainSlider_OnValueChange(
		XnFloat fXValue, XnFloat fYValue, void* ctx) {

	float difX;
	float difY;
	float errop = 0.1;
	float erron = -0.1;

	int keyCode = CodeMap::KEY_NULL;

	if (handsNumber == 1) {
		if (oldX == -1) {
			oldX = fXValue;
		}

		if (oldY == -1) {
			oldY = fYValue;
		}

		difX = fXValue - oldX;
		difY = fYValue - oldY;

		if ((difX > 0) && (difX > errop)) {
			oldX = fXValue;
			oldY = fYValue;
			keyCode = CodeMap::KEY_CURSOR_RIGHT;

		} else if ((difX < 0) && (difX < erron)) {
			oldX = fXValue;
			oldY = fYValue;
			keyCode = CodeMap::KEY_CURSOR_LEFT;

		} else if ((difY > 0) && (difY > errop)) {
			oldX = fXValue;
			oldY = fYValue;
			keyCode = CodeMap::KEY_CURSOR_UP;

		} else if ((difY < 0) && (difY < erron)) {
			oldX = fXValue;
			oldY = fYValue;
			keyCode = CodeMap::KEY_CURSOR_DOWN;
		}

		if (keyCode != CodeMap::KEY_NULL && ctx != NULL) {
			((IInputManager*)ctx)->postEvent(keyCode);
		}
	}
}

void XN_CALLBACK_TYPE handsUpdate(const XnVMultipleHands& mh, void* ctx) {
	int oldnhands = handsNumber;

	handsNumber = mh.ActiveEntries();

	if (handsNumber == 2 && oldnhands != 2 && ctx != NULL) {
		((IInputManager*)ctx)->postEvent(CodeMap::KEY_BACK);
	}
}

void XN_CALLBACK_TYPE pushDetector(
		XnFloat fVelocity, XnFloat fAngle, void* ctx) {

	if (handsNumber == 1 && ctx != NULL) {
		((IInputManager*)ctx)->postEvent(CodeMap::KEY_OK);
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
namespace system {
namespace io {
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
			sel2d = new XnVSelectableSlider2D(3, 3);

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

			pushdetector.RegisterPush(NULL, &pushDetector);
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
}
