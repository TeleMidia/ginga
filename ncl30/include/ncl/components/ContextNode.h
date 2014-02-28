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

#ifndef _CONTEXTNODE_H_
#define _CONTEXTNODE_H_

#include "../descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "../link/Link.h"
#include "../link/LinkComposition.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "NodeEntity.h"
#include "IllegalNodeTypeException.h"
#include "CompositeNode.h"

#include <map>
#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {
	class ContextNode : public CompositeNode, public LinkComposition {
		protected:
			map<string, GenericDescriptor*> descriptorCollection;

		private:
			set<Link*> linkSet;

		public:
			ContextNode(string id);
			virtual ~ContextNode();

			bool instanceOf(string className) {
				return Entity::instanceOf(className);
			};

			bool addLink(Link* link);
			bool addNode(Node* node)
				    throw(IllegalNodeTypeException*);

			void clearLinks();
			bool containsLink(Link* link);
			virtual set<Link*>* getLinks();
			Link* getLink(string linkId);
			GenericDescriptor* getNodeDescriptor(Node *node);
			int getNumLinks();
			bool removeLink(string linkId);
			bool removeLink(Link* link);
			bool setNodeDescriptor(
				    string nodeId,
				    GenericDescriptor* descriptor);
	};
}
}
}
}
}

#endif //_CONTEXTNODE_H_
