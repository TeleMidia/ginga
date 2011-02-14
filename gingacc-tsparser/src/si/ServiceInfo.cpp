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

#include "tsparser/ServiceInfo.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	ServiceInfo::ServiceInfo() {
		descriptorsLoopLength   = 0;
		serviceId               = 0;
		runningStatus           = 0;
		eitPresentFollowingFlag = false;
		eitScheduleFlag         = false;
		descriptors             = new vector<IMpegDescriptor*>;

	}

	ServiceInfo::~ServiceInfo() {
		vector<IMpegDescriptor*>::iterator i;
		if (descriptors != NULL) {
			i = descriptors->begin();
			while (i != descriptors->end()) {
				delete (*i);
				++i;
			}
			delete descriptors;
			descriptors = NULL;
		}
	}

	size_t ServiceInfo::getSize() {
		return (descriptorsLoopLength + 5);
	}

	unsigned short ServiceInfo::getServiceId() {
		return serviceId;
	}

	bool ServiceInfo::getEitScheduleFlag() {
		return eitScheduleFlag;
	}

	bool ServiceInfo::getEitPresentFollowingFlag() {
		return eitPresentFollowingFlag;
	}

	unsigned char ServiceInfo::getRunningStatus() {
		return runningStatus;
	}

	string ServiceInfo::getRunningStatusDescription () {
		switch (runningStatus) {
			case 0:
				return "Undefined";
				break;

			case 1:
				return "Off";
				break;

			case 2:
				return "Start within few minutes";
				break;

			case 3:
				return "Paused";
				break;

			case 4:
				return "Running";
				break;
			//5-7 are reserved for future used
		}

		return "";
	}

	unsigned char ServiceInfo::getFreeCAMode() {
		return freeCAMode;
	}

	unsigned short ServiceInfo::getDescriptorsLoopLength() {
		return descriptorsLoopLength;
	}

	void ServiceInfo::insertDescriptor(IMpegDescriptor* descriptor) {
		size_t count = 0;

		descriptors->push_back(descriptor);

		count += (descriptor->getDescriptorLength()+ 2);
		descriptorsLoopLength = count;
	}

	vector<IMpegDescriptor*>* ServiceInfo::getDescriptors() {
		return descriptors;
	}

	void ServiceInfo::print() {
		vector<IMpegDescriptor*>::iterator i;

		cout << "ServiceInfo::print printing..." << endl;
		cout << " -serviceId: "               << serviceId        << endl;
		cout << " -eitScheduleFlag: "         << eitScheduleFlag  << endl;
		cout << " -eitPresentFlag:"           << eitPresentFollowingFlag;
		cout << endl;
		cout << " -runningStatusDesc: "       << getRunningStatusDescription();
		cout << endl;

		for (i =  descriptors->begin(); i!= descriptors->end(); ++i) {
			(*i)->print();
		}
	}
	size_t ServiceInfo::process(char* data, size_t pos) {
		IMpegDescriptor*  descriptor;
		size_t localpos;
		unsigned char remainingBytesDescriptor, value;

		cout <<"ServiceInfo::process with pos " << pos << endl;

		serviceId = ((((data[pos] << 8) & 0xFF00) |
								(data[pos+1] & 0xFF)));

		//cout <<"ServiceId = " << serviceId << endl;
		pos += 2;
		//jumping reserved_future_use
		eitScheduleFlag = ((data[pos] & 0x02) >> 1);
		eitPresentFollowingFlag = (data[pos] & 0x01);
		pos++;

		runningStatus = ((data[pos] & 0xE0) >> 5);
		freeCAMode = ((data[pos] & 0x10) >> 4);
		descriptorsLoopLength = (
				(((data[pos] & 0x0F) << 8) & 0xFF00) |
				(data[pos+1] & 0xFF));

		pos += 2;

		//cout << "DescriptorsLoopLength = " << descriptorsLoopLength << endl;
		remainingBytesDescriptor = descriptorsLoopLength;

		while (remainingBytesDescriptor) {//there's at least one descriptor
			value = data[pos+1] + 2;
			remainingBytesDescriptor -= value;

			switch (data[pos]) {
				case LOGO_TRANMISSION:
					descriptor = new LogoTransmissionDescriptor();
					localpos = descriptor->process(data, pos);
					pos += value;
					descriptors->push_back(descriptor);
					break;

				case SERVICE:
					descriptor = new ServiceDescriptor();
					localpos = descriptor->process(data, pos);
					pos += value;
					descriptors->push_back(descriptor);
					break;

				default: //Unrecognized Descriptor
					cout << "ServiceInfo:: process default descriptor with tag: ";
					cout << hex << (unsigned int)data[pos] << dec ;
					cout << " with length = " << (unsigned int)data[pos+1];
					cout << " and with pos = " << pos << endl;
					pos += value;
					break;
			}
		}
		return pos;
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
