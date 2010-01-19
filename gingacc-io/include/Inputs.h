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

/* 
 * CAUTION: contents may be included only in cpp files and header files that
 * are not provided to others libs.
 */

#ifndef INPUTS_H_
#define INPUTS_H_

#include "Graphics.h"

#if HAVE_DIRECTFB
typedef struct _DeviceInfo DeviceInfo;

struct _DeviceInfo {
     DFBInputDeviceID device_id;
     DFBInputDeviceDescription desc;
     DeviceInfo* next;
};
#endif /*HAVE_DIRECTFB*/

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {
	class Inputs {
#if HAVE_DIRECTFB
		private:
			static set<IDirectFBEventBuffer*>* bufferPool;
			static DFBEnumerationResult enum_input_device(
				    DFBInputDeviceID device_id,
				    DFBInputDeviceDescription desc,
				    void* data);

#endif /*HAVE_DIRECTFB*/
		public:
			static void* createInputEventBuffer();
			static void releaseInputBuffer(void* buffer);
			static void release();
	};
}
}
}
}
}
}

#endif /*INPUTS_H_*/
