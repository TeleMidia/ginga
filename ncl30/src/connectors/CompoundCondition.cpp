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

#include "ncl/connectors/CompoundCondition.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
	CompoundCondition::CompoundCondition() :
		    TriggerExpression() {

		expressions = new vector<ConditionExpression*>;
		typeSet.insert("CompoundCondition");
	}

	CompoundCondition::CompoundCondition(
		    ConditionExpression* c1,
		    ConditionExpression* c2, short op) : TriggerExpression() {

		expressions = new vector<ConditionExpression*>;
		expressions->push_back(c1);
		expressions->push_back(c2);
		myOperator = op;
		typeSet.insert("CompoundCondition");
	}

	CompoundCondition::~CompoundCondition() {
		vector<ConditionExpression*>::iterator i;

		if (expressions != NULL) {
			i = expressions->begin();
			while (i != expressions->end()) {
				delete *i;
				++i;
			}

			delete expressions;
			expressions = NULL;
		}
	}

	void CompoundCondition::setOperator(short op) {
		myOperator = op;
	}

	short CompoundCondition::getOperator() {
		return myOperator;
	}

	vector<ConditionExpression*>* CompoundCondition::getConditions() {
		if (expressions->empty())
			return NULL;

		return expressions;
	}

	void CompoundCondition::addConditionExpression(
		    ConditionExpression* condition) {

		if (condition == NULL) {
			clog << "CompoundCondition::addConditionExpression ";
			clog << "Warning! Trying to add a NULL condition." << endl;
		} else {
			expressions->push_back(condition);
		}
	}

	void CompoundCondition::removeConditionExpression(
		    ConditionExpression* condition) {

		vector<ConditionExpression*>::iterator iterator;
		vector<ConditionExpression*>::iterator i;

		iterator = expressions->begin();
		while (iterator != expressions->end()) {
			if ((*iterator) == condition) {
				i = expressions->erase(iterator);
				if (i == expressions->end())
					return;
			}
			++iterator;
		}
	}

	vector<Role*> *CompoundCondition::getRoles() {
		vector<Role*> *roles;
		int i, size;
		ConditionExpression *condition;
		vector<Role*> *childRoles;

		roles = new vector<Role*>;
		size = expressions->size();
		for (i = 0; i < size; i++) {
			condition = (*expressions)[i];
			if (condition == NULL) {
				clog << "CompoundCondition::getRoles ";
				clog << "Warning! condition = NULL." << endl;

			} else if (condition->instanceOf("SimpleCondition")) {
				roles->push_back((SimpleCondition*)condition);

			} else {
				if (condition->instanceOf("CompoundCondition")) {
					childRoles = ((CompoundCondition*)condition)->
						    getRoles();

				} else if (condition->instanceOf("AssessmentStatement")) {
					childRoles = ((AssessmentStatement*)condition)->getRoles();

				} else { // ICompoundStatement
					childRoles = ((CompoundStatement*)condition)->getRoles();
				}

				vector<Role*>::iterator it;
				for (it = childRoles->begin(); it != childRoles->end(); ++it) {
					roles->push_back( *it );
				}

				delete childRoles;
			}
		}
		return roles;
	}
}
}
}
}
}
