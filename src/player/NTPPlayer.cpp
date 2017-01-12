/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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

#include "player/NTPPlayer.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {

NTPPlayer::NTPPlayer(GingaScreenID screenId, string mrl) : Player(screenId, mrl) {
	size_t pos = mrl.find_first_of(":");

	if (pos == std::string::npos) {
		this->host = mrl;
		this->port = 123;

	} else {
		this->host = mrl.substr(0, pos);
		this->port = util::stof(mrl.substr(pos + 1, mrl.length() - (pos + 1)));
	}
}

NTPPlayer::~NTPPlayer() {

}

time_t NTPPlayer::getTime(unsigned short* milliseconds) {
	timeb currTimeSnapshop;
	ftime(&currTimeSnapshop);
	addTime(&currTimeSnapshop, diff);
	if (milliseconds) {
		*milliseconds = currTimeSnapshop.millitm;
	}
	return currTimeSnapshop.time;
}

bool NTPPlayer::updateTime() {
	UDPClient udp;
	NTPDatagram *ntpSend, *ntpReceive;
	char *sendBuffer, *recvBuffer;
	timeb tsent;
	unsigned short ms;
	struct tm* valueStruct;
	double intpart, fractpart;

	udp.connectSocket(host, port);
	ntpSend = new NTPDatagram();
	ntpSend->setVersionNumber(3);
	ntpSend->setMode(NTP_CLIENT);
	ntpSend->updateStream();
	ntpSend->getStream(&sendBuffer);

	for(int num = 1; num < TRY_NUM; num++){
		ftime(&tsent);
		if (!udp.send(sendBuffer, NTP_BUFFER_SIZE) == NTP_BUFFER_SIZE) {
			cout << "NTPPlayer::updateTime - Error sending NTP packet." << endl;
			delete ntpSend;
			return false;
		}
		if (udp.dataAvailable(TIMEOUT)) {
			ftime(&currentTime);
			recvBuffer = new char[NTP_BUFFER_SIZE];
			if (udp.receive(&recvBuffer) == NTP_BUFFER_SIZE) {
				ntpReceive = new NTPDatagram(recvBuffer);
				if (ntpReceive->getLeapIndicator() == 3) {
					cout << "NTPPlayer::updateTime - Server clock not ";
					cout << "synchronized." << endl;
					return false;
				}
				valueStruct = NTPDatagram::ntp2unix(ntpReceive->getRecvTimestamp1());
				timeb* recb = tm2timeb(valueStruct,
						ntpFrac2ms(ntpReceive->getRecvTimestamp2()));

				valueStruct = NTPDatagram::ntp2unix(ntpReceive->getTransTimestamp1());
				ms = ntpFrac2ms(ntpReceive->getTransTimestamp2());
				timeb* transb = tm2timeb(valueStruct, ms);
				utcTime.time = mktime(valueStruct) + NTPPlayer::localTimezone();

				double diff1 = diffTime(*transb, *recb);
				double diff2 = diffTime(currentTime, tsent);
				diff2 = ((diff2 - diff1) / 2);
				diff2 = abs(diff2);
				diff2 += ((double)ms/1000);
				fractpart = modf(diff2 , &intpart);
				utcTime.time += (unsigned int) intpart;
				utcTime.millitm = (unsigned int) (fractpart*1000);

				diffNtpLocal();

				delete recb;
				delete transb;
				delete recvBuffer;
				delete ntpSend;
				delete ntpReceive;
				return true;
			}
		}
	}
	cout << "NTPPlayer::updateTime - Connection timeout." << endl;
	delete ntpSend;
	return false;
}

void NTPPlayer::diffNtpLocal() {
	if (utcTime.time > currentTime.time) {
		//utcTime is greater
		diff = diffTime(utcTime, currentTime);
	} else if (utcTime.time == currentTime.time) {
		if (utcTime.millitm >= currentTime.millitm) {
			// utcTime is greater
			diff = diffTime(utcTime, currentTime);
		} else {
			//currentTime is greater
			diff = -diffTime(currentTime, utcTime);
		}
	} else {
		//currentTime is greater
		diff = -diffTime(currentTime, utcTime);
	}
}

double NTPPlayer::diffTime(timeb a, timeb b) {
	//a must be greater than b
	double ret;
	if (a.millitm >= b.millitm) {
		ret = a.millitm - b.millitm;
	} else {
		a.time -= 1;
		ret = (1000 + a.millitm) - b.millitm;
	}
	return (double) (a.time - b.time) + (ret/1000);
}

void NTPPlayer::addTime(timeb* a, double seconds) {
	unsigned short ms;
	double fractpart, intpart;

	fractpart = modf (seconds , &intpart);
	if (fractpart < 0) fractpart = abs(fractpart);
	ms = (unsigned short) (fractpart * 1000);

	if (seconds >= 0) {
		a->millitm += ms;
		if (a->millitm >= 1000) {
			a->millitm = a->millitm % 1000;
			a->time += 1;
		}
		a->time += (unsigned short) intpart;
	} else {
		a->time -= (unsigned short) intpart;
		if (a->millitm >= ms) {
			a->millitm -= ms;
		} else {
			a->millitm = (1000 + a->millitm) - ms;
			a->time -= 1;
		}
	}
}

timeb* NTPPlayer::tm2timeb(struct tm* time, unsigned short milliseconds) {
	time_t t;
	timeb* tb;
	tb = new timeb();
	ftime(tb);
	t = mktime(time) + NTPPlayer::localTimezone();
	tb->time = t;
	tb->millitm = milliseconds;
	return tb;
}

unsigned short NTPPlayer::ntpFrac2ms(alt_u32 value) {
	return (unsigned short)(((double)value)/0x100000000*1000);
}

short NTPPlayer::localTimezone() {
	struct tm when = {0};

	when.tm_mday = 1;
	when.tm_mon = 0;
	when.tm_year = 70;
	when.tm_hour = 0;
	when.tm_min = 0;
	when.tm_sec = 0;

	return (0 - mktime(&when));
}

string NTPPlayer::getTimeString() {
	struct tm* valueStruct;
	time_t a;
	unsigned short ms;
	a = getTime(&ms);
	valueStruct = localtime(&a);
	strftime(text, 20, "%Y:%m:%d:%H:%M:%S", valueStruct);
	sprintf(text, "%s.%03d", text, ms);
	string result(text);
	return result;
}

double NTPPlayer::elapsedTime(string& oldNclTime) {
	int dd, mo, yy, hh, mm, ss, ms, r;
	struct tm when = {0};
	timeb tb, otb;

	if (sscanf(oldNclTime.c_str(), "%d:%d:%d:%d:%d:%d.%d",
			&yy, &mo, &dd, &hh, &mm, &ss, &ms) != 7) {
		return -1.0;
	}

	when.tm_mday = dd;
	when.tm_mon = mo - 1;
	when.tm_year = yy - 1900;
	when.tm_hour = hh;
	when.tm_min = mm;
	when.tm_sec = ss;

	otb.time = mktime(&when);
	otb.millitm = ms;
	ftime(&tb);
	return NTPPlayer::diffTime(tb, otb);
}

}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
		createTimePlayer(GingaScreenID screenId, const char* mrl) {

	return new ::br::pucrio::telemidia::ginga::core::player::NTPPlayer(
			screenId, mrl);
}

extern "C" void destroyTimePlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
