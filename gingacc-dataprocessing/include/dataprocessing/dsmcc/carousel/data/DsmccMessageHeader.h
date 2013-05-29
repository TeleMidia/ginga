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

#ifndef DSMCCMESSAGEHEADER_H_
#define DSMCCMESSAGEHEADER_H_

extern "C" {
	#include <sys/stat.h>
	#include <stdio.h>
	#include <stdio.h>
	#include <string.h>
}

#include <iostream>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	class DsmccMessageHeader {
		private:
			//origin es is
			unsigned int pid;
			string fileName;

			//
			// ISO/IEC 13818-6 DSM-CC MESSAGE HEADER
			//

			// Must be 0x11 for DSMCC message. (1 byte)
			unsigned int protocolDiscriminator;

			// Defines message type. (1 byte)
			//   0x01    Configuration message
			//   0x02    Session message
			//   0x03    Download message
			//   0x04    Channel change
			//   0x05    Passthru message
			unsigned int dsmccType;

			// Indicates the message type depending on
			// DSMCC type. (2 bytes)
			//   0x1001    Dowload info request
			//   0x1002    Download info response or DII
			//   0x1003    Download data block (actual data)
			//   0x1004    Download data request
			//   0x1005    Download cancel
			//   0x1006    Download server initiate (DSI)
			unsigned int messageId;

			// Used for session integrity and
			// error. (4 bytes)
			unsigned int transactionId;

			// RESERVED BYTE, must be 0xFF.

			// Indicates the lenght in bytes of the
			// adaptation header. (1 byte)
			unsigned int adaptationLength;

			// The total lenght in bytes of this message
			// following this field.  This lenght includes
			// any adaptation headers. (2 bytes)
			unsigned int messageLength;

		public:
			DsmccMessageHeader();
			int readMessageFromFile(string fileName, unsigned int pid);
			unsigned int getESId();
			string getFileName();
			unsigned int getDsmccType();
			unsigned int getMessageId();
			unsigned int getTrasnsactionId();
			unsigned int getAdaptationLength();
			unsigned int getMessageLength();
			void print();
	};
}
}
}
}
}
}
}

#endif /*DSMCCMESSAGEHEADER_H_*/
