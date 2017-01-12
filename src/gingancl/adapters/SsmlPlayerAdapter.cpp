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

#include "adapters/SsmlPlayerAdapter.h"

#include "adapters/AdaptersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace tts {
	SsmlPlayerAdapter::SsmlPlayerAdapter() :
				FormatterPlayerAdapter() {

          		typeSet.insert("SsmlPlayerAdapter");
	}

	void SsmlPlayerAdapter::createPlayer() {
		clog << "SsmlPlayerAdapter::createPlayer '" << mrl << "'" << endl;

		string paramValue;
		CascadingDescriptor* descriptor;

		bool hasVisual = false;

		player = new SsmlPlayer(myScreen, mrl.c_str());

                clog << "SsmlPlayerAdapter::createPlayer '";
		clog << mrl << "' ALL DONE" << endl;


		FormatterPlayerAdapter::createPlayer();
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::ncl::adapters::IPlayerAdapter*
		createSsmlAdapter() {

	return new ::br::pucrio::telemidia::ginga::ncl::adapters::tts::
                SsmlPlayerAdapter();
}

extern "C" void destroySsmlAdapter(
		::br::pucrio::telemidia::ginga::ncl::adapters::IPlayerAdapter* player) {

	delete player;
}
