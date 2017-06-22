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

#ifndef _LINKACTION_H_
#define _LINKACTION_H_

#include "NclEvents.h"
#include "ncl/Action.h"
#include "ncl/SimpleAction.h"

GINGA_FORMATTER_BEGIN

class NclLinkCondition;
class NclLinkSimpleAction;

class NclLinkActionProgressListener
{
public:
  virtual ~NclLinkActionProgressListener (){}
  virtual void actionProcessed (bool start) = 0;
};

class INclLinkActionListener
{
public:
  virtual void scheduleAction (NclLinkSimpleAction *action) = 0;
  virtual void addAction (NclLinkSimpleAction *action) = 0;
  virtual void removeAction (NclLinkSimpleAction *action) = 0;
};

class NclLinkAction
{
public:
  NclLinkAction ();
  NclLinkAction (GingaTime delay);

  virtual ~NclLinkAction () {}
  bool instanceOf (const string &s);
  void setWaitDelay (GingaTime delay);

  void
  addActionProgressionListener (NclLinkActionProgressListener *listener);

  void removeActionProgressionListener (
      NclLinkActionProgressListener *listener);

  void notifyProgressionListeners (bool start);
  virtual vector<NclEvent *> *getEvents () = 0;
  virtual vector<NclLinkAction *> getImplicitRefRoleActions () = 0;

  void setSatisfiedCondition (NclLinkCondition *satisfiedCondition);
  void run (NclLinkCondition *satisfiedCondition);

  virtual void run () {}

protected:
  set<string> typeSet;
  NclLinkCondition *satisfiedCondition;

private:
  GingaTime delay;
  vector<NclLinkActionProgressListener *> progressListeners;

  void initLinkAction (GingaTime delay);
};

class NclLinkSimpleAction : public NclLinkAction
{
public:
  NclLinkSimpleAction (NclEvent *event, SimpleActionType type);
  virtual ~NclLinkSimpleAction ();

  virtual void run ();

  NclEvent *getEvent ();
  SimpleActionType getType ();
  string getTypeString ();
  void setSimpleActionListener (INclLinkActionListener *listener);
  virtual vector<NclEvent *> *getEvents ();
  virtual vector<NclLinkAction *> getImplicitRefRoleActions ();

protected:
  NclEvent *event;
  SimpleActionType actionType;

private:
  INclLinkActionListener *listener;
};

class NclLinkRepeatAction : public NclLinkSimpleAction
{
public:
  NclLinkRepeatAction (NclEvent *, SimpleActionType);
  virtual ~NclLinkRepeatAction ();

  virtual void run () override;

  void setRepetitions (int repetitions);
  void setRepetitionInterval (GingaTime delay);

private:
  int repetitions;
  GingaTime repetitionInterval;
};

class NclLinkAssignmentAction : public NclLinkRepeatAction
{
private:
  string value;
  Animation *animation;

public:
  NclLinkAssignmentAction (NclEvent *event,
                           SimpleActionType actionType,
                           const string &value);

  virtual ~NclLinkAssignmentAction ();

  string getValue ();
  Animation *getAnimation ();
  void setAnimation (Animation *animation);
};

class NclLinkCompoundAction : public NclLinkAction,
                              public NclLinkActionProgressListener
{
public:
  NclLinkCompoundAction (short _op);
  virtual ~NclLinkCompoundAction ();

  virtual void run () override;

  void addAction (NclLinkAction *action);

  void getSimpleActions (vector<NclLinkSimpleAction *> *simpleActions);
  void setCompoundActionListener (INclLinkActionListener *listener);

  virtual vector<NclEvent *> *getEvents () override;
  void actionProcessed (bool start) override;
  virtual vector<NclLinkAction *> getImplicitRefRoleActions () override;

protected:
  vector<NclLinkAction *> _actions;
  short _op;

private:
  int _pendingActions;
  bool _hasStart;
  bool _running;

  INclLinkActionListener *_listener;
};

GINGA_FORMATTER_END

#endif //_LINKACTION_H_
