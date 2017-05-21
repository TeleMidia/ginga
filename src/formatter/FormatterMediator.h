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

class FormatterMediator : public Player,
                          public IFormatterSchedulerListener
{
private:
  EntryEventListener *entryEventListener;
  NclPlayerData *data;
  string currentFile;
  NclDocument *currentDocument;

  map<string, NclFormatterEvent *> documentEvents;
  map<string, vector<NclFormatterEvent *> *> documentEntryEvents;
  map<Port *, NclFormatterEvent *> portsToEntryEvents;
  FormatterScheduler *scheduler;
  RuleAdapter *ruleAdapter;
  FormatterConverter *compiler;
  AdapterPlayerManager *playerManager;
  DeviceLayout *deviceLayout;
  FormatterMultiDevice *multiDevice;
  PresentationContext *presContext;
  bool docCompiled;
  pthread_mutex_t pteMutex;

public:
  FormatterMediator ();
  ~FormatterMediator ();
  void *addDocument (const string &file);

private:
  bool removeDocument ();
  ContextNode *getDocumentContext (const string &documentId);

public:
  Port *getPortFromEvent (NclFormatterEvent *event);

private:
  vector<Port *> *getContextPorts (ContextNode *context,
                                   const string &interfaceId);

  vector<NclFormatterEvent *> *processDocument (const string &documentId,
                                                const string &interfaceId);

  void initializeSettingNodes (Node *node);

  vector<NclFormatterEvent *> *getDocumentEntryEvent (const string &documentId);

  bool compileDocument (const string &documentId);

  NclFormatterEvent *getEntryEvent (const string &interfaceId,
                                    vector<NclFormatterEvent *> *events);

  void presentationCompleted (NclFormatterEvent *documentEvent);

public:
  bool play ();
  void stop ();
  void abort ();
  void pause ();
  void resume ();
};

GINGA_FORMATTER_END

#endif /* FORMATTER_MEDIATOR_H */
