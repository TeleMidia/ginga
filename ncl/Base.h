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

#ifndef BASE_H_
#define BASE_H_

#include "IllegalBaseTypeException.h"

#include <pthread.h>
#include <vector>
#include <map>
#include <string>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
	class Base {
		private:
			static set<Base*> baseInstances;
			static pthread_mutex_t biMutex;
			static bool initMutex;

		protected:
			string id;
			vector<Base*> baseSet;
			map<string, Base*> baseAliases;
			map<string, Base*> baseLocations;

			set<string> typeSet; //informacoes de tipo

		public:
			Base(string id);
			virtual ~Base();

			static bool hasInstance(Base* base, bool eraseFromList);

			virtual bool addBase(
				    Base* base,
				    string alias,
				    string location) throw(IllegalBaseTypeException*);

			virtual void clear();
			Base* getBase(string baseId);
			string getBaseAlias(Base* base);
			string getBaseLocation(Base* base);
			vector<Base*>* getBases();
			bool removeBase(Base* base);
			void setBaseAlias(Base* base, string alias);
			void setBaseLocation(Base* base, string location);
			string getId();
			void setId(string id);

			bool instanceOf(string s);
	};
}
}
}
}

#endif /*BASE_H_*/
