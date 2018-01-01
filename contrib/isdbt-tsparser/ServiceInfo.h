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

#ifndef SERVICEINFO_H_
#define SERVICEINFO_H_

#include "ServiceDescriptor.h"
#include "LogoTransmissionDescriptor.h"

#include "IServiceInfo.h"

GINGA_TSPARSER_BEGIN

class ServiceInfo : public IServiceInfo
{
protected:
  unsigned short serviceId;
  bool eitScheduleFlag;
  bool eitPresentFollowingFlag;
  unsigned char runningStatus;
  unsigned char freeCAMode;
  unsigned short descriptorsLoopLength;
  vector<IMpegDescriptor *> *descriptors;

public:
  static const unsigned char LOGO_TRANSMISSION = 0xCF;
  static const unsigned char SERVICE = 0x48;

public:
  ServiceInfo ();
  ~ServiceInfo ();
  size_t getSize ();
  unsigned short getServiceId ();
  bool getEitScheduleFlag ();
  bool getEitPresentFollowingFlag ();
  unsigned char getRunningStatus ();
  string getRunningStatusDescription ();
  unsigned char getFreeCAMode ();
  unsigned short getDescriptorsLoopLength ();
  void insertDescriptor (IMpegDescriptor *info);
  vector<IMpegDescriptor *> *getDescriptors ();
  void print ();
  size_t process (char *data, size_t pos);
};

GINGA_TSPARSER_END

#endif /*SERVICEINFO_H_*/
