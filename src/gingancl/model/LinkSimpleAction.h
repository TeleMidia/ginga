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

#ifndef _LINKSIMPLEACTION_H_
#define _LINKSIMPLEACTION_H_

#include "ncl/SimpleAction.h"
using namespace ::ginga::ncl;

#include "FormatterEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "ILinkActionListener.h"
#include "LinkAction.h"

#include <set>
#include <vector>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_BEGIN

	class LinkSimpleAction : public LinkAction {
		protected:
			FormatterEvent* event;
			short actionType;

		private:
			ILinkActionListener* listener;

		protected:
			virtual void run();

		public:
			LinkSimpleAction(FormatterEvent* event, short type);
			virtual ~LinkSimpleAction();
			FormatterEvent* getEvent();
			short getType();
			void setSimpleActionListener(ILinkActionListener* listener);
			virtual vector<FormatterEvent*>* getEvents();
			virtual vector<LinkAction*>* getImplicitRefRoleActions();
	};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_END
#endif //_LINKSIMPLEACTION_H_
