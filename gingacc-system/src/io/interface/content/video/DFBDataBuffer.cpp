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

#include "system/io/interface/content/video/DFBDataBuffer.h"
#include "system/io/interface/content/video/DFBVideoProvider.h"
#include "system/io/LocalDeviceManager.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	DFBDataBuffer::DFBDataBuffer(void* data, unsigned int dataSize) : Thread() {
		IDirectFB* dfb = NULL;

		deviceUri          = NULL;
		deviceFd           = -1;
		this->dataSize     = dataSize;
		this->data         = new char[dataSize];

		desc.file          = NULL;
		desc.memory.data   = data;
		desc.memory.length = dataSize;
		desc.flags         = (DFBDataBufferDescriptionFlags)DBDESC_MEMORY;

		dfb = (IDirectFB*)(LocalDeviceManager::getInstance()->getGfxRoot());
		dfb->CreateDataBuffer(dfb, &desc, &dataBuffer);
	}

	DFBDataBuffer::~DFBDataBuffer() {
		delete data;
	}

	IContinuousMediaProvider* DFBDataBuffer::createProvider(const char* mrl) {
		IDirectFBVideoProvider* dec;

		deviceUri = mrl;
		deviceFd  = open(deviceUri, O_RDONLY | O_NONBLOCK);

		start();

		dataBuffer->WaitForData(dataBuffer, 188 * 1024 * 8);
		dataBuffer->CreateVideoProvider(dataBuffer, &dec);

		//clog << "DFBDataBuffer::createProvider done" << endl;
		return new DFBVideoProvider(dec);
	}

	void DFBDataBuffer::run() {
		int rval;

		while(true) {
			rval = read(deviceFd, data, dataSize);
			if (rval > 0) {
				//clog << "DFBDataBuffer::run " << rval << endl;
				dataBuffer->PutData(dataBuffer, data, rval);
			}
		}
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::
		IDataBuffer* createDFBDataBuffer(void* data, unsigned int dataSize) {

	return (new ::br::pucrio::telemidia::ginga::core::system::io::
			DFBDataBuffer(data, dataSize));
}
