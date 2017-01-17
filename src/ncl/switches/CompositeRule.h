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

#ifndef _COMPOSITERULE_H_
#define _COMPOSITERULE_H_

#include "Rule.h"

#include <string>
#include <vector>
using namespace std;

BR_PUCRIO_TELEMIDIA_NCL_SWITCHES_BEGIN

	class CompositeRule : public Rule {
		private:
			vector<Rule*>* rules;
			short ruleOperator;
			string opStr;

		public:
			static const short OP_AND = 0;
			static const short OP_OR  = 1;

			CompositeRule(string id, short someOperator);
			CompositeRule(string id, Rule* firstRule,
				    Rule* secondRule, short someOperator);

			virtual ~CompositeRule();
			bool addRule(Rule* rule);
			vector<Rule*>* getRules();
			unsigned int getNumRules();
			short getOperator();
			bool removeRule(Rule* rule);
			string toString();
			void setOperator(short op);
	};		

BR_PUCRIO_TELEMIDIA_NCL_SWITCHES_END
#endif //_COMPOSITERULE_H_
