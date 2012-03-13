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

#include "IEventInfo.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "ShortEventDescriptor.h"
#include "ExtendedEventDescriptor.h"
#include "ComponentDescriptor.h"
#include "ContentDescriptor.h"
#include "DigitalCCDescriptor.h"
#include "AudioComponentDescriptor.h"
#include "DataContentDescriptor.h"
#include "SeriesDescriptor.h"
#include "ParentalRatingDescriptor.h"
#include "ContentAvailabilityDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser::si::descriptors;

#include "IMpegDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;


#include <iostream>
#include <sstream>
//#include <time.h>
//#include <vector>
//#include <map>
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
			vector<IMpegDescriptor*>* descriptors;
			map<unsigned char, IMpegDescriptor*>* desc;

			/*attention: tm_mon represents month from 0(January) to 11(December).
			 * the print function is printing month in 1(jan) to 12(dec)*/
			struct tm startTime;
			struct tm duration;
			struct tm endTime;

		public:
			EventInfo();
			~EventInfo();
			void setStartTime(char* date);
			void setDuration(char* dur);
			struct tm calcEndTime(struct tm start, struct tm end);

			time_t getStartTimeSecs();
			time_t getEndTimeSecs();
			unsigned int getDurationSecs();

			virtual string getStartTimeSecsStr();
			virtual string getEndTimeSecsStr();
			virtual string getDurationSecsStr();

			struct tm getStartTime();
			struct tm getDuration();
			struct tm getEndTime();

			string getStartTimeEncoded();
			string getDurationEncoded();
			string getStartTimeStr();
			string getEndTimeStr();
			string getDurationStr();

			unsigned short getLength();
			unsigned short getEventId();
			string getRunningStatus();
			string getRunningStatusDescription();
			unsigned char getFreeCAMode();
			unsigned short getDescriptorsLoopLength();

			vector<IMpegDescriptor*>* getDescriptors();
			map<unsigned char, IMpegDescriptor*>* getDescriptorsMap();

			void print();
			size_t process(char* data, size_t pos);

		protected:
			int convertDecimaltoBCD(int dec);
			int convertBCDtoDecimal(int bcd);
			struct tm convertMJDtoUTC(unsigned int mjd);
			int convertUTCtoMJD (int day, int month, int year);

		private:
			string getFormatNumStr(int un);
			//void clearDescriptors();


		};
}
}
}
}
}
}
}

#endif /*EVENTINFO_H_*/
