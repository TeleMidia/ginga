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
		descriptors           = NULL;
		eventId               = 0;
		freeCAMode            = 0;
		runningStatus         = 0;
		sectionNumber         = 0;
		tableId               = 0;
		sectionVersion        = 0;

	}

	EventInfo::~EventInfo() {
		vector<IMpegDescriptor*>::iterator i;
		if(descriptors != NULL){
			i = descriptors->begin();
			while(i != descriptors->end()){
				delete (*i);
				++i;
			}
			delete descriptors;
			descriptors = NULL;
		}
	}
	/* Function to set what kind of section the EventInfo was part of
	 * tableId give information if the EventInfo is present/following (0x4E) or
	 * schedule (0x50, 5F).
	 * These functions should be call only by the section processor (EPGProcessor)
	 */
	void EventInfo::setSectionNumber (unsigned int number){
		sectionNumber = number;
	}
	void EventInfo::setTableId(unsigned int id) {
		tableId = id;
	}
	void EventInfo::setSectionVersion(unsigned int version) {
		sectionVersion = version;
	}
	unsigned int EventInfo::getTableId() {
		return tableId;
	}
	unsigned int EventInfo::getSectionVersion(){
		return sectionVersion;
	}


	/*
	 *
	 *
	 * convertMJDtoUTC Method obtains day, month and year through MJD value. The
	 * Algorithm used is described in ETSI DVD-SI. It is also described
	 * in ABNT SI standard, but there is some errors on it.
	 *
	 */
	struct tm EventInfo::convertMJDtoUTC (unsigned int mjd){

		struct tm start;
		int year,month,day,k, weekDay;

		year = ((mjd-15078.2) / 365.25);
		month =  (( mjd - 14956.1 - (int) (year * 365.25) ) / 30.6001 ) ;
		day = mjd - 14956 - ((int) (year * 365.25)) - ((int)(month * 30.6001));
		if (month == 14 || month == 15){
			k = 1;
		}
		else {
			k = 0;
		}
		year = year + k;
		month = month - 1 - k * 12;
		weekDay = (((mjd + 2) % 7) + 1);

		start.tm_year = year;
		start.tm_mon  = month - 1;
		/*tm.mon goes from 0(January) to 11 (December).
		 *if month is february ('month'= 2), tm.mon is 1
		 */
		start.tm_mday = day;
		start.tm_wday = weekDay;
		start.tm_hour = 0;
		start.tm_min  = 0;
		start.tm_sec  = 0;

		/*
		cout << "EventInfo::convertMJDtoUTC printing..." <<endl;
		cout << "   year: " << start.tm_year << "month: " << start.tm_mon;
		cout << "day: " << start.tm_mday <<endl;
		*/
		return start;

	}
	/*
	 * convertUTCtoMJD Method obtains MJD value through day, month and year in
	 * UTC. The Algorithm used is described in ETSI DVD-SI. It is also described
	 * in ABNT SI standard, but there is some errors on it.
	 */
	int EventInfo::convertUTCtoMJD (int day, int month, int year){
		if (day == 0 || month == 0 || year == 0){
			return 0;
		}
		int l, mjd;
		if (month == 1 || month == 2){
			l = 1;
		}
		else{
			l = 0;
		}
		mjd = 14956 + day + ((int)((year - l)* 365.25)) +
				((int)((month + 1 + l * 12 ) * 30.6001));

		return mjd;
	}

	int EventInfo::convertDecimaltoBCD(int dec){
		return ((dec / 10) << 4) + (dec % 10);
	}

	int EventInfo::convertBCDtoDecimal(int bcd){
		return ((bcd >> 4) * 10) + bcd % 16;
	}
	/*
	 * setStartTime receives an array of bytes which is start_time field.
	 * This 40-bit field contains the start time of the event in UTC-3
	 * and Modified Julian Date (MJD). This field is coded as 16
	 * bits giving the 16 LSBs of MJD followed by 24 bits coded as 6 digits in
	 * 4-bit Binary Coded Decimal (BCD). If the start time is undefined (e.g.
	 * for an event in a NVOD reference service) all bits of the field are set
	 *  to "1"
	 *
	 * The MJD date is stored in 5 bytes vector. The method calls the
	 * convertMJDtoUTC to convert MJD to UTC properly
	 */
	void EventInfo::setStartTime (char* date){
		unsigned int mjd = 0;
		memcpy(startTimeEncoded, date, 5);
		//40 bits (5 bytes) no campo start_time

		if (((startTimeEncoded[0] & 0xFF) == 0xFF) &&
				((startTimeEncoded[1] & 0xFF) == 0xFF) &&
				((startTimeEncoded[2] & 0xFF) == 0xFF) &&
				((startTimeEncoded[3] & 0xFF) == 0xFF) &&
				((startTimeEncoded[4] & 0xFF) == 0xFF)) {

			//startTime = 0;
			return;
		}
		mjd = (((startTimeEncoded[0] << 8) & 0xFF00) |
						(startTimeEncoded[1] & 0xFF));

		startTime = convertMJDtoUTC(mjd);

		startTime.tm_hour = convertBCDtoDecimal((int)startTimeEncoded[2]);
		startTime.tm_min  = convertBCDtoDecimal((int)startTimeEncoded[3]);
		startTime.tm_sec  = convertBCDtoDecimal((int)startTimeEncoded[4]);
		/*
		cout << "EventInfo: setStartTime startTime = " << startTime.tm_mday;
		cout << "/" << startTime.tm_mon << "/" << startTime.tm_year;
		cout << " " << startTime.tm_hour << ":" << startTime.tm_min << endl;
		*/
	}
	/*
	 * getStartTimeSecs and getEndTimeSecs return the startTime and endTime
	 * values in seconds since Epoch (Jan 1, 1970)
	 */
	time_t EventInfo::getStartTimeSecs (){
		time_t secs;
		struct tm start =  startTime;

		secs = mktime(&start);
		localtime(&secs);
		return secs;
	}
	time_t EventInfo::getEndTimeSecs (){
		time_t secs;
		struct tm end =  endTime;

		secs = mktime(&end);
		localtime(&secs);
		return secs;
	}

	unsigned int EventInfo::getDurationSecs() {
		unsigned int secs = 0;
		secs = (((duration.tm_hour * 60) + duration.tm_min) * 60)
				+ duration.tm_sec;

		return secs;
	}

	string EventInfo::getStartTimeSecsStr() {
		time_t secs;
		stringstream str;
		struct tm start =  startTime;

		secs = mktime(&start);
		localtime(&secs);
		str << secs ;

		return str.str();
	}

	string EventInfo::getDurationSecsStr() {
		stringstream str;
		unsigned int secs = 0;

		secs = (((duration.tm_hour * 60) + duration.tm_min) * 60)
				+ duration.tm_sec;

		str << secs;
		return str.str();
	}

	string EventInfo::getEndTimeSecsStr() {
		time_t secs;
		stringstream str;
		struct tm end =  endTime;

		secs = mktime(&end);
		localtime(&secs);
		str << secs ;

		return str.str();
	}


	/*using same struct tm to represent duration, but converting to time_t
	 * directly is a mistake because duration has no day, month and year.*/
	void EventInfo::setDuration(char* dur){

		memcpy(durationEncoded, dur, 3);
		if (((durationEncoded[0] & 0xFF) == 0xFF) &&
						((durationEncoded[1] & 0xFF) == 0xFF) &&
						((durationEncoded[2] & 0xFF) == 0xFF) ){
			return;
		}
		duration.tm_hour = convertBCDtoDecimal((int)durationEncoded[0]);
		duration.tm_min  = convertBCDtoDecimal((int)durationEncoded[1]);
		duration.tm_sec  = convertBCDtoDecimal((int)durationEncoded[2]);

		/*
		cout << "EventInfo::setDuration duration= ";
		cout << "    " << duration.tm_hour << ":" << duration.tm_min << ":";
		cout << duration.tm_sec << endl;
		*/

		endTime = calcEndTime(startTime, duration);
		/*
		cout << "EventInfo::setDuration endTime = " << endTime.tm_mday;
		cout << "/" << endTime.tm_mon << "/" << endTime.tm_year;
		cout << " " << endTime.tm_hour << ":" << endTime.tm_min << endl;
		*/
	}
	/*
	 * Method calcEndTime calculates the end time of an event through start time
	 * and duration values, updating correctly minute, hour, day, month and year.
	 * minute: 0 - 59, hour: 0 - 23, day: 1 - 31 (restricted to the month),
	 * month: 0 - 11 (attention! january = 0, december = 11), year:1900 - ...
	 * (values stored are year - 100, ie, for 1900, the stored value is 100).
	 * Ano bissexto = leap year
	 */

	struct tm EventInfo::calcEndTime(struct tm start, struct tm dur){
		struct tm end;

		end.tm_sec = start.tm_sec + dur.tm_sec;
		int div = end.tm_sec / 60;
		if (div >= 1) {
			end.tm_sec = end.tm_sec - (div * 60);
		}

		end.tm_min = start.tm_min + dur.tm_min + div;
		div = end.tm_min / 60;
		if (div >=1){
			end.tm_min = end.tm_min - (div * 60);
		}

		end.tm_hour = start.tm_hour + dur.tm_hour + div;
		div = end.tm_hour / 24 ;
		if (div>=1){
			end.tm_hour = end.tm_hour - (div * 24);

		}
		end.tm_mday = start.tm_mday + div;
		end.tm_mon = start.tm_mon;
		end.tm_year = start.tm_year;

		if (end.tm_mday > 28){
			if (start.tm_mon == 1){
				int year = start.tm_year + 1900;
				if( (year %4) == 0){
					cout<<"multiplo de 4" <<endl;
					if ( year % 100 != 0){
						//cout<<"ano bissexto multiplo de 4 nao"
						//		" multiplo de 100"<<endl;
						if (end.tm_mday>=30){
							end.tm_mday = div;
							end.tm_mon = 2;
						}
					}
					else if  ((year/100)%4 == 0){ //multiple of 400
							//leap year
							//cout <<" ano bissexto multiplo de 4,"
							//		" 100 e 400"<<endl;
							if (end.tm_mday>=30){
								end.tm_mday = div;
								end.tm_mon = 2;
							}
					}
					else if (end.tm_mday>=29){
							//cout <<" ano nao bissexto multiplo de 100 nao"
							//		" multiplo de 400" <<endl;
							//non leap year
							end.tm_mday = div;
							end.tm_mon = 2;

					}
				}// non multiple of 4
				else if (end.tm_mday>=29){
						//cout <<"ano nao bissexto nao multiplo de 4" <<endl;
						//ano nao bissexto
						end.tm_mday = div;
						end.tm_mon = 2;
				}
			}//april, june, september and november have 30 days
			else if ( end.tm_mday >= 31 && (start.tm_mon == 3 || //april
										    start.tm_mon == 5 || //june
										    start.tm_mon == 8 || //september
										    start.tm_mon == 10)){//november

				end.tm_mday = div;
				end.tm_mon = start.tm_mon + 1;
			}//january,march,may,july,august,october and december have 31 days
			else if ( end.tm_mday >= 32 && (start.tm_mon == 0 || //january
											start.tm_mon == 2 || //march
											start.tm_mon == 4 || //may
											start.tm_mon == 6 || //july
											start.tm_mon == 7 || //august
											start.tm_mon == 9 || //october
											start.tm_mon == 11)){//december
				end.tm_mday = div;
				end.tm_mon = start.tm_mon + 1;
				if (start.tm_mon == 11) {//going to next year
					end.tm_mon = 0;
					end.tm_year = start.tm_year + 1;
				}
			}
		}
		return end;
	}

	struct tm EventInfo::getStartTime(){
		return startTime;
	}

	struct tm EventInfo::getDuration() {
		return duration;
	}

	struct tm EventInfo::getEndTime() {
		return endTime;
	}

	string EventInfo::getStartTimeEncoded() {
		string str;

		str.append(startTimeEncoded, 5);
		return str;
	}
	string EventInfo::getDurationEncoded() {
		string str;

		str.append(startTimeEncoded, 3);
		return str;
	}

	unsigned short EventInfo::getLength() {
		return (descriptorsLoopLength + 12);
	}

	unsigned short EventInfo::getEventId() {
		return eventId;
	}

	string EventInfo::getStartTimeStr() {
		stringstream str;

		str << startTime.tm_mday << "/";
		str << startTime.tm_mon + 1 << " ";
		// tm_mon goes from 0 to 11, so + 1 to print month in 1-12.
		str << startTime.tm_hour << ":";
		str << startTime.tm_min;

		return str.str();
	}
	string EventInfo::getEndTimeStr(){
		stringstream str;

		str << endTime.tm_mday << "/";
		str << endTime.tm_mon + 1  << " ";
		// tm_mon goes from 0 to 11, so + 1 to print month in 1-12.
		str << endTime.tm_hour << ":";
		str << endTime.tm_min;

		return str.str();
	}
	string EventInfo::getDurationStr() {
		stringstream str;

		str << duration.tm_hour << ":";
		str << duration.tm_min;

		return str.str();
	}

	string EventInfo::getFormatNumStr(int un) {
		int fUn;
		fUn = convertDecimaltoBCD(un);
		if (fUn < 10) {
			return "0" + itos(fUn);
		} else {
			return itos(fUn);
		}
	}

	string EventInfo::getRunningStatus() {
		stringstream str;

		str << runningStatus;

		return str.str();
	}
	string EventInfo::getRunningStatusDescription () {
		switch (runningStatus) {
			case 0:
				return "Undefined";
				break;

			case 1:
				return "Off";
				break;

			case 2:
				return "Start within few minutes";
				break;

			case 3:
				return "Paused";
				break;

			case 4:
				return "Running";
				break;
			//5-7 are reserved for future used
		}
	}
	unsigned char EventInfo::getFreeCAMode() {
		return freeCAMode;
	}

	unsigned short EventInfo::getDescriptorsLoopLength() {
		return descriptorsLoopLength;
	}

	vector<IMpegDescriptor*>* EventInfo::getDescriptors() {

		/*vector<IMpegDescriptor*>* descs;

		//lock();
		descs = new vector<IMpegDescriptor*>(descriptors);
		//unlock();

		return descs;
		*/
		return descriptors;
	}
	map<unsigned char, IMpegDescriptor*>* EventInfo::getDescriptorsMap() {
		return desc;
	}
	void EventInfo::print() {
		vector<IMpegDescriptor*>::iterator i;

		cout << "EventInfo::print printing..." << endl;
		cout << " -eventId = "                 << eventId            << endl;
		cout << " -startTime = "               << getStartTimeStr()  << endl;
		cout << " -duration = "                << getDurationStr()   << endl;
		cout << " -endTime = "                 << getEndTimeStr() 	 << endl;
  		cout << " -runningStatus = "           << (unsigned int)runningStatus ;
		cout << endl;
		cout << " -descriptorsLoopLength = "   << descriptorsLoopLength << endl;

		for(i = descriptors->begin(); i != descriptors->end(); ++i) {
			((IMpegDescriptor*)(*i))->print();
		}

	}
	/*
	map<string, string> EventInfo::mapGenerator() {
		map<string, string> t;
		stringstream ss;

		t["startTime"] = getStartTimeStr();
		t["endTime"] = getEndTimeStr();
		ss << runningStatus;
		t["runningStatus"] = ss.str();


	}
	*/
	size_t EventInfo::process (char* data, size_t pos){
		IMpegDescriptor* descriptor;
		unsigned short remainingBytesDescriptor, value;
		stringstream ss;

		cout << "EventInfo::start process" << endl;

		eventId = ((((data[pos] << 8) & 0xFF00) |
				(data[pos+1] & 0xFF)));

		//cout << " and eventId = " << eventId;

		pos += 2;
		setStartTime(data+pos);
		//cout << "startTime = " << getStartTimeStr() << endl;

		pos += 5;
		setDuration(data+pos);
		//cout << "duration = " << getDurationStr() << endl;
		pos += 3;
		runningStatus = ((data[pos] & 0xE0) >> 5);
		freeCAMode = ((data[pos] & 0x10) >> 4);
		descriptorsLoopLength = (
				(((data[pos] & 0x0F) << 8) & 0xFF00) |
				(data[pos+1] & 0xFF));
		pos += 2;

		remainingBytesDescriptor = descriptorsLoopLength;
		if(remainingBytesDescriptor > 0){
			descriptors = new vector<IMpegDescriptor*>;
		}
		
		//cout << "EventInfo::process going to descriptors" << endl; 
		size_t localpos = pos;
		while (remainingBytesDescriptor) {//there's at least one descriptor
			value = ((data[pos+1] & 0xFF) + 2);
			remainingBytesDescriptor -= value;

			switch (data[pos]) {//pos = descriptorTag
				case SHORT_EVENT:
					descriptor = new ShortEventDescriptor();
					//cout << "EventInfo::process sed in" << endl;
					localpos = descriptor->process(data, pos);
					//cout << "EventInfo::process sed in" << endl;

					//ShortEventDescriptor* sed = new ShortEventDescriptor();
					//localpos = sed->process(data, pos);
					//cout << "EventInfo::process ending short and localpos is = ";
					//cout << localpos << " and pos = " << pos << endl;
					pos += value;
					descriptors->push_back(descriptor);

					break;

				case EXTENDED_EVENT:
					descriptor = new ExtendedEventDescriptor();
					//cout << "EventInfo::process eed in" << endl;
					localpos = descriptor->process(data, pos);
					//cout << "EventInfo::process eed out" << endl;
					pos += value;
					//cout << "EventInfo::process ending extended. localpos = ";
					//cout << localpos << " and pos = " << pos << endl;
					descriptors->push_back(descriptor);
					break;

				case COMPONENT:
					descriptor = new ComponentDescriptor();
					//cout << "EventInfo::process component in" << endl;
					localpos = descriptor->process(data, pos);
					//cout << "EventInfo::process component out" << endl;
					pos += value;
					//cout << "EventInfo::process ending Component. localpos is = ";
					//cout << localpos << " and pos = " << pos << endl;
					descriptors->push_back(descriptor);
					break;

				case CONTENT:
					descriptor = new ContentDescriptor();
					//cout << "EventInfo::process content in" << endl;
					localpos = descriptor->process(data, pos);
					//cout << "EventInfo::process content out" << endl;
					pos += value;
					descriptors->push_back(descriptor);
					break;

				case DIGITAL_COPY:
					descriptor = new DigitalCCDescriptor();
					//cout << "EventInfo::process dccd in" << endl;
					localpos = descriptor->process(data, pos);
					//cout << "EventInfo::process dccd out" << endl;
					pos += value;
					descriptors->push_back(descriptor);
					break;

				case AUDIO_COMPONENT:
					descriptor = new AudioComponentDescriptor();
					//cout << "EventInfo::process audio in" << endl;
					localpos = descriptor->process(data, pos);
					//cout << "EventInfo::process content out" << endl;
					pos += value;
					//cout << "EventInfo::process ending audio and localpos is = " << localpos;
					//cout << " and pos = " << pos << endl;

					descriptors->push_back(descriptor);
					break;

				case DATA_CONTENTS:
					descriptor = new DataContentDescriptor();
					//cout << "EventInfo::process data_content in" << endl;
					localpos = descriptor->process(data, pos);
					//cout << "EventInfo::process data_content out" << endl;
					pos += value;
					//cout << "EventInfo:: process ending Data Content and localpos is = ";
					//cout << localpos << " and pos = " << pos << endl;
					descriptors->push_back(descriptor);
					break;

				case SERIES:
					descriptor = new SeriesDescriptor();
					//cout << "EventInfo::process series in" << endl;
					localpos = descriptor->process(data, pos);
					//cout << "EventInfo::process series out" << endl;
					pos += value;
					//cout << "EventInfo::process ending Series and localpos is = " << localpos;
					//cout << " and pos = " << pos << endl;
					descriptors->push_back(descriptor);
					break;

				case PARENTAL_RATING:
					descriptor = new ParentalRatingDescriptor();
					//cout << "EventInfo::process parental in" << endl;
					localpos = descriptor->process(data, pos);
					//cout << "EventInfo::process parental out" << endl;
					pos += value;
					//cout << "EventInfo::process ending parental and localpos = " << localpos;
					//cout << " and pos = " << pos << endl;
					descriptors->push_back(descriptor);
					break;

				case CONTENT_AVAILABILITY:
					descriptor = new ContentAvailabilityDescriptor();
					//cout << "EventInfo::process CA in" << endl;
					localpos = descriptor->process(data, pos);
					//cout << "EventInfo::process CA out" << endl;
					pos += value;
					//cout << "EventInfo::process ending ContentAvail. and localpos =";
					//cout << localpos << " and pos = " << pos << endl;
					descriptors->push_back(descriptor);
					break;

				default: //Event Group Des., Stuffing Des. and Component Group Des.
					cout << "EventInfo::process default descriptor with tag = ";
					cout << hex << (data[pos] & 0xFF) << dec << endl;
					//cout << " and pos = " << pos << " and length = ";
					//cout << (value - 2) << endl;
					pos += value; // pos no proximo descriptorTag
					break;
				}
				//cout << "EventInfo::process remaining = " << remainingBytesDescriptor;
				//cout << endl;
		}
		cout << "EventInfo::process finished pos = " << pos << endl;

		print();
		return pos;
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
