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

#ifndef APPLICATIONDESCRIPTOR_H_
#define APPLICATIONDESCRIPTOR_H_

#include <iostream>
#include <vector>
using namespace std;

#include "IMpegDescriptor.h"
using namespace br::pucrio::telemidia::ginga::core::tsparser;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {

	struct Profile {
		unsigned short applicationProfile;
		unsigned char versionMajor;
		unsigned char versionMinor;
		unsigned char versionMicro;
	};

	class ApplicationDescriptor : public IMpegDescriptor{
		private:
			unsigned char applicationProfilesLength;
			vector<Profile*>* profiles;
			bool serviceBoundFlag;
			unsigned char visibility;
			unsigned char applicationPriority;
			char* transportProtocolLabels;
			unsigned char transportProtocolLabelsLength;

		public:
			ApplicationDescriptor();
			virtual ~ApplicationDescriptor();

			unsigned char getDescriptorTag();
			unsigned int getDescriptorLength();
			size_t process(char* data, size_t pos);
			void print();
			unsigned char getApplicationPriority();
			unsigned char getVisibility();
			unsigned char getTransportProtocolLabelsLength();
			char* getTransportProtocolLabels();
			vector<struct Profile*>* getProfiles();
	};

}
}
}
}
}
}
}
#endif /* APPLICATIONDESCRIPTOR_H_ */
