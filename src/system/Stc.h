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

#ifndef STC_H_
#define STC_H_

#define SYSTEM_CLOCK_FREQUENCY 27000000
#define SYSTEM_CLOCK_FREQUENCY_90 90000

#include "SystemCompat.h"
using namespace ::ginga::system;


GINGA_SYSTEM_BEGIN

	class Stc {
		protected:
			uint64_t stc;
			struct timeval clockRef;

			uint64_t reference;

			virtual void refreshStcSample();

		public:
			Stc();
			virtual ~Stc();

			static int timevalSubtract(
					struct timeval *result,
					struct timeval *x,
					struct timeval *y);

			static uint64_t baseExtToStc(uint64_t base, uint64_t ext);
			static uint64_t stcToBase(uint64_t stc);
			static uint64_t stcToExt(uint64_t stc);
			static double stcToSecond(uint64_t stc);
			static double baseToSecond(uint64_t base);
			static uint64_t secondToStc(double seconds);
			static uint64_t secondToBase(double seconds);

			uint64_t getReference();
			void setReference(uint64_t pcr);
			void setReference(uint64_t base, uint64_t ext);
			uint64_t getStc();
			uint64_t getStcBase();
			uint64_t getStcExt();
			double getBaseToSecond();
	};

GINGA_SYSTEM_END

#endif /* STC_H_ */
