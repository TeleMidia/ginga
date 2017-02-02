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

#ifndef _COMPOSITEEXECUTIONOBJECT_H_
#define _COMPOSITEEXECUTIONOBJECT_H_

#include "ncl/Node.h"
#include "ncl/ContextNode.h"
using namespace ::ginga::ncl;

#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "ncl/Link.h"
using namespace ::ginga::ncl;

#include "INclEventListener.h"
#include "NclFormatterEvent.h"
#include "NclPresentationEvent.h"

#include "NclFormatterCausalLink.h"
#include "NclFormatterLink.h"
#include "NclLinkListener.h"

#include "NclNodeNesting.h"
#include "NclCascadingDescriptor.h"
#include "NclExecutionObject.h"

GINGA_FORMATTER_BEGIN

class NclCompositeExecutionObject : public NclExecutionObject,
                                    public NclLinkListener,
                                    public INclEventListener,
                                    public Thread
{
private:
  static const short mSleepTime = 800;
  set<NclFormatterLink *> links;
  set<Link *> uncompiledLinks;

  set<NclFormatterEvent *> runningEvents; // child events occurring
  set<NclFormatterEvent *> pausedEvents;  // child events paused
  short lastTransition;

  map<NclFormatterLink *, int> pendingLinks;
  bool running;

  map<string, NclExecutionObject *> execObjList;

  pthread_mutex_t compositeMutex;
  //pthread_mutex_t parentMutex;
  pthread_mutex_t stlMutex;

public:
  NclCompositeExecutionObject (const string &id, Node *dataObject, bool handling,
                               INclLinkActionListener *seListener);

  NclCompositeExecutionObject (const string &id, Node *dataObject,
                               NclCascadingDescriptor *descriptor,
                               bool handling,
                               INclLinkActionListener *seListener);

  virtual ~NclCompositeExecutionObject ();

protected:
  void
  initializeCompositeExecutionObject (const string &id, Node *dataObject,
                                      NclCascadingDescriptor *descriptor);

public:
  NclCompositeExecutionObject *getParentFromDataObject (Node *dataObject);
  void suspendLinkEvaluation (bool suspend);
  bool addExecutionObject (NclExecutionObject *execObj);
  bool containsExecutionObject (const string &execObjId);
  NclExecutionObject *getExecutionObject (const string &execObjId);
  map<string, NclExecutionObject *> *getExecutionObjects ();
  map<string, NclExecutionObject *> *recursivellyGetExecutionObjects ();
  int getNumExecutionObjects ();
  bool removeExecutionObject (NclExecutionObject *execObj);
  set<Link *> *getUncompiledLinks ();
  bool containsUncompiledLink (Link *dataLink);
  void removeLinkUncompiled (Link *ncmLink);
  void setLinkCompiled (NclFormatterLink *formatterLink);
  void addNcmLink (Link *ncmLink);
  void removeNcmLink (Link *ncmLink);
  void setAllLinksAsUncompiled (bool isRecursive);
  void setParentsAsListeners ();
  void unsetParentsAsListeners ();
  void eventStateChanged (void *event, short transition,
                          short previousState);

  short getPriorityType ();

  void linkEvaluationStarted (NclFormatterCausalLink *link);
  void linkEvaluationFinished (NclFormatterCausalLink *link, bool start);

  bool setPropertyValue (NclAttributionEvent *event, const string &value);

private:
  void checkLinkConditions ();
  void run ();
  void listRunningObjects ();
  void listPendingLinks ();

  void lockComposite ();
  void unlockComposite ();

  void lockSTL ();
  void unlockSTL ();
};

GINGA_FORMATTER_END

#endif //_COMPOSITEEXECUTIONOBJECT_H_
