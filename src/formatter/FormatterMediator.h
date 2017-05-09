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

#ifndef FORMATTER_MEDIATOR_H
#define FORMATTER_MEDIATOR_H

#include "player/INCLPlayer.h"
#include "player/IApplicationPlayer.h"
#include "player/Player.h"
using namespace ::ginga::player;

#include "ncl/ContextNode.h"
#include "ncl/CompositeNode.h"
#include "ncl/ContentNode.h"
#include "ncl/Node.h"
#include "ncl/NodeEntity.h"
using namespace ::ginga::ncl;

#include "ncl/Anchor.h"
#include "ncl/PropertyAnchor.h"
#include "ncl/Port.h"
#include "ncl/SwitchPort.h"
#include "ncl/InterfacePoint.h"
using namespace ::ginga::ncl;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "ncl/Rule.h"
using namespace ::ginga::ncl;

#include "ncl/GenericDescriptor.h"
using namespace ::ginga::ncl;

#include "ncl/Bind.h"
#include "ncl/CausalLink.h"
#include "ncl/Link.h"
using namespace ::ginga::ncl;

#include "ncl/EventUtil.h"
#include "ncl/SimpleAction.h"
#include "ncl/Connector.h"
using namespace ::ginga::ncl;

#include "ncl/LayoutRegion.h"
using namespace ::ginga::ncl;

#include "ncl/ReferNode.h"
using namespace ::ginga::ncl;

#include "util/functions.h"
using namespace ::ginga::util;

#include "ncl/Base.h"
#include "ncl/NclDocument.h"
#include "ncl/ConnectorBase.h"
#include "ncl/DescriptorBase.h"
#include "ncl/RegionBase.h"
#include "ncl/RuleBase.h"
using namespace ::ginga::ncl;

#include "ncl/Transition.h"
#include "ncl/TransitionBase.h"
using namespace ::ginga::ncl;

#include "NclCompositeExecutionObject.h"
#include "NclExecutionObject.h"
#include "NclNodeNesting.h"

#include "NclFormatterEvent.h"
#include "NclAttributionEvent.h"

#include "INclLinkActionListener.h"
#include "NclLinkAssignmentAction.h"

#include "RuleAdapter.h"

#include "FormatterConverter.h"

#include "FormatterScheduler.h"
#include "PrivateBaseManager.h"

GINGA_FORMATTER_BEGIN

class EntryEventListener : public INclEventListener
{
private:
  Player *player;
  set<NclFormatterEvent *> events;
  int eventsRunning;
  bool hasStartPoint;
  pthread_mutex_t evMutex;

public:
  EntryEventListener (Player *player, const string &interfaceId);
  virtual ~EntryEventListener ();

  virtual void listenEvent (NclFormatterEvent *event);
  virtual void eventStateChanged (void *event, short transition,
                                  short previousState);

  short getPriorityType ();
};

class FormatterMediator : public INCLPlayer,
                          public Player,
                          public IFormatterSchedulerListener
{
private:
  EntryEventListener *entryEventListener;
  NclPlayerData *data;
  string currentFile;
  NclDocument *currentDocument;

  PrivateBaseManager *privateBaseManager;
  map<string, NclFormatterEvent *> documentEvents;
  map<string, vector<NclFormatterEvent *> *> documentEntryEvents;
  map<Port *, NclFormatterEvent *> portsToEntryEvents;
  FormatterScheduler *scheduler;
  RuleAdapter *ruleAdapter;
  FormatterConverter *compiler;
  AdapterPlayerManager *playerManager;
  vector<string> compileErrors;
  DeviceLayout *deviceLayout;
  FormatterMultiDevice *multiDevice;
  PresentationContext *presContext;
  bool isEmbedded;
  bool docCompiled;
  pthread_mutex_t pteMutex;

public:
  FormatterMediator (NclPlayerData *data);
  virtual ~FormatterMediator ();

  static void release ();

  set<string> *createPortIdList ();
  short getMappedInterfaceType (const string &portId);

  void setMrl (const string &mrl, bool visible = true);
  void reset (){};
  void rebase (){};
  void printData (NclPlayerData *data);
  void setTimeBaseProvider (ITimeBaseProvider *timeBaseProvider);

  void setParentLayout (void *parentLayout);

  vector<string> *getCompileErrors ();
  void *setCurrentDocument (const string &fName);

private:
  virtual void *addDocument (const string &fName);
  bool removeDocument (const string &documentId);
  ContextNode *getDocumentContext (const string &documentId);

public:
  void setDepthLevel (int level);
  int getDepthLevel ();

  Port *getPortFromEvent (NclFormatterEvent *event);

private:
  vector<Port *> *getContextPorts (ContextNode *context,
                                   const string &interfaceId);

  vector<NclFormatterEvent *> *processDocument (const string &documentId,
                                                const string &interfaceId);

  void initializeSettingNodes (Node *node);

  vector<NclFormatterEvent *> *getDocumentEntryEvent (const string &documentId);

  bool compileDocument (const string &documentId);
  bool prepareDocument (const string &documentId);

  NclFormatterEvent *getEntryEvent (const string &interfaceId,
                                    vector<NclFormatterEvent *> *events);

  bool startDocument (const string &documentId, const string &interfaceId);
  bool stopDocument (const string &documentId);
  bool pauseDocument (const string &documentId);
  bool resumeDocument (const string &documentId);
  void presentationCompleted (NclFormatterEvent *documentEvent);

public:
  bool nclEdit (const string &nclEditApi);
  bool editingCommand (const string &commandTag, const string &privateDataPayload);

private:
  LayoutRegion *addRegion (const string &documentId, const string &regionBaseId,
                           const string &regionId, const string &xmlRegion);

  LayoutRegion *removeRegion (const string &documentId, const string &regionBaseId,
                              const string &regionId);

  RegionBase *addRegionBase (const string &documentId, const string &xmlRegionBase);
  RegionBase *removeRegionBase (const string &documentId, const string &regionBaseId);

  Rule *addRule (const string &documentId, const string &xmlRule);
  Rule *removeRule (const string &documentId, const string &ruleId);
  RuleBase *addRuleBase (const string &documentId, const string &xmlRuleBase);
  RuleBase *removeRuleBase (const string &documentId, const string &ruleBaseId);
  Transition *addTransition (const string &documentId, const string &xmlTransition);
  Transition *removeTransition (const string &documentId, const string &transitionId);

  TransitionBase *addTransitionBase (const string &documentId,
                                     const string &xmlTransitionBase);

  TransitionBase *removeTransitionBase (const string &documentId,
                                        const string &transitionBaseId);

  Connector *addConnector (const string &documentId, const string &xmlConnector);
  Connector *removeConnector (const string &documentId, const string &connectorId);
  ConnectorBase *addConnectorBase (const string &documentId,
                                   const string &xmlConnectorBase);

  ConnectorBase *removeConnectorBase (const string &documentId,
                                      const string &connectorBaseId);

  GenericDescriptor *addDescriptor (const string &documentId,
                                    const string &xmlDescriptor);

  GenericDescriptor *removeDescriptor (const string &documentId,
                                       const string &descriptorId);

  DescriptorBase *addDescriptorBase (const string &documentId,
                                     const string &xmlDescriptorBase);

  DescriptorBase *removeDescriptorBase (const string &documentId,
                                        const string &descriptorBaseId);

  Base *addImportBase (const string &documentId, const string &docBaseId,
                       const string &xmlImportBase);

  Base *removeImportBase (const string &documentId, const string &docBaseId,
                          const string &documentURI);

  NclDocument *addImportedDocumentBase (const string &documentId,
                                        const string &xmlImportedDocumentBase);

  NclDocument *removeImportedDocumentBase (const string &documentId,
                                           const string &importedDocumentBaseId);

  NclDocument *addImportNCL (const string &documentId, const string &xmlImportNCL);
  NclDocument *removeImportNCL (const string &documentId, const string &documentURI);

  void processInsertedReferNode (ReferNode *referNode);
  void processInsertedComposition (CompositeNode *composition);

  Node *addNode (const string &documentId, const string &compositeId, const string &xmlNode);

  Node *removeNode (const string &documentId, const string &compositeId, const string &nodeId);

  InterfacePoint *addInterface (const string &documentId, const string &nodeId,
                                const string &xmlInterface);

  void removeInterfaceMappings (Node *node, InterfacePoint *interfacePoint,
                                CompositeNode *composition);

  void removeInterfaceLinks (Node *node, InterfacePoint *interfacePoint,
                             ContextNode *composition);

  void removeInterface (Node *node, InterfacePoint *interfacePoint);

  InterfacePoint *removeInterface (const string &documentId, const string &nodeId,
                                   const string &interfaceId);

  Link *addLink (const string &documentId, const string &compositeId, const string &xmlLink);

  void removeLink (ContextNode *composition, Link *link);

  Link *removeLink (const string &documentId, const string &compositeId, const string &linkId);

  bool setPropertyValue (const string &documentId, const string &nodeId,
                         const string &propertyId, const string &value);

public:
  void setNotifyContentUpdate (arg_unused (bool notify)){};
  void addListener (IPlayerListener *listener);
  void removeListener (IPlayerListener *listener);
  void notifyPlayerListeners (short code,
                              const string &paremeter,
                              short type,
                              const string &value);
  guint32 getMediaTime ();
  double getTotalMediaTime ();
  void setMediaTime (guint32 newTime);
  bool setKeyHandler (bool isHandler);
  void setScope (const string &scope, short type, double begin = -1,
                 double end = -1, double outTransDur = -1);

  bool play ();
  void stop ();
  void abort ();
  void pause ();
  void resume ();

  string getPropertyValue (const string &name);
  void setPropertyValue (const string &name, const string &value);

  void setReferenceTimePlayer (IPlayer *player);
  void addTimeReferPlayer (IPlayer *referPlayer);
  void removeTimeReferPlayer (IPlayer *referPlayer);
  void notifyReferPlayers (int transition);
  void timebaseObjectTransitionCallback (int transition);
  void setTimeBasePlayer (IPlayer *timeBasePlayer);
  bool isVisible ();
  void setVisible (bool visible);
  bool immediatelyStart ();
  void setImmediatelyStart (bool immediatelyStartVal);
  void forceNaturalEnd (bool forceIt);
  bool isForcedNaturalEnd ();
  bool setOutWindow (SDLWindow* windowId);

  // Channel player only.
  IPlayer *
  getSelectedPlayer ()
  {
    return NULL;
  };
  void setPlayerMap (arg_unused (map<string, IPlayer *> *objs)){};
  map<string, IPlayer *> *
  getPlayerMap ()
  {
    return NULL;
  };
  IPlayer *
  getPlayer (arg_unused (const string &objectId))
  {
    return NULL;
  };
  void select (arg_unused (IPlayer *selObject)){};

  // Application player only.
  void setCurrentScope (const string &scopeId);
  string getActiveUris (vector<string> *uris);
  string getDepUris (vector<string> *uris, int targetDev = 0);
  PresentationContext *getPresentationContext ();

private:
  string getDepUrisFromNodes (vector<string> *uris,
                              vector<Node *> *nodes,
                              int targetDev = 0);
  string getDepUriFromNode (vector<string> *uris,
                            Node *node,
                            int targetDev = 0);
  string getBaseUri (const string &baseA, const string &baseB);

public:
  void timeShift (const string &direction);
};

GINGA_FORMATTER_END

#endif /* FORMATTER_MEDIATOR_H */
