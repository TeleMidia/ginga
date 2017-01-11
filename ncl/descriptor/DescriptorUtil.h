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

#ifndef DESCRIPTORUTIL_H_
#define DESCRIPTORUTIL_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "Descriptor.h"

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace descriptor {
  class DescriptorUtil {
	public:
		static short getFitCode(string fitName) {
			if (upperCase(fitName) == upperCase("fill")) {
  				return Descriptor::FIT_FILL;

  			} else if (upperCase(fitName) == upperCase("hidden")) {
		  		return Descriptor::FIT_HIDDEN;

		  	} else if (upperCase(fitName) == upperCase("meet")) {
		  		return Descriptor::FIT_MEET;

		  	} else if (upperCase(fitName) == upperCase("meetBest")) {
		  		return Descriptor::FIT_MEETBEST;

		  	} else if (upperCase(fitName) == upperCase("slice")) {
		  		return Descriptor::FIT_SLICE;

		  	} else {
		  		return -1;
		  	}
		}

		static string getFitName(short fit) {
			switch (fit) {
				case Descriptor::FIT_FILL:
					return "fill";

				case Descriptor::FIT_HIDDEN:
					return "hidden";

				case Descriptor::FIT_MEET:
					return "meet";

				case Descriptor::FIT_MEETBEST:
					return "meetBest";

				case Descriptor::FIT_SLICE:
					return "slice";

				default:
					return "";
			}
		}

		static short getScrollCode(string scrollName) {
			if (upperCase(scrollName) == upperCase("none")) {
		  		return Descriptor::SCROLL_NONE;

		  	} else if (upperCase(scrollName) == upperCase("horizontal")) {
		  		return Descriptor::SCROLL_HORIZONTAL;

		  	} else if (upperCase(scrollName) == upperCase("vertical")) {
		  		return Descriptor::SCROLL_VERTICAL;

		  	} else if (upperCase(scrollName) == upperCase("both")) {
		  		return Descriptor::SCROLL_BOTH;

		  	} else if (upperCase(scrollName) == upperCase("automatic")) {
		  		return Descriptor::SCROLL_AUTOMATIC;

		  	} else {
		  		return -1;
		  	}
		}

		static string getScrollName(short scroll) {
			switch (scroll) {
				case Descriptor::SCROLL_NONE:
					return "none";

				case Descriptor::SCROLL_HORIZONTAL:
					return "horizontal";

				case Descriptor::SCROLL_VERTICAL:
					return "vertical";

				case Descriptor::SCROLL_BOTH:
					return "both";

				case Descriptor::SCROLL_AUTOMATIC:
					return "automatic";

				default:
					return "";
			}
		}
  };
}
}
}
}
}

#endif /*DESCRIPTORUTIL_H_*/
