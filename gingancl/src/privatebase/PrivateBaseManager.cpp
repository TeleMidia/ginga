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

#include "gingancl/privatebase/PrivateBaseManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
	PrivateBaseManager::PrivateBaseManager() {
		Thread::mutexInit(&mutexTable, false);
	}

	PrivateBaseManager::~PrivateBaseManager() {
		releasePrivateBases();

		Thread::mutexDestroy(&mutexTable);
	}

	void PrivateBaseManager::releasePrivateBases() {
		map<string, PrivateBaseContext*>::iterator i;

		lockTable();
		i = privateBases.begin();
		while (i != privateBases.end()) {
			delete i->second;
			++i;
		}
		privateBases.clear();
		unlockTable();
	}

	void PrivateBaseManager::createPrivateBase(string id) {
		PrivateBaseContext* privateBaseContext;

		lockTable();
		if (privateBases.count(id) == 0) {
			privateBaseContext = new PrivateBaseContext();
			privateBaseContext->createPrivateBase(id);
			privateBases[id] = privateBaseContext;

		} else {
			clog << "PrivateBaseManager::createPrivateBase Warning! ";
			clog << "Trying to overwrite the '" << id << "' private base";
			clog << endl;
		}
		unlockTable();
	}

	NclDocument* PrivateBaseManager::addDocument(
			string id, string location, IDeviceLayout* deviceLayout) {

		PrivateBaseContext* privateBaseContext;
		NclDocument* document = NULL;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			document = privateBaseContext->addDocument(location, deviceLayout);
		}

		return document;
	}

	NclDocument* PrivateBaseManager::embedDocument(
			string id,
			string docId,
			string nodeId,
			string location,
			IDeviceLayout* deviceLayout) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->embedDocument(
					docId, nodeId, location, deviceLayout);
		}

		return NULL;
	}

	void* PrivateBaseManager::addVisibleDocument(
			string id, string location, IDeviceLayout* deviceLayout) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addVisibleDocument(
					location, deviceLayout);
		}

		return NULL;
	}

	string PrivateBaseManager::getDocumentLocation(
			string id, string docId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->getDocumentLocation(docId);
		}

		return "";
	}

	string PrivateBaseManager::getEmbeddedDocumentLocation(
			string id, string parentDocId, string nodeId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->getEmbeddedDocumentLocation(
					parentDocId, nodeId);
		}

		return "";
	}

	NclDocument* PrivateBaseManager::getDocument(
			string id, string docId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->getDocument(docId);
		}

		return NULL;
	}

	NclDocument* PrivateBaseManager::getEmbeddedDocument(
			string id, string parentDocId, string nodeId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->getEmbeddedDocument(
					parentDocId, nodeId);
		}

		return NULL;
	}

	vector<NclDocument*>* PrivateBaseManager::getDocuments(string id) {
		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->getDocuments();
		}

		return NULL;
	}

	NclDocument* PrivateBaseManager::removeDocument(string id, string docId) {
		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeDocument(docId);
		}

		return NULL;
	}

	NclDocument* PrivateBaseManager::removeEmbeddedDocument(
			string baseId, string parentDocId, string nodeId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(baseId);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeEmbeddedDocument(
					parentDocId, nodeId);
		}

		return NULL;
	}

	LayoutRegion* PrivateBaseManager::addRegion(
			string id,
		    string documentId,
		    string regionBaseId,
		    string regionId,
		    string xmlRegion) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addRegion(
					documentId, regionBaseId, regionId, xmlRegion);
		}

		return NULL;
	}

	LayoutRegion* PrivateBaseManager::removeRegion(
			string id,
			string documentId,
			string regionBaseId,
			string regionId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeRegion(
					documentId, regionBaseId, regionId);
		}

		return NULL;
	}

	RegionBase* PrivateBaseManager::addRegionBase(
			string id, string documentId, string xmlRegionBase) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addRegionBase(documentId, xmlRegionBase);
		}

		return NULL;
	}

	RegionBase* PrivateBaseManager::removeRegionBase(
			string id, string documentId, string regionBaseId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeRegionBase(
					documentId, regionBaseId);
		}

		return NULL;
	}

	Rule* PrivateBaseManager::addRule(
			string id, string documentId, string xmlRule) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addRule(documentId, xmlRule);
		}

		return NULL;
	}

	Rule* PrivateBaseManager::removeRule(
			string id, string documentId, string ruleId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeRule(documentId, ruleId);
		}

		return NULL;
	}

	RuleBase* PrivateBaseManager::addRuleBase(
			string id, string documentId, string xmlRuleBase) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addRuleBase(documentId, xmlRuleBase);
		}

		return NULL;
	}

	RuleBase* PrivateBaseManager::removeRuleBase(
			string id, string documentId, string ruleBaseId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeRuleBase(documentId, ruleBaseId);
		}

		return NULL;
	}

	Transition* PrivateBaseManager::addTransition(
			string id, string documentId, string xmlTransition) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addTransition(documentId, xmlTransition);
		}

		return NULL;
	}

	Transition* PrivateBaseManager::removeTransition(
			string id, string documentId, string transitionId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeTransition(
					documentId, transitionId);
		}

		return NULL;
	}

	TransitionBase* PrivateBaseManager::addTransitionBase(
			string id, string documentId, string xmlTransitionBase) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addTransitionBase(
					documentId, xmlTransitionBase);
		}

		return NULL;
	}

	TransitionBase* PrivateBaseManager::removeTransitionBase(
			string id, string documentId, string transitionBaseId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeTransitionBase(
					documentId, transitionBaseId);
		}

		return NULL;
	}

	Connector* PrivateBaseManager::addConnector(
			string id, string documentId, string xmlConnector) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addConnector(documentId, xmlConnector);
		}

		return NULL;
	}

	Connector* PrivateBaseManager::removeConnector(
			string id, string documentId, string connectorId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeConnector(documentId, connectorId);
		}

		return NULL;
	}

	ConnectorBase* PrivateBaseManager::addConnectorBase(
			string id, string documentId, string xmlConnectorBase) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addConnectorBase(
					documentId, xmlConnectorBase);
		}

		return NULL;
	}

	ConnectorBase* PrivateBaseManager::removeConnectorBase(
			string id, string documentId, string connectorBaseId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeConnectorBase(
					documentId, connectorBaseId);
		}

		return NULL;
	}

	GenericDescriptor* PrivateBaseManager::addDescriptor(
			string id, string documentId, string xmlDescriptor) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addDescriptor(documentId, xmlDescriptor);
		}

		return NULL;
	}

	GenericDescriptor* PrivateBaseManager::removeDescriptor(
			string id, string documentId, string descriptorId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeDescriptor(
					documentId, descriptorId);
		}

		return NULL;
	}

	DescriptorBase* PrivateBaseManager::addDescriptorBase(
			string id, string documentId, string xmlDescriptorBase) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addDescriptorBase(
					documentId, xmlDescriptorBase);
		}

		return NULL;
	}

	DescriptorBase* PrivateBaseManager::removeDescriptorBase(
			string id, string documentId, string descriptorBaseId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeDescriptorBase(
					documentId, descriptorBaseId);
		}

		return NULL;
	}

	Base* PrivateBaseManager::addImportBase(
			string id,
			string documentId,
			string docBaseId,
			string xmlImportBase) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addImportBase(
					documentId, docBaseId, xmlImportBase);
		}

		return NULL;
	}

	Base* PrivateBaseManager::removeImportBase(
			string id,
			string documentId,
			string docBaseId,
			string documentURI) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeImportBase(
					documentId, docBaseId, documentURI);
		}

		return NULL;
	}

	NclDocument* PrivateBaseManager::addImportedDocumentBase(
			string id,
			string documentId,
			string xmlImportedDocumentBase) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addImportedDocumentBase(
					documentId, xmlImportedDocumentBase);
		}

		return NULL;
	}

	NclDocument* PrivateBaseManager::removeImportedDocumentBase(
			string id,
			string documentId,
			string importedDocumentBaseId) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeImportedDocumentBase(
					documentId, importedDocumentBaseId);
		}

		return NULL;
	}

	NclDocument* PrivateBaseManager::addImportNCL(
			string id, string documentId, string xmlImportNCL) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addImportNCL(documentId, xmlImportNCL);
		}

		return NULL;
	}

	NclDocument* PrivateBaseManager::removeImportNCL(
			string id, string documentId, string documentURI) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->removeImportNCL(documentId, documentURI);
		}

		return NULL;
	}

	Node* PrivateBaseManager::addNode(
			string id,
			string documentId,
			string compositeId,
			string xmlNode) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addNode(
					documentId, compositeId, xmlNode);
		}

		return NULL;
	}

	InterfacePoint* PrivateBaseManager::addInterface(
			string id, string documentId, string nodeId, string xmlInterface) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addInterface(
					documentId, nodeId, xmlInterface);
		}

		return NULL;
	}

	Link* PrivateBaseManager::addLink(
			string id,
			string documentId,
			string compositeId,
			string xmlLink) {

		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->addLink(
					documentId, compositeId, xmlLink);
		}

		return NULL;
	}

	PrivateBaseContext* PrivateBaseManager::getPrivateBaseContext(string id) {
		map<string, PrivateBaseContext*>::iterator i;
		PrivateBaseContext* privateBaseContext = NULL;

		lockTable();
		i = privateBases.find(id);
		if (i != privateBases.end()) {
			privateBaseContext = i->second;
		}
		unlockTable();

		return privateBaseContext;
	}

	PrivateBase* PrivateBaseManager::getPrivateBase(string id) {
		PrivateBaseContext* privateBaseContext;

		privateBaseContext = getPrivateBaseContext(id);
		if (privateBaseContext != NULL) {
			return privateBaseContext->getPrivateBase();
		}

		return NULL;
	}

	void PrivateBaseManager::lockTable() {
		Thread::mutexLock(&mutexTable);
	}

	void PrivateBaseManager::unlockTable() {
		Thread::mutexUnlock(&mutexTable);
	}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::ncl::IPrivateBaseManager*
		createPrivateBaseManager() {

	return (new ::br::pucrio::telemidia::ginga::ncl::PrivateBaseManager());
}
