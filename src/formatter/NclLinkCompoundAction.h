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

#include "FormatterEvents.h"

#include "NclLinkAction.h"
#include "NclLinkSimpleAction.h"
#include "INclLinkActionListener.h"
#include "NclLinkActionProgressionListener.h"

GINGA_FORMATTER_BEGIN

class NclLinkCompoundAction : public NclLinkAction,
                              public NclLinkActionProgressionListener
{
public:
  NclLinkCompoundAction (short _op);
  virtual ~NclLinkCompoundAction ();

  virtual void run () override;

  short getOperator ();
  void addAction (NclLinkAction *action);

  vector<NclLinkAction *> *getActions ();
  void getSimpleActions (vector<NclLinkSimpleAction *> *simpleActions);
  void setCompoundActionListener (INclLinkActionListener *_listener);

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

#endif //_LINKCOMPOUNDACTION_H_
