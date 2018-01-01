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

#ifndef CONTENTAVAILABILITYDESCRIPTOR_H_
#define CONTENTAVAILABILITYDESCRIPTOR_H_

#include "IMpegDescriptor.h"

GINGA_TSPARSER_BEGIN

class ContentAvailabilityDescriptor : public IMpegDescriptor
{
protected:
  unsigned char imageConstraintToken;
  unsigned char retentionMode;
  unsigned char retentionState;
  unsigned char encriptionMode;

public:
  ContentAvailabilityDescriptor ();
  ~ContentAvailabilityDescriptor ();
  unsigned char getDescriptorTag ();
  unsigned int getDescriptorLength ();
  void print ();
  size_t process (char *data, size_t pos);
};

GINGA_TSPARSER_END

#endif /* CONTENTAVAILABILITYDESCRIPTOR_H_ */
