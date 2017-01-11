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

#ifndef _NCLMetadataParser_H_
#define _NCLMetadataParser_H_

#include "expat.h"

#include "IMetadata.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <set>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace ncl {
	class NCLMetadataParser {
		public:
			static IMetadata* parse(string xmlDocument);
			static IMetadata* parse(char* xmlStream, int streamSize);

		private:
			static void startElementHandler(
					void* data,
					const XML_Char* element, const XML_Char** attrs);

			static void parseMetadata(void* data, const XML_Char** attrs);
			static void parseBaseData(void* data, const XML_Char** attrs);
			static void parseRoot(void* data, const XML_Char** attrs);
			static void parseData(void* data, const XML_Char** attrs);

			static IDataFile* createObject(
					void* data, const XML_Char** attrs);

			static void stopElementHandler(void* data, const XML_Char* element);
	};
}
}
}
}
}
}
}

#endif //_NCLMetadataParser_H_
