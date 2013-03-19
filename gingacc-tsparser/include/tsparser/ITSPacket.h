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

#ifndef ITSPACKET_
#define ITSPACKET_

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
	class ITSPacket {
	public:
		static const unsigned char TS_PACKET_SIZE = 188;
		static const unsigned char TS_PAYLOAD_SIZE = TS_PACKET_SIZE - 4;
		static const unsigned char TS_PACKET_SYNC_BYTE = 0x47;
		virtual ~ITSPacket(){};
		virtual bool isConstructionFailed()=0;
		virtual unsigned short getPid()=0;
		virtual char getPacketData(char **dataStream)=0;
		virtual void getPayload(char streamData[TS_PAYLOAD_SIZE])=0;
		virtual unsigned char getPayloadSize()=0;
		virtual void getPayload2(char streamData[TS_PAYLOAD_SIZE])=0;
		virtual unsigned char getPayloadSize2()=0;
		virtual bool getStartIndicator()=0;
		virtual unsigned char getPointerField()=0;
		virtual unsigned char getAdaptationFieldControl()=0;
		virtual unsigned char getContinuityCounter()=0;
		virtual void setContinuityCounter(unsigned int counter)=0;
		virtual void print()=0;
		virtual void setPacketCount(unsigned int count)=0;
	};
}
}
}
}
}
}

#endif /*TSPACKET_*/
