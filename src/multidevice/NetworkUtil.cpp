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
#include "NetworkUtil.h"
#include "DeviceDomain.h"

#include <map>
using namespace std;

map<int, char> _rcf; //received control frames;
map<int, char> _rdf; //received data frames;

char _scf = -1; //sending control frame;
char _sdf = -1; //sending data frame;

/*static char sendingControlFrameId   = -1;
static char receivedControlFrameId = -2;

static char sendingDataFrameId      = -1;
static char receivedDataFrameId    = -2;*/

BR_PUCRIO_TELEMIDIA_GINGA_CORE_MULTIDEVICE_BEGIN

	bool isValidRecvFrame(int recvFrom, int myIP, char* frame) {
		//ATS: if the recvFrom = myIP the frame is sent along, in order to be validated based on its destClass
		map<int, char>::iterator i;
		int frameType;
		char frameId;

		frameId   = getFrameId(frame);
		frameType = getFrameType(frame);

		if (isControlFrame(frameType)) {
			i = _rcf.find(recvFrom);
			if (i != _rcf.end()) {
				if (frameId == i->second) {
					//TODO: improve the //ATS section code
					//ATS BEGIN
					if (recvFrom == myIP) {
						clog << "NetworkUtil::isValidRecvFrame():isControlFrame(frameType) and (recvFrom == myIP)" << endl;
						_rcf[recvFrom] = frameId;
						return true;
					}
					else {
						clog << "NetworkUtil::isValidRecvFrame():isControlFrame(frameType) error" << endl;
						return false;
					}
					//ATS END
				}
			}
			clog << "NetworkUtil::isValidRecvFrame():isControlFrame == true"<<endl;
			_rcf[recvFrom] = frameId;
			/*if ((frameId == 0 && receivedControlFrameId > 110) ||
					(frameId > receivedControlFrameId) ||
					(receivedControlFrameId == 120)) {

			receivedControlFrameId = frameId;*/
			return true;
			//}

		} else {
			i = _rdf.find(recvFrom);
			if (i != _rdf.end()) {
				if (frameId == i->second) {
					return false;
				}
			}

			_rdf[recvFrom] = frameId;
			/*if ((frameId == 0 && receivedDataFrameId > 110) ||
					(frameId > receivedDataFrameId) ||
					(receivedDataFrameId == 120)) {

			receivedDataFrameId = frameId;*/
			return true;
			//}
		}

		return false;
	}

	bool isControlFrame(int frameType) {
		if (frameType < DeviceDomain::FT_MEDIACONTENT) {
			return true;
		}

		return false;
	}

	char getFrameId(char* frame) {
		return frame[0];
	}

	int getFrameType(char* frame) {
		return (((unsigned char)frame[6]) & 0xFF);
	}

	unsigned int getUIntFromStream(const char* stream) {
		return ((((unsigned char)stream[0]) & 0xFF) |
				((((unsigned char)stream[1]) << 8) & 0xFF00) |
				((((unsigned char)stream[2]) << 16) & 0xFF0000) |
				((((unsigned char)stream[3]) << 24) & 0xFF000000));
	}

	char* getStreamFromUInt(unsigned int uint) {
		char* stream;

		stream = new char[4];
		stream[0] = uint & 0xFF;
		stream[1] = (uint & 0xFF00) >> 8;
		stream[2] = (uint & 0xFF0000) >> 16;
		stream[3] = (uint & 0xFF000000) >> 24;

		return stream;
	}

	string getStrIP(unsigned int someIp) {
		return itos(someIp & 0xFF) + "." +
				itos((someIp & 0xFF00) >> 8) + "." +
				itos((someIp & 0xFF0000) >> 16) + "." +
				itos((someIp & 0xFF000000) >> 24);
	}

	char* mountFrame(
			int sourceIp,
			int destDevClass,
			int mountFrameType,
			unsigned int payloadSize) {

		char* fourBytesStream;
		char* frame = NULL;

		try {
			frame = new char[HEADER_SIZE + payloadSize];

		} catch (bad_alloc &e) {
			clog << "BaseDeviceDomain::mountFrame Warning! ";
			clog << "can't alloc '" << HEADER_SIZE + payloadSize << "'";
			clog << " bytes" << endl;

			return NULL;
		}

		if (isControlFrame(mountFrameType)) {
			if (_scf >= 120) {
				_scf = -1;
			}

			_scf++;
			frame[0] = _scf;

		} else {
			if (_sdf >= 120) {
				_sdf = -1;
			}

			_sdf++;
			frame[0] = _sdf;
		}

		fourBytesStream = getStreamFromUInt(sourceIp);
		memcpy(frame + 1, fourBytesStream, 4);
		delete[] fourBytesStream;

		frame[5] = destDevClass;
		frame[6] = mountFrameType;

		fourBytesStream = getStreamFromUInt(payloadSize);
		memcpy(frame + 7, fourBytesStream, 4);
		delete[] fourBytesStream;

		return frame;
	}

BR_PUCRIO_TELEMIDIA_GINGA_CORE_MULTIDEVICE_END
