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

#ifndef _COMPARATOR_H_
#define _COMPARATOR_H_

#include <iostream>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace util {
	class Comparator {
	  public:
		/**
		 * Constant that represents an equality.
		 */
		static const short CMP_EQ = 0;

		/**
		 * Constant that represents a disequality.
		 */	
		static const short CMP_NE = 1;

		/**
		 * Constant that represents something is smaller than other thing.
		 */	
		static const short CMP_LT = 2;

		/**
		 * Constant that represents something is smaller (or equal)
		 * than other thing.
		 */		
		static const short CMP_LTE = 3;

		/**
		 * Constant that represents something is bigger than other thing.
		 */			
		static const short CMP_GT = 4;

		/**
		 * Constant that represents something is bigger (or equal)
		 * than other thing.
		 */			
		static const short CMP_GTE = 5;

		static bool evaluate(string first, string second, short comparator);
		static bool evaluate(float first, float second, short comparator);

		/**
		 * This method returns a string representing one comparator.
		 * 
		 * @param comparator the comparator to be transformed in string.
		 * @return a string representing one comparator.
		 */	
		static string toString(short comparator);
		static short fromString(string comp);
	};
}
}
}
}
}

#endif //_COMPARATOR_H_
