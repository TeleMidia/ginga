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

#include "GingaInternal.h"
#include "NclEvents.h"
#include "NclAction.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

#include "player/Player.h"
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN

class ExecutionObjectContext;
class ExecutionObjectSettings;

class ExecutionObject: public IGingaInternalEventListener
{
public:
  ExecutionObject (GingaInternal *, const string &, Node *);
  virtual ~ExecutionObject ();

  virtual bool isSleeping ();
  virtual bool isPaused ();
  virtual bool isOccurring ();

  Node *getNode ();
  string getId ();

  const vector <string> *getAliases ();
  bool hasAlias (const string &);
  bool addAlias (const string &);

  ExecutionObjectContext *getParent ();
  void initParent (ExecutionObjectContext *);

  const set<NclEvent *> *getEvents ();
  NclEvent *getEventById (const string &);
  bool addEvent (NclEvent *);
  PresentationEvent *getLambda ();

  virtual bool prepare (NclEvent *event);
  virtual bool start ();
  virtual bool stop ();
  virtual bool abort ();
  virtual bool pause ();
  virtual bool resume ();

protected:
  Node *_node;
  set<NclEvent *> _events;
  NclEvent *_mainEvent;

  // ------------------------------------------

public:
  bool isFocused ();
  string getProperty (const string &);
  void setProperty (const string &, const string &,
                    const string &, GingaTime);

  // From IGingaInternalEventListener.
  virtual void handleKeyEvent (const string &, bool) override;
  virtual void handleTickEvent (GingaTime, GingaTime, int) override;

protected:
  GingaInternal *_ginga;        // ginga handle
  Scheduler *_scheduler;        // scheduler

  string _id;                      // object id
  vector<string> _aliases;         // aliases
  ExecutionObjectContext *_parent; // parent object
  Player *_player;                 // associated player
  GingaTime _time;                 // playback time
  bool _destroying;                // true if object is being destroyed
};

GINGA_FORMATTER_END

#endif // EXECUTION_OBJECT_H
