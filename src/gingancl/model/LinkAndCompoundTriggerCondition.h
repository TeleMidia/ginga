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

#ifndef LINKANDCOMPOUNDTRIGGERCONDITION_H_
#define LINKANDCOMPOUNDTRIGGERCONDITION_H_

#include "LinkCondition.h"
#include "LinkStatement.h"
#include "LinkTriggerCondition.h"
#include "LinkTriggerListener.h"
#include "LinkCompoundTriggerCondition.h"


BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_BEGIN

	class LinkAndCompoundTriggerCondition :
		    public LinkCompoundTriggerCondition {

		private:
			vector<LinkCondition*> unsatisfiedConditions;
			vector<LinkCondition*> statements;

		public:
			LinkAndCompoundTriggerCondition();
			virtual ~LinkAndCompoundTriggerCondition();
			void addCondition(LinkCondition* condition);
			void conditionSatisfied(void* condition); //LinkTriggerCondition
			vector<FormatterEvent*>* getEvents();
	};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_END
#endif /*LINKANDCOMPOUNDTRIGGERCONDITION_H_*/
