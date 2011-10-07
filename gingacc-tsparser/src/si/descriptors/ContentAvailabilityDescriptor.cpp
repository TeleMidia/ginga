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
//jumping reserved_future_use (first 4 bits of data[pos])
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

#include "tsparser/ContentAvailabilityDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
	ContentAvailabilityDescriptor::ContentAvailabilityDescriptor() {
		descriptorTag        = 0xDE;
		descriptorLength     = 0;
		retentionMode        = 0;
		retentionState       = 0;
		imageConstraintToken = 0;
		encriptionMode       = 0;

	}

	ContentAvailabilityDescriptor::~ContentAvailabilityDescriptor() {
	}
	unsigned char ContentAvailabilityDescriptor::getDescriptorTag(){
		return descriptorTag;
	}
	unsigned int ContentAvailabilityDescriptor::getDescriptorLength(){
		return (unsigned int)descriptorLength;
	}
	void ContentAvailabilityDescriptor::print() {
		clog << "ContentAvailabilityDescriptor::print printing..." << endl;
	}
	size_t ContentAvailabilityDescriptor::process(char* data, size_t pos){
		//clog << "ContentAvailabiltyDescriptor::process with pos = " << pos;

		descriptorLength = data[pos+1];
		//clog << " and length = " << (unsigned int)descriptorLength << endl;
		pos += 2;

		//2 bits reserved for future use
		//clog << "CA debug = " << (data[pos] & 0xFF) << endl;
		imageConstraintToken = ((data[pos] & 0x20) >> 5); //1 bit
		//clog << " CA ImageConstraingToken = " << (int)imageConstraintToken << endl;
		retentionMode = ((data[pos] & 0x10) >> 4); //1 bit
		//clog << "CA retentionMode = " << (int)retentionMode << endl;
		retentionState = ((data[pos] & 0x0E) >> 1);//3 bits
		//clog << "CA retentionState = " << (int)retentionState << endl;
		encriptionMode = (data[pos] & 0x01); // 1 bit
		//clog << "CA encriptionMode = " << (int)encriptionMode << endl;
		pos ++;

		pos += descriptorLength - 2; //jumping reserved future use
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
