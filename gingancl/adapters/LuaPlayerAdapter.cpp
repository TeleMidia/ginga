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

#include "adapters/LuaPlayerAdapter.h"

#include "adapters/AdaptersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace application {
namespace lua {
	LuaPlayerAdapter::LuaPlayerAdapter() : ApplicationPlayerAdapter()  {

	}

	void LuaPlayerAdapter::createPlayer() {
		if (fileExists(mrl)) {
			player = new LuaPlayer(myScreen, mrl.c_str());

		} else {
			player = NULL;
			clog << "LuaPlayerAdapter::createPlayer Warning! ";
			clog << "file not found: '" << mrl.c_str() << "'" << endl;
		}

		ApplicationPlayerAdapter::createPlayer();
	}

	bool LuaPlayerAdapter::setAndLockCurrentEvent(FormatterEvent* event) {
		string interfaceId;

		lockEvent();
		if (preparedEvents.count(event->getId()) != 0 &&
				!event->instanceOf("SelectionEvent") &&
				event->instanceOf("AnchorEvent")) {

			interfaceId = ((AnchorEvent*)event)->getAnchor()->getId();

			if ((((AnchorEvent*)event)->getAnchor())->instanceOf(
					"LabeledAnchor")) {

				interfaceId = ((LabeledAnchor*)((AnchorEvent*)event)->
						getAnchor())->getLabel();

			} else if ((((AnchorEvent*)event)->getAnchor())->instanceOf(
					"LambdaAnchor")) {

				interfaceId = "";
			}

			currentEvent = event;
			((ApplicationExecutionObject*)object)->setCurrentEvent(
					currentEvent);

			player->setCurrentScope(interfaceId);

		} else if (event->instanceOf("AttributionEvent")) {
			interfaceId = ((AttributionEvent*)
					event)->getAnchor()->getPropertyName();

			player->setScope(interfaceId, IPlayer::TYPE_ATTRIBUTION);

			currentEvent = event;
			((ApplicationExecutionObject*)object)->setCurrentEvent(
					currentEvent);

			player->setCurrentScope(interfaceId);

		} else {
			clog << "LuaPlayerAdapter::setAndLockCurrentEvent Warning! ";
			clog << "event '" << event->getId() << "' isn't prepared" << endl;

			unlockEvent();
			return false;
		}

		return true;
	}

	void LuaPlayerAdapter::unlockCurrentEvent(FormatterEvent* event) {
		if (event != currentEvent) {
			clog << "LuaPlayerAdapter::unlockCurrentEvent ";
			clog << "Handling events Warning: currentEvent = '";
			if (currentEvent != NULL) {
				clog << currentEvent->getId();
			}
			clog << "' event requested to unlock = '" << event->getId();
			clog << "'" << endl;
		}
		unlockEvent();
	}
}
}
}
}
}
}
}
}
