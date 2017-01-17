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

#ifndef PROGRAMAVPLAYERADAPTER_H_
#define PROGRAMAVPLAYERADAPTER_H_

#include "gingancl/model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "gingancl/model/AttributionEvent.h"
#include "gingancl/model/FormatterEvent.h"
#include "gingancl/model/PresentationEvent.h"
#include "gingancl/model/SelectionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "ncl/components/Content.h"
#include "ncl/components/ReferenceContent.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "player/IProgramAV.h"
using namespace ::ginga::player;

#include "FormatterPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include <vector>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_AV_TV_BEGIN

	class ProgramAVPlayerAdapter : public FormatterPlayerAdapter {
		private:
			string lastValue;

		protected:
			ProgramAVPlayerAdapter();
			virtual ~ProgramAVPlayerAdapter(){};
			static ProgramAVPlayerAdapter* _instance;

		public:
			static ProgramAVPlayerAdapter* getInstance();

			virtual bool hasPrepared();
			virtual bool start();
			virtual bool stop();
			virtual bool resume();

		protected:
			void createPlayer();
			bool setPropertyValue(AttributionEvent* event, string value);

		private:
			void updateAVBounds();
	};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_AV_TV_END
#endif /*PROGRAMAVPLAYERADAPTER_H_*/
