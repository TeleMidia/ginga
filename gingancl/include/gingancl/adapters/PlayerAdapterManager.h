/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#ifndef _PLAYERADAPTERMANAGER_H_
#define _PLAYERADAPTERMANAGER_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "ncl/components/NodeEntity.h"
#include "ncl/components/ContentNode.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "gingancl/model/CascadingDescriptor.h"
#include "gingancl/model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "IPlayerAdapterManager.h"
#include "FormatterPlayerAdapter.h"

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
  class PlayerAdapterManager : public Thread, public IPlayerAdapterManager {
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
		FormatterPlayerAdapter* getObjectPlayer(ExecutionObject* execObj);

	private:
		string getMimeTypeFromSchema(string url);

	public:
		//void setStandByState(bool standBy);
		static bool isEmbeddedApp(NodeEntity* dataObject);

	private:
		static bool isEmbeddedAppMediaType(string mediaType);

	public:
		void timeShift(string direction);

	protected:
		void clearDeletePlayers();
		void run();
  };
}
}
}
}
}
}

#endif //_PLAYERADAPTERMANAGER_H_
