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

#include "ncl/link/CausalLink.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace link {
	CausalLink::CausalLink(string uid, Connector* connector) :
		     Link(uid, connector) {

		typeSet.insert("CausalLink");
	}

	bool CausalLink::containsSourceNode(
			Node *node, GenericDescriptor* descriptor) {

		bool contains;
		vector<Bind*>* conds;

		conds = getConditionBinds();
		contains = Link::containsNode(node, descriptor, conds);

		delete conds;
		return contains;
	}

	vector<Bind*>* CausalLink::getActionBinds() {
		vector<Bind*>* actionsVector;
		actionsVector = new vector<Bind*>;
		vector<Bind*>::iterator i;

		if (binds->empty()) {
			delete actionsVector;
			return NULL;
		}

		for (i = binds->begin(); i != binds->end(); ++i) {
			if (((*i)->getRole())->instanceOf("SimpleAction"))
				actionsVector->push_back(*i);
		}

		if (actionsVector->empty()) {
			delete actionsVector;
			return NULL;
		}

		return actionsVector;
	}

	vector<Bind*>* CausalLink::getConditionBinds() {
		vector<Bind*>* conditionsVector;
		vector<Bind*>::iterator iterator;
		Bind* bind;
		Role* role;

		if (binds->empty()) {
			return NULL;
		}

		conditionsVector = new vector<Bind*>;
		for (iterator = binds->begin(); iterator != binds->end(); ++iterator) {
			bind = (Bind*)(*iterator);
			role = bind->getRole();
			if (role != NULL) {
				if (role->instanceOf("SimpleCondition") ||
						role->instanceOf("AttributeAssessment")) {

					conditionsVector->push_back(bind);
				}
			}
		}

		if (conditionsVector->empty()) {
			delete conditionsVector;
			conditionsVector = NULL;
		}

		return conditionsVector;
	}
}
}
}
}
}
