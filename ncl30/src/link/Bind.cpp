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

#include "ncl/link/Bind.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace link {
	Bind::Bind(Node* node, InterfacePoint* interfPt,
		    GenericDescriptor* desc, Role* role) {

		this->node           = node;
		this->interfacePoint = interfPt;
		this->descriptor     = desc;
		this->role           = role;
		this->parameters     = new map<string, Parameter*>;

		typeSet.insert("Bind");
	}

	Bind::~Bind() {
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

	bool Bind::instanceOf(string s) {
		if(typeSet.empty())
			return false;
		else
			return (typeSet.find(s) != typeSet.end());
	}

	GenericDescriptor* Bind::getDescriptor() {
		return descriptor;
	}

	InterfacePoint* Bind::getInterfacePoint() {
		return this->interfacePoint;
	}

	Node* Bind::getNode() {
		return this->node;
	}

	Role* Bind::getRole() {
		return this->role;
	}

	void Bind::setInterfacePoint(InterfacePoint* interfPt) {
		this->interfacePoint = interfPt;
	}

	void Bind::setNode(Node* node) {
		this->node = node;
	}

	void Bind::setRole(Role* role) {
		this->role = role;
	}

	void Bind::setDescriptor(GenericDescriptor* desc) {
		this->descriptor = desc;
	}

	Parameter *Bind::setParameterValue(
		    string propertyLabel, Parameter *propertyValue) {

		return (*parameters)[propertyLabel] = propertyValue;
	}

	string Bind::toString() {
		return (getRole()->getLabel() +
			    '/' + getNode()->getId() +
			    '/' + getInterfacePoint()->getId());
	}

	void Bind::addParameter(Parameter* parameter) {
		if (parameter == NULL || parameters == NULL) {
			return;
		}

		(*parameters)[parameter->getName()] = parameter;
	}

	vector<Parameter*>* Bind::getParameters() {
		if (parameters->empty())
			return NULL;

		map<string, Parameter*>::iterator i;
		vector<Parameter*>* params;

		params = new vector<Parameter*>;
		for (i=parameters->begin(); i!=parameters->end();++i)
			params->push_back(i->second);

		return params;
	}

	Parameter* Bind::getParameter(string paramName) {
		if (parameters->empty())
			return NULL;

		map<string, Parameter*>::iterator i;
		for (i=parameters->begin();i!=parameters->end();++i)
			if (i->first == paramName)
				return (Parameter*)(i->second);

		return NULL;
	}

	bool Bind::removeParameter(Parameter *parameter) {
		if (parameters->empty())
			return false;

		map<string, Parameter*>::iterator i;

		for (i=parameters->begin(); i!=parameters->end();++i) {
			if (i->first == parameter->getName()) {
				parameters->erase(i);
				return true;
			}
		}

		return false;
	}

	vector<Node*>* Bind::getNodeNesting() {
		vector<Node*>* nodeNesting;
		vector<Node*>* nodeSequence;
		vector<Node*>::iterator i;

		nodeNesting = new vector<Node*>;
		nodeNesting->push_back(node);
		if (interfacePoint != NULL) {
			if (interfacePoint->instanceOf("Port") &&
				    !(interfacePoint->instanceOf("SwitchPort"))) {

				nodeSequence = ((Port*)interfacePoint)->
					    getMapNodeNesting();

				if (nodeSequence != NULL) {
					for (i = nodeSequence->begin();
						    i != nodeSequence->end(); ++i) {

						nodeNesting->push_back(*i);
					}
				}

				delete nodeSequence;
			}
		}

		return nodeNesting;
	}

	InterfacePoint* Bind::getEndPointInterface() {
		NodeEntity* nodeEntity;
		CompositeNode* compositeNode;
		Port* port;

		nodeEntity = (NodeEntity*)(node->getDataEntity());
		if (nodeEntity->instanceOf("CompositeNode") &&
			    interfacePoint->instanceOf("Port")) {

			compositeNode = (CompositeNode*)nodeEntity;
			port = (Port*)interfacePoint;
			return compositeNode->getMapInterface(port);

		} else {
			return interfacePoint;
		}
	}
}
}
}
}
}
