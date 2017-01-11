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

#ifndef MODULE_H_
#define MODULE_H_

extern "C" {
	#include <sys/stat.h>
	#include <stdio.h>
	#include <stdio.h>
}

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;
#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <set>
#include <string>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	class Module {
		private:
			//id of es module origin
			unsigned int pid;
			FILE* moduleFd;
			unsigned int carouselId;
			unsigned int id;
			unsigned int size;
			unsigned int version;
			unsigned int infoLength;
			unsigned int currentDownloadSize;
			bool overflowNotification;
			set<unsigned int> blocks;

		public:
			Module(unsigned int moduleId);
			void setESId(unsigned int pid);
			unsigned int getESId();
			void openFile();
			void setCarouselId(unsigned int id);
			void setSize(unsigned int size);
			void setVersion(unsigned int version);
			void setInfoLength(unsigned int length);
			bool isConsolidated();
			unsigned int getId();
			unsigned int getCarouselId();
			unsigned int getSize();
			unsigned int getVersion();
			unsigned int getInfoLength();
			string getModuleFileName();
			void pushDownloadData(
					unsigned int blockNumber,
					void* data,
					unsigned int dataSize);

			void print();
	};
}
}
}
}
}
}
}

#endif /*MODULE_H_*/
