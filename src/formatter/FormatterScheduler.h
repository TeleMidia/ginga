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

#ifndef FORMATTER_SCHEDULER_H
#define FORMATTER_SCHEDULER_H

#include "NclExecutionObject.h"
#include "NclNodeNesting.h"
#include "NclCompositeExecutionObject.h"

#include "NclExecutionObjectSwitch.h"
#include "NclSwitchEvent.h"

#include "NclAttributionEvent.h"
#include "INclEventListener.h"
#include "NclFormatterEvent.h"
#include "NclPresentationEvent.h"

#include "NclLinkAssignmentAction.h"
#include "NclLinkSimpleAction.h"

#include "NclFormatterLayout.h"

#include "RuleAdapter.h"
#include "PresentationContext.h"

#include "AdapterApplicationPlayer.h"

#include "AdapterFormatterPlayer.h"
#include "AdapterPlayerManager.h"

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

#include "FormatterMultiDevice.h"

GINGA_FORMATTER_BEGIN

class FormatterScheduler : public INclLinkActionListener,
                           public INclEventListener
{
private:
  RuleAdapter *ruleAdapter;
  AdapterPlayerManager *playerManager;
  PresentationContext *presContext;
  FormatterMultiDevice *multiDevPres;
  FormatterFocusManager *focusManager;
  void *compiler; // FormatterConverter*

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
  void *getFormatterLayout ();

private:
  bool isDocumentRunning (NclFormatterEvent *event);

public:
  void scheduleAction (void *action);

private:
  void runAction (NclLinkSimpleAction *action);

  void runAction (NclFormatterEvent *event, NclLinkSimpleAction *action);

  void runActionOverProperty (NclFormatterEvent *event,
                              NclLinkSimpleAction *action);

  void runActionOverApplicationObject (
      NclApplicationExecutionObject *executionObject,
      NclFormatterEvent *event, AdapterFormatterPlayer *player,
      NclLinkSimpleAction *action);

  void
  runActionOverComposition (NclCompositeExecutionObject *compositeObject,
                            NclLinkSimpleAction *action);

  void runActionOverSwitch (NclExecutionObjectSwitch *switchObject,
                            NclSwitchEvent *event,
                            NclLinkSimpleAction *action);

  void runSwitchEvent (NclExecutionObjectSwitch *switchObject,
                       NclSwitchEvent *switchEvent,
                       NclExecutionObject *selectedObject,
                       NclLinkSimpleAction *action);

  string solveImplicitRefAssessment (const string &propValue,
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
  void eventStateChanged (void *someEvent, short transition,
                          short previousState);
};

GINGA_FORMATTER_END

#endif // FORMATTER_SCHEDULER_H
