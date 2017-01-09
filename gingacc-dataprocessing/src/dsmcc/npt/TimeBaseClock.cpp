/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "dataprocessing/dsmcc/npt/TimeBaseClock.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace dsmcc {
namespace npt {
TimeBaseClock::TimeBaseClock() : Stc() {
	numerator   = 1;
	denominator = 1;
	endpointAvailable = false;
}

TimeBaseClock::~TimeBaseClock() {

}

int64_t TimeBaseClock::convertToNpt(
		int64_t base,
		short numerator,
		unsigned short denominator) {

	double scale, ret;

	scale = ((double) numerator) / denominator;
	ret   = base;
	ret   = ret * scale;

	return (int64_t) ret;
}

unsigned char TimeBaseClock::getContentId() {
	return contentId;
}

void TimeBaseClock::setContentId(unsigned char id) {
	contentId = id;
}

short TimeBaseClock::getScaleNumerator() {
	return numerator;
}

unsigned short TimeBaseClock::getScaleDenominator() {
	return denominator;
}

void TimeBaseClock::setScaleNumerator(short num) {
	numerator = num;
}

void TimeBaseClock::setScaleDenominator(unsigned short den) {
	denominator = den;
}

uint64_t TimeBaseClock::getStartNpt() {
	return startNpt;
}

uint64_t TimeBaseClock::getStopNpt() {
	return stopNpt;
}

void TimeBaseClock::setStartNpt(uint64_t start) {
	startNpt = start;
}

void TimeBaseClock::setStopNpt(uint64_t stop) {
	stopNpt = stop;
}

void TimeBaseClock::setEndpointAvailable(bool epa) {
	endpointAvailable = epa;
}

bool TimeBaseClock::getEndpointAvailable() {
	return endpointAvailable;
}

void TimeBaseClock::refreshStcSample() {
	// future problem: never returns a negative value!
	double doubleStc;
	double scale;

	Stc::refreshStcSample();

	scale     = ((double) numerator) / denominator;
	doubleStc = stc;
	doubleStc = doubleStc * scale;
	stc       = (uint64_t) doubleStc;
}

}
}
}
}
}
}
}
}
