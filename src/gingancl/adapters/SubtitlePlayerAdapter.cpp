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

#include "SubtitlePlayerAdapter.h"

#include "AdaptersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace text {
	SubtitlePlayerAdapter::SubtitlePlayerAdapter() : FormatterPlayerAdapter() {
		typeSet.insert("SubtitlePlayerAdapter");
	}

	void SubtitlePlayerAdapter::createPlayer() {
		string paramValue;
		CascadingDescriptor* descriptor;
		player = new SrtPlayer(myScreen, mrl.c_str());

		descriptor = object->getDescriptor();
		if (descriptor != NULL) {
			paramValue = descriptor->getParameterValue("x-setFontUri");
			if (paramValue == "") {
				paramValue = SystemCompat::appendGingaFilesPrefix("font/decker.ttf");
			}
			player->setPropertyValue("x-setFontUri", paramValue);

			paramValue = descriptor->getParameterValue("x-setFontSize");
			if (paramValue == "") {
				paramValue = "30";
			}
			player->setPropertyValue("x-setFontSize", paramValue);

			paramValue = descriptor->getParameterValue("x-controlVisibility");
			if (paramValue == "") {
				paramValue = "FALSE";
			}
			player->setPropertyValue("x-controlVisibility", paramValue);

			paramValue = descriptor->getParameterValue("x-setFontColor");
			if (paramValue == "") {
				paramValue = descriptor->getParameterValue("x-setRGBFontColor");
				if (paramValue == "") {
					paramValue = "191,191,0";
				}
				player->setPropertyValue("x-setRGBFontColor", paramValue);

			} else {
				player->setPropertyValue("x-setFontColor", paramValue);
			}
		}

		player->setPropertyValue("loadFile", mrl);
		FormatterPlayerAdapter::createPlayer();
	}
}
}
}
}
}
}
}
