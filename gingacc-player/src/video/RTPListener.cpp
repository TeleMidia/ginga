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

#include "player/RTPListener.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	RTPListener::RTPListener(string url) {
		this->fd  = -1;
		this->url = "";

		this->icm = NULL;
		this->ic  = NULL;

		if (url.find("rtp://") != std::string::npos) {
			this->url = url.substr(6, url.length() - 6);
		}

		if (this->url != "") {
#if HAVE_COMPSUPPORT
			icm = ((ICMCreator*)(cm->getObject("InteractiveChannelManager")))();
#else
			icm = InteractiveChannelManager::getInstance();
#endif

			ic = icm->createInteractiveChannel(url);
			if (ic != NULL) {
				ic->reserveUrl(url, this);
				Thread::startThread();

			} else {
				clog << "RTPListener::RTPListener Warning! Can't create ";
				clog << "IC for '" << url << "'" << endl;
			}
		}
	}

	RTPListener::~RTPListener() {
		closeFile();
		this->url = "";

		if (ic != NULL) {
			ic->releaseUrl();
			delete ic;
			ic = NULL;
		}
	}

	void RTPListener::closeFile() {
		if (fd > -1) {
			fclose(fd);
			fd = -1;
		}
	}

	string RTPListener::getUrl() {
		return url;
	}

	void RTPListener::performIC() {
		ic->performUrl();
	}

	void RTPListener::releaseIC() {
		ic->releaseUrl();
		closeFile();
	}

	void RTPListener::receiveCode(long respCode) {

	}

	void RTPListener::receiveDataStream(char* buffer, int size) {
		if ((fd != NULL) && (size > 0)) {
			try {
				fwrite(buffer, 1, size, fd);

			} catch (const char *except) {
				clog << "RTPListener::receiveDataStream catch: ";
				clog << except << endl;
			}

		} else {
			clog << "RTPListener::receiveDataStream can't write fd = '";
			clog << fd << "' size = '" << size << "'" << endl;
		}
	}

	void RTPListener::receiveDataPipe(FILE* fd, int size) {

	}

	void RTPListener::downloadCompleted(const char* localUri) {

	}

	void RTPListener::run() {
		if (fd < 0) {
			mkfifo(this->url.c_str(), S_IFIFO);
			clog << "RTPListener::RTPListener '" << url << "' CREATEP" << endl;
			fd = fopen(this->url.c_str(), "wb");
			clog << "RTPListener::RTPListener CREATEP OPENED" << endl;
		}
	}
}
}
}
}
}
}
