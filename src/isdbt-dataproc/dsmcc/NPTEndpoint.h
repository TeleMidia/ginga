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

#ifndef NPTENDPOINT_H_
#define NPTENDPOINT_H_

#include <stdint.h>
#include "MpegDescriptor.h"

using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_DSMCC_NPT_BEGIN


class NPTEndpoint : public MpegDescriptor {
	private:

	protected:
		uint64_t startNPT;
		uint64_t stopNPT;

		virtual int process();
		int updateStream();

		unsigned int calculateDescriptorSize();

	public:
		NPTEndpoint();
		~NPTEndpoint();

		uint64_t getStartNPT();
		uint64_t getStopNPT();
		void setStartNPT(uint64_t npt);
		void setStopNPT(uint64_t npt);

};


BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_DSMCC_NPT_END
#endif /* NPTENDPOINT_H_ */
