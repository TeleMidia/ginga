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

#ifndef ITRANSPORTSECTION_H_
#define ITRANSPORTSECTION_H_

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
	class ITransportSection {
		public:
			virtual ~ITransportSection(){};
			virtual void setESId(unsigned int id)=0;
			virtual unsigned int getESId()=0;
			virtual void addData(char* bytes, unsigned int size)=0;
			virtual string getSectionName()=0;
			virtual unsigned int getTableId()=0;
			virtual bool getSectionSyntaxIndicator()=0;
			virtual unsigned int getSectionLength()=0;
			virtual unsigned int getCurrentSize()=0;
			virtual unsigned int getExtensionId()=0;
			virtual unsigned int getVersionNumber()=0;
			virtual bool getCurrentNextIndicator()=0;
			virtual unsigned int getSectionNumber()=0;
			virtual unsigned int getLastSectionNumber()=0;
			virtual void* getPayload()=0;
			virtual unsigned int getPayload(char** buffer)=0;
			virtual unsigned int getPayloadSize()=0;
			virtual bool isConsolidated()=0;
			virtual bool isConstructionFailed()=0;
			virtual void print()=0;
	};
}
}
}
}
}
}

#endif /*ITRANSPORTSECTION_H_*/
