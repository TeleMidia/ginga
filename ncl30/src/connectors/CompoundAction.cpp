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

#include "ncl/connectors/CompoundAction.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
	CompoundAction::CompoundAction() : Action() {
		actions = new vector<Action*>;
		typeSet.insert("CompoundAction");
	}

	CompoundAction::CompoundAction(
		    Action* a1, Action* a2, short op) : Action() {

		actions = new vector<Action*>;
		actions->push_back(a1);
		actions->push_back(a2);
		myOperator = op;
		typeSet.insert("CompoundAction");
	}

	CompoundAction::~CompoundAction() {
		vector<Action*>::iterator i;

		if (actions != NULL) {
			i = actions->begin();
			while (i != actions->end()) {
				delete (*i);
				++i;
			}

			delete actions;
			actions = NULL;
		}
	}

	void CompoundAction::setOperator(short op) {
		myOperator = op;
	}

	short CompoundAction::getOperator() {
		return myOperator;
	}

	vector<Action*>* CompoundAction::getActions() {
		if (actions->begin() == actions->end())
			return NULL;

		return actions;
	}

	void CompoundAction::addAction(Action* action) {
		vector<Action*>::iterator i;

		i = actions->begin();
		while (i != actions->end()) {
			if (action == *i) {
				clog << "CompoundAction::addAction ";
				clog << "Warning! Trying to add the action twice";
				clog << endl;
				return;
			}
			++i;
		}
		actions->push_back(action);
	}

	void CompoundAction::removeAction(Action* action) {
		vector<Action*>::iterator iterator;
		vector<Action*>::iterator i;

		iterator = actions->begin();
		while (iterator != actions->end()) {
			if ((*iterator) == action) {
				i = actions->erase(iterator);
				if (i == actions->end())
					return;
			}
			++iterator;
		}
	}

	vector<Role*>* CompoundAction::getRoles() {
		vector<Role*>* roles;
		int i, size;
		Action* action;
		vector<Role*>* childRoles;

		roles = new vector<Role*>;
		size = actions->size();
		for (i = 0; i < size; i++) {
			action = (Action*)((*actions)[i]);
			if (action->instanceOf("SimpleAction")) {
				roles->push_back((SimpleAction*)action);
			}
			else {
				childRoles = ((CompoundAction*)action)->getRoles();
				vector<Role*>::iterator it;
				for (it = childRoles->begin(); it != childRoles->end(); ++it) {
					roles->push_back( *it );
				}

				delete childRoles;
			}
		}
		return roles;
	}

	string CompoundAction::toString() {
		vector<Action*>::iterator i;
		Action* action;
		string actionStr;

		actionStr = "";
		for (i = actions->begin(); i != actions->end(); i++) {
			action = (*i);
			actionStr += (action->toString());
		}
		return actionStr;
	}
}
}
}
}
}
