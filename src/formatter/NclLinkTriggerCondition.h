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

#ifndef NCL_LINK_TRIGGER_CONDITION
#define NCL_LINK_TRIGGER_CONDITION

#include "NclLinkTriggerListener.h"
#include "NclLinkCondition.h"

GINGA_FORMATTER_BEGIN

class NclLinkTriggerCondition : public NclLinkCondition
{
protected:
  NclLinkTriggerListener *_listener;
  GingaTime _delay;

public:
  NclLinkTriggerCondition ();
  virtual ~NclLinkTriggerCondition ();

protected:
  virtual void notifyConditionObservers (short satus);

public:
  void setTriggerListener (NclLinkTriggerListener *listener);
  NclLinkTriggerListener *getTriggerListener ();
  GingaTime getDelay ();
  void setDelay (GingaTime);
  virtual void conditionSatisfied (NclLinkCondition *condition);
};

GINGA_FORMATTER_END

#endif // NCL_LINK_TRIGGER_CONDITION
