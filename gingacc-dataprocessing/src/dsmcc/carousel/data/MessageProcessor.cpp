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

#include "dataprocessing/dsmcc/carousel/data/MessageProcessor.h"

#ifndef DII_MESSAGE
	#define DII_MESSAGE 0x1002
	#define DDB_MESSAGE 0x1003
	#define DSI_MESSAGE 0x1006
	#define MSG_HEADER_LEN 0xC
#endif //MESSAGES

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	MessageProcessor::MessageProcessor() {
		sd = NULL;
		dii = NULL;
		dsi = NULL;

		Thread::mutexInit(&msgMutex, true);
	}

	MessageProcessor::~MessageProcessor() {
		if (sd != NULL) {
			delete sd;
			sd = NULL;
		}

		if (dii != NULL) {
			delete dii;
			dii = NULL;
		}

		if (dsi != NULL) {
			delete dsi;
			dsi = NULL;
		}

		Thread::mutexLock(&msgMutex);
		Thread::mutexUnlock(&msgMutex);
		Thread::mutexDestroy(&msgMutex);
	}

	ServiceDomain* MessageProcessor::pushMessage(DsmccMessageHeader* message) {
		unsigned int messageId;

		if (message == NULL) {
			clog << "MessageProcessor::pushMessage " << endl;
			clog << "Warning! Try to push NULL message" << endl;

		} else {
			messageId = message->getMessageId();
			if (messageId == DSI_MESSAGE) {
				return processDSIMessage(message);

			} else if (messageId == DII_MESSAGE) {
				return processDIIMessage(message);

			} else if (messageId == DDB_MESSAGE) {
				Thread::mutexLock(&msgMutex);
				msgs.push_back(message);
				Thread::mutexUnlock(&msgMutex);
			}
		}

		processDDBMessages();
		return NULL;
	}

	ServiceDomain* MessageProcessor::processDSIMessage(
			DsmccMessageHeader* message) {

		if (dsi == NULL) {
			clog << "Message Processor dsi done!" << endl;
			dsi = new DownloadServerInitiate(message);
			if (dii != NULL && sd == NULL) {
				clog << "Creating SD" << endl;
				sd = new ServiceDomain(dsi, dii);
				return sd;
			}

		} else {
			//TODO: received another DSI (is it an OC update)
		}
		return NULL;
	}

	ServiceDomain* MessageProcessor::processDIIMessage(
			DsmccMessageHeader* message) {

		if (dii == NULL) {
			dii = new DownloadInfoIndication(message);

			/*
			 * TODO: start process all DII file?
			 * or has every DII file less then 4066
			 * bytes?
			 */
			clog << "Message Processor dii done!" << endl;
			if (dsi != NULL && sd == NULL) {
				clog << "Creating SD" << endl;
				sd = new ServiceDomain(dsi, dii);

				return sd;
			}

		} else {
			//TODO: received another DII (is it an OC update)
		}
		return NULL;
	}

	void MessageProcessor::processDDBMessages() {
		DsmccMessageHeader* msg;
		DownloadDataBlock* ddb;

		if (sd != NULL) {
			Thread::mutexLock(&msgMutex);
			while (!msgs.empty()) {
				msg = *(msgs.begin());
				ddb = new DownloadDataBlock(msg);
				sd->receiveDDB(ddb);
				delete ddb;

				msgs.erase(msgs.begin());
			}
			Thread::mutexUnlock(&msgMutex);
		}
	}

	ServiceDomain* MessageProcessor::getServiceDomain() {
		return sd;
	}

	void MessageProcessor::checkTasks() {
		processDDBMessages();
	}
}
}
}
}
}
}
}
