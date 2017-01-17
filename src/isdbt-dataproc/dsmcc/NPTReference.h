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

#ifndef NPTREFERENCE_H_
#define NPTREFERENCE_H_

#include <stdint.h>
#include "MpegDescriptor.h"

using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_DSMCC_NPT_BEGIN


class NPTReference : public MpegDescriptor {
	private:

	protected:
		char postDiscontinuityIndicator;
		unsigned char contentId;
		uint64_t stcRef;
		uint64_t nptRef;
		short scaleNumerator;
		unsigned short scaleDenominator;

		virtual int process();
		int updateStream();

		unsigned int calculateDescriptorSize();

	public:
		NPTReference();
		~NPTReference();

		char getPostDiscontinuityIndicator();
		unsigned char getContentId();
		uint64_t getStcRef();
		uint64_t getNptRef();
		short getScaleNumerator();
		unsigned short getScaleDenominator();

		void setPostDiscontinuityIndicator(char indicator);
		void setContentId(unsigned char id);
		void setStcRef(uint64_t stc);
		void setNptRef(uint64_t npt);
		void setScaleNumerator(short numerator);
		void setScaleDenominator(unsigned short denominator);

};


BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_DSMCC_NPT_END
#endif /* NPTREFERENCE_H_ */
