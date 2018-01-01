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

#ifndef ILOGOTRANSMISSIONDESCRIPTOR_H_
#define ILOGOTRANSMISSIONDESCRIPTOR_H_

#include "IMpegDescriptor.h"

GINGA_TSPARSER_BEGIN

class ILogoTransmissionDescriptor : public IMpegDescriptor
{
public:
  virtual ~ILogoTransmissionDescriptor (){};
  virtual void setDescriptorLength (unsigned short length) = 0;
  virtual unsigned char getDescriptorLength () = 0;
  virtual void setType (unsigned char type) = 0;
  virtual unsigned char getType () = 0;
  virtual void setLogoId (unsigned short id) = 0;
  virtual unsigned short getLogoId () = 0;
  virtual void setLogoVersion (unsigned short version) = 0;
  virtual unsigned short getLogoVersion () = 0;
  virtual void setDownloadDataId (unsigned short id) = 0;
  virtual unsigned short getDownloadDataId () = 0;
  virtual void setName (const string &name) = 0;
  virtual string getName () = 0;
  virtual unsigned char getNameLength () = 0;
  virtual unsigned char getDescriptorTag () = 0;
};

GINGA_TSPARSER_END

#endif /*ILOGOTRANSMISSIONDESCRIPTOR_H_*/
