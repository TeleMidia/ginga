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

#ifndef EXECUTION_OBJECT_H
#define EXECUTION_OBJECT_H

#include "NclEvents.h"
#include "NclEventTransitionManager.h"
#include "NclActions.h"
#include "NclNodeNesting.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

#include "player/Player.h"
using namespace ::ginga::player;

#include "mb/IEventListener.h"
using namespace ::ginga::mb;

GINGA_FORMATTER_BEGIN

class ExecutionObjectContext;
class ExecutionObjectSettings;

class ExecutionObject : public IEventListener
{
public:
  ExecutionObject (const string &, Node *, INclActionListener *);
  virtual ~ExecutionObject ();

  virtual bool isSleeping ();
  virtual bool isPaused ();
  virtual bool isOccurring ();

  Node *getNode ();
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

  virtual bool addEvent (NclEvent *event);
  void addPresentationEvent (PresentationEvent *event);
  bool containsEvent (NclEvent *event);

  NclEvent *getEvent (const string &id);
  vector<NclEvent *> getEvents ();

  PresentationEvent *getWholeContentPresentationEvent ();
  bool removeEvent (NclEvent *event);
  bool isCompiled ();
  void setCompiled (bool status);
  vector<Node *> getNodes ();
  Property *getNCMProperty (const string &propertyName);
  NclNodeNesting *getNodePerspective ();
  NclNodeNesting *getNodePerspective (Node *node);
  NclEvent *getMainEvent ();

  virtual bool prepare (NclEvent *event);
  virtual bool start ();
  virtual bool stop ();
  virtual bool abort ();
  virtual bool pause ();
  virtual bool resume ();

protected:
  Node *_node;
  PresentationEvent *_wholeContent;
  INclActionListener *_seListener;

  map<Node *, ExecutionObjectContext *> _parentTable;
  map<string, NclEvent *> _events;
  vector<PresentationEvent *> _presEvents;
  set<SelectionEvent *> _selectionEvents;
  vector<NclEvent *> _otherEvents;

  NclEvent *_mainEvent;
  NclEventTransitionManager _transMan;

  void destroyEvents ();
  virtual void unsetParentsAsListeners ();
  virtual void removeParentListenersFromEvent (NclEvent *event);

private:
  bool _isCompiled;
  map<Node *, Node *> _nodeParentTable;

  // ------------------------------------------

public:
  static ExecutionObjectSettings *getSettings ();
  static void setSettings (ExecutionObjectSettings *);

  bool isFocused ();
  string getProperty (const string &);
  void setProperty (const string &, const string &,
                    const string &, GingaTime);

  // From IEventListener.
  virtual void handleKeyEvent (const string &, bool) override;
  virtual void handleTickEvent (GingaTime, GingaTime, int) override;

protected:
  static ExecutionObjectSettings *_settings; // settings object
  static set<ExecutionObject *> _objects;    // set of all objects

  string _id;                   // object id
  Player *_player;              // associated player
  GingaTime _time;              // playback time
};

GINGA_FORMATTER_END

#endif // EXECUTION_OBJECT_H
