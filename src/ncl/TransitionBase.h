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

#ifndef TRANSITIONBASE_H_
#define TRANSITIONBASE_H_

#include "Base.h"
using namespace ::ginga::ncl;

#include <vector>
#include <string>
using namespace std;

#include "Transition.h"

GINGA_NCL_BEGIN

   	class TransitionBase : public Base {
		private:
			vector<Transition*>* transitionSet;

		public:
			TransitionBase(string id);
			virtual ~TransitionBase();
			bool addTransition(Transition* transition);
			bool addBase(Base* base, string alias, string location);
			void clear();

		private:
			Transition* getTransitionLocally(string transitionId);

		public:
			Transition* getTransition(string transitionId);
			vector<Transition*>* getTransitions();
			bool removeTransition(Transition* transition);
   	};

GINGA_NCL_END
#endif /*TRANSITIONBASE_H_*/
