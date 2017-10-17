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

#ifndef NCL_CONDITION
#define NCL_CONDITION

#include "NclEvents.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class INclConditionListener
{
public:
  virtual void conditionSatisfied () = 0;
};

class NclCondition: INclEventListener
{
public:
  NclCondition (NclEvent *, EventStateTransition);
  virtual ~NclCondition ();

  NclEvent *getEvent ();

  void setTriggerListener (INclConditionListener *);
  void conditionSatisfied ();

  // INclEventListener
  virtual void eventStateChanged (NclEvent *, EventStateTransition) override;
private:
  NclEvent *_event;
  EventStateTransition _transition;
  INclConditionListener *_listener;
};

GINGA_FORMATTER_END

#endif // NCL_CONDITION
