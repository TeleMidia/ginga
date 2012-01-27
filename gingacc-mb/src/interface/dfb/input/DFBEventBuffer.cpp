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

#include "mb/interface/dfb/input/DFBEventBuffer.h"
#include "mb/interface/dfb/input/DFBGInputEvent.h"

/* macro for a safe call to DirectFB functions */
#ifndef DFBCHECK
#define DFBCHECK(x...)                                            \
{                                                                 \
	DFBResult err = x;                                            \
	if (err != DFB_OK) {                                          \
		fprintf( stderr, "%s <%d>: \n\t", __FILE__, __LINE__ );   \
		DirectFBError( #x, err );                                 \
	}                                                             \
}
#endif /*DFBCHECK*/

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	/*DFBEnumerationResult DFBEventBuffer::enum_input_device(
		    DFBInputDeviceID device_id,
		    DFBInputDeviceDescription desc,
		    void* data) {

		DeviceInfo **devices = (DeviceInfo **)data;
		DeviceInfo  *device;
		device = new DeviceInfo;
		device->device_id = device_id;
		device->desc      = desc;
		device->next      = *devices;
		*devices = device;
		return DFENUM_OK;
	}*/

	IDirectFBEventBuffer* DFBEventBuffer::eventBuffer = NULL;

	DFBEventBuffer::DFBEventBuffer(GingaScreenID screen) {
		IDirectFB* dfb;
		dfb = (IDirectFB*)(LocalScreenManager::getInstance()->getGfxRoot(
				screen));

		//dfb_true => the focus doesn't matter
		DFBCHECK(dfb->CreateInputEventBuffer(
			    dfb,
			    DICAPS_ALL,
			    DFB_TRUE,
			    &eventBuffer));
	}

	DFBEventBuffer::~DFBEventBuffer() {
		clog << "DFBEventBuffer::~DFBEventBuffer()" << endl;

		eventBuffer->Release(eventBuffer);
		eventBuffer = NULL;
	}

	void DFBEventBuffer::wakeUp() {
		if (eventBuffer != NULL) {
			DFBCHECK(eventBuffer->WakeUp(eventBuffer));
		}
	}

	void DFBEventBuffer::postEvent(IInputEvent* event) {
		DFBEvent* ev;

		if (eventBuffer != NULL &&
				event != NULL && event->getContent() != NULL) {

			ev = (DFBEvent*)(event->getContent());
			DFBCHECK(eventBuffer->PostEvent(eventBuffer, ev));
		}

		if (event != NULL) {
			event->clearContent();
			delete event;
		}
	}

	void DFBEventBuffer::waitEvent() {
		if (eventBuffer != NULL) {
			eventBuffer->WaitForEvent(eventBuffer);
		}
	}

	IInputEvent* DFBEventBuffer::getNextEvent() {
		if (eventBuffer != NULL &&
				(eventBuffer->HasEvent(eventBuffer) == DFB_OK)) {

			DFBInputEvent* evt = new DFBInputEvent;
			if (eventBuffer->GetEvent(eventBuffer, DFB_EVENT(evt)) == DFB_OK) {
				return new DFBGInputEvent((void*)evt);

			} else {
				delete evt;
				evt = NULL;
			}
		}

		return NULL;
	}

	void* DFBEventBuffer::getContent() {
		return eventBuffer;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::IEventBuffer*
		createDFBEventBuffer(GingaScreenID screen) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::DFBEventBuffer(
			screen));
}

extern "C" void destroyDFBEventBuffer(
		::br::pucrio::telemidia::ginga::core::mb::IEventBuffer* ieb) {

	delete ieb;
}
