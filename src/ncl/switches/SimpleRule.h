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

#ifndef _SIMPLERULE_H_
#define _SIMPLERULE_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "Rule.h"

#include <string>
#include <iostream>
#include <sstream>
using namespace std;

BR_PUCRIO_TELEMIDIA_NCL_SWITCHES_BEGIN

	class SimpleRule : public Rule {
		private:
			string attribute;
			short ruleOperator;
			string value;

		public:
			SimpleRule(string id, string attr, short op,
				    string val);

			virtual ~SimpleRule(){};
			string getAttribute();
			short getOperator();
			string getValue();
			void setOperator(short newOp);
			void setValue(string newValue);
			string toString();
			void setAttribute(string someAttribute);
	};

BR_PUCRIO_TELEMIDIA_NCL_SWITCHES_END
#endif //_SIMPLERULE_H_
