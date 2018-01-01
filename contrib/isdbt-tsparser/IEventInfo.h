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

#ifndef IEVENTINFO_H_
#define IEVENTINFO_H_

#include "IMpegDescriptor.h"

GINGA_TSPARSER_BEGIN

class IEventInfo
{
public:
  // obrigatory descriptors tag:
  static const unsigned char DT_SHORT_EVENT = 0x4D;
  static const unsigned char DT_COMPONENT = 0X50;
  static const unsigned char DT_AUDIO_COMPONENT = 0XC4;
  static const unsigned char DT_PARENTAL_RATING = 0x55;
  // optional descriptors tag:
  static const unsigned char DT_EXTENDED_EVENT = 0x4E;
  static const unsigned char DT_CONTENT = 0x54;
  static const unsigned char DT_DIGITAL_COPY = 0xC1;
  static const unsigned char DT_DATA_CONTENTS = 0XC7;
  static const unsigned char DT_SERIES = 0XD5;
  static const unsigned char DT_EVENT_GROUP = 0x55;
  static const unsigned char DT_CONTENT_AVAILABILITY = 0XDE;
  static const unsigned char DT_STUFFING = 0X42;
  static const unsigned char DT_COMPONENT_GROUP = 0xD9;

public:
  virtual ~IEventInfo (){};

  virtual time_t getStartTimeSecs () = 0; // startTime in secs since Epoch
  virtual time_t getEndTimeSecs () = 0;   // endTime in secs since Epoch
  virtual unsigned int getDurationSecs () = 0;

  virtual string getStartTimeSecsStr () = 0;
  virtual string getEndTimeSecsStr () = 0;
  virtual string getDurationSecsStr () = 0;

  virtual struct tm getStartTime () = 0;
  virtual struct tm getDuration () = 0;
  virtual struct tm getEndTime () = 0;
  virtual string getStartTimeStr () = 0;
  virtual string getEndTimeStr () = 0;
  virtual string getDurationStr () = 0;
  virtual unsigned short getLength () = 0;
  virtual unsigned short getEventId () = 0;
  virtual string getRunningStatus () = 0;
  virtual string getRunningStatusDescription () = 0;
  virtual unsigned char getFreeCAMode () = 0;
  virtual unsigned short getDescriptorsLoopLength () = 0;
  virtual vector<IMpegDescriptor *> *getDescriptors () = 0;
  virtual size_t process (char *data, size_t pos) = 0;
  virtual void print () = 0;
};

GINGA_TSPARSER_END

#endif /*IEVENTINFO_H_*/
