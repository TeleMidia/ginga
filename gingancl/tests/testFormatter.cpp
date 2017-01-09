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

#include "gingancl/FormatterMediator.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;

#include "gingancl/adapters/image/ImagePlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::image;

#include "config.h"

#include "mb/LocalScreenManager.h"
#include "mb/ILocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

int main(int argc, char** argv, char** envp) {
	FormatterMediator* formatter;
	IScreenManager* dm;
	NclPlayerData* data = new NclPlayerData;
	IPlayerAdapter* player;
	IWindow* w;
	ISurface* s;
	GingaScreenID screenId;

	dm = ScreenManagerFactory::getInstance();
	screenId = dm->createScreen(0, NULL);

	if (argc > 1 && strcmp(argv[1], "links") == 0) {
	} else {
		clog << "Test::main() initializing formatter" << endl;
		data->screenId          = screenId;
		data->baseId            = "test";
		data->playerId          = "test";
		data->devClass          = 0;
		data->x                 = 0;
		data->y                 = 0;
		data->w                 = 0;
		data->h                 = 0;
		data->enableGfx         = false;
		data->parentDocId       = "";
		data->nodeId            = "";
		data->docId             = "";
		data->focusManager      = NULL;
		data->editListener      = NULL;

		formatter = new FormatterMediator(data);
	}

	dm->clearWidgetPools(screenId);

	//TODO: tests
	return 0;
}
