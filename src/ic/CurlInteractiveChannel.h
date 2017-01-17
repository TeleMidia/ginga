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

#ifndef _CurlInteractiveChannel_H_
#define _CurlInteractiveChannel_H_

#include "IInteractiveChannelListener.h"

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "curl/curl.h"

#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

BR_PUCRIO_TELEMIDIA_GINGA_CORE_IC_BEGIN

  class CurlInteractiveChannel {
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

BR_PUCRIO_TELEMIDIA_GINGA_CORE_IC_END
#endif /*CurlInteractiveChannel*/
