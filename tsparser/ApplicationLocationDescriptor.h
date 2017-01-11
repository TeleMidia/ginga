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

#ifndef APPLICATIONLOCATIONDESCRIPTOR_H_
#define APPLICATIONLOCATIONDESCRIPTOR_H_

#include "IMpegDescriptor.h"
using namespace br::pucrio::telemidia::ginga::core::tsparser;

#include <string.h>
#include <string>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	class ApplicationLocationDescriptor : public IMpegDescriptor{
		private:
			unsigned char baseDirectoryLength;
			char* baseDirectoryByte;
			unsigned char classPathExtensionLength;
			char* classPathExtensionByte;
			unsigned char initialClassLentgh;
			char* initialClassByte;

		public:
			ApplicationLocationDescriptor();
			virtual ~ApplicationLocationDescriptor();
			unsigned char getDescriptorTag();
			unsigned int getDescriptorLength();
			size_t process(char* data, size_t pos);
			void print();
			unsigned int getBaseDirectoryLength();
			string getBaseDirectory();
			unsigned int getClassPathExtensionLength();
			string getClassPathExtension();
			unsigned int getInitialClassLength();
			string getInitialClass();
		};
}
}
}
}
}
}
}

#endif /* APPLICATIONLOCATIONDESCRIPTOR_H_ */
