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

#ifndef LINK_ACTION_H
#define LINK_ACTION_H

#include "NclEvents.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class NclLinkCondition;
class NclSimpleAction;

class INclActionListener
{
public:
  virtual void scheduleAction (NclSimpleAction *action) {(void) action;}
  virtual void actionProcessed (bool start) {(void) start;}
};

class NclAction
{
public:
  explicit NclAction ();
  virtual ~NclAction () {}

  void addProgressListener (INclActionListener *listener);

  virtual vector<NclEvent *> getEvents () = 0;
  virtual vector<NclAction *> getImplicitRefRoleActions () = 0;

  void run (NclLinkCondition *);

  virtual void run () = 0;

protected:
  NclLinkCondition *_satisfiedCondition;

  void notifyProgressListeners (bool start);

private:
  vector<INclActionListener *> _listeners;
};

class NclSimpleAction : public NclAction
{
public:
  NclSimpleAction (NclEvent *event, EventStateTransition type);
  virtual ~NclSimpleAction () {}

  virtual void run () override;

  NclEvent *getEvent ();
  EventStateTransition getType ();

  void setSimpleActionListener (INclActionListener *_listener);

  virtual vector<NclEvent *> getEvents () override;
  virtual vector<NclAction *> getImplicitRefRoleActions () override;

protected:
  NclEvent *_event;
  EventStateTransition _actType;

private:
  INclActionListener *_listener;
};

class NclAssignmentAction : public NclSimpleAction
{
public:
  NclAssignmentAction (NclEvent *evt,
                       EventStateTransition actType,
                       const string &value,
                       const string &duration);

  virtual ~NclAssignmentAction () {}

  string getValue ();
  string getDuration ();

private:
  string _value;
  string _duration;
};

class NclCompoundAction : public NclAction, public INclActionListener
{
public:
  NclCompoundAction ();
  virtual ~NclCompoundAction ();

  virtual void run () override;

  void addAction (NclAction *);

  void setCompoundActionListener (INclActionListener *listener);

  void getSimpleActions (vector<NclSimpleAction *> &simpleActions);
  virtual vector<NclEvent *> getEvents () override;
  void actionProcessed (bool start) override;
  virtual vector<NclAction *> getImplicitRefRoleActions () override;

protected:
  vector<NclAction *> _actions;

private:
  int _pendingActions;
  bool _hasStart;

  INclActionListener *_listener;
};

GINGA_FORMATTER_END

#endif // LINK_ACTION_H
