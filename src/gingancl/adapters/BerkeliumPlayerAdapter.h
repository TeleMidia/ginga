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

#ifndef BerkeliumPlayerAdapter_H_
#define BerkeliumPlayerAdapter_H_

#include "../../../../../model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "../../../../../model/PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "../../../../FormatterPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "mb/LocalScreenManager.h"
using namespace ::ginga::mb;

#include <string>
#include <iostream>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_XHTML_BEGIN

	class BerkeliumPlayerAdapter : public FormatterPlayerAdapter {
		public:
			BerkeliumPlayerAdapter();
			virtual ~BerkeliumPlayerAdapter(){};

		protected:
			void rebase();
			void updateProperties();
			void createPlayer();

		public:
			bool setPropertyValue(
				    AttributionEvent* event, string value);
	};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_XHTML_END
#endif /*BerkeliumPlayerAdapter_H_*/
