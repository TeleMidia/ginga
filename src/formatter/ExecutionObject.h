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

#include "FormatterEvents.h"
#include "NclEventTransition.h"
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
  virtual bool addEvent (NclFormatterEvent *event);
  void addPresentationEvent (NclPresentationEvent *event);
  bool containsEvent (NclFormatterEvent *event);
  NclFormatterEvent *getEventFromAnchorId (const string &anchorId);

  NclFormatterEvent *getEvent (const string &id);
  vector<NclFormatterEvent *> getEvents ();

  NclPresentationEvent *getWholeContentPresentationEvent ();
  bool removeEvent (NclFormatterEvent *event);
  bool isCompiled ();
  void setCompiled (bool status);
  vector<Node *> getNodes ();
  PropertyAnchor *getNCMProperty (const string &propertyName);
  NclNodeNesting *getNodePerspective ();
  NclNodeNesting *getNodePerspective (Node *node);
  NclFormatterEvent *getMainEvent ();

  void updateTransitionTable (GingaTime value, Player *player);
  virtual NclEventTransition *getNextTransition ();

  virtual bool prepare (NclFormatterEvent *event, GingaTime offsetTime);
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
  NclPresentationEvent *_wholeContent;

  set<string> _typeSet;
  INclLinkActionListener *_seListener;

  bool _isLocked;
  bool _isDeleting;
  bool _isHandler;
  bool _isHandling;

  map<Node *, ExecutionObjectContext *> _parentTable;
  bool _visible;
  map<string, NclFormatterEvent *> _events;
  vector<NclPresentationEvent *> _presEvents;
  set<NclSelectionEvent *> _selectionEvents;
  vector<NclFormatterEvent *> _otherEvents;

  int _pauseCount;
  NclFormatterEvent *_mainEvent;
  NclEventTransitionManager *_transMan;
  ExecutionObject *_mirrorSrc;

  static set<ExecutionObject *> _objects;

  void prepareTransitionEvents (GingaTime startTime);
  void destroyEvents ();
  virtual void unsetParentsAsListeners ();
  virtual void removeParentListenersFromEvent (NclFormatterEvent *event);

  static void addInstance (ExecutionObject *object);
  static bool removeInstance (ExecutionObject *object);

private:
  bool _isCompiled;
  map<Node *, Node *> _nodeParentTable;
};

GINGA_FORMATTER_END

#endif //_EXECUTIONOBJECT_H_
