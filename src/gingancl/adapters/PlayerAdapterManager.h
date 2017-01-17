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

#ifndef _PLAYERADAPTERMANAGER_H_
#define _PLAYERADAPTERMANAGER_H_

#include "util/functions.h"
using namespace ::ginga::util;

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "ncl/components/NodeEntity.h"
#include "ncl/components/ContentNode.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "gingancl/model/CascadingDescriptor.h"
#include "gingancl/model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "system/ITimeBaseProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::time;

#include "player/INCLPlayer.h"
using namespace ::ginga::player;

#include "FormatterPlayerAdapter.h"
#include "IPlayerAdapter.h"

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <map>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_BEGIN

  class PlayerAdapterManager : public Thread {
	private:
		map<string, IPlayerAdapter*> objectPlayers;
		map<string, IPlayerAdapter*> deletePlayers;
		map<IPlayerAdapter*, string> playerNames;
		map<string, string> mimeDefaultTable;
		map<string, string> playerTable;
		IPlayerListener* editingCommandListener;
		IPlayerAdapter* epgFactoryAdapter;
		bool running;
		ITimeBaseProvider* timeBaseProvider;
		NclPlayerData* nclPlayerData;
		pthread_mutex_t mutexPlayer;

		string getPlayerClass(
			    CascadingDescriptor* descriptor, NodeEntity* dataObject);

	public:
		PlayerAdapterManager(NclPlayerData* nclPlayerData);
		virtual ~PlayerAdapterManager();

		bool hasPlayer(IPlayerAdapter* player);

		NclPlayerData* getNclPlayerData();
		void setTimeBaseProvider(ITimeBaseProvider* timeBaseProvider);
		ITimeBaseProvider* getTimeBaseProvider();
		
		void setVisible(
				string objectId,
				string visible,
				AttributionEvent* event);

		bool removePlayer(void* object);

	private:
		bool removePlayer(string objectId);
		void clear();

	public:
		void setNclEditListener(IPlayerListener* listener);

	private:
		void readConfigFiles();
		FormatterPlayerAdapter* initializePlayer(ExecutionObject* object);

	public:
		void* getObjectPlayer(void* execObj);

	private:
		string getMimeTypeFromSchema(string url);

	public:
		static bool isEmbeddedApp(NodeEntity* dataObject);

	private:
		static bool isEmbeddedAppMediaType(string mediaType);

	public:
		void timeShift(string direction);

	protected:
		void clearDeletePlayers();
		void run();
  };

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_END
#endif //_PLAYERADAPTERMANAGER_H_
