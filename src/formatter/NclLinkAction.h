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

#include "NclFormatterEvent.h"
#include "NclAttributionEvent.h"
#include "NclLinkActionProgressionListener.h"

GINGA_FORMATTER_BEGIN

class NclLinkCondition;

class NclLinkAction
{
protected:
  set<string> typeSet;
  NclLinkCondition *satisfiedCondition;

private:
  GingaTime delay;
  vector<NclLinkActionProgressionListener *> *progressionListeners;

public:
  NclLinkAction ();
  NclLinkAction (GingaTime delay);

private:
  void initLinkAction (GingaTime delay);

public:
  virtual ~NclLinkAction ();
  bool instanceOf (const string &s);
  GingaTime getWaitDelay ();
  void setWaitDelay (GingaTime delay);
  bool hasDelay ();

  void
  addActionProgressionListener (NclLinkActionProgressionListener *listener);

  void removeActionProgressionListener (
      NclLinkActionProgressionListener *listener);

  void notifyProgressionListeners (bool start);
  virtual vector<NclFormatterEvent *> *getEvents () = 0;
  virtual vector<NclLinkAction *> *getImplicitRefRoleActions () = 0;

  void setSatisfiedCondition (NclLinkCondition *satisfiedCondition);
  void run (NclLinkCondition *satisfiedCondition);

public:
  virtual void run ();

private:
  bool tryLock ();
};

GINGA_FORMATTER_END

#endif //_LINKACTION_H_
