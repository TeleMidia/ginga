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

#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

extern "C" {
#include "pthread.h"
}

#include <string>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
	class Entity {
		protected:
			set<string> typeSet; //type information

		private:
			static set<Entity*> instances;
			static pthread_mutex_t iMutex;
			static bool initMutex;

			string id; // id=comparable unique entity Id

		public:
			Entity(string someId);
			virtual ~Entity();

			static bool hasInstance(Entity* instance, bool eraseFromList);
			void printHierarchy();
			bool instanceOf(string s);
			int compareTo(Entity* otherEntity);
			string getId();
			int hashCode();

			virtual void setId(string someId);

			virtual string toString();
			virtual Entity *getDataEntity();
	};
}
}
}
}

#endif //_ENTITY_H_
