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

#ifndef StcWrapper_H_
#define StcWrapper_H_

#include "config.h"

#if WITH_ISDBT
#include "isdbt-tuner/ISTCProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::tuning;

#include "player/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;
#endif

#include <string>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_LSSM_BEGIN

#if WITH_ISDBT
  class StcWrapper : public ISTCProvider {
	private:
		IPlayer* realStcProvider;

	public:
		StcWrapper(IPlayer* realStcProvider);
		virtual ~StcWrapper();
		bool getSTCValue(uint64_t* stc, int* valueType);
  };
#endif

BR_PUCRIO_TELEMIDIA_GINGA_LSSM_END
#endif /*StcWrapper_H_*/
