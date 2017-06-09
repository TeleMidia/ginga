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

#ifndef _APPLICATIONEXECUTIONOBJECT_H_
#define _APPLICATIONEXECUTIONOBJECT_H_

#include "NclCompositeExecutionObject.h"
#include "NclExecutionObject.h"

GINGA_FORMATTER_BEGIN

class NclApplicationExecutionObject : public NclExecutionObject
{
private:
  map<string, NclFormatterEvent *> preparedEvents;
  NclFormatterEvent *currentEvent;

public:
  NclApplicationExecutionObject (const string &id, Node *node, bool handling,
                                 INclLinkActionListener *seListener);

  NclApplicationExecutionObject (const string &id, Node *node,
                                 GenericDescriptor *descriptor,
                                 bool handling,
                                 INclLinkActionListener *seListener);

  NclApplicationExecutionObject (const string &id, Node *node,
                                 NclCascadingDescriptor *descriptor,
                                 bool handling,
                                 INclLinkActionListener *seListener);

  virtual ~NclApplicationExecutionObject ();

private:
  void initializeApplicationObject ();

public:
  bool isSleeping ();
  bool isPaused ();
  NclFormatterEvent *getCurrentEvent ();
  bool hasPreparedEvent (NclFormatterEvent *event);
  void setCurrentEvent (NclFormatterEvent *event);
  bool prepare (NclFormatterEvent *event, GingaTime offsetTime);
  bool start ();
  NclEventTransition *getNextTransition ();
  bool stop ();
  bool abort ();
  bool pause ();
  bool resume ();
  bool unprepare ();

private:
  void unprepareEvents ();
  void removeEventListeners ();
  void removeParentObject (Node *parentNode, NclCompositeExecutionObject *parentObject);
  void removeParentListenersFromEvent (NclFormatterEvent *event);
  void lockEvents ();
  void unlockEvents ();
};

GINGA_FORMATTER_END

#endif //_APPLICATIONEXECUTIONOBJECT_H_
