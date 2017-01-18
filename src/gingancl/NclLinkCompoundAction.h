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

#ifndef _LINKCOMPOUNDACTION_H_
#define _LINKCOMPOUNDACTION_H_

#include "ncl/CompoundAction.h"
using namespace ::ginga::ncl;

#include "NclFormatterEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "NclLinkAction.h"
#include "NclLinkSimpleAction.h"
#include "INclLinkActionListener.h"
#include "NclLinkActionProgressionListener.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_BEGIN

class NclLinkCompoundAction : public NclLinkAction,
                           public NclLinkActionProgressionListener
{

protected:
  vector<NclLinkAction *> actions;
  short op;

private:
  pthread_mutex_t mutexActions;
  int pendingActions;
  bool hasStart;
  bool runing;

  INclLinkActionListener *listener;

  virtual void run ();

public:
  NclLinkCompoundAction (short op);
  virtual ~NclLinkCompoundAction ();

  short getOperator ();
  void addAction (NclLinkAction *action);

  vector<NclLinkAction *> *getActions ();
  void getSimpleActions (vector<NclLinkSimpleAction *> *simpleActions);
  void setCompoundActionListener (INclLinkActionListener *listener);

  virtual vector<NclFormatterEvent *> *getEvents ();
  void actionProcessed (bool start);
  virtual vector<NclLinkAction *> *getImplicitRefRoleActions ();
};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_END
#endif //_LINKCOMPOUNDACTION_H_
