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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "Stc.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace time {
	Stc::Stc() {
		reference        = 0;
		stc              = 0;
		clockRef.tv_sec  = 0;
		clockRef.tv_usec = 0;
	}

	Stc::~Stc() {

	}

	int Stc::timevalSubtract(
			struct timeval *result, struct timeval *x, struct timeval *y) {

		int nsec;

		if (x->tv_usec < y->tv_usec) {
			nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
			y->tv_usec -= 1000000 * nsec;
			y->tv_sec  += nsec;

		}

		if (x->tv_usec - y->tv_usec > 1000000) {
			nsec = (x->tv_usec - y->tv_usec) / 1000000;
			y->tv_usec += 1000000 * nsec;
			y->tv_sec  -= nsec;
		}

		result->tv_sec  = x->tv_sec - y->tv_sec;
		result->tv_usec = x->tv_usec - y->tv_usec;

		return x->tv_sec < y->tv_sec;
	}

	uint64_t Stc::baseExtToStc(uint64_t base, uint64_t ext) {
		return ((base * 300) + ext);
	}

	uint64_t Stc::stcToBase(uint64_t stc) {
		uint64_t value = 0;
		value = 1 << 16;
		value = value << 17;
		return (((uint64_t)(stc / 300)) % value);
	}

	uint64_t Stc::stcToExt(uint64_t stc) {
		return (((uint64_t) stc) % 300);
	}

	double Stc::stcToSecond(uint64_t stc) {
		double ret = stc;
		ret = ret / SYSTEM_CLOCK_FREQUENCY;
		return ret;
	}

	double Stc::baseToSecond(uint64_t base) {
		double ret = base;
		ret = ret / SYSTEM_CLOCK_FREQUENCY_90;
		return ret;
	}

	uint64_t Stc::secondToStc(double seconds) {
		return (uint64_t)(seconds * SYSTEM_CLOCK_FREQUENCY);
	}

	uint64_t Stc::secondToBase(double seconds) {
		return (uint64_t) (seconds * SYSTEM_CLOCK_FREQUENCY_90);
	}

	void Stc::refreshStcSample() {
		struct timeval currentRef;
		struct timeval result;
		uint64_t clockedSec, clockedUsec;

		SystemCompat::getUserClock(&currentRef);
		timevalSubtract(&result, &currentRef, &clockRef);
		clockedSec  = result.tv_sec * 27000000;
		clockedUsec = result.tv_usec * 27;
		stc = clockedSec + clockedUsec;
	}

	uint64_t Stc::getReference() {
		return reference;
	}

	void Stc::setReference(uint64_t pcr) {
		reference = pcr;
		SystemCompat::getUserClock(&clockRef);
	}

	void Stc::setReference(uint64_t base, uint64_t ext) {
		reference = ((base * 300) + ext);
		SystemCompat::getUserClock(&clockRef);
	}

	uint64_t Stc::getStc() {
		refreshStcSample();
		stc = stc + reference;
		return stc;
	}

	uint64_t Stc::getStcBase() {
		refreshStcSample();
		stc = stc + reference;
		uint64_t value = 0;
		value = 1 << 16;
		value = value << 17;
		return (((uint64_t)(stc / 300)) % value);
	}

	uint64_t Stc::getStcExt() {
		refreshStcSample();
		stc = stc + reference;
		return (((uint64_t) stc) % 300);
	}

	double Stc::getBaseToSecond() {
		return baseToSecond(getStcBase());
	}
}
}
}
}
}
}
}
