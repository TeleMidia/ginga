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

#ifndef _INTERVALANCHOR_H_
#define _INTERVALANCHOR_H_

#include "ContentAnchor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace interfaces {
	class IntervalAnchor : public ContentAnchor {
		protected:
			double begin;
			double end;
			string strBegin;
			string strEnd;

		public:
			static const double OBJECT_DURATION;

			IntervalAnchor(string id, double begin, double end);
			void setStrValues(string begin, string end);
			string getStrBegin();
			string getStrEnd();
			double getBegin();
			double getEnd();
			void setBegin(double b);
			void setEnd(double e);
			static bool isObjectDuration(double value);
	};
}
}
}
}
}

#endif //_INTERVALANCHOR_H_
