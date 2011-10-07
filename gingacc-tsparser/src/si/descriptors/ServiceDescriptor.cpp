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

#include "tsparser/ServiceDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
	ServiceDescriptor::ServiceDescriptor() {
		descriptorTag             = 0x48;
		descriptorLength          = 0;
		serviceProviderNameLength = 0;
		serviceProviderNameChar   = NULL;
		serviceNameLength         = 0;
		serviceNameChar           = NULL;
		serviceType               = 0;
	}

	ServiceDescriptor::~ServiceDescriptor() {
		if(serviceProviderNameChar != NULL) {
			delete serviceProviderNameChar;
			serviceProviderNameChar = NULL;
		}

		if(serviceNameChar != NULL) {
			delete serviceNameChar;
			serviceNameChar = NULL;
		}
	}

	unsigned char ServiceDescriptor::getDescriptorTag() {
		return descriptorTag;
	}

	unsigned int ServiceDescriptor::getDescriptorLength() {
		return (unsigned int) descriptorLength;
	}

	unsigned int ServiceDescriptor::getServiceProviderNameLength() {
		return (unsigned int)serviceProviderNameLength;
	}

	unsigned int ServiceDescriptor::getServiceNameLength() {
		return (unsigned int)serviceNameLength;
	}

	string ServiceDescriptor::getServiceProviderNameChar() {
		string str;

		if (serviceProviderNameChar == NULL) {
			return "";
		}
		str.append(serviceProviderNameChar, serviceProviderNameLength);
		return str;
	}

	string ServiceDescriptor::getServiceNameChar() {
		string str;

		if (serviceNameChar == NULL) {
			return "";
		}
		//using serviceNameChar[1] because [0] is usually sent with encoding
		//value, value is equal to 0x0E. So the serviceName starts in [1].
		str.assign(serviceNameChar+1, (serviceNameLength & 0xFF)-1);
		return str;
	}

	void ServiceDescriptor::print() {
		clog << "ServiceDescriptor::print printing..." << endl;
		clog << " -descriptorLength = " << getDescriptorLength() << endl;

		if (serviceProviderNameLength > 0) {
			clog << "-serviceProviderNameChar = "
					<< getServiceProviderNameChar() << endl;
		}

		if (serviceNameLength > 0) {
			clog << " -serviceNameLength: " << getServiceNameLength() << endl;
			clog << " -serviceNameChar: "   << getServiceNameChar()   << endl;
			//clog << " -charEnconde:" << hex << (serviceNameChar[0] & 0xFF);
			//clog << dec << endl;
		}
	}

	size_t ServiceDescriptor::process(char* data, size_t pos) {
		//clog << "ServiceDescriptor:: process with pos = " << pos << endl;
		descriptorLength = data[pos+1];
		pos += 2;

		serviceType = data[pos];
		pos++;

		//cout <<"service length = " << (unsigned int)descriptorLength;
		//clog << " and serviceType = ";
		//clog << (unsigned int)serviceType << endl;

		serviceProviderNameLength = data[pos];
		if (serviceProviderNameLength > 0) {
			//clog << "ServiceProviderNameLength = ";
			//clog << (unsigned int)serviceProviderNameLength;
			//clog << endl;
			serviceProviderNameChar = new char[serviceProviderNameLength];
			memset(serviceProviderNameChar, 0 , serviceProviderNameLength);
			memcpy(serviceProviderNameChar,data+pos+1,
					serviceProviderNameLength);

		}
		pos += serviceProviderNameLength + 1;
		serviceNameLength = data[pos];

		if (serviceNameLength > 0) {
			serviceNameChar = new char[serviceNameLength];
			memset(serviceNameChar, 0 , serviceNameLength);
			memcpy(serviceNameChar, data+pos+1, serviceNameLength);

		}
		pos += serviceNameLength;
		return pos;

	}
}
}
}
}
}
}
}
}
