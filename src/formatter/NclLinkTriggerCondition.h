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

#include "mb/IEventListener.h"
using namespace ::ginga::mb;

GINGA_FORMATTER_BEGIN

typedef struct
{
  NclLinkTriggerListener *listener;
  short status;
  NclLinkCondition *condition;
} ConditionStatus;

class NclLinkTriggerCondition : public NclLinkCondition,
                                public IEventListener
{
protected:
  NclLinkTriggerListener *listener;
  GingaTime delay;

  static bool initialized;
  static bool running;
  static vector<ConditionStatus *> notes;

public:
  NclLinkTriggerCondition ();
  virtual ~NclLinkTriggerCondition ();

  virtual vector<FormatterEvent *> *getEvents () = 0;

protected:
  virtual void notifyConditionObservers (short satus);

public:
  void setTriggerListener (NclLinkTriggerListener *listener);
  NclLinkTriggerListener *getTriggerListener ();
  GingaTime getDelay ();
  void setDelay (GingaTime delay);
  virtual void conditionSatisfied (NclLinkCondition *condition);

  void handleTickEvent (GingaTime, GingaTime, int);
  void handleKeyEvent (SDL_EventType, SDL_Keycode) {};
};

GINGA_FORMATTER_END

#endif // NCL_LINK_TRIGGER_CONDITION
