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

#ifndef StcWrapper_H_
#define StcWrapper_H_

#include "config.h"

#if HAVE_TUNER && HAVE_TSPARSER && HAVE_DSMCC
#include "tuner/ISTCProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::tuning;

#include "player/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;
#endif

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace lssm {
#if HAVE_TUNER && HAVE_TSPARSER && HAVE_DSMCC
  class StcWrapper : public ISTCProvider {
	private:
		IPlayer* realStcProvider;

	public:
		StcWrapper(IPlayer* realStcProvider);
		virtual ~StcWrapper();
		bool getSTCValue(uint64_t* stc, int* valueType);
  };
#endif
}
}
}
}
}

#endif /*StcWrapper_H_*/
