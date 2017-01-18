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

#ifndef BEGINEVENTTRANSITION_H_
#define BEGINEVENTTRANSITION_H_

#include "NclPresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "NclEndEventTransition.h"
#include "NclEventTransition.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_TRANSITION_BEGIN

class NclBeginEventTransition : public NclEventTransition
{
private:
  NclEndEventTransition *endTransition;

public:
  NclBeginEventTransition (double time, NclPresentationEvent *event);
  virtual ~NclBeginEventTransition ();
  NclEndEventTransition *getEndTransition ();
  void setEndTransition (NclEndEventTransition *entry);
};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_TRANSITION_END
#endif /*BEGINEVENTTRANSITION_H_*/
