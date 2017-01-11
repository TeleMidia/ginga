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

#ifndef ISTCProvider_H_
#define ISTCProvider_H_

#include <stdint.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	static const int ST_90KHz = 0x01;
	static const int ST_27MHz = 0x02;

	class ISTCProvider {
		public:
			virtual ~ISTCProvider(){};
			virtual bool getSTCValue(uint64_t* stc, int* valueType)=0;
	};
}
}
}
}
}
}

#endif /*ISTCProvider_H_*/
