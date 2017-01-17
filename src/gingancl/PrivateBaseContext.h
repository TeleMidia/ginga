/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef PrivateBaseContext_H_
#define PrivateBaseContext_H_

#include "ncl/components/PrivateBase.h"
#include "ncl/components/ContextNode.h"
#include "ncl/components/Node.h"
#include "ncl/components/ReferenceContent.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/switches/Rule.h"
#include "ncl/switches/SwitchNode.h"
#include "ncl/switches/RuleBase.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl/interfaces/SwitchPort.h"
#include "ncl/interfaces/Anchor.h"
#include "ncl/interfaces/InterfacePoint.h"
#include "ncl/interfaces/Port.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/connectors/Connector.h"
#include "ncl/connectors/ConnectorBase.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/layout/IDeviceLayout.h"
#include "ncl/layout/LayoutRegion.h"
#include "ncl/layout/RegionBase.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl/descriptor/GenericDescriptor.h"
#include "ncl/descriptor/DescriptorBase.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/link/Link.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "ncl/transition/Transition.h"
#include "ncl/transition/TransitionBase.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "ncl/Base.h"
#include "ncl/NclDocument.h"
#include "ncl/IPrivateBaseContext.h"
using namespace ::br::pucrio::telemidia::ncl;

#include <map>
#include <vector>
#include <string>
using namespace std;

typedef struct {
	string embeddedNclNodeId;
	string embeddedNclNodeLocation;
	NclDocument* embeddedDocument;
} EmbeddedNclData;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_BEGIN

  class PrivateBaseContext : public IPrivateBaseContext {
	private:
		map<string, NclDocument*> baseDocuments;
		map<string, NclDocument*> visibleDocuments;
		map<string, EmbeddedNclData*> embeddedDocuments;
		map<NclDocument*, IDeviceLayout*> layouts;
		set<IDeviceLayout*> layoutsGB; //layouts garbage collector
		PrivateBase* privateBase;

	public:
		PrivateBaseContext();
		virtual ~PrivateBaseContext();

		void createPrivateBase(string id);

	private:
		NclDocument* compileDocument(
				string location, IDeviceLayout* deviceLayout);

	public:
		NclDocument* addDocument(
				string location, IDeviceLayout* deviceLayout);

		NclDocument* embedDocument(
				string docId,
				string nodeId,
				string location,
				IDeviceLayout* deviceLayout);

		void* addVisibleDocument(string location, IDeviceLayout* deviceLayout);
		NclDocument* getVisibleDocument(string docId);

	private:
		bool eraseVisibleDocument(string docLocation);

	public:
		string getDocumentLocation(string docId);
		string getEmbeddedDocumentLocation(string parentDocId, string nodeId);
		NclDocument* getDocument(string id);
		NclDocument* getEmbeddedDocument(string parendDocId, string nodeId);
		vector<NclDocument*>* getDocuments();

	private:
		void removeDocumentBase(NclDocument* document, Base* base);
		void removeDocumentBases(NclDocument* document);

	public:
		NclDocument* removeDocument(string id);
		NclDocument* removeEmbeddedDocument(string parentDocId, string nodeId);

	private:
		NclDocument* getBaseDocument(string documentId);
		void* compileEntity(
			     string location,
			     NclDocument* document,
			     void* parentObject);

	public:
		LayoutRegion* addRegion(
			     string documentId,
			     string regionBaseId,
			     string regionId,
			     string xmlRegion);

		LayoutRegion* removeRegion(
				string documentId,
				string regionBaseId,
				string regionId);

		RegionBase* addRegionBase(string documentId, string xmlRegionBase);

		RegionBase* removeRegionBase(
			    string documentId, string regionBaseId);

		Rule* addRule(string documentId, string xmlRule);
		Rule* removeRule(string documentId, string ruleId);
		RuleBase* addRuleBase(string documentId, string xmlRuleBase);
		RuleBase* removeRuleBase(string documentId, string ruleBaseId);
		Transition* addTransition(string documentId, string xmlTransition);
		Transition* removeTransition(string documentId, string transitionId);
		TransitionBase* addTransitionBase(
			    string documentId, string xmlTransitionBase);

		TransitionBase* removeTransitionBase(
			    string documentId, string transitionBaseId);

		Connector* addConnector(string documentId, string xmlConnector);
		Connector* removeConnector(string documentId, string connectorId);
		ConnectorBase* addConnectorBase(
			    string documentId, string xmlConnectorBase);

		ConnectorBase* removeConnectorBase(
			    string documentId, string connectorBaseId);

		GenericDescriptor* addDescriptor(
			    string documentId, string xmlDescriptor);

		GenericDescriptor* removeDescriptor(
			    string documentId, string descriptorId);

		DescriptorBase* addDescriptorBase(
			    string documentId, string xmlDescriptorBase);

		DescriptorBase* removeDescriptorBase(
			    string documentId, string descriptorBaseId);

	private:
		Base* getBase(NclDocument* document, string baseId);

	public:
		Base* addImportBase(
			    string documentId, string docBaseId, string xmlImportBase);

		Base* removeImportBase(
			    string documentId, string docBaseId, string documentURI);

		NclDocument* addImportedDocumentBase(
			    string documentId, string xmlImportedDocumentBase);

		NclDocument* removeImportedDocumentBase(
			    string documentId, string importedDocumentBaseId);

		NclDocument* addImportNCL(string documentId, string xmlImportNCL);
		NclDocument* removeImportNCL(string documentId, string documentURI);
		Node* addNode(string documentId, string compositeId, string xmlNode);
		InterfacePoint* addInterface(
			    string documentId, string nodeId, string xmlInterface);

		Link* addLink(string documentId, string compositeId, string xmlLink);
		PrivateBase* getPrivateBase();
  };

BR_PUCRIO_TELEMIDIA_GINGA_NCL_END
#endif /*PrivateBaseContext_H_*/
