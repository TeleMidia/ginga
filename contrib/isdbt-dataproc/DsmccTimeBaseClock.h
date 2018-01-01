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

#ifndef TIMEBASECLOCK_H_
#define TIMEBASECLOCK_H_

#include "system/Stc.h"
using namespace ::ginga::system;

GINGA_DATAPROC_BEGIN

class DsmccTimeBaseClock : public Stc
{
private:
protected:
  unsigned char contentId;
  short numerator;
  unsigned short denominator;
  bool endpointAvailable;
  uint64_t startNpt;
  uint64_t stopNpt;

  void refreshStcSample ();

public:
  DsmccTimeBaseClock ();
  ~DsmccTimeBaseClock ();

  static int64_t convertToNpt (int64_t base, short numerator,
                               unsigned short denominator);

  unsigned char getContentId ();
  void setContentId (unsigned char id);
  short getScaleNumerator ();
  unsigned short getScaleDenominator ();
  uint64_t getStartNpt ();
  uint64_t getStopNpt ();
  bool getEndpointAvailable ();
  void setScaleNumerator (short num);
  void setScaleDenominator (unsigned short den);
  void setStartNpt (uint64_t start);
  void setStopNpt (uint64_t stop);
  void setEndpointAvailable (bool epa);
};

GINGA_DATAPROC_END

#endif /* TIMEBASECLOCK_H_ */
