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

#ifndef PrivateBaseManager_H_
#define PrivateBaseManager_H_

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "IPrivateBaseManager.h"
#include "PrivateBaseContext.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
  class PrivateBaseManager : public IPrivateBaseManager {
	private:
		map<string, PrivateBaseContext*> privateBases;
		pthread_mutex_t mutexTable;

	public:
		PrivateBaseManager();
		virtual ~PrivateBaseManager();

		void createPrivateBase(string id);
		void releasePrivateBases();

		NclDocument* addDocument(
				string id, string location, IDeviceLayout* deviceLayout);

		NclDocument* embedDocument(
				string id,
				string docId,
				string nodeId,
				string location,
				IDeviceLayout* deviceLayout);

		void* addVisibleDocument(
				string id, string location, IDeviceLayout* deviceLayout);

		string getDocumentLocation(string id, string docId);
		string getEmbeddedDocumentLocation(
				string baseId, string parentDocId, string nodeId);

		NclDocument* getDocument(string id, string docId);
		NclDocument* getEmbeddedDocument(
				string baseId, string parendDocId, string nodeId);

		vector<NclDocument*>* getDocuments(string id);
		NclDocument* removeDocument(string id, string docId);
		NclDocument* removeEmbeddedDocument(
				string baseId, string parentDocId, string nodeId);

		LayoutRegion* addRegion(
				string id,
				string documentId,
				string regionBaseId,
				string regionId,
				string xmlRegion);

		LayoutRegion* removeRegion(
				string id,
				string documentId,
				string regionBaseId,
				string regionId);

		RegionBase* addRegionBase(
				string id,
				string documentId,
				string xmlRegionBase);

		RegionBase* removeRegionBase(
				string id,
				string documentId,
				string regionBaseId);

		Rule* addRule(
				string id,
				string documentId,
				string xmlRule);

		Rule* removeRule(
				string id,
				string documentId,
				string ruleId);

		RuleBase* addRuleBase(
				string id,
				string documentId,
				string xmlRuleBase);

		RuleBase* removeRuleBase(
				string id,
				string documentId,
				string ruleBaseId);

		Transition* addTransition(
				string id,
				string documentId,
				string xmlTransition);

		Transition* removeTransition(
				string id,
				string documentId,
				string transitionId);

		TransitionBase* addTransitionBase(
				string id,
				string documentId,
			    string xmlTransitionBase);

		TransitionBase* removeTransitionBase(
				string id,
				string documentId,
				string transitionBaseId);

		Connector* addConnector(
				string id,
				string documentId,
				string xmlConnector);

		Connector* removeConnector(
				string id,
				string documentId,
				string connectorId);

		ConnectorBase* addConnectorBase(
				string id,
				string documentId,
				string xmlConnectorBase);

		ConnectorBase* removeConnectorBase(
				string id,
				string documentId,
				string connectorBaseId);

		GenericDescriptor* addDescriptor(
				string id,
				string documentId,
				string xmlDescriptor);

		GenericDescriptor* removeDescriptor(
				string id,
				string documentId,
				string descriptorId);

		DescriptorBase* addDescriptorBase(
				string id,
				string documentId,
				string xmlDescriptorBase);

		DescriptorBase* removeDescriptorBase(
				string id,
				string documentId,
				string descriptorBaseId);

		Base* addImportBase(
				string id,
				string documentId,
				string docBaseId,
				string xmlImportBase);

		Base* removeImportBase(
				string id,
				string documentId,
				string docBaseId,
				string documentURI);

		NclDocument* addImportedDocumentBase(
				string id,
				string documentId,
				string xmlImportedDocumentBase);

		NclDocument* removeImportedDocumentBase(
				string id,
				string documentId,
				string importedDocumentBaseId);

		NclDocument* addImportNCL(
				string id,
				string documentId,
				string xmlImportNCL);

		NclDocument* removeImportNCL(
				string id,
				string documentId,
				string documentURI);

		Node* addNode(
				string id,
				string documentId,
				string compositeId,
				string xmlNode);

		InterfacePoint* addInterface(
				string id,
				string documentId,
				string nodeId,
				string xmlInterface);

		Link* addLink(
				string id,
				string documentId,
				string compositeId,
				string xmlLink);

	private:
		PrivateBaseContext* getPrivateBaseContext(string id);

	public:
		PrivateBase* getPrivateBase(string id);

	private:
		void lockTable();
		void unlockTable();
  };
}
}
}
}
}

#endif /*PrivateBaseManager_H_*/
