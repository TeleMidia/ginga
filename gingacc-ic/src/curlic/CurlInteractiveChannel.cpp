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

#include "ic/curlic/CurlInteractiveChannel.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace ic {
	bool CurlInteractiveChannel::mutexInit = false;
	pthread_mutex_t CurlInteractiveChannel::mutex;
	CURL* CurlInteractiveChannel::curl = NULL;

	CurlInteractiveChannel::CurlInteractiveChannel() {
		fd        = 0;
		buffer    = NULL;
		type      = 0;
		rate      = 0;
		uri       = "";
		listener  = NULL;
		localUri  = "";
		userAgent = "";

		defaultServer = "http://www.club.ncl.org.br";

		if (!mutexInit) {
			mutexInit = true;
			pthread_mutex_init(&mutex, NULL);
		}
	}

	CurlInteractiveChannel::~CurlInteractiveChannel() {

	}

	bool CurlInteractiveChannel::hasConnection() {
		CURLcode res;

		pthread_mutex_lock(&mutex);
		if (curl != NULL) {
			clog << "CurlInteractiveChannel::hasConnection Warning CURL!NULL";
			clog << endl;
		}

		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	        curl_easy_setopt(curl, CURLOPT_URL, defaultServer.c_str());
	        curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1);

			res = curl_easy_perform(curl);
			if (res != CURLE_OK) {
				clog << "CurlInteractiveChannel::hasConnection Warning! ";
				clog << "cant connect, code '" << res << "'" << endl;
				releaseUrl();
				pthread_mutex_unlock(&mutex);
				return false;
			}

	        curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 0);

	        releaseUrl();
			clog << endl;
			clog << "CurlInteractiveChannel::hasConnection OK!" << endl;
			clog << endl;

			pthread_mutex_unlock(&mutex);
			return true;
		}

		pthread_mutex_unlock(&mutex);
		return false;
	}

	void CurlInteractiveChannel::setSourceTarget(string url) {
		string localPath;

		localUri = url;

		if (localUri.find(SystemCompat::getIUriD()) != std::string::npos) {
			localPath = localUri.substr(
					0,
					localUri.find_last_of(SystemCompat::getIUriD()) + 1);

			clog << "CurlInteractiveChannel::setSourceTarget creating '";
			clog << localPath << "' local path" << endl;
			mkdir(localPath.c_str(), 0755);
		}
		fd = fopen(localUri.c_str(), "w+b");
	}

	void CurlInteractiveChannel::setTarget(FILE* fd) {
		this->fd = fd;
	}

	void CurlInteractiveChannel::setTarget(char* buffer) {
		//TODO: buffer handler
	}

	short CurlInteractiveChannel::getType() {
		return type;
	}

	float CurlInteractiveChannel::getRate() {
		return rate;
	}

	void CurlInteractiveChannel::setListener(
			IInteractiveChannelListener* listener) {

		this->listener = listener;
	}

	bool CurlInteractiveChannel::reserveUrl(
			string uri,
			IInteractiveChannelListener* listener,
			string userAgent) {

		clog << "RESERVE '" << uri << "'" << endl;
		this->uri = uri;
		this->listener = listener;
		this->userAgent = userAgent;
		return true;
	}

	bool CurlInteractiveChannel::performUrl() {
		long respCode;
		CURLcode res;
		bool success = false;

		clog << "CurlInteractiveChannel::performUrl '" << uri << "'" << endl;

		pthread_mutex_lock(&mutex);
		if (curl != NULL) {
			clog << "CurlInteractiveChannel::hasConnection Warning CURL!NULL";
			clog << endl;
		}

		curl = curl_easy_init();
		if (curl != NULL) {
	        curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());

	        if (userAgent != "") {
				curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
			}

	        curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 0);
	        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 500000);
	        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)this);
	        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
	        curl_easy_setopt(
	        		curl, CURLOPT_WRITEFUNCTION,
	        		CurlInteractiveChannel::writeCallBack);
		}

		if (curl != NULL) {
			res = curl_easy_perform(curl);
			if (res != CURLE_OK) {
				clog << endl;
				clog << "CurlInteractiveChannel::performUrl Warning! ";
				clog << res << " for '" << uri << "'" << endl;
				clog << endl;

			} else {

				clog << "CurlInteractiveChannel::performUrl '" << uri << "'";
				clog << " done, notifying listeners" << endl;

				if (listener != NULL) {
					if (!positiveResponse(&respCode)) {
						listener->receiveCode(respCode);
					}

					listener->downloadCompleted(localUri.c_str());
				}
				success = true;
			}

			releaseUrl();

		} else {
			clog << "CurlInteractiveChannel::performUrl Warning! ";
			clog << " NULL CURL FOR '" << uri << "'" << endl;
		}

		clog << "CurlInteractiveChannel::performUrl '" << uri << "'";
		clog << " all done!!!" << endl;
		pthread_mutex_unlock(&mutex);
		return success;
	}

	bool CurlInteractiveChannel::releaseUrl() {
		curl_easy_cleanup(curl);
		curl = NULL;

		if (fd > 0) {
			fclose(fd);
			fd = 0;
		}

		return true;
	}

	size_t CurlInteractiveChannel::writeCallBack(
			void* ptr, size_t size, size_t nmemb, void* stream) {

		int w;
		FILE* fd;
		CurlInteractiveChannel* channel;
		IInteractiveChannelListener* l;

		channel = (CurlInteractiveChannel*)stream;
		l  = channel->getListener();
		fd = channel->getLocalFileDescriptor();
		w  = 0;

		if (fd != NULL) {
			w = fwrite(ptr, 1, (size * nmemb), fd);
			if (w != (int)(size * nmemb)) {
				clog << "CurlInteractiveChannel::writeCallBack can't write";
				clog << endl;

			} else if (l != NULL) {
				l->receiveDataPipe(fd, w);
				l->receiveDataStream((char*)ptr, w);
			}

		} else {
			clog << "CurlInteractiveChannel::writeCallBack throw write";
			clog << endl;
		}

		return w;
	}

	bool CurlInteractiveChannel::positiveResponse(long *respCode) {
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, respCode);

		clog << "CurlInteractiveChannel::positiveResponse received code is '";
		clog << *respCode << "' for '" << uri << "'" << endl;
		return false;
	}

	FILE* CurlInteractiveChannel::getLocalFileDescriptor() {
		return fd;
	}

	IInteractiveChannelListener* CurlInteractiveChannel::getListener() {
		return listener;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::ic::IInteractiveChannel*
		createCurlIC() {

	return new ::br::pucrio::telemidia::ginga::core::ic::
			CurlInteractiveChannel();
}

extern "C" void destroyCurlIC(
		::br::pucrio::telemidia::ginga::core::ic::IInteractiveChannel* ic) {

	delete ic;
}
