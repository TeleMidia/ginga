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

#ifndef ISERVICEINFO_H_
#define ISERVICEINFO_H_

#include "IMpegDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
    class IServiceInfo {

		public:
			static const unsigned char DT_SERVICE           = 0x48;
			static const unsigned char DT_LOGO_TRANSMISSION = 0xCF;

		public:
			virtual ~IServiceInfo(){};
			virtual size_t getSize()=0;
			virtual unsigned short getServiceId()=0;
			virtual bool getEitScheduleFlag()=0;
			virtual bool getEitPresentFollowingFlag()=0;
			virtual unsigned char getRunningStatus()=0;
			virtual string getRunningStatusDescription()=0;
			virtual unsigned char getFreeCAMode()=0;
			virtual unsigned short getDescriptorsLoopLength()=0;
			virtual void insertDescriptor(IMpegDescriptor* descriptor)=0;
			virtual vector<IMpegDescriptor*>* getDescriptors()=0;
			virtual size_t process (char* data, size_t pos)=0;
			virtual void print()=0;
	};
}
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::tsparser::si::IServiceInfo*
		ServiceInfoCreator();

typedef void ServiceInfoDestroyer(
		::br::pucrio::telemidia::ginga::core::tsparser::si::IServiceInfo* si);

#endif /*ISERVICEINFO_H_*/
