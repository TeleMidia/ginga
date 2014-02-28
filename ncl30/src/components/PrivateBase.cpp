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

#include "ncl/components/PrivateBase.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {
	PrivateBase::PrivateBase(string uid) :
		    CompositeNode(uid) {

		typeSet.insert("PrivateBase");
	}

	PrivateBase::~PrivateBase() {
		vector<Node*>::iterator i;
		Node* node;

		i = nodes.begin();
		while (i != nodes.end()) {
			node = *i;
			if (node != this && node->getParentComposition() == this) {
				delete node;
				node = NULL;
			}
			++i;
		}
		nodes.clear();
	}

	bool PrivateBase::addNode(Node* node) {
		if (node == NULL || this->getNode(node->getId()) != NULL) {
			return false;
		}

		nodes.push_back(node);
		node->setParentComposition(this);
		return true;
	}

	bool PrivateBase::removeAllNodeOccurrences(string nodeUID) {
		Node* node;

		node = getNode(nodeUID);
		return removeAllNodeOccurrences(node);
	}

	bool PrivateBase::removeAllNodeOccurrences(Node* node) {
		if (node == NULL)
			return false;

		vector<Node*>::iterator iterNode;
		vector<Node*>::iterator i;

		iterNode = nodes.begin();
		while (iterNode != nodes.end()) {
			if ((*(*iterNode)).getId() == node->getId()) {
				i = nodes.erase(iterNode);
				if (i == nodes.end()) {
					if ((*iterNode)->instanceOf("ContextNode")) {
						((ContextNode*)(*iterNode))->removeNode(node);
					}
					return true;
				}
			}
			if ((*iterNode)->instanceOf("ContextNode")) {
				((ContextNode*)(*iterNode))->removeNode(node);
			}
			++iterNode;
		}
		return true;
	}
}
}
}
}
}
