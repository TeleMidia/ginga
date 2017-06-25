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
class NclSimpleAction;

class NclActionProgressListener
{
public:
  virtual ~NclActionProgressListener (){}
  virtual void actionProcessed (bool start) = 0;
};

class INclActionListener
{
public:
  virtual void scheduleAction (NclSimpleAction *action) = 0;
  virtual void addAction (NclSimpleAction *action) = 0;
  virtual void removeAction (NclSimpleAction *action) = 0;
};

class NclAction
{
public:
  explicit NclAction (GingaTime _delay);

  virtual ~NclAction () {}
  void setWaitDelay (GingaTime delay);

  void addProgressListener (NclActionProgressListener *listener);
  void removeProgressListener (NclActionProgressListener *listener);

  virtual vector<NclEvent *> getEvents () = 0;
  virtual vector<NclAction *> getImplicitRefRoleActions () = 0;

  void setSatisfiedCondition (NclLinkCondition *satisfiedCondition);
  void run (NclLinkCondition *satisfiedCondition);

  virtual void run () {}

protected:
  NclLinkCondition *_satisfiedCondition;

  void notifyProgressListeners (bool start);

private:
  GingaTime _delay;
  vector<NclActionProgressListener *> _progressListeners;
};

class NclSimpleAction : public NclAction
{
public:
  NclSimpleAction (NclEvent *event, SimpleActionType type);
  virtual ~NclSimpleAction ();

  virtual void run () override;

  NclEvent *getEvent ();
  SimpleActionType getType ();
  string getTypeString ();

  void setSimpleActionListener (INclActionListener *listener);

  virtual vector<NclEvent *> getEvents () override;
  virtual vector<NclAction *> getImplicitRefRoleActions () override;

protected:
  NclEvent *_event;
  SimpleActionType _actType;

private:
  INclActionListener *listener;
};

class NclRepeatAction : public NclSimpleAction
{
public:
  NclRepeatAction (NclEvent *, SimpleActionType);
  virtual ~NclRepeatAction ();

  virtual void run () override;

  void setRepetitions (int repetitions);
  void setRepetitionInterval (GingaTime _delay);

private:
  int _repetitions;
  GingaTime _repetitionInterval;
};

class NclAssignmentAction : public NclRepeatAction
{
public:
  NclAssignmentAction (NclEvent *evt,
                       SimpleActionType actType,
                       const string &value);

  virtual ~NclAssignmentAction ();

  string getValue ();
  Animation *getAnimation ();
  void setAnimation (Animation *anim);

private:
  string _value;
  Animation *_anim;
};

class NclCompoundAction : public NclAction,
    public NclActionProgressListener
{
public:
  NclCompoundAction (short op);
  virtual ~NclCompoundAction ();

  virtual void run () override;

  void addAction (NclAction *action);

  void getSimpleActions (vector<NclSimpleAction *> *simpleActions);
  void setCompoundActionListener (INclActionListener *listener);

  virtual vector<NclEvent *> getEvents () override;
  void actionProcessed (bool start) override;
  virtual vector<NclAction *> getImplicitRefRoleActions () override;

protected:
  vector<NclAction *> _actions;
  short _op;

private:
  int _pendingActions;
  bool _hasStart;
  bool _running;

  INclActionListener *_listener;
};

GINGA_FORMATTER_END

#endif //_LINKACTION_H_
