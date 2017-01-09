/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef LDTLINKAGEDESCRIPTOR_H_
#define LDTLINKAGEDESCRIPTOR_H_

#include "IMpegDescriptor.h"
using namespace br::pucrio::telemidia::ginga::core::tsparser;

#include <vector>
using namespace std;

struct Description{
	unsigned short descriptionId;
	unsigned char descriptorType;
	unsigned char userDefined;
};

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
	class LDTLinkageDescriptor : public IMpegDescriptor {
		protected:
			unsigned short originalServiceId;
			unsigned short transportStreamId;
			unsigned short originalNetworkdId;
			//unsigned char descriptionsLength; //length of 'descriptions' array
			//Description* descriptions;
			vector<Description*>*descriptions;
		public:
			LDTLinkageDescriptor();
			~LDTLinkageDescriptor();
			unsigned char getDescriptorTag();
			unsigned int getDescriptorLength();
			void print();
			size_t process(char* data, size_t pos);
			unsigned short getTSId();
			unsigned short getOriginalNetworkId();
			unsigned short getOriginalServideId();
			//Description* getDescriptions();
			vector<Description*>* getDescriptions();
			//unsigned int getDescriptionLength();
			unsigned char getDescriptorType(struct Description* description);
			unsigned char getUserDefined(struct Description* description);
			//unsigned char* getAllDescriptorType();
			//unsigned char* getAllUserDefined();


		};

/* Values of descritptorType field of Description struct:
0x00 Reservado para uso futuro
0x01 Serviço de informação
0x02 Serviço de EPG
0x03 Serviço de substituição de CA
0x04 TS contendo network/buquê SI completo
0x05 Serviço de substituição de serviço
0x06 Serviço de radiodifusão de dados
0x07 - 0x7F Reservado para uso futuro
0x80 - 0xBF Definido pelo usuário
0xC0 - 0xFD Reservado para uso futuro (definido pelo grupo de
padronização)
0xFE Reservado para retransmissão
0xFF Reservado para uso futuro
 *
 *
 *
 */
}
}
}
}
}
}
}
}

#endif /* LDTLINKAGEDESCRIPTOR_H_ */
