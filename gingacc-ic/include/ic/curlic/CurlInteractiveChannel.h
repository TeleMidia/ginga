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

#ifndef _CurlInteractiveChannel_H_
#define _CurlInteractiveChannel_H_

#include "../IInteractiveChannel.h"

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "curl/curl.h"

#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace ic {
  class CurlInteractiveChannel : public IInteractiveChannel {
	private:
		FILE* fd;
		char* buffer;
		short type;
		float rate;
		string localUri;
		string defaultServer;
		string uri;
		string userAgent;
		static CURL* curl;
		static pthread_mutex_t mutex;
		static bool mutexInit;
		IInteractiveChannelListener* listener;

	public:
		CurlInteractiveChannel();
		~CurlInteractiveChannel();
		bool hasConnection();
		void setSourceTarget(string url);
		void setTarget(FILE* fd);
		void setTarget(char* buffer);
		short getType();
		float getRate();
		void setListener(IInteractiveChannelListener* listener);
		bool reserveUrl(
				string uri,
				IInteractiveChannelListener* listener=NULL,
				string userAgent="");

		bool performUrl();
		bool releaseUrl();

	private:
		static size_t writeCallBack(
				void* ptr, size_t size, size_t nmemb, void* stream);

		bool positiveResponse(long* respCode);
		FILE* getLocalFileDescriptor();
		IInteractiveChannelListener* getListener();
  };
}
}
}
}
}
}

#endif /*CurlInteractiveChannel*/
