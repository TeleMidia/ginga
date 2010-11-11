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

#ifndef IEVENTINFO_H_
#define IEVENTINFO_H_

#include "IMpegDescriptor.h"

#include <time.h>
#include <map>
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
    class IEventInfo {
		public:
			//obrigatory descriptors tag:
			static const unsigned char DT_SHORT_EVENT          = 0x4D;
			static const unsigned char DT_COMPONENT            = 0X50;
			static const unsigned char DT_AUDIO_COMPONENT      = 0XC4;
			static const unsigned char DT_PARENTAL_RATING      = 0x55;
			//optional descriptors tag:
			static const unsigned char DT_EXTENDED_EVENT       = 0x4E;
			static const unsigned char DT_CONTENT              = 0x54;
			static const unsigned char DT_DIGITAL_COPY         = 0xC1;
			static const unsigned char DT_DATA_CONTENTS        = 0XC7;
			static const unsigned char DT_SERIES               = 0XD5;
			static const unsigned char DT_EVENT_GROUP          = 0x55;
			static const unsigned char DT_CONTENT_AVAILABILITY = 0XDE;
			static const unsigned char DT_STUFFING             = 0X42;
			static const unsigned char DT_COMPONENT_GROUP      = 0xD9;

		public:
			virtual ~IEventInfo(){};

			virtual time_t getStartTimeSecs()=0; //startTime in secs since Epoch
			virtual time_t getEndTimeSecs()=0; //endTime in secs since Epoch
			virtual unsigned int getDurationSecs()=0;


			virtual string getStartTimeSecsStr()=0;
			virtual string getEndTimeSecsStr()=0;
			virtual string getDurationSecsStr()=0;

			virtual struct tm getStartTime()=0;
			virtual struct tm getDuration()=0;
			virtual struct tm getEndTime()=0;
			virtual string getStartTimeStr()=0;
			virtual string getEndTimeStr()=0;
			virtual string getDurationStr()=0;
			virtual unsigned short getLength()=0;
			virtual unsigned short getEventId()=0;
			virtual	string getRunningStatus()=0;
			virtual string getRunningStatusDescription()=0;
			virtual unsigned char getFreeCAMode()=0;
			virtual unsigned short getDescriptorsLoopLength()=0;
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

typedef ::br::pucrio::telemidia::ginga::core::tsparser::si::IEventInfo*
		EICreator();

typedef void EIDestroyer(
		::br::pucrio::telemidia::ginga::core::tsparser::si::IEventInfo* ei);

#endif /*IEVENTINFO_H_*/
