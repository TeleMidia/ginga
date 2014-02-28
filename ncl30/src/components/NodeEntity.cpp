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

#include "ncl/components/NodeEntity.h"
#include "ncl/components/CompositeNode.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {
	NodeEntity::NodeEntity(string uid, Content* someContent) : Node(uid) {
		descriptor = NULL;
		anchorList.push_back(new LambdaAnchor(uid));
		this->content = someContent;
		typeSet.insert("NodeEntity");
	}

	NodeEntity::~NodeEntity() {
		vector<Anchor*>::iterator i;
		set<ReferNode*>::iterator j;
		Anchor* anchor;

		if (descriptor != NULL) {
			//descriptor is deleted in descriptor base
			descriptor = NULL;
		}

		if (content != NULL) {
			delete content;
			content = NULL;
		}

		for (j = instSameInstances.begin();
				j != instSameInstances.end(); ++j) {

			if ((Node*)(*j) != (Node*)this && Entity::hasInstance((*j), true)) {
				delete (*j);
			}
		}
		instSameInstances.clear();

		for (j = gradSameInstances.begin();
				j != gradSameInstances.end(); ++j) {

			delete (*j);
		}
		gradSameInstances.clear();

		i = anchorList.begin();
		while (i != anchorList.end()) {
			anchor = (*i);
			if (Entity::hasInstance(anchor, true)) {
				delete (*i);
			}
			++i;
		}
		anchorList.clear();
	}

	bool NodeEntity::addAnchor(int index, Anchor* anchor) {
		if (index == 0) {
			return false;
		}
		return Node::addAnchor(index, anchor);
	}

	LambdaAnchor* NodeEntity::getLambdaAnchor() {
		LambdaAnchor* lambda;
		lambda = static_cast<LambdaAnchor*>(*(anchorList.begin()));
		return lambda;
	}

	void NodeEntity::setId(string id) {
		LambdaAnchor* anchor;

		Entity::setId(id);
		anchor = getLambdaAnchor();
		anchor->setId(id);
	}

	bool NodeEntity::removeAnchor(int index) {
		if (index == 0) {
			return false;
		}
		return Node::removeAnchor(index);
	}

	GenericDescriptor* NodeEntity::getDescriptor() {
	   return descriptor;
	}

	void NodeEntity::setDescriptor(GenericDescriptor* someDescriptor) {
		descriptor = someDescriptor;
	}

	Content* NodeEntity::getContent() {
		return content;
	}

	void NodeEntity::setContent(Content* someContent) {
		content = someContent;
	}

	bool NodeEntity::addAnchor(Anchor *anchor) {
		return Node::addAnchor(anchor);
	}

	bool NodeEntity::removeAnchor(Anchor *anchor) {
		return Node::removeAnchor(anchor);
	}

	set<ReferNode*>* NodeEntity::getInstSameInstances() {
		if (instSameInstances.empty()) {
			return NULL;
		}

		return &instSameInstances;
	}

	set<ReferNode*>* NodeEntity::getGradSameInstances() {
		if (gradSameInstances.empty()) {
			return NULL;
		}

		return &gradSameInstances;
	}

	bool NodeEntity::addSameInstance(ReferNode* node) {
		if (node->getInstanceType() == "instSame") {
			if (instSameInstances.count(node) != 0) {
				return false;
			}

			instSameInstances.insert(node);

		} else if (node->getInstanceType() == "gradSame") {
			if (gradSameInstances.count(node) != 0) {
				return false;
			}

			gradSameInstances.insert(node);

		} else {
			return false;
		}

		return true;
	}

	void NodeEntity::removeSameInstance(ReferNode* node) {
		set<ReferNode*>::iterator i;

		i = gradSameInstances.find(node);
		if (i != gradSameInstances.end()) {
			gradSameInstances.erase(i);
		}

		i = instSameInstances.find(node);
		if (i != instSameInstances.end()) {
			instSameInstances.erase(i);
		}
	}
}
}
}
}
}
