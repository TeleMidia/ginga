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

#ifndef _COMPOUNDACTION_H_
#define _COMPOUNDACTION_H_

#include "Action.h"
#include "SimpleAction.h"


GINGA_NCL_BEGIN

	class CompoundAction : public Action {
		public:
			static const short OP_PAR = 0;
			static const short OP_SEQ = 1;
			static const short OP_EXCL = 2;

		protected:
			vector<Action*>* actions;
			short myOperator;

		public:
			CompoundAction();
			CompoundAction(
				    Action* a1, Action* a2, short op);

			virtual ~CompoundAction();

			void setOperator(short op);
			short getOperator();
			vector<Action*>* getActions();
			void addAction(Action* action);
			void removeAction(Action* action);
			vector<Role*> *getRoles();
			string toString();
			bool instanceOf(string type) {
				return Action::instanceOf(type);
			}
	};

GINGA_NCL_END

#endif //_COMPOUNDACTION_H_
