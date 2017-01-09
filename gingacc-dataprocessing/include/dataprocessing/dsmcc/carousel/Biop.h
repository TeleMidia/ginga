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

#ifndef BIOP_H_
#define BIOP_H_

extern "C" {
	#include <sys/stat.h>
	#include <errno.h>
	#include <stdio.h>
}

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;


#include "object/ObjectProcessor.h"
#include "data/Module.h"

#include <map>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	class Biop {
		private:
			//Biop Attributes
			Module* module;
			unsigned int idx;
			unsigned int currentSize;
			FILE* moduleFd;
			bool isValidHdr;
			bool hasMoreBiopMessage;
			char* data;

			// MessageHeader
			unsigned int messageSize;

			// MessageSubHeader
			unsigned int objectKey;
			string objectKind;
			string objectInfo;

			// MessageBody
			map<string, Object*> objects;

			ObjectProcessor* processor;

			pthread_mutex_t dataMutex;

		public:
			Biop(Module* module, ObjectProcessor* processor);
			virtual ~Biop();

		private:
			void closeModule();
			void createData(unsigned int dataSize);
			void releaseData();

			string getStringFromData(unsigned int offset, unsigned int len);

			string getObjectKind();
			string getObjectInfo();

			void abortProcess(string warningText);
			bool processServiceContext();
			bool processMessageHeader();
			int processMessageSubHeader();
			int skipObject();

		public:
			int processServiceGateway(unsigned int srgObjectKey);

		private:
			Binding* processBinding();
			void processIor(Binding* binding);

			void processDirectory();
			void processFile();

		public:
			void print();

		private:
			void processObject();

		public:
			int process();
	};
}
}
}
}
}
}
}

#endif /*BIOP_H_*/
