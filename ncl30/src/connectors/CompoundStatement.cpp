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

#include "ncl/connectors/CompoundStatement.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
	CompoundStatement::CompoundStatement() : Statement() {
		statements = new vector<Statement*>;
		myOperator = OP_OR;
		negated    = false;

		typeSet.insert("CompoundStatement");
	}

	CompoundStatement::CompoundStatement(
		    Statement* p1,
		    Statement* p2, short op) : Statement() {

		statements = new vector<Statement*>;
		negated    = false;
		myOperator = op;

		statements->push_back(p1);
		statements->push_back(p2);

		typeSet.insert("CompoundStatement");
	}

	CompoundStatement::~CompoundStatement() {
		vector<Statement*>::iterator i;

		if (statements != NULL) {
			i = statements->begin();
			while (i != statements->end()) {
				delete *i;
				++i;
			}

			delete statements;
			statements = NULL;
		}
	}

	void CompoundStatement::setOperator(short op) {
		switch (op) {
			case OP_AND:
				myOperator = op;
				break;

			case OP_OR:
			default:
				myOperator = OP_OR;
				break;
		}
	}

	short CompoundStatement::getOperator() {
		return myOperator;
	}

	vector<Statement*>* CompoundStatement::getStatements() {
		if (statements->empty())
			return NULL;
		return statements;
	}

	void CompoundStatement::addStatement(Statement* statement) {
		statements->push_back(statement);
	}

	void CompoundStatement::removeStatement(Statement* statement) {
		vector<Statement*>::iterator iterator;
		vector<Statement*>::iterator i;

		iterator = statements->begin();
		while (iterator != statements->end()) {
			if ((*iterator) == statement) {
				i = statements->erase(iterator);
				if (i == statements->end())
					return;
			}
			++iterator;
		}
	}

	void CompoundStatement::setNegated(bool newNegated) {
		negated = newNegated;
	}

	bool CompoundStatement::isNegated() {
		return negated;
	}

	vector<Role*>* CompoundStatement::getRoles() {
		vector<Role*>* roles;
		int i, size;
		Statement* statement;
		vector<Role*>* childRoles;

		roles = new vector<Role*>;
		size = statements->size();
		for (i = 0; i < size; i++) {
			statement = (Statement*)((*statements)[i]);
			if (statement->instanceOf("AssessmentStatement")) {
				childRoles = ((AssessmentStatement*)statement)->getRoles();

			} else { // ICompoundStatement
				childRoles = ((CompoundStatement*)statement)->getRoles();
			}

			vector<Role*>::iterator it;
			for (it = childRoles->begin(); it != childRoles->end(); ++it) {
				roles->push_back( *it );
			}

			delete childRoles;
		}
		return roles;
	}
}
}
}
}
}
