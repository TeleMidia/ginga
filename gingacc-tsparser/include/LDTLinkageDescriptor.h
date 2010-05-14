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
			size_t process(char* data, size_pos);
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
