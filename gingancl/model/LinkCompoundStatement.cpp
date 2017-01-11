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

#include "model/LinkCompoundStatement.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkCompoundStatement::LinkCompoundStatement(short op) : LinkStatement() {
		statements = new vector<LinkStatement*>;
		this->op = op;
		typeSet.insert("LinkCompoundStatement");
	}

	LinkCompoundStatement::~LinkCompoundStatement() {
		vector<LinkStatement*>::iterator i;
		LinkStatement* statement;

		if (statements != NULL) {
			for (i = statements->begin(); i != statements->end(); ++i) {
				statement = (LinkStatement*)(*i);
				delete statement;
				statement = NULL;
			}

			statements->clear();
			delete statements;
			statements = NULL;
		}
	}

	short LinkCompoundStatement::getOperator() {
		return op;
	}

	void LinkCompoundStatement::addStatement(LinkStatement* statement) {
		statements->push_back(statement);
	}

	vector<LinkStatement*>* LinkCompoundStatement::getStatements() {
		if (statements->begin() == statements->end())
			return NULL;

		return statements;
	}

	bool LinkCompoundStatement::isNegated() {
		return negated;
	}

	void LinkCompoundStatement::setNegated(bool neg) {
		negated = neg;
	}

	bool LinkCompoundStatement::returnEvaluationResult(bool result) {
		return (negated ^ result);
	}

	vector<FormatterEvent*>* LinkCompoundStatement::getEvents() {
		if (statements->empty())
			return NULL;

		vector<FormatterEvent*>* events = new vector<FormatterEvent*>;
		vector<FormatterEvent*>* statementEvents;
		vector<FormatterEvent*>::iterator j;
		vector<LinkStatement*>::iterator i;
		LinkStatement* statement;

		for (i = statements->begin(); i != statements->end(); i++) {
			statement = (LinkStatement*)(*i);
			statementEvents = statement->getEvents();
			if (statementEvents != NULL) {
				for (j = statementEvents->begin();
					    j != statementEvents->end(); j++) {

					events->push_back(*j);
				}
				delete statementEvents;
				statementEvents = NULL;
			}
		}

		if (events->begin() == events->end()) {
			delete events;
			return NULL;
		}

		return events;
	}

	bool LinkCompoundStatement::evaluate() {
		int i, size;
		LinkStatement *childStatement;

		size = statements->size();
		if (op == CompoundStatement::OP_OR) {
			for (i = 0; i < size; i++) {
				childStatement = (LinkStatement*)(*statements)[i];
				if (childStatement->evaluate())
					return returnEvaluationResult(true);
			}
			return returnEvaluationResult(false);

		} else {
			for (i = 0; i < size; i++) {
				childStatement = (LinkStatement*)(*statements)[i];
				if (!childStatement->evaluate())
					return returnEvaluationResult(false);
			}
			return returnEvaluationResult(true);
		}
	}
}
}
}
}
}
}
}
