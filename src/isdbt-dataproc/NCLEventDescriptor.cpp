/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#include "config.h"
#include "util/functions.h"
using namespace ::ginga::util;

#include "NCLEventDescriptor.h"

BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_NCL_BEGIN

	string NCLEventDescriptor::getEventId(string event) {
		return event.substr(0, 2);
	}

	uint64_t NCLEventDescriptor::getEventNPT(string event) {
		uint64_t nptRef = 0;
		char* strNpt;

		strNpt = (char*)(event.substr(5, 5).c_str());

		nptRef = (strNpt[0] & 0x01);
		nptRef = nptRef << 8;
		nptRef = nptRef | (strNpt[1] & 0xFF);
		nptRef = nptRef << 8;
		nptRef = nptRef | (strNpt[2] & 0xFF);
		nptRef = nptRef << 8;
		nptRef = nptRef | (strNpt[3] & 0xFF);
		nptRef = nptRef << 8;
		nptRef = nptRef | (strNpt[4] & 0xFF);

		return nptRef;
	}

	string NCLEventDescriptor::getCommandTag(string event) {
		string cmdTag = "0x" + itos(event[11] & 0xFF);

		return cmdTag;
	}

	int NCLEventDescriptor::getSequenceNumber(string event) {
		char* strSeq;

		strSeq = (char*)(event.substr(12, 1).c_str());
		return strSeq[0] & 0xFE;
	}

	bool NCLEventDescriptor::getFinalFlag(string event) {
		char* strFF;

		strFF = (char*)(event.substr(12, 1).c_str());
		return strFF[0] & 0x01;
	}

	string NCLEventDescriptor::getPrivateDataPayload(string event) {
		unsigned int privateDataLength;

		privateDataLength = event[10] & 0xFF;
		if (privateDataLength + 11 != event.length()) {
			clog << "NCLEventDescriptor::getPrivateDataPayload Warning! ";
			clog << "invalid private data length(" << privateDataLength;
			clog << ") for event length(" << event.length() << ")";
			clog << endl;
		}
		return event.substr(13, privateDataLength - 3);
	}

	bool NCLEventDescriptor::checkFCS(string event) {
		//TODO: check FCS
		return true;
	}

	string NCLEventDescriptor::extractMarks(string eventParam) {
		string noMarks = trim(eventParam);

		if (eventParam.find("\"") != std::string::npos) {
			noMarks = eventParam.substr(
					eventParam.find_first_of("\"") + 1,
					eventParam.length() - (eventParam.find_first_of("\"") + 1));

			noMarks = noMarks.substr(0, noMarks.find_last_of("\""));
		}

		return noMarks;
	}

BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_NCL_END
