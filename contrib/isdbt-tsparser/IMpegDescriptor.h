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

#ifndef I_MPEG_DESCRIPTOR_H
#define I_MPEG_DESCRIPTOR_H

#include "ginga.h"

GINGA_TSPARSER_BEGIN

class IMpegDescriptor
{
protected:
  unsigned char descriptorTag;
  unsigned char descriptorLength;

public:
  static const int NPT_REFERENCE_TAG = 0x01;
  static const int NPT_ENDPOINT_TAG = 0x02;
  static const int STR_MODE_TAG = 0x03;
  static const int STR_EVENT_TAG = 0x04;

  virtual ~IMpegDescriptor (){};
  virtual unsigned char getDescriptorTag () = 0;
  virtual unsigned int getDescriptorLength () = 0;
  virtual size_t process (char *data, size_t pos) = 0;
  virtual void print () = 0;
};

GINGA_TSPARSER_END

#endif /* I_MPEG_DESCRIPTOR_H_*/
