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

#ifndef _EXECUTIONOBJECT_H_
#define _EXECUTIONOBJECT_H_

#include "system/Thread.h"
using namespace ::ginga::system;

#include "player/Player.h"
using namespace ::ginga::player;

#include "ncl/Animation.h"
using namespace ::ginga::ncl;

#include "ncl/GenericDescriptor.h"
using namespace ::ginga::ncl;

#include "ncl/ReferenceContent.h"
using namespace ::ginga::ncl;

#include "ncl/LabeledAnchor.h"
#include "ncl/LambdaAnchor.h"
#include "ncl/IntervalAnchor.h"
#include "ncl/PropertyAnchor.h"
using namespace ::ginga::ncl;

#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "NclFormatterEvent.h"
#include "NclPresentationEvent.h"
#include "NclSelectionEvent.h"
#include "NclAttributionEvent.h"

#include "NclBeginEventTransition.h"
#include "NclEndEventTransition.h"
#include "NclEventTransition.h"
#include "NclEventTransitionManager.h"

#include "NclCascadingDescriptor.h"
#include "NclFormatterRegion.h"

#include "NclLinkSimpleAction.h"
#include "INclLinkActionListener.h"

#include "NclNodeNesting.h"



GINGA_FORMATTER_BEGIN

class NclExecutionObject
{
protected:
  string id;
  Node *dataObject;
  NclCascadingDescriptor *descriptor;
  double offsetTime;
  double startTime;
  NclPresentationEvent *wholeContent;

  set<string> typeSet;
  pthread_mutex_t mutex;
  pthread_mutex_t mutexEvent;
  pthread_mutex_t mutexParentTable;

  INclLinkActionListener *seListener;

  bool isLocked;
  bool deleting;
  bool isHandler;
  bool isHandling;

  Player* player;

private:
  map<Node *, Node *> nodeParentTable;

protected:
  map<Node *, void *> parentTable; // CompositionExecutionObject
  bool visible;

private:
  bool isItCompiled;

protected:
  map<string, NclFormatterEvent *> events;
  vector<NclPresentationEvent *> presEvents;
  set<NclSelectionEvent *> selectionEvents;
  vector<NclFormatterEvent *> otherEvents;
  int pauseCount;
  NclFormatterEvent *mainEvent;
  NclEventTransitionManager *transMan;
  NclExecutionObject *mirrorSrc;

private:
  static set<NclExecutionObject *> objects;
  static bool initMutex;
  static pthread_mutex_t _objMutex;

protected:
  static void addInstance (NclExecutionObject *object);
  static bool removeInstance (NclExecutionObject *object);

public:
  NclExecutionObject (const string &id, Node *node, bool handling,
                      INclLinkActionListener *seListener);

  NclExecutionObject (const string &id, Node *node, GenericDescriptor *descriptor,
                      bool handling, INclLinkActionListener *seListener);

  NclExecutionObject (const string &id, Node *node,
                      NclCascadingDescriptor *descriptor, bool handling,
                      INclLinkActionListener *seListener);

  virtual ~NclExecutionObject ();

  static bool hasInstance (NclExecutionObject *object, bool eraseFromList);

private:
  void initializeExecutionObject (const string &id, Node *node,
                                  NclCascadingDescriptor *descriptor,
                                  bool handling,
                                  INclLinkActionListener *seListener);

protected:
  void destroyEvents ();
  virtual void unsetParentsAsListeners ();
  virtual void removeParentListenersFromEvent (NclFormatterEvent *event);

public:
  virtual bool isSleeping ();
  virtual bool isPaused ();
  bool instanceOf (const string &s);
  int compareToUsingId (NclExecutionObject *object);
  Node *getDataObject ();
  NclCascadingDescriptor *getDescriptor ();
  string getId ();

  void notifyTimeAnchorCallBack();
  void setPlayer(Player*);

  void *getParentObject ();           // NclCompositeExecutionObject
  void *getParentObject (Node *node); // NclCompositeExecutionObject
  void addParentObject (void *parentObject, Node *parentNode);
  void addParentObject (Node *node, void *parentObject, Node *parentNode);
  virtual void removeParentObject (Node *parentNode, void *parentObject);

  void setDescriptor (NclCascadingDescriptor *cascadingDescriptor);
  void setDescriptor (GenericDescriptor *descriptor);
  virtual bool addEvent (NclFormatterEvent *event);
  void addPresentationEvent (NclPresentationEvent *event);
  int compareTo (NclExecutionObject *object);
  int compareToUsingStartTime (NclExecutionObject *object);
  bool containsEvent (NclFormatterEvent *event);
  NclFormatterEvent *getEventFromAnchorId (const string &anchorId);

public:
  NclFormatterEvent *getEvent (const string &id);
  vector<NclFormatterEvent *> *getEvents ();
  bool hasSampleEvents ();
  set<NclAnchorEvent *> *getSampleEvents ();
  double getExpectedStartTime ();
  NclPresentationEvent *getWholeContentPresentationEvent ();
  void setStartTime (double t);
  void updateEventDurations ();
  void updateEventDuration (NclPresentationEvent *event);
  bool removeEvent (NclFormatterEvent *event);
  bool isCompiled ();
  void setCompiled (bool status);
  void removeNode (Node *node);
  vector<Node *> *getNodes ();
  vector<Anchor *> *getNCMAnchors ();
  PropertyAnchor *getNCMProperty (const string &propertyName);
  NclNodeNesting *getNodePerspective ();
  NclNodeNesting *getNodePerspective (Node *node);
  vector<NclExecutionObject *> *getObjectPerspective ();
  vector<NclExecutionObject *> *getObjectPerspective (Node *node);
  vector<Node *> *getParentNodes ();
  NclFormatterEvent *getMainEvent ();
  virtual bool prepare (NclFormatterEvent *event, double offsetTime);
  virtual bool start ();

  void timeBaseNaturalEnd (int64_t timeValue, short int transType);
  void updateTransitionTable (double value, Player *player,
                              short int transType);

  void resetTransitionEvents (short int transType);
  void prepareTransitionEvents (short int transType, double startTime);
  set<double> *getTransitionsValues (short int transType);
  virtual NclEventTransition *getNextTransition ();
  virtual bool stop ();
  virtual bool abort ();
  virtual bool pause ();
  virtual bool resume ();
  virtual bool setPropertyValue (NclAttributionEvent *event, const string &value);

  virtual string getPropertyValue (const string &propName);

  virtual bool unprepare ();

  void setHandling (bool isHandling);
  void setHandler (bool isHandler);
  bool selectionEvent (SDL_Keycode key, double currentTime);

protected:
  bool lock ();
  bool unlock ();

private:
  void lockEvents ();
  void unlockEvents ();

  void lockParentTable ();
  void unlockParentTable ();
};

GINGA_FORMATTER_END

#endif //_EXECUTIONOBJECT_H_
