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

#ifndef PrefecthManager_H_
#define PrefecthManager_H_

#include "config.h"

#include "IPrefetchManager.h"

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#if HAVE_IC
#include "ic/IInteractiveChannelManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::ic;
#endif

#include <sys/stat.h>
#include <map>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace prefetch {
#if HAVE_IC
	class PrefetchManager : public IPrefetchManager,
			public IInteractiveChannelListener {
#else
	class PrefetchManager : public IPrefetchManager {
#endif

		private:
			map<string, string>* localToRemoteUris;
			map<string, string>* urisToLocalRoots;
			set<string>* scheduledRemoteUris;
			set<string>* scheduledLocalUris;
#if HAVE_IC
			IInteractiveChannelManager* icm;
#else
			void* icm;
#endif
			string prefetchRoot;
			bool synch;

			int kbytes;
			int filesDown;
			int filesSched;

			PrefetchManager();
			~PrefetchManager();
			static IPrefetchManager* _instance;

		public:
			static IPrefetchManager* getInstance();

		private:
			void createDirectory(string newDir);

		public:
			void release();
			void releaseContents();
			string createDocumentPrefetcher(string remoteDocUri);
			string createSourcePrefetcher(string localDocUri, string srcUri);
			bool hasIChannel();
			bool hasRemoteLocation(string localUri);
			string getRemoteLocation(string localUri);
			string getLocalRoot(string remoteUri);

		private:
			void getContent(string remoteUri, string localUri);

		public:
			void setSynchPrefetch(bool synch);

		private:
			void scheduleContent(string remoteUri, string localUri);
			void receiveCode(long respCode){};
			void receiveDataStream(char* buffer, int size){};
			void receiveDataPipe(FILE* fd, int size);
			void downloadCompleted(const char* localUri){};

		public:
			void getScheduledContent(string clientUri);
			void getScheduledContents();
	};
}
}
}
}
}
}

#endif /*PrefetchManager_H_*/
