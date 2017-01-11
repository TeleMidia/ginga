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
#ifndef PARENTALRATINGDESCRIPTOR_H_
#define PARENTALRATINGDESCRIPTOR_H_



#include "IParentalRatingDescriptor.h"

#include <string.h>

#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si{
namespace descriptors{

	class ParentalRatingDescriptor : public IParentalRatingDescriptor {
		protected:
			vector<Parental*>* countryRatings;
		public:
			ParentalRatingDescriptor();
			~ParentalRatingDescriptor();
			unsigned char getDescriptorTag();
			unsigned int getDescriptorLength();
			unsigned int getContentDescription(Parental* parental);
			unsigned int getAge(Parental* parental);
			vector<Parental*>* getCountryRatings();
			string getCountryCode(Parental* parental);
			void print();
			size_t process (char* data, size_t pos);
	};

}
}
}
}
}
}
}
}

#endif /* PARENTALRATINGDESCRIPTOR_H_ */
