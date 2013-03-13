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

#ifndef IAPPLICATION_H_
#define IAPPLICATION_H_

#include "IMpegDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include <string>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	class IApplication {
		public:
			static const unsigned char DT_APPLICATION                   = 0x00;
			static const unsigned char DT_APPLICATION_NAME              = 0x01;
			static const unsigned char DT_TRANSPORT_PROTOCOL            = 0x02;
			static const unsigned char DT_GINGAJ_APPLICATION            = 0x03;
			static const unsigned char DT_GINGAJ_APPLICATION_LOCATION   = 0x04;
			static const unsigned char DT_EXTERNAL_APPLICATION          = 0x05;
			static const unsigned char DT_GINGANCL_APPLICATION          = 0x06;
			static const unsigned char DT_GINGANCL_APPLICATION_LOCATION = 0x07;
			static const unsigned char DT_APPLICATION_ICONS             = 0x0B;
			static const unsigned char DT_PREFETCH                      = 0x0C;
			static const unsigned char DT_DII_LOCATION                  = 0x0D;
			static const unsigned char DT_IP_SIGNALLING                 = 0x11;
			static const unsigned char DT_PRIVATE_DATA_SPECIFIER        = 0x5F;

			static const unsigned char CC_AUTOSTART                     = 0x01;
			static const unsigned char CC_PRESENT                       = 0x02;
			static const unsigned char CC_DESTROY                       = 0x03;
			static const unsigned char CC_KILL                          = 0x04;
			static const unsigned char CC_PREFETCH                      = 0x05;
			static const unsigned char CC_REMOTE                        = 0x06;
			static const unsigned char CC_UNBOUND                       = 0x07;

			virtual ~IApplication(){};

			virtual string getBaseDirectory()=0;
			virtual string getInitialClass()=0;
			virtual unsigned short getControlCode()=0;
			virtual unsigned short getLength()=0;
			virtual size_t process(char* data, size_t pos)=0;
		};

}
}
}
}
}
}
}

#endif /* IAPPLICATION_H_ */
