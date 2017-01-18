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

#ifndef IEXTENDEDEVENTDESCRIPTOR_H_
#define IEXTENDEDEVENTDESCRIPTOR_H_

#include "IMpegDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;


struct Item {
	unsigned char itemDescriptionLength;
	char* itemDescriptionChar;
	unsigned char itemLength;
	char* itemChar;
};
BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_SI_DESCRIPTORS_BEGIN

	class IExtendedEventDescriptor : public IMpegDescriptor {
		public:
			~IExtendedEventDescriptor(){};
			virtual string getLanguageCode()=0;
			virtual string getTextChar()=0;

			virtual vector<Item*>* getItems()=0;
			virtual string getItemDescriptionChar(struct Item* item)=0;
			virtual string getItemChar(struct Item* item)=0;

	};

BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_SI_DESCRIPTORS_END
#endif /* IEXTENDEDEVENTDESCRIPTOR_H_ */
