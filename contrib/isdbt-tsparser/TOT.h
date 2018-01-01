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

#ifndef TOT_H_
#define TOT_H_

#include "IMpegDescriptor.h"

#include "LocalTimeOffsetDescriptor.h"

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "ITOT.h"

GINGA_TSPARSER_BEGIN

class TOT : public ITOT
{
protected:
  unsigned char UTC3Time[5];
  struct tm time;
  unsigned short descriptorsLoopLength;
  vector<IMpegDescriptor *> *descriptors;

public:
  TOT ();
  ~TOT ();
  string getUTC3TimeStr ();
  struct tm getUTC3TimeTm ();
  vector<IMpegDescriptor *> *getDescriptors ();
  void print ();
  void process (void *payloadBytes, unsigned int payloadSize);

private:
  void calculateTime ();
  struct tm convertMJDtoUTC (unsigned int mjd);
  int convertBCDtoDecimal (int bcd);
};

GINGA_TSPARSER_END

#endif /* TOT_H_ */
