/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licen�a Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribu�do na expectativa de que seja util, porem, SEM
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

#include "../../../include/Inputs.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {
#if HAVE_DIRECTFB
	set<IDirectFBEventBuffer*>* Inputs::bufferPool = (
			new set<IDirectFBEventBuffer*>);

	DFBEnumerationResult Inputs::enum_input_device(
		    DFBInputDeviceID device_id,
		    DFBInputDeviceDescription desc,
		    void* data) {

		DeviceInfo **devices = (DeviceInfo **)data;
		DeviceInfo  *device;
		device = (DeviceInfo*)malloc( sizeof(DeviceInfo) );
		device->device_id = device_id;
		device->desc      = desc;
		device->next      = *devices;
		*devices = device;
		return DFENUM_OK;
	}
#endif

	void* Inputs::createInputEventBuffer() {
#if HAVE_DIRECTFB
		IDirectFB* dfb = (IDirectFB*)Graphics::getRoot();
		IDirectFBEventBuffer* eventBuffer;

		//dfb_true => the focus doesnt matter
		DFBCHECK(dfb->CreateInputEventBuffer(
			    dfb,
			    DICAPS_ALL,
			    DFB_TRUE,
			    &eventBuffer));

		bufferPool->insert(eventBuffer);

		return (void*)eventBuffer;
#else
		return NULL;
#endif
	}

	void Inputs::releaseInputBuffer(void* buffer) {
#if HAVE_DIRECTFB
		set<IDirectFBEventBuffer*>::iterator i;
		IDirectFBEventBuffer* b = (IDirectFBEventBuffer*)buffer;

		i = bufferPool->find(b);
		if (i != bufferPool->end()) {
			bufferPool->erase(i);

		} else {
			return;
		}
#if DFB_VER == 120
		b->Release(b);
#else
		DFBCHECK(b->Release(b));
#endif /*DFB_VER*/
#endif /*HAVE_DIRECTFB*/
	}

	void Inputs::release() {
#if HAVE_DIRECTFB
		//Releasing still Buffer objects in Buffer Pool
		set<IDirectFBEventBuffer*>::iterator b;
		for (b = bufferPool->begin(); b != bufferPool->end(); ++b) {
			if ((*b) != NULL) {
#if DFB_VER == 120
				(*b)->Release(*b);
#else
				DFBCHECK((*b)->Release(*b));
#endif /*DFB_VER*/
			}
		}
		bufferPool->clear();
#endif /*HAVE_DIRECTFB*/
	}
}
}
}
}
}
}
