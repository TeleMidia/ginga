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

#ifndef _LINKTRANSITIONTRIGGERCONDITION_H_
#define _LINKTRANSITIONTRIGGERCONDITION_H_

#include <vector>
using namespace std;

#include "ncl/link/Bind.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "FormatterEvent.h"
#include "IEventListener.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "LinkTriggerCondition.h"
#include "LinkTriggerListener.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_BEGIN

  class LinkTransitionTriggerCondition : public LinkTriggerCondition,
  	    public IEventListener {

	protected:
		FormatterEvent* event;
		short transition;
		Bind* bind;

	public:
		LinkTransitionTriggerCondition(
				FormatterEvent* event, short transition, Bind* bind);

		virtual ~LinkTransitionTriggerCondition();

		Bind* getBind();

		virtual void eventStateChanged(
			    void* event, short transition, short previousState);

		short getPriorityType();
		FormatterEvent* getEvent();
		short getTransition();
		virtual vector<FormatterEvent*>* getEvents();
  };

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_END
#endif //_LINKTRANSITIONTRIGGERCONDITION_H_
