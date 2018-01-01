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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef ISERVICEINFO_H_
#define ISERVICEINFO_H_

#include "IMpegDescriptor.h"

GINGA_TSPARSER_BEGIN

class IServiceInfo
{
public:
  static const unsigned char DT_SERVICE = 0x48;
  static const unsigned char DT_LOGO_TRANSMISSION = 0xCF;

public:
  virtual ~IServiceInfo (){};
  virtual size_t getSize () = 0;
  virtual unsigned short getServiceId () = 0;
  virtual bool getEitScheduleFlag () = 0;
  virtual bool getEitPresentFollowingFlag () = 0;
  virtual unsigned char getRunningStatus () = 0;
  virtual string getRunningStatusDescription () = 0;
  virtual unsigned char getFreeCAMode () = 0;
  virtual unsigned short getDescriptorsLoopLength () = 0;
  virtual void insertDescriptor (IMpegDescriptor *descriptor) = 0;
  virtual vector<IMpegDescriptor *> *getDescriptors () = 0;
  virtual size_t process (char *data, size_t pos) = 0;
  virtual void print () = 0;
};

GINGA_TSPARSER_END

#endif /*ISERVICEINFO_H_*/
