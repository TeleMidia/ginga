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

#ifndef RECTANGLESPATIALANCHOR_H_
#define RECTANGLESPATIALANCHOR_H_

#include "../Entity.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "SpatialAnchor.h"

BR_PUCRIO_TELEMIDIA_NCL_INTERFACES_BEGIN

	class RectangleSpatialAnchor : public SpatialAnchor {
		private :
			long left;
			long top;
			long width;
			long height;

		public:
			RectangleSpatialAnchor(
				    string id, long left, long top, long width, long height);

			long getLeft();
			long getTop();
			long getWidth();
			long getHeight();
			void setArea(long left, long top, long width, long height);
	};

BR_PUCRIO_TELEMIDIA_NCL_INTERFACES_END
#endif /*RECTANGLESPATIALANCHOR_H_*/
