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

#ifndef DIGITALCCDESCRIPTOR_H_
#define DIGITALCCDESCRIPTOR_H_

#include "IMpegDescriptor.h"

GINGA_TSPARSER_BEGIN

struct Component
{
  unsigned char componentTag;
  unsigned char digitalRecordingControlData;
  bool maximumBitrateFlag;
  unsigned char copyControlType;
  unsigned char APSControlData;
  unsigned char maximumBitrate;
};
class DigitalCCDescriptor : public IMpegDescriptor
{
protected:
  unsigned char digitalRecordingControlData;
  bool maximumBitrateFlag;
  bool componentControlFlag;
  unsigned char copyControlType;
  unsigned char APSControlData;
  // unsigned char reservedFutureUse;
  unsigned char maximumBitRate;
  unsigned char componentControlLength;
  vector<Component *> *components;

public:
  DigitalCCDescriptor ();
  ~DigitalCCDescriptor ();
  unsigned int getDescriptorLength ();
  unsigned char getDescriptorTag ();
  void print ();
  size_t process (char *data, size_t pos);
};

GINGA_TSPARSER_END

#endif /* DIGITALCCDESCRIPTOR_H_ */
