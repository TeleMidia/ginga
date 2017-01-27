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

#include "ObjectCreationForbiddenException.h"
#include "FormatterConverter.h"

#include "PrefetchManager.h"

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
  EntryEventListener (Player *player, string interfaceId);
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
  static PrefetchManager *pm;
  map<string, NclFormatterEvent *> documentEvents;
  map<string, vector<NclFormatterEvent *> *> documentEntryEvents;
  map<Port *, NclFormatterEvent *> portsToEntryEvents;
  FormatterScheduler *scheduler;
  RuleAdapter *ruleAdapter;
  FormatterConverter *compiler;
  AdapterPlayerManager *playerManager;
  ITimeBaseProvider *timeBaseProvider;
  vector<string> compileErrors;
  DeviceLayout *deviceLayout;
  FormatterMultiDevice *multiDevice;
  PresentationContext *presContext;
  bool isEmbedded;
  bool enablePrefetch;
  bool docCompiled;
  pthread_mutex_t pteMutex;

public:
  FormatterMediator (NclPlayerData *data);
  virtual ~FormatterMediator ();

  static void release ();

  void setMirrorSrc (IPlayer *mirrorSrc);
  void printGingaWindows ();
  set<string> *createPortIdList ();
  short getMappedInterfaceType (string portId);

  void setMrl (string mrl, bool visible = true);
  void reset (){};
  void rebase (){};
  void printData (NclPlayerData *data);
  void setTimeBaseProvider (ITimeBaseProvider *timeBaseProvider);

  void setParentLayout (void *parentLayout);
  string getScreenShot ();

  vector<string> *getCompileErrors ();
  void *setCurrentDocument (string fName);

private:
  virtual void *addDocument (string fName);
  bool removeDocument (string documentId);
  ContextNode *getDocumentContext (string documentId);

public:
  void setDepthLevel (int level);
  int getDepthLevel ();

  Port *getPortFromEvent (NclFormatterEvent *event);

private:
  vector<Port *> *getContextPorts (ContextNode *context,
                                   string interfaceId);

  vector<NclFormatterEvent *> *processDocument (string documentId,
                                                string interfaceId);

  void initializeSettingNodes (Node *node);

  vector<NclFormatterEvent *> *getDocumentEntryEvent (string documentId);

  bool compileDocument (string documentId);
  bool prepareDocument (string documentId);

  void solveRemoteDescriptorsUris (string docLocation,
                                   vector<GenericDescriptor *> *descs,
                                   bool isRemoteDoc);

  void solveRemoteNodesUris (string docLocation, vector<Node *> *nodes,
                             bool isRemoteDoc);

  void solveRemoteNclDeps (string docLocation, bool isRemoteDoc);

  void solveRemoteLuaDeps (string docLocation, string src,
                           bool isRemoteDoc);

  string solveRemoteSourceUri (string docLocation, string src);

  NclFormatterEvent *getEntryEvent (string interfaceId,
                                    vector<NclFormatterEvent *> *events);

  bool startDocument (string documentId, string interfaceId);
  bool stopDocument (string documentId);
  bool pauseDocument (string documentId);
  bool resumeDocument (string documentId);
  void presentationCompleted (NclFormatterEvent *documentEvent);

public:
  bool nclEdit (string nclEditApi);
  bool editingCommand (string commandTag, string privateDataPayload);

private:
  LayoutRegion *addRegion (string documentId, string regionBaseId,
                           string regionId, string xmlRegion);

  LayoutRegion *removeRegion (string documentId, string regionBaseId,
                              string regionId);

  RegionBase *addRegionBase (string documentId, string xmlRegionBase);
  RegionBase *removeRegionBase (string documentId, string regionBaseId);

  Rule *addRule (string documentId, string xmlRule);
  Rule *removeRule (string documentId, string ruleId);
  RuleBase *addRuleBase (string documentId, string xmlRuleBase);
  RuleBase *removeRuleBase (string documentId, string ruleBaseId);
  Transition *addTransition (string documentId, string xmlTransition);
  Transition *removeTransition (string documentId, string transitionId);

  TransitionBase *addTransitionBase (string documentId,
                                     string xmlTransitionBase);

  TransitionBase *removeTransitionBase (string documentId,
                                        string transitionBaseId);

  Connector *addConnector (string documentId, string xmlConnector);
  Connector *removeConnector (string documentId, string connectorId);
  ConnectorBase *addConnectorBase (string documentId,
                                   string xmlConnectorBase);

  ConnectorBase *removeConnectorBase (string documentId,
                                      string connectorBaseId);

  GenericDescriptor *addDescriptor (string documentId,
                                    string xmlDescriptor);

  GenericDescriptor *removeDescriptor (string documentId,
                                       string descriptorId);

  DescriptorBase *addDescriptorBase (string documentId,
                                     string xmlDescriptorBase);

  DescriptorBase *removeDescriptorBase (string documentId,
                                        string descriptorBaseId);

  Base *addImportBase (string documentId, string docBaseId,
                       string xmlImportBase);

  Base *removeImportBase (string documentId, string docBaseId,
                          string documentURI);

  NclDocument *addImportedDocumentBase (string documentId,
                                        string xmlImportedDocumentBase);

  NclDocument *removeImportedDocumentBase (string documentId,
                                           string importedDocumentBaseId);

  NclDocument *addImportNCL (string documentId, string xmlImportNCL);
  NclDocument *removeImportNCL (string documentId, string documentURI);

  void processInsertedReferNode (ReferNode *referNode);
  void processInsertedComposition (CompositeNode *composition);

  Node *addNode (string documentId, string compositeId, string xmlNode);

  Node *removeNode (string documentId, string compositeId, string nodeId);

  InterfacePoint *addInterface (string documentId, string nodeId,
                                string xmlInterface);

  void removeInterfaceMappings (Node *node, InterfacePoint *interfacePoint,
                                CompositeNode *composition);

  void removeInterfaceLinks (Node *node, InterfacePoint *interfacePoint,
                             ContextNode *composition);

  void removeInterface (Node *node, InterfacePoint *interfacePoint);

  InterfacePoint *removeInterface (string documentId, string nodeId,
                                   string interfaceId);

  Link *addLink (string documentId, string compositeId, string xmlLink);

  void removeLink (ContextNode *composition, Link *link);

  Link *removeLink (string documentId, string compositeId, string linkId);

  bool setPropertyValue (string documentId, string nodeId,
                         string propertyId, string value);

public:
  void setNotifyContentUpdate (arg_unused (bool notify)){};
  void addListener (IPlayerListener *listener);
  void removeListener (IPlayerListener *listener);
  void notifyPlayerListeners (short code, string paremeter, short type,
                              string value);
  void setSurface (SDLSurface* surface);
  SDLSurface* getSurface ();
  void flip ();
  double getMediaTime ();
  double getTotalMediaTime ();
  void setMediaTime (double newTime);
  bool setKeyHandler (bool isHandler);
  void setScope (string scope, short type, double begin = -1,
                 double end = -1, double outTransDur = -1);

  bool play ();
  void stop ();
  void abort ();
  void pause ();
  void resume ();

  string getPropertyValue (string const &name);
  void setPropertyValue (const string &name, const string &value);

  void setReferenceTimePlayer (IPlayer *player);
  void addTimeReferPlayer (IPlayer *referPlayer);
  void removeTimeReferPlayer (IPlayer *referPlayer);
  void notifyReferPlayers (int transition);
  void timebaseObjectTransitionCallback (int transition);
  void setTimeBasePlayer (IPlayer *timeBasePlayer);
  bool hasPresented ();
  void setPresented (bool presented);
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
  getPlayer (arg_unused (string objectId))
  {
    return NULL;
  };
  void select (arg_unused (IPlayer *selObject)){};

  // Application player only.
  void setCurrentScope (string scopeId);
  string getActiveUris (vector<string> *uris);
  string getDepUris (vector<string> *uris, int targetDev = 0);
  PresentationContext *getPresentationContext ();

private:
  string getDepUrisFromNodes (vector<string> *uris, vector<Node *> *nodes,
                              int targetDev = 0);
  string getDepUriFromNode (vector<string> *uris, Node *node,
                            int targetDev = 0);
  string getBaseUri (string baseA, string baseB);

public:
  void timeShift (string direction);
};

GINGA_FORMATTER_END

#endif /* FORMATTER_MEDIATOR_H */
