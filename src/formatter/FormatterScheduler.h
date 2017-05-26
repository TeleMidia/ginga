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

#include "AdapterApplicationPlayer.h"
#include "AdapterFormatterPlayer.h"
#include "AdapterPlayerManager.h"
#include "FormatterConverter.h"
#include "FormatterFocusManager.h"
#include "NclLinkAssignmentAction.h"
#include "RuleAdapter.h"

#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class FormatterConverter;

class FormatterScheduler : public INclLinkActionListener,
                           public INclEventListener
{
private:
  AdapterPlayerManager *playerManager;
  FormatterConverter *compiler;
  FormatterFocusManager *focusManager;
  NclFormatterLayout *layout;
  PresentationContext *presContext;
  RuleAdapter *ruleAdapter;

  string file;
  NclDocument *doc;
  vector<NclFormatterEvent *> documentEvents;
  map<NclFormatterEvent *, bool> documentStatus;
  set<void *> actions;

  bool running;

  pthread_mutex_t mutexD;
  pthread_mutex_t mutexActions;
  set<NclFormatterEvent *> listening;
  pthread_mutex_t lMutex;

public:
  FormatterScheduler ();
  virtual ~FormatterScheduler ();

  void addAction (void *action);
  void removeAction (void *action);

  bool setKeyHandler (bool isHandler);
  FormatterFocusManager *getFocusManager ();
  NclFormatterLayout *getFormatterLayout ();

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
  void startDocument (const string &);

public:
  void stopDocument (NclFormatterEvent *documentEvent);
  void eventStateChanged (void *someEvent, short transition,
                          short previousState);

private:
  SDLWindow*
  prepareFormatterRegion (NclExecutionObject *);
  void showObject (NclExecutionObject *);
  void hideObject (NclExecutionObject *executionObject);
};

GINGA_FORMATTER_END

#endif // FORMATTER_SCHEDULER_H
