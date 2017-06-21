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

#include "player/Player.h"
using namespace ::ginga::player;

#include "ncl/Animation.h"
#include "ncl/GenericDescriptor.h"
#include "ncl/ReferenceContent.h"
#include "ncl/LabeledAnchor.h"
#include "ncl/LambdaAnchor.h"
#include "ncl/IntervalAnchor.h"
#include "ncl/PropertyAnchor.h"
#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "NclEvents.h"
#include "NclEventTransitionManager.h"
#include "NclCascadingDescriptor.h"
#include "NclFormatterRegion.h"
#include "NclLinkSimpleAction.h"
#include "INclLinkActionListener.h"
#include "NclNodeNesting.h"

GINGA_FORMATTER_BEGIN

class ExecutionObjectContext;

class ExecutionObject
{
public:
  ExecutionObject (const string &, Node *, NclCascadingDescriptor *, bool,
                   INclLinkActionListener *);

  virtual ~ExecutionObject ();

  static bool hasInstance (ExecutionObject *object, bool eraseFromList);

  virtual bool isSleeping ();
  virtual bool isPaused ();
  bool instanceOf (const string &);
  Node *getDataObject ();
  NclCascadingDescriptor *getDescriptor ();
  string getId ();

  ExecutionObjectContext *getParentObject ();
  ExecutionObjectContext *getParentObject (Node *node);
  void addParentObject (ExecutionObjectContext *parentObject,
                        Node *parentNode);
  void addParentObject (Node *node,
                        ExecutionObjectContext *parentObject,
                        Node *parentNode);
  virtual void removeParentObject (Node *parentNode,
                                   ExecutionObjectContext *parentObject);

  void setDescriptor (NclCascadingDescriptor *cascadingDescriptor);
  void setDescriptor (GenericDescriptor *_descriptor);
  virtual bool addEvent (NclEvent *event);
  void addPresentationEvent (PresentationEvent *event);
  bool containsEvent (NclEvent *event);
  NclEvent *getEventFromAnchorId (const string &anchorId);

  NclEvent *getEvent (const string &id);
  vector<NclEvent *> getEvents ();

  PresentationEvent *getWholeContentPresentationEvent ();
  bool removeEvent (NclEvent *event);
  bool isCompiled ();
  void setCompiled (bool status);
  vector<Node *> getNodes ();
  PropertyAnchor *getNCMProperty (const string &propertyName);
  NclNodeNesting *getNodePerspective ();
  NclNodeNesting *getNodePerspective (Node *node);
  NclEvent *getMainEvent ();

  void updateTransitionTable (GingaTime value, Player *player);
  virtual EventTransition *getNextTransition ();

  virtual bool prepare (NclEvent *event, GingaTime offsetTime);
  virtual bool start ();
  virtual bool stop ();
  virtual bool abort ();
  virtual bool pause ();
  virtual bool resume ();

  virtual bool unprepare ();

  void setHandling (bool isHandling);
  void setHandler (bool isHandler);
  bool selectionEvent (SDL_Keycode key, GingaTime currentTime);

protected:
  string _id;
  Node *_dataObject;
  NclCascadingDescriptor *_descriptor;
  GingaTime _offsetTime;
  PresentationEvent *_wholeContent;

  set<string> _typeSet;
  INclLinkActionListener *_seListener;

  bool _isLocked;
  bool _isDeleting;
  bool _isHandler;
  bool _isHandling;

  map<Node *, ExecutionObjectContext *> _parentTable;
  map<string, NclEvent *> _events;
  vector<PresentationEvent *> _presEvents;
  set<SelectionEvent *> _selectionEvents;
  vector<NclEvent *> _otherEvents;

  int _pauseCount;
  NclEvent *_mainEvent;
  NclEventTransitionManager _transMan;
  ExecutionObject *_mirrorSrc;

  static set<ExecutionObject *> _objects;

  void prepareTransitionEvents (GingaTime startTime);
  void destroyEvents ();
  virtual void unsetParentsAsListeners ();
  virtual void removeParentListenersFromEvent (NclEvent *event);

  static void addInstance (ExecutionObject *object);
  static bool removeInstance (ExecutionObject *object);

private:
  bool _isCompiled;
  map<Node *, Node *> _nodeParentTable;
};

GINGA_FORMATTER_END

#endif //_EXECUTIONOBJECT_H_
