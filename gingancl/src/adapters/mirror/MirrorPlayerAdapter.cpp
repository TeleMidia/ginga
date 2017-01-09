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

#include "gingancl/adapters/mirror/MirrorPlayerAdapter.h"
#include "gingancl/adapters/AdaptersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace mirror {
	MirrorPlayerAdapter::MirrorPlayerAdapter() : FormatterPlayerAdapter() {
		
	}

	MirrorPlayerAdapter::~MirrorPlayerAdapter() {
		player = NULL;
	}

	void MirrorPlayerAdapter::createPlayer() {
		FormatterRegion* fRegion;
		CascadingDescriptor* descriptor;
		LayoutRegion* ncmRegion = NULL;
		GingaSurfaceID mirrorSur;

		clog << "MirrorPlayerAdapter::createPlayer '" << mrl << "'" << endl;

		string prefix = "ncl-mirror://";
		FormatterPlayerAdapter* sourceAdapter = NULL;
		ExecutionObject* execObjSrc;
		size_t pos;

		FormatterPlayerAdapter::createPlayer();

		pos = mrl.find(prefix);
		assert(pos != std::string::npos);
		if (object != NULL && object->getMirrorSrc() != NULL && player != NULL) {
			execObjSrc = object->getMirrorSrc();
			sourceAdapter = (FormatterPlayerAdapter*)manager->getObjectPlayer(execObjSrc);
			mirrorSrc = sourceAdapter->getPlayer();

			descriptor = object->getDescriptor();
			if (descriptor != NULL) {
				fRegion = descriptor->getFormatterRegion();
				if (fRegion != NULL) {
					ncmRegion = fRegion->getLayoutRegion();
					mirrorSur = dm->createSurface(
							myScreen,
							ncmRegion->getWidthInPixels(),
							ncmRegion->getHeightInPixels());

					player->setSurface(mirrorSur);
				}
			}
		}

		clog << "MirrorPlayerAdapter::createPlayer '";
		clog << mrl << "' ALL DONE" << endl;
	}
}
}
}
}
}
}
}

extern "C" IPlayerAdapter* createMirAdapter() {
	return new ::br::pucrio::telemidia::ginga::ncl::adapters::mirror::MirrorPlayerAdapter();
}
