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

#ifndef LINKASSIGNMENTACTION_H_
#define LINKASSIGNMENTACTION_H_

#include "FormatterEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "LinkRepeatAction.h"

#include <string>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_BEGIN

	class LinkAssignmentAction : public LinkRepeatAction {
		private:
			string value;
			Animation* animation;

		public:
			LinkAssignmentAction(
					FormatterEvent* event, short actionType, string value);

			virtual ~LinkAssignmentAction();

			string getValue();
			void setValue(string value);
			Animation* getAnimation();
			void setAnimation(Animation* animation);
	};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_END
#endif /*LINKASSIGNMENTACTION_H_*/
