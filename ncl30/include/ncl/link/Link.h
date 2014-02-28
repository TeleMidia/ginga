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

#ifndef _LINK_H_
#define _LINK_H_

#include "../Entity.h"
#include "../Parameter.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "../descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "../components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "../connectors/Connector.h"
#include "../connectors/Role.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "../interfaces/InterfacePoint.h"
#include "../interfaces/SwitchPort.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "Bind.h"

#include <map>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace link {
	class LinkComposition;
	class Link : public Entity {
		protected:
			Connector *connector;
			map<string, vector<Bind*>*> *roleBinds;
			vector<Bind*> *binds;
			map<string, Parameter*> *parameters;

		private:
			LinkComposition* composition;

		public:
			Link(string id, Connector *connector);
			virtual ~Link();
			Bind* bind(Node *node, InterfacePoint *interfPt,
				    GenericDescriptor *desc, string roleId);

			Bind* bind(Node *node, InterfacePoint *interfPt,
				    GenericDescriptor *desc, Role *role);

			bool isConsistent();
			Bind* getBind(Node *node, InterfacePoint *interfPt,
				    GenericDescriptor *desc, Role *role);

			vector<Bind*> *getBinds();
			Connector *getConnector();
			void setParentComposition(LinkComposition* composition);
			LinkComposition* getParentComposition();
			unsigned int getNumBinds();
			unsigned int getNumRoleBinds(Role *role);
			vector<Bind*> *getRoleBinds(Role *role);
			bool isMultiPoint();
			string toString();
			void setConnector(Connector *connector);
			bool unBind(Bind *bind);
			void addParameter(Parameter *parameter);
			vector<Parameter*> *getParameters();
			Parameter *getParameter(string name);
			void removeParameter(Parameter *parameter);
			void updateConnector(Connector *newConnector);
			bool containsNode(Node *node, GenericDescriptor *descriptor);

		protected:
			bool containsNode(Node *node, GenericDescriptor *descriptor,
				    vector<Bind*> *binds);
	};
}
}
}
}
}

#endif //_LINK_H_
