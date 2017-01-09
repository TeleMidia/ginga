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

#ifndef IPlayerAdapter_H_
#define IPlayerAdapter_H_

#include "gingancl/model/AttributionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "player/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "system/IComponentInstance.h"
using namespace ::br::pucrio::telemidia::ginga::core::system;

#include "IPlayerAdapterManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
	class IPlayerAdapter : public IComponentInstance {
		public:
			virtual ~IPlayerAdapter(){};

			virtual void setAdapterManager(IPlayerAdapterManager* manager)=0;

		private:
			virtual void initializeInstance(std::string& data, short scenario)=0;
			virtual void testInstance(std::string& data, short scenario)=0;

		public:
			virtual bool instanceOf(string s)=0;

		protected:
			virtual void createPlayer()=0;

		public:
			virtual IPlayer* getPlayer()=0;

			virtual double getOutTransTime()=0;

			virtual bool setPropertyValue(
			    AttributionEvent* event, string value)=0;

			virtual string getPropertyValue(string name)=0;

	};
}
}
}
}
}
}

#endif /*IPlayerAdapter_H_*/
