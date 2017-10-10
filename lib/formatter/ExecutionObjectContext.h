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

#ifndef EXECUTION_OBJECT_CONTEXT_H
#define EXECUTION_OBJECT_CONTEXT_H

#include "ExecutionObject.h"
#include "NclEvents.h"
#include "NclLink.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class ExecutionObjectContext: public ExecutionObject,
                              public INclEventListener
{
public:
  ExecutionObjectContext (GingaInternal *, const string &,
                          Node *,
                          INclActionListener *);

  virtual ~ExecutionObjectContext ();

  void suspendLinkEvaluation (bool suspend);
  set<Link *> *getUncompiledLinks ();
  bool containsUncompiledLink (Link *dataLink);
  void removeLinkUncompiled (Link *ncmLink);
  void setLinkCompiled (NclLink *formatterLink);
  void eventStateChanged (NclEvent *event,
                          EventStateTransition transition,
                          EventState previousState) override;

  // Sanity.
  const set<ExecutionObject *> *getChildren ();
  ExecutionObject *getChildById (const string &);
  bool addChild (ExecutionObject *);

  // Callbacks
  void linkEvaluationStarted (NclLink *);
  void linkEvaluationFinished (NclLink *);

private:
  void checkLinkConditions ();

private:
  set<ExecutionObject *> _children;

  set<NclLink *> _links;
  set<Link *> _uncompiledLinks;
  set<NclEvent *> _runningEvents; // child events occurring
  set<NclEvent *> _pausedEvents;  // child events paused
  EventStateTransition lastTransition;
  map<NclLink *, int> _pendingLinks;

};

GINGA_FORMATTER_END

#endif // EXECUTION_OBJECT_CONTEXT_H
