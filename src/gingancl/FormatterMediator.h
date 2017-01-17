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

#ifndef _FormatterMediator_H_
#define _FormatterMediator_H_

#include "player/INCLPlayer.h"
#include "player/IApplicationPlayer.h"
#include "player/Player.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "ncl/components/ContextNode.h"
#include "ncl/components/CompositeNode.h"
#include "ncl/components/ContentNode.h"
#include "ncl/components/Node.h"
#include "ncl/components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/interfaces/Anchor.h"
#include "ncl/interfaces/PropertyAnchor.h"
#include "ncl/interfaces/Port.h"
#include "ncl/interfaces/SwitchPort.h"
#include "ncl/interfaces/InterfacePoint.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "ncl/switches/Rule.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/link/Bind.h"
#include "ncl/link/CausalLink.h"
#include "ncl/link/Link.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "ncl/connectors/EventUtil.h"
#include "ncl/connectors/SimpleAction.h"
#include "ncl/connectors/Connector.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl/reuse/ReferNode.h"
using namespace ::br::pucrio::telemidia::ncl::reuse;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "ncl/Base.h"
#include "ncl/NclDocument.h"
#include "ncl/connectors/ConnectorBase.h"
#include "ncl/descriptor/DescriptorBase.h"
#include "ncl/layout/RegionBase.h"
#include "ncl/switches/RuleBase.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl/transition/Transition.h"
#include "ncl/transition/TransitionBase.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "model/CompositeExecutionObject.h"
#include "model/ExecutionObject.h"
#include "model/NodeNesting.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "model/FormatterEvent.h"
#include "model/AttributionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "model/ILinkActionListener.h"
#include "model/LinkAssignmentAction.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "adaptation/RuleAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adaptation::context;

#include "ObjectCreationForbiddenException.h"
#include "FormatterConverter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::emconverter;

#include "PrefetchManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::prefetch;

#include "FormatterScheduler.h"
#include "PrivateBaseManager.h"

#include <map>
#include <set>
#include <vector>
#include <string>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_BEGIN

  class EntryEventListener : public IEventListener {
	private:
		Player* player;
		set<FormatterEvent*> events;
		int eventsRunning;
		bool hasStartPoint;
		pthread_mutex_t evMutex;

	public:
		EntryEventListener(Player* player, string interfaceId);
		virtual ~EntryEventListener();

		virtual void listenEvent(FormatterEvent* event);
		virtual void eventStateChanged(
			     void* event, short transition, short previousState);

		short getPriorityType();
  };

  class FormatterMediator : public INCLPlayer, public Player,
		public IFormatterSchedulerListener {

	private:
		EntryEventListener* entryEventListener;
		NclPlayerData* data;
		string currentFile;
		NclDocument* currentDocument;

		PrivateBaseManager* privateBaseManager;
		static PrefetchManager* pm;
		map<string, FormatterEvent*> documentEvents;
		map<string, vector<FormatterEvent*>*> documentEntryEvents;
		map<Port*, FormatterEvent*> portsToEntryEvents;
		FormatterScheduler* scheduler;
		RuleAdapter* ruleAdapter;
		FormatterConverter* compiler;
		PlayerAdapterManager* playerManager;
		ITimeBaseProvider* timeBaseProvider;
		vector<string> compileErrors;
		IDeviceLayout* deviceLayout;
		FormatterMultiDevice* multiDevice;
		PresentationContext* presContext;
		bool isEmbedded;
		bool enablePrefetch;
		bool docCompiled;
		pthread_mutex_t pteMutex;

	public:
		FormatterMediator(NclPlayerData* data);
		virtual ~FormatterMediator();

		static void release();

		void setMirrorSrc(IPlayer* mirrorSrc);
		void printGingaWindows();
		set<string>* createPortIdList();
		short getMappedInterfaceType(string portId);

		void setMrl(string mrl, bool visible=true);
		void reset(){};
		void rebase(){};
		void printData(NclPlayerData* data);
		void setTimeBaseProvider(ITimeBaseProvider* timeBaseProvider);

		void setBackgroundImage(string uri);
		void setParentLayout(void* parentLayout);
		string getScreenShot();

		vector<string>* getCompileErrors();
		void* setCurrentDocument(string fName);

	private:
		virtual void* addDocument(string fName);
		bool removeDocument(string documentId);
		ContextNode* getDocumentContext(string documentId);

	public:
		void setDepthLevel(int level);
		int getDepthLevel();

		Port* getPortFromEvent(FormatterEvent* event);

	private:
		vector<Port*>* getContextPorts(
				ContextNode* context, string interfaceId);

		vector<FormatterEvent*>* processDocument(
				string documentId, string interfaceId);

		void initializeSettingNodes(Node* node);

		vector<FormatterEvent*>* getDocumentEntryEvent(string documentId);

		bool compileDocument(string documentId);
		bool prepareDocument(string documentId);

		void solveRemoteDescriptorsUris(string docLocation,
					vector<GenericDescriptor*>* descs, bool isRemoteDoc);

		void solveRemoteNodesUris(
				string docLocation, vector<Node*>* nodes, bool isRemoteDoc);

		void solveRemoteNclDeps(string docLocation, bool isRemoteDoc);

		void solveRemoteLuaDeps(
				string docLocation, string src, bool isRemoteDoc);

		string solveRemoteSourceUri(string docLocation, string src);

		FormatterEvent* getEntryEvent(
				string interfaceId, vector<FormatterEvent*>* events);

		bool startDocument(string documentId, string interfaceId);
		bool stopDocument(string documentId);
		bool pauseDocument(string documentId);
		bool resumeDocument(string documentId);
		void presentationCompleted(FormatterEvent* documentEvent);

	public:
		bool nclEdit(string nclEditApi);
		bool editingCommand(string commandTag, string privateDataPayload);

	private:
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
		Transition* removeTransition(
				string documentId, string transitionId);

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

		void processInsertedReferNode(ReferNode* referNode);
		void processInsertedComposition(CompositeNode* composition);

		Node* addNode(
				string documentId, string compositeId, string xmlNode);

		Node* removeNode(
				string documentId, string compositeId, string nodeId);

		InterfacePoint* addInterface(
				string documentId, string nodeId, string xmlInterface);

		void removeInterfaceMappings(
				Node* node,
				InterfacePoint* interfacePoint,
				CompositeNode* composition);

		void removeInterfaceLinks(
				Node* node,
				InterfacePoint* interfacePoint,
				ContextNode* composition);

		void removeInterface(
				Node* node, InterfacePoint* interfacePoint);

		InterfacePoint* removeInterface(
				string documentId, string nodeId, string interfaceId);

		Link* addLink(
				string documentId, string compositeId, string xmlLink);

		void removeLink(ContextNode* composition, Link* link);

		Link* removeLink(
				string documentId, string compositeId, string linkId);

		bool setPropertyValue(
				string documentId,
				string nodeId,
				string propertyId,
				string value);

	public:
		void setNotifyContentUpdate(bool notify){};
		void addListener(IPlayerListener* listener);
		void removeListener(IPlayerListener* listener);
		void notifyPlayerListeners(
				short code, string paremeter, short type, string value);

		void setSurface(GingaSurfaceID surface);
		GingaSurfaceID getSurface();
		void flip();
		double getMediaTime();
		double getTotalMediaTime();
		void setMediaTime(double newTime);
		bool setKeyHandler(bool isHandler);
		void setScope(
				string scope,
				short type,
				double begin=-1, double end=-1, double outTransDur=-1);

		bool play();
		void stop();
		void abort();
		void pause();
		void resume();

		string getPropertyValue(string name);
		void setPropertyValue(string name, string value);

		void setReferenceTimePlayer(IPlayer* player);
		void addTimeReferPlayer(IPlayer* referPlayer);
		void removeTimeReferPlayer(IPlayer* referPlayer);
		void notifyReferPlayers(int transition);
		void timebaseObjectTransitionCallback(int transition);
		void setTimeBasePlayer(IPlayer* timeBasePlayer);
		bool hasPresented();
		void setPresented(bool presented);
		bool isVisible();
		void setVisible(bool visible);
		bool immediatelyStart();
		void setImmediatelyStart(bool immediatelyStartVal);
		void forceNaturalEnd(bool forceIt);
		bool isForcedNaturalEnd();
		bool setOutWindow(GingaWindowID windowId);

		/*Exclusive for ChannelPlayer*/
		IPlayer* getSelectedPlayer() {return NULL;};
		void setPlayerMap(map<string, IPlayer*>* objs){};
		map<string, IPlayer*>* getPlayerMap() {return NULL;};
		IPlayer* getPlayer(string objectId) {return NULL;};
		void select(IPlayer* selObject){};

		/*Exclusive for Application Players*/
		void setCurrentScope(string scopeId);

		string getActiveUris(vector<string>* uris);
		string getDepUris(vector<string>* uris, int targetDev=0);

		PresentationContext* getPresentationContext();

	private:
		string getDepUrisFromNodes(
				vector<string>* uris, vector<Node*>* nodes, int targetDev=0);

		string getDepUriFromNode(
				vector<string>* uris, Node* node, int targetDev=0);

		string getBaseUri(string baseA, string baseB);

	public:
		void timeShift(string direction);
  };

BR_PUCRIO_TELEMIDIA_GINGA_NCL_END
#endif //_FormatterMediator_H_
