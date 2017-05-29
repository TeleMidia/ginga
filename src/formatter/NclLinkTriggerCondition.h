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

#ifndef _LINKTRIGGERCONDITION_H_
#define _LINKTRIGGERCONDITION_H_

#include "system/Thread.h"
using namespace ::ginga::system;

#include "NclLinkTriggerListener.h"
#include "NclLinkCondition.h"

GINGA_FORMATTER_BEGIN

typedef struct
{
  NclLinkTriggerListener *listener;
  short status;
  NclLinkCondition *condition;
} ConditionStatus;

class NclLinkTriggerCondition : public NclLinkCondition, public Thread
{
protected:
  NclLinkTriggerListener *listener;
  double delay;

  static pthread_mutex_t sMutex;
  static bool initialized;
  static bool running;
  static vector<ConditionStatus *> notes;

public:
  NclLinkTriggerCondition ();
  virtual ~NclLinkTriggerCondition ();

  virtual vector<NclFormatterEvent *> *getEvents () = 0;

protected:
  virtual void notifyConditionObservers (short satus);

public:
  void setTriggerListener (NclLinkTriggerListener *listener);
  NclLinkTriggerListener *getTriggerListener ();
  double getDelay ();
  void setDelay (double delay);
  virtual void conditionSatisfied (NclLinkCondition *condition);

private:
  static void *notificationThread (void *ptr);

  void run ();
};

GINGA_FORMATTER_END

#endif //_LINKTRIGGERCONDITION_H_
