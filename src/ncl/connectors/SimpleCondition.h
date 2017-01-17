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

#ifndef _SIMPLECONDITION_H_
#define _SIMPLECONDITION_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "EventUtil.h"
#include "TriggerExpression.h"
#include "Role.h"

#include <string>
using namespace std;

BR_PUCRIO_TELEMIDIA_NCL_CONNECTORS_BEGIN

	class SimpleCondition : public TriggerExpression, public Role {
		private:
			string key;
			short transition;
			short qualifier;

		public:
			static const short NO_QUALIFIER = -1;
			SimpleCondition(string role);
			virtual ~SimpleCondition(){};

			void setLabel(string id);
			string getKey();
			void setKey(string key);
			short getTransition();
			void setTransition(short transition);
			short getQualifier();
			void setQualifier(short qualifier);
			bool instanceOf(string type) {
				return TriggerExpression::instanceOf(type);
			}
	};

BR_PUCRIO_TELEMIDIA_NCL_CONNECTORS_END
#endif //_SIMPLECONDITION_H_
