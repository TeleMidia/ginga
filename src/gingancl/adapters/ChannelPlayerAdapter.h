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

#ifndef CHANNELPLAYERADAPTER_H_
#define CHANNELPLAYERADAPTER_H_

#include "gingancl/model/CompositeExecutionObject.h"
#include "gingancl/model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "gingancl/model/AttributionEvent.h"
#include "gingancl/model/PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "ncl/components/Content.h"
#include "ncl/components/NodeEntity.h"
#include "ncl/components/ReferenceContent.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/transition/Transition.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "FormatterPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include <map>
#include <vector>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_AV_BEGIN

  	class ChannelPlayerAdapter : public FormatterPlayerAdapter {
		public:
			ChannelPlayerAdapter();
			virtual ~ChannelPlayerAdapter(){};

		protected:
			void createPlayer();
			bool setPropertyValue(AttributionEvent* event, string value);
	};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_AV_END
#endif /*CHANNELPLAYERADAPTER_H_*/
