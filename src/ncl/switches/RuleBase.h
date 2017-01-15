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

#ifndef RULEBASE_H_
#define RULEBASE_H_

#include "../Base.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "Rule.h"

#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace switches {
	class RuleBase : public Base {
		private:
			vector<Rule*>* ruleSet;

		public:
			RuleBase(string id);
			virtual ~RuleBase();

			bool addRule(Rule* rule);
			bool addBase(
				    Base* base,
				    string alias,
				    string location);

			void clear();

		private:
			Rule* getRuleLocally(string ruleId);

		public:
			Rule* getRule(string ruleId);
			vector<Rule*>* getRules();
			bool removeRule(Rule* rule);
	};
}
}
}
}
}

#endif /*RULEBASE_H_*/
