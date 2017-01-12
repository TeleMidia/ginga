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

#include "adapters/AVPlayerAdapter.h"

#include "adapters/AdaptersComponentSupport.h"

#include <string.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace av {
	AVPlayerAdapter::AVPlayerAdapter() : FormatterPlayerAdapter() {
		typeSet.insert("AVPlayerAdapter");
	}

	void AVPlayerAdapter::createPlayer() {
		CascadingDescriptor* descriptor;
		string soundLevel;

		clog << "AVPlayerAdapter::createPlayer for '" << mrl << "'" << endl;

		if (mrl != "") {
			player = new AVPlayer(myScreen, mrl.c_str());
		}

		FormatterPlayerAdapter::createPlayer();

		clog << "AVPlayerAdapter::createPlayer for '" << mrl << "'" << endl;
	}

	bool AVPlayerAdapter::setPropertyValue(
		    AttributionEvent* event,
		    string value) {

		return FormatterPlayerAdapter::setPropertyValue(
				event, value);

	}
}
}
}
}
}
}
}
