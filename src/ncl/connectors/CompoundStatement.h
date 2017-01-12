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

#ifndef _COMPOUNDSTATEMENT_H_
#define _COMPOUNDSTATEMENT_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "Statement.h"
#include "AssessmentStatement.h"
#include "Role.h"

#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
	class CompoundStatement : public Statement {
		public:
			static const short OP_AND = 0;
			static const short OP_OR = 1;

		protected:
			vector<Statement*>* statements;
			short myOperator;
			bool negated;

		public:
			CompoundStatement();
			CompoundStatement(
				    Statement* p1,
				    Statement* p2,
				    short op);

			virtual ~CompoundStatement();
			void setOperator(short op);
			short getOperator();
			vector<Statement*>* getStatements();
			void addStatement(Statement* statement);
			void removeStatement(Statement* statement);
			void setNegated(bool newNegated);
			bool isNegated();
			vector<Role*> *getRoles();
			bool instanceOf(string type) {
				return Statement::instanceOf(type);
			}
	};
}
}
}
}
}

#endif //_COMPOUNDSTATEMENT_H_
