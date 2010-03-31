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

#include "../../include/AIT.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	AIT::AIT() {
		cout << "AIT::AIT" << endl;

		secName                 = "";
		applicationType         = 0;
		commonDescriptorsLength = 0;
		descriptors             = new vector<IMpegDescriptor*>;
		applications            = new vector<IApplication*>;
	}

	AIT::~AIT() {
		vector<IMpegDescriptor*>::iterator i;
		vector<IApplication*>:: iterator j;

		if (descriptors != NULL) {
			i = descriptors->begin();
			while (i != descriptors->end()) {
				delete (*i);
				++i;
			}
			delete descriptors;
			descriptors = NULL;
		}

		if (applications != NULL) {
			j = applications->begin();
			while(j != applications->end()){
				delete (*j);
				++j;
			}
			delete applications;
			applications = NULL;
		}
	}

	string AIT::getSectionName() {
		return secName;
	}

	void AIT::setSectionName(string secName) {
		this->secName = secName;
	}

	void AIT::setApplicationType(unsigned int type) {
		applicationType = type;
		cout << "AIT::setApplicationType type = " << applicationType << endl;
	}

	vector<IMpegDescriptor*>* AIT::getDescriptors() {
		return descriptors;
	}

	vector<IApplication*>* AIT::getApplications() {
		return applications;
	}

	void AIT::process(void* payloadBytes, unsigned int payloadSize) {
		char* data;
		size_t pos, descpos, localpos;
		unsigned char descriptorTag;
		unsigned short remainingBytes;
		Application* application;
		IMpegDescriptor* descriptor;
		unsigned char value;

		cout << "AIT::process with size =" << payloadSize << endl;

		data = new char[payloadSize];
		memcpy((void*)&(data[0]), payloadBytes, payloadSize);

		/*
		pos = 3;
		applicationType  = (((section[pos] & 0xFF) << 8)
				| (section[pos+1] & 0xFF));
		pos = 8; // jumping to payload (heard has 8 bytes)
*/
		pos = 0;
		commonDescriptorsLength = ((((data[pos] & 0x0F) << 8) & 0xFF00) |
				(data[pos+1] & 0xFF));

		cout << "AIT::process commonDescriptorsLength = ";
		cout << commonDescriptorsLength << endl;

		//handle Descriptors!
		pos += 2;
		descpos = pos;
		remainingBytes = commonDescriptorsLength;
		while (remainingBytes) {
			descriptorTag = data[descpos];
			cout << "Ait::process descriptorTag = " << hex <<  descriptorTag ;
			cout << dec << endl;
			value = ((data[pos+1] & 0xFF) + 2);
			cout << "Ait::process with vale = " << value << endl;
			remainingBytes -= value;

			switch (descriptorTag) {
				case IApplication::DT_APPLICATION:
					descriptor = new ApplicationDescriptor();
					descriptor->process(data, pos);
					descriptors->push_back(descriptor);
					break;

				case IApplication::DT_APPLICATION_NAME:
					descriptor = new ApplicationNameDescriptor();
					descriptor->process(data, pos);
					descriptors->push_back(descriptor);
					break;

				case IApplication::DT_TRANSPORT_PROTOCOL:
					descriptor = new TransportProtocolDescriptor();
					descriptor->process(data, pos);
					descriptors->push_back(descriptor);
					break;

				case IApplication::DT_GINGAJ_APPLICATION_LOCATION:
					descriptor = new ApplicationLocationDescriptor();
					descriptor->process(data, pos);
					descriptors->push_back(descriptor);
					break;

				case IApplication::DT_GINGANCL_APPLICATION_LOCATION:
					descriptor = new ApplicationLocationDescriptor();
					descriptor->process(data, pos);
					descriptors->push_back(descriptor);
					break;

				case IApplication::DT_PREFETCH:
					descriptor = new PrefetchDescriptor();
					descriptor->process(data, pos);
					descriptors->push_back(descriptor);
					break;

				default:
					break;
			}
			pos += value;
		}

		pos =  descpos + commonDescriptorsLength;
		cout << "Ait::process pos after desclength = " << pos << endl;
		applicationLoopLength = ((((data[pos] & 0x0F) << 8) & 0xFF00) |
				(data[pos+1] & 0xFF));

		cout << "Ait::process applicationLoopLength = " << applicationLoopLength;
		cout << endl;

		pos += 2;
		remainingBytes = applicationLoopLength;
		while (pos < payloadSize) {
			application = new Application();
			application->process(data, pos);
			pos += application->getLength();
			applications->push_back(application);
		}
		cout << "Ait::process ALL DONE!" << endl;

		delete data;
	}


}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::tsparser::si::
		IAIT* createAIT() {

	return new ::br::pucrio::telemidia::ginga::core::tsparser::si::AIT();
}
