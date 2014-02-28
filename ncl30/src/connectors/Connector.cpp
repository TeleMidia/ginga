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

#include "ncl/connectors/Connector.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
	Connector::Connector(string id) : Entity(id) {
		parameters = new map<string, Parameter*>;

		typeSet.insert("Connector");
	}

	Connector::~Connector() {
		map<string, Parameter*>::iterator i;

		if (parameters != NULL) {
			i = parameters->begin();
			while (i != parameters->end()) {
				delete i->second;
				++i;
			}
			delete parameters;
			parameters = NULL;
		}
	}

	int Connector::getNumRoles() {
		int numOfRoles;

		vector<Role*>* childRoles;
		childRoles = getRoles();
		numOfRoles = childRoles->size();
		delete childRoles;

		return numOfRoles;
	}

	Role* Connector::getRole(string roleId) {
		Role* wRole;
		vector<Role*> *roles = getRoles();
		if (roles == NULL) {
			return NULL;
		}

		vector<Role*>::iterator i;
		for (i=roles->begin(); i != roles->end(); ++i) {
			wRole = *i;
			if (wRole->getLabel() == roleId) {
				delete roles;
				return wRole;
			}
		}
		delete roles;
		return NULL;
	}

	void Connector::addParameter(Parameter* parameter) {
		if (parameter == NULL)
			return;

		map<string, Parameter*>::iterator i;
		for (i=parameters->begin(); i!=parameters->end(); ++i)
			if (i->first == parameter->getName())
				return;

		(*parameters)[parameter->getName()] = parameter;
	}

	vector<Parameter*>* Connector::getParameters() {
		if (parameters->empty())
			return NULL;

		vector<Parameter*>* params;
		params = new vector<Parameter*>;
		map<string, Parameter*>::iterator i;
		for (i=parameters->begin(); i!=parameters->end(); ++i)
			params->push_back(i->second);

		return params;
	}

	Parameter* Connector::getParameter(string name) {
		if (parameters->empty())
			return NULL;

		map<string, Parameter*>::iterator i;
		for (i=parameters->begin(); i!=parameters->end(); ++i)
			if (i->first == name)
				return (Parameter*)(i->second);

		return NULL;
	}

	bool Connector::removeParameter(string name) {
		if (parameters->empty())
			return false;

		map<string, Parameter*>::iterator i;
		for (i=parameters->begin(); i!=parameters->end(); ++i) {
			if (i->first == name) {
				parameters->erase(i);
				return true;
			}
		}

		return false;
	}
}
}
}
}
}
