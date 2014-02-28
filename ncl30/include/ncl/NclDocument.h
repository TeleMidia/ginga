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

#ifndef NCLDOCUMENT_H_
#define NCLDOCUMENT_H_

extern "C" {
	#include <assert.h>
}

#include "switches/Rule.h"
#include "switches/RuleBase.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "components/ContextNode.h"
#include "components/Node.h"
#include "components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "connectors/Connector.h"
#include "connectors/ConnectorBase.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "descriptor/GenericDescriptor.h"
#include "descriptor/DescriptorBase.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "layout/LayoutRegion.h"
#include "layout/RegionBase.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "transition/Transition.h"
#include "transition/TransitionBase.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "metainformation/Meta.h"
#include "metainformation/Metadata.h"
using namespace ::br::pucrio::telemidia::ncl::metainformation;

#include "Base.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
  class NclDocument {
	private:
		ContextNode* body;
		ConnectorBase* connectorBase;
		DescriptorBase* descriptorBase;
		map<string, NclDocument*> documentAliases;
		vector<NclDocument*> documentBase;
		map<string, NclDocument*> documentLocations;
		string id;
		map<int, RegionBase*> regionBases;
		RuleBase* ruleBase;
		TransitionBase* transitionBase;
		vector<Meta*> metainformationList;
		vector<Metadata*> metadataList;

		string docLocation;
		NclDocument* parentDocument;

	public:
		NclDocument(string id, string docLocation);
		~NclDocument();

		NclDocument* getParentDocument();
		void setParentDocument(NclDocument* parentDocument);
		string getDocumentPerspective();

		string getDocumentLocation();

		bool addDocument(NclDocument* document, string alias, string location);
		void clearDocument();
		Connector* getConnector(string connectorId);
		ConnectorBase* getConnectorBase();
		Transition* getTransition(string transitionId);
		TransitionBase* getTransitionBase();
		GenericDescriptor* getDescriptor(string descriptorId);
		DescriptorBase* getDescriptorBase();
		NclDocument* getDocument(string documentId);
		string getDocumentAlias(NclDocument* document);
		ContextNode* getBody();
		string getDocumentLocation(NclDocument* document);
		vector<NclDocument*>* getDocuments();
		string getId();

	private:
		Node* getNodeLocally(string nodeId);

	public:
		Node* getNode(string nodeId);
		LayoutRegion* getRegion(string regionId);

	private:
		LayoutRegion* getRegion(string regionId, RegionBase* regionBase);

	public:
		RegionBase* getRegionBase(int devClass);
		RegionBase* getRegionBase(string regionBaseId);
		map<int, RegionBase*>* getRegionBases();
		Rule* getRule(string ruleId);
		RuleBase* getRuleBase();
		bool removeDocument(NclDocument* document);
		void setConnectorBase(ConnectorBase* connectorBase);
		void setTransitionBase(TransitionBase* transitionBase);
		void setDescriptorBase(DescriptorBase* descriptorBase);
		void setDocumentAlias(NclDocument* document, string alias);
		void setBody(ContextNode* node);
		void setDocumentLocation(NclDocument* document, string location);
		void setId(string id);
		void addRegionBase(RegionBase* regionBase);
		void setRuleBase(RuleBase* ruleBase);
		void addMetainformation(Meta* meta);
		void addMetadata(Metadata* metadata);
		vector<Meta*>* getMetainformation();
		vector<Metadata*>* getMetadata();
		void removeRegionBase(string regionBaseId);
		void removeMetainformation(Meta* meta);
		void removeMetadata(Metadata* metadata);
		void removeAllMetainformation();
		void removeAllMetadata();
  };
}
}
}
}

#endif /*NCLDOCUMENT_H_*/
