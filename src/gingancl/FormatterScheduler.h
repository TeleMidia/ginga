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

#ifndef _FORMATTERSCHEDULER_H_
#define _FORMATTERSCHEDULER_H_

#include "ctxmgmt/IContextListener.h"
using namespace ::ginga::ctxmgmt;

#include "NclExecutionObject.h"
#include "NclNodeNesting.h"
#include "NclCompositeExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "NclExecutionObjectSwitch.h"
#include "NclSwitchEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::switches;

#include "NclAttributionEvent.h"
#include "INclEventListener.h"
#include "NclFormatterEvent.h"
#include "NclPresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "NclLinkAssignmentAction.h"
#include "NclLinkSimpleAction.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "NclFormatterLayout.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "RuleAdapter.h"
#include "PresentationContext.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adaptation::context;

#include "AdapterApplicationPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::application;

#include "AdapterFormatterPlayer.h"
#include "AdapterPlayerManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "ncl/SimpleAction.h"
#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "ncl/CompositeNode.h"
#include "ncl/ContentNode.h"
#include "ncl/Node.h"
#include "ncl/NodeEntity.h"
using namespace ::ginga::ncl;

#include "ncl/Port.h"
#include "ncl/Anchor.h"
#include "ncl/ContentAnchor.h"
#include "ncl/PropertyAnchor.h"
#include "ncl/SwitchPort.h"
using namespace ::ginga::ncl;

#include "ncl/ReferNode.h"
using namespace ::ginga::ncl;

#include "FormatterFocusManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::focus;

#include "FormatterMultiDevice.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::multidevice;

#include "IFormatterSchedulerListener.h"
#include "ObjectCreationForbiddenException.h"

#include "AnimationController.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::animation;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_BEGIN

class FormatterScheduler : public INclLinkActionListener,
                           public INclEventListener,
                           public IContextListener
{

private:
  RuleAdapter *ruleAdapter;
  AdapterPlayerManager *playerManager;
  PresentationContext *presContext;
  FormatterMultiDevice *multiDevPres;
  FormatterFocusManager *focusManager;

  void *compiler; // FormatterConverter*
  vector<IFormatterSchedulerListener *> schedulerListeners;
  vector<NclFormatterEvent *> documentEvents;
  map<NclFormatterEvent *, bool> documentStatus;
  set<void *> actions;

  bool running;

  set<string> typeSet;
  pthread_mutex_t mutexD;
  pthread_mutex_t mutexActions;

  set<NclFormatterEvent *> listening;
  pthread_mutex_t lMutex;

public:
  FormatterScheduler (AdapterPlayerManager *playerManager,
                      RuleAdapter *ruleAdapter,
                      FormatterMultiDevice *multiDevice,
                      void *compiler); // FormatterConverter

  virtual ~FormatterScheduler ();

  void addAction (void *action);
  void removeAction (void *action);

  bool setKeyHandler (bool isHandler);
  FormatterFocusManager *getFocusManager ();
  void *getFormatterLayout (void *descriptor, void *object);

private:
  bool isDocumentRunning (NclFormatterEvent *event);

  void setTimeBaseObject (NclExecutionObject *object,
                          AdapterFormatterPlayer *objectPlayer,
                          string nodeId);

  static void printAction (string action, NclLinkCondition *condition,
                           NclLinkSimpleAction *linkAction);

public:
  void scheduleAction (void *condition, void *action);

private:
  void runAction (NclLinkCondition *condition, NclLinkSimpleAction *action);

  void runAction (NclFormatterEvent *event, NclLinkCondition *condition,
                  NclLinkSimpleAction *action);

  void runActionOverProperty (NclFormatterEvent *event,
                              NclLinkSimpleAction *action);

  void runActionOverApplicationObject (
      NclApplicationExecutionObject *executionObject, NclFormatterEvent *event,
      AdapterFormatterPlayer *player, NclLinkSimpleAction *action);

  void runActionOverComposition (NclCompositeExecutionObject *compositeObject,
                                 NclLinkSimpleAction *action);

  void runActionOverSwitch (NclExecutionObjectSwitch *switchObject,
                            NclSwitchEvent *event, NclLinkSimpleAction *action);

  void runSwitchEvent (NclExecutionObjectSwitch *switchObject,
                       NclSwitchEvent *switchEvent,
                       NclExecutionObject *selectedObject,
                       NclLinkSimpleAction *action);

  string solveImplicitRefAssessment (string propValue,
                                     NclAttributionEvent *event);

public:
  void startEvent (NclFormatterEvent *event);
  void stopEvent (NclFormatterEvent *event);
  void pauseEvent (NclFormatterEvent *event);
  void resumeEvent (NclFormatterEvent *event);

private:
  void initializeDefaultSettings ();
  void initializeDocumentSettings (Node *node);

public:
  void startDocument (NclFormatterEvent *documentEvent,
                      vector<NclFormatterEvent *> *entryEvents);

private:
  void removeDocument (NclFormatterEvent *documentEvent);

public:
  void stopDocument (NclFormatterEvent *documentEvent);
  void pauseDocument (NclFormatterEvent *documentEvent);
  void resumeDocument (NclFormatterEvent *documentEvent);
  void stopAllDocuments ();
  void pauseAllDocuments ();
  void resumeAllDocuments ();
  void eventStateChanged (void *someEvent, short transition,
                          short previousState);

  short getPriorityType ();
  void addSchedulerListener (IFormatterSchedulerListener *listener);
  void removeSchedulerListener (IFormatterSchedulerListener *listener);
  void receiveGlobalAttribution (string propertyName, string value);
};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_END
#endif //_FORMATTERSCHEDULER_H_
