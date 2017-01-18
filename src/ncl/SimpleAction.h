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

#ifndef _SIMPLEACTION_H_
#define _SIMPLEACTION_H_

#include "util/functions.h"
using namespace ::ginga::util;

#include "Animation.h"
using namespace ::ginga::ncl;

#include "Action.h"
#include "CompoundAction.h"
#include "EventUtil.h"
#include "Role.h"

GINGA_NCL_BEGIN

class SimpleAction : public Action, public Role
{
private:
  short actionType;
  short qualifier;
  string repeat;
  string repeatDelay;
  string value;
  Animation *animation;

public:
  static const short ACT_START = 1;
  static const short ACT_PAUSE = 2;
  static const short ACT_RESUME = 3;
  static const short ACT_STOP = 4;
  static const short ACT_ABORT = 5;
  static const short ACT_SET = 6;

  SimpleAction (string role);
  virtual ~SimpleAction ();

  void setLabel (string id);

  short getQualifier ();
  void setQualifier (short qualifier);
  string getRepeat ();
  string getRepeatDelay ();
  void setRepeatDelay (string time);
  void setRepeat (string newRepetitions);
  short getActionType ();
  void setActionType (short action);
  string getValue ();
  void setValue (string value);
  Animation *getAnimation ();
  void setAnimation (Animation *animation);

  bool
  instanceOf (string type)
  {
    return Action::instanceOf (type);
  }
};

GINGA_NCL_END

#endif //_SIMPLEACTION_H_
