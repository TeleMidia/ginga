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

#include "../../include/EventInfo.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	EventInfo::EventInfo() {
		descriptorsLoopLength = 0;
	}

	EventInfo::~EventInfo() {

	}

	int EventInfo::bcd(int dec)
	{
		return ((dec / 10) << 4) + (dec % 10);
	}

	int EventInfo::decimal(int bcd)
	{
		return ((bcd >> 4) * 10) + bcd % 16;
	}

	int EventInfo::mjd(time_t date) {
		struct tm t;
		time_t oldDate;

		t.tm_year = 1970 - 1900;
		t.tm_mon = 0;
		t.tm_mday = 1;
		t.tm_hour = 0;
		t.tm_min = 0;
		t.tm_sec = 0;
		t.tm_isdst = 0;

		oldDate = mktime(&t);
		//TODO: validate this method
		return (((date - oldDate) / 86400) + 40587);
	}

	time_t EventInfo::decodeMjd(unsigned short date) {
		struct tm t;
		time_t oldDate, dec;

		t.tm_year = 1970 - 1900;
		t.tm_mon = 0;
		t.tm_mday = 1;
		t.tm_hour = 0;
		t.tm_min = 0;
		t.tm_sec = 0;
		t.tm_isdst = 0;

		oldDate = mktime(&t);
		dec = (((date - 40587) * 86400) - oldDate + 86400);

		t = *(localtime(&dec));

		t.tm_hour = 0;
		t.tm_min = 0;
		t.tm_sec = 0;
		t.tm_isdst = 0;

		//TODO: validate this method
		return (mktime(&t));
	}

	size_t EventInfo::getSize() {
		return (descriptorsLoopLength + 12);
	}

	void EventInfo::setEventId(unsigned short id) {
		eventId = id;
	}

	unsigned short EventInfo::getEventId() {
		return eventId;
	}

	void EventInfo::setStartTime(time_t time) {
		int date;
		struct tm p;

		startTime = time;

		if (time == 0) {
			memset(startTimeEncoded, 0xFF, 5);
			return;
		}

		date = mjd(time);

		startTimeEncoded[0] = (date & 0xFF00) >> 8;
		startTimeEncoded[1] = (date & 0xFF);

		p = *(localtime(&time));
		startTimeEncoded[2] = bcd(p.tm_hour);
		startTimeEncoded[3] = bcd(p.tm_min);
		startTimeEncoded[4] = bcd(p.tm_sec);
	}

	time_t EventInfo::getStartTime() {
		return startTime;
	}

	char * EventInfo::getStartTimeEncoded() {
		return startTimeEncoded;
	}

	string EventInfo::getFormattedStartTime() {
		struct tm p;

		if (startTime == 0) {
			return "";
		}

		p = *(localtime(&startTime));

		return (getFormatNumStr(p.tm_hour) + ":" + getFormatNumStr(p.tm_min) +
				":" + getFormatNumStr(p.tm_sec));
	}

	string EventInfo::getFormatNumStr(int un) {
		int fUn;
		fUn = bcd(un);
		if (fUn < 10) {
			return "0" + itos(fUn);
		} else {
			return itos(fUn);
		}
	}

	void EventInfo::setStartTimeEncoded(char* ste) {
		struct tm t;
		time_t date;
		unsigned short myDate = 0;

		memcpy(startTimeEncoded, ste, 5);

		if (((startTimeEncoded[0] & 0xFF) == 0xFF) &&
				((startTimeEncoded[1] & 0xFF) == 0xFF) &&
				((startTimeEncoded[2] & 0xFF) == 0xFF) &&
				((startTimeEncoded[3] & 0xFF) == 0xFF) &&
				((startTimeEncoded[4] & 0xFF) == 0xFF)) {

			startTime = 0;
			return;
		}

		myDate = (((startTimeEncoded[0] << 8) & 0xFF00) |
		(startTimeEncoded[1] & 0xFF));

		date = decodeMjd(myDate);

		t = *(localtime(&date));

		t.tm_hour = decimal(startTimeEncoded[2]);
		t.tm_min = decimal(startTimeEncoded[3]);
		t.tm_sec = decimal(startTimeEncoded[4]);
		t.tm_isdst = 0;

		startTime = mktime(&t);
	}

	void EventInfo::setDuration(time_t duration) {
		struct tm p;

		this->duration = duration;

		p = *(localtime(&duration));
		durationEncoded[0] = bcd(p.tm_hour);
		durationEncoded[1] = bcd(p.tm_min);
		durationEncoded[2] = bcd(p.tm_sec);
	}

	void EventInfo::setDurationEncoded(char* duration) {
		struct tm t;

		memcpy(durationEncoded, duration, 3);
		t.tm_year = 2000 - 1900;
		t.tm_mon = 0;
		t.tm_mday = 1;
		t.tm_hour = decimal(duration[0]);
		t.tm_min = decimal(duration[1]);
		t.tm_sec = decimal(duration[2]);
		t.tm_isdst = 0;

		this->duration = mktime(&t);
	}

	string EventInfo::getFormattedDuration() {
		struct tm p;

		if (duration == 0) {
			return "";
		}

		p = *(localtime(&duration));

		return (getFormatNumStr(p.tm_hour) + ":" + getFormatNumStr(p.tm_min) +
				":" + getFormatNumStr(p.tm_sec));
	}

	time_t EventInfo::getDuration() {
		return duration;
	}

	char * EventInfo::getDurationEncoded() {
		return durationEncoded;
	}

	void EventInfo::setRunningStatus(unsigned char status) {
		runningStatus = status;
	}

	unsigned char EventInfo::getRunningStatus() {
		return runningStatus;
	}

	void EventInfo::setFreeCAMode(unsigned char mode) {
		freeCAMode = mode;
	}

	unsigned char EventInfo::getFreeCAMode() {
		return freeCAMode;
	}

	unsigned short EventInfo::getDescriptorsLoopLength() {
		return descriptorsLoopLength;
	}

	void EventInfo::setDescriptorsLoopLength(unsigned short length) {
		descriptorsLoopLength = length;
	}

	void EventInfo::insertDescriptor(IMpegDescriptor* info) {
		ShortEventDescriptor* se;
		set<IMpegDescriptor*>::iterator it;
		size_t count = 0;

		descriptors.insert(info);

		for (it = descriptors.begin(); it != descriptors.end(); it++) {
			switch ((*it)->getDescriptorTag()) {
			case 0x4D:
				se = (ShortEventDescriptor*) (*it);
				break;
			default:
				break;
			}
			count += (se->getDescriptorLength() + 2);
		}
		descriptorsLoopLength = count;
	}

	set<IMpegDescriptor*> * EventInfo::getDescriptors() {
		return &descriptors;
	}

}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::tsparser::si::IEventInfo*
		createEventInfo() {

	return new ::br::pucrio::telemidia::ginga::core::tsparser::si::EventInfo();
}

extern "C" void destroyEventInfo(
		::br::pucrio::telemidia::ginga::core::tsparser::si::IEventInfo* ei) {

	delete ei;
}
