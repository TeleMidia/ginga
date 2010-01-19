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

#ifndef EVENTINFO_H_
#define EVENTINFO_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "IMpegDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include "ShortEventDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser::si::descriptors;

#include "IEventInfo.h"

#include <iostream>
#include <time.h>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
    class EventInfo : public IEventInfo {
		protected:
			unsigned short eventId;
			char startTimeEncoded[5];
			char durationEncoded[3];
			unsigned char runningStatus;
			unsigned char freeCAMode;
			unsigned short descriptorsLoopLength;
			set<IMpegDescriptor*> descriptors;
			time_t startTime;
			time_t duration;

		public:
			EventInfo();
			~EventInfo();
			static int bcd(int dec);
			static int decimal(int bcd);
			static int mjd(time_t date);
			static time_t decodeMjd(unsigned short date);
			size_t getSize();
			void setEventId(unsigned short id);
			unsigned short getEventId();
			void setStartTime(time_t time);
			time_t getStartTime();
			char * getStartTimeEncoded();

		private:
			string getFormatNumStr(int un);

		public:
			string getFormattedStartTime();
			void setStartTimeEncoded(char* ste);
			void setDuration(time_t duration);
			time_t getDuration();
			void setDurationEncoded(char* duration);
			char * getDurationEncoded();
			string getFormattedDuration();
			void setRunningStatus(unsigned char status);
			unsigned char getRunningStatus();
			void setFreeCAMode(unsigned char mode);
			unsigned char getFreeCAMode();
			unsigned short getDescriptorsLoopLength();
			void setDescriptorsLoopLength(unsigned short length);
			void insertDescriptor(IMpegDescriptor* info);
			set<IMpegDescriptor*> * getDescriptors();
	};
}
}
}
}
}
}
}

#endif /*EVENTINFO_H_*/
