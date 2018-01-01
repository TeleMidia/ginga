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

#include "ginga.h"
#include "DsmccTimeBaseClock.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_DATAPROC_BEGIN

DsmccTimeBaseClock::DsmccTimeBaseClock () : Stc ()
{
  numerator = 1;
  denominator = 1;
  endpointAvailable = false;
}

DsmccTimeBaseClock::~DsmccTimeBaseClock () {}

int64_t
DsmccTimeBaseClock::convertToNpt (int64_t base, short numerator,
                                  unsigned short denominator)
{
  double scale, ret;

  scale = ((double)numerator) / denominator;
  ret = base;
  ret = ret * scale;

  return (int64_t)ret;
}

unsigned char
DsmccTimeBaseClock::getContentId ()
{
  return contentId;
}

void
DsmccTimeBaseClock::setContentId (unsigned char id)
{
  contentId = id;
}

short
DsmccTimeBaseClock::getScaleNumerator ()
{
  return numerator;
}

unsigned short
DsmccTimeBaseClock::getScaleDenominator ()
{
  return denominator;
}

void
DsmccTimeBaseClock::setScaleNumerator (short num)
{
  numerator = num;
}

void
DsmccTimeBaseClock::setScaleDenominator (unsigned short den)
{
  denominator = den;
}

uint64_t
DsmccTimeBaseClock::getStartNpt ()
{
  return startNpt;
}

uint64_t
DsmccTimeBaseClock::getStopNpt ()
{
  return stopNpt;
}

void
DsmccTimeBaseClock::setStartNpt (uint64_t start)
{
  startNpt = start;
}

void
DsmccTimeBaseClock::setStopNpt (uint64_t stop)
{
  stopNpt = stop;
}

void
DsmccTimeBaseClock::setEndpointAvailable (bool epa)
{
  endpointAvailable = epa;
}

bool
DsmccTimeBaseClock::getEndpointAvailable ()
{
  return endpointAvailable;
}

void
DsmccTimeBaseClock::refreshStcSample ()
{
  // future problem: never returns a negative value!
  double doubleStc;
  double scale;

  Stc::refreshStcSample ();

  scale = ((double)numerator) / denominator;
  doubleStc = stc;
  doubleStc = doubleStc * scale;
  stc = (uint64_t)doubleStc;
}

GINGA_DATAPROC_END
