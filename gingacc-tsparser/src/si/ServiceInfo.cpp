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

#include "../../include/ServiceInfo.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	ServiceInfo::ServiceInfo() {
		descriptorsLoopLength = 0;
	}

	ServiceInfo::~ServiceInfo() {

	}

	size_t ServiceInfo::getSize() {
		return (descriptorsLoopLength + 5);
	}

	void ServiceInfo::setServiceId(unsigned short id) {
		serviceId = id;
	}

	unsigned short ServiceInfo::getServiceId() {
		return serviceId;
	}

	void ServiceInfo::setEitScheduleFlag(unsigned char flag) {
		eitScheduleFlag = flag;
	}

	unsigned char ServiceInfo::getEitScheduleFlag() {
		return eitScheduleFlag;
	}

	void ServiceInfo::setEitPresentFollowingFlag(unsigned char flag) {
		eitPresentFollowingFlag = flag;
	}

	unsigned char ServiceInfo::getEitPresentFollowingFlag() {
		return eitPresentFollowingFlag;
	}

	void ServiceInfo::setRunningStatus(unsigned char status) {
		runningStatus = status;
	}

	unsigned char ServiceInfo::getRunningStatus() {
		return runningStatus;
	}

	void ServiceInfo::setFreeCAMode(unsigned char mode) {
		freeCAMode = mode;
	}

	unsigned char ServiceInfo::getFreeCAMode() {
		return freeCAMode;
	}

	unsigned short ServiceInfo::getDescriptorsLoopLength() {
		return descriptorsLoopLength;
	}

	void ServiceInfo::setDescriptorsLoopLength(unsigned short length) {
		descriptorsLoopLength = length;
	}

	void ServiceInfo::insertDescriptor(IMpegDescriptor* info) {
		LogoTransmissionDescriptor* ltd;
		set<IMpegDescriptor*>::iterator it;
		size_t count = 0;

		descriptors.insert(info);

		for (it = descriptors.begin(); it != descriptors.end(); it++) {
			switch ((*it)->getDescriptorTag()) {
			case 0xCF:
				ltd = (LogoTransmissionDescriptor*) (*it);
				break;
			default:
				break;
			}
			count += (ltd->getDescriptorLength() + 2);
		}
		descriptorsLoopLength = count;
	}

	set<IMpegDescriptor*> * ServiceInfo::getDescriptors() {
		return &descriptors;
	}

}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::tsparser::si::IServiceInfo*
		createServiceInfo() {

	return (new
			::br::pucrio::telemidia::ginga::core::tsparser::si::ServiceInfo());
}

extern "C" void destroyServiceInfo(
		::br::pucrio::telemidia::ginga::core::tsparser::si::IServiceInfo* si) {

	delete si;
}
