/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef EVENTINFO_H_
#define EVENTINFO_H_

#include "IEventInfo.h"

#include "util/functions.h"
using namespace ::ginga::util;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

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

#include "IMpegDescriptor.h"

//#include <time.h>
//#include <vector>
//#include <map>

GINGA_TSPARSER_BEGIN

class EventInfo : public IEventInfo
{
protected:
  unsigned short eventId;
  char startTimeEncoded[5];
  char durationEncoded[3];
  unsigned char runningStatus;
  unsigned char freeCAMode;
  unsigned short descriptorsLoopLength;
  vector<IMpegDescriptor *> *descriptors;
  map<unsigned char, IMpegDescriptor *> *desc;

  /*attention: tm_mon represents month from 0(January) to 11(December).
   * the print function is printing month in 1(jan) to 12(dec)*/
  struct tm startTime;
  struct tm duration;
  struct tm endTime;

public:
  EventInfo ();
  ~EventInfo ();
  void setStartTime (char *date);
  void setDuration (char *dur);
  struct tm calcEndTime (struct tm start, struct tm end);

  time_t getStartTimeSecs ();
  time_t getEndTimeSecs ();
  unsigned int getDurationSecs ();

  virtual string getStartTimeSecsStr ();
  virtual string getEndTimeSecsStr ();
  virtual string getDurationSecsStr ();

  struct tm getStartTime ();
  struct tm getDuration ();
  struct tm getEndTime ();

  string getStartTimeEncoded ();
  string getDurationEncoded ();
  string getStartTimeStr ();
  string getEndTimeStr ();
  string getDurationStr ();

  unsigned short getLength ();
  unsigned short getEventId ();
  string getRunningStatus ();
  string getRunningStatusDescription ();
  unsigned char getFreeCAMode ();
  unsigned short getDescriptorsLoopLength ();

  vector<IMpegDescriptor *> *getDescriptors ();
  map<unsigned char, IMpegDescriptor *> *getDescriptorsMap ();

  void print ();
  size_t process (char *data, size_t pos);

protected:
  int convertDecimaltoBCD (int dec);
  int convertBCDtoDecimal (int bcd);
  struct tm convertMJDtoUTC (unsigned int mjd);
  int convertUTCtoMJD (int day, int month, int year);

private:
  string getFormatNumStr (int un);
  // void clearDescriptors();
};

GINGA_TSPARSER_END

#endif /*EVENTINFO_H_*/
