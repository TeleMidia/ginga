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

GINGA_FORMATTER_BEGIN

class NclAction;
class INclActionListener
{
public:
  virtual void scheduleAction (NclAction *) = 0;
};

class NclAction
{
public:
  NclAction (NclEvent *, EventStateTransition, INclActionListener *);
  virtual ~NclAction ();

  NclEvent *getEvent ();
  EventType getEventType ();
  EventStateTransition getEventStateTransition ();

  string getDuration ();
  void setDuration (const string &);

  string getValue ();
  void setValue (const string &);

  void run ();

private:
  NclEvent *_event;
  EventStateTransition _transition;
  INclActionListener *_listener;
  string _duration;
  string _value;
};

GINGA_FORMATTER_END

#endif // LINK_ACTION_H
