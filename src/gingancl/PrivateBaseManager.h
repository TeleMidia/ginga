/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef PrivateBaseManager_H_
#define PrivateBaseManager_H_

#include "system/Thread.h"
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
