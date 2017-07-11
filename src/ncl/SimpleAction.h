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

#include "Action.h"
#include "CompoundAction.h"
#include "EventUtil.h"
#include "Role.h"

GINGA_NCL_BEGIN

enum SimpleActionType
{
   ACT_START = 1,
   ACT_PAUSE,
   ACT_RESUME,
   ACT_STOP,
   ACT_ABORT,
};

class SimpleAction : public Action, public Role
{
public:
  static string actionTypeToString (SimpleActionType t)
  {
    switch (t)
      {
      case ACT_START:
        return "start";
      case ACT_PAUSE:
        return "pause";
      case ACT_RESUME:
        return "resume";
      case ACT_STOP:
        return "stop";
      case ACT_ABORT:
        return "abort";
      default:
        g_assert_not_reached ();
      }
  }

  static SimpleActionType stringToActionType (const string &s)
  {
    if (s == "start")  return ACT_START;
    if (s == "pause")  return ACT_PAUSE;
    if (s == "resume") return ACT_RESUME;
    if (s == "stop")   return ACT_STOP;
    if (s == "abort")  return ACT_ABORT;
    g_assert_not_reached ();
  }

  SimpleAction (SimpleActionType, const string &, const string &,
                const string &, const string &, const string &,
                const string &, const string &);
  virtual ~SimpleAction ();

  SimpleActionType getActionType ();
  string getDelay ();
  string getRepeat ();
  string getRepeatDelay ();
  string getValue ();
  string getDuration ();
  string getBy ();

private:
  SimpleActionType _type;
  string _delay;
  string _repeat;
  string _repeatDelay;
  string _value;
  string _duration;
  string _by;
};

GINGA_NCL_END

#endif //_SIMPLEACTION_H_
