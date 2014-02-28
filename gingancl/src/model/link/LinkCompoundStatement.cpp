/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "gingancl/model/LinkCompoundStatement.h"

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
