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

#include "LinkCompoundAction.h"

#include <stdexcept>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkCompoundAction::LinkCompoundAction(short op) : LinkAction() {
		this->op = op;
		hasStart = false;
		runing   = false;
		listener = NULL;

		Thread::mutexInit(&mutexActions, true);
		typeSet.insert("LinkCompoundAction");
	}

	LinkCompoundAction::~LinkCompoundAction() {
		vector<LinkAction*>::iterator i;
		LinkAction* action;

		isDeleting = true;

		//clog << "LinkCompoundAction::~LinkCompoundAction" << endl;
		Thread::mutexLock(&mutexActions);
		i = actions.begin();
		while (i != actions.end()) {
			action = (LinkAction*)(*i);

			actions.erase(i);
			i = actions.begin();

			action->removeActionProgressionListener(this);
			delete action;
			action = NULL;
		}

		actions.clear();
		Thread::mutexUnlock(&mutexActions);
		Thread::mutexDestroy(&mutexActions);
	}

	short LinkCompoundAction::getOperator() {
		return op;
	}

	void LinkCompoundAction::addAction(LinkAction* action) {
		vector<LinkAction*>::iterator i;

		if (runing) {
			clog << "LinkCompoundAction::addAction ";
			clog << "Warning! Can't add action: status = running" << endl;
			return;
		}

		action->addActionProgressionListener(this);
		Thread::mutexLock(&mutexActions);
		i = actions.begin();
		while (i != actions.end()) {
			if (*i == action) {
				clog << "LinkCompoundAction::addAction Warning!";
				clog << " Trying to add same action twice";
				clog << endl;
				Thread::mutexUnlock(&mutexActions);
				return;
			}
			++i;
		}
		actions.push_back(action);
		Thread::mutexUnlock(&mutexActions);
	}

	vector<LinkAction*>* LinkCompoundAction::getActions() {
		vector<LinkAction*>* acts;

		if (runing) {
			return NULL;
		}

		Thread::mutexLock(&mutexActions);
		if (actions.empty()) {
			Thread::mutexUnlock(&mutexActions);
			return NULL;
		}

		acts = new vector<LinkAction*>(actions);
		Thread::mutexUnlock(&mutexActions);
		return acts;
	}

	void LinkCompoundAction::getSimpleActions(
			vector<LinkSimpleAction*>* simpleActions) {

		vector<LinkAction*>::iterator i;
		LinkAction* currentAction;

		Thread::mutexLock(&mutexActions);
		if (actions.empty()) {
			Thread::mutexUnlock(&mutexActions);
			return;
		}

		i = actions.begin();
		while (i != actions.end()) {
			currentAction = (*i);

			if (currentAction->instanceOf("LinkCompoundAction")) {
				((LinkCompoundAction*)currentAction)->getSimpleActions(
						simpleActions);

			} else if (currentAction->instanceOf("LinkSimpleAction")) {
				simpleActions->push_back((LinkSimpleAction*)currentAction);
			}

			++i;
		}

		Thread::mutexUnlock(&mutexActions);
	}

	void LinkCompoundAction::setCompoundActionListener(
		    ILinkActionListener* listener) {

		this->listener = listener;
	}

	vector<FormatterEvent*>* LinkCompoundAction::getEvents() {
		vector<LinkAction*>* acts;
		vector<LinkAction*>::iterator i;
		LinkAction* action;
		vector<FormatterEvent*>* events;
		vector<FormatterEvent*>* actionEvents;
		vector<FormatterEvent*>::iterator j;

		if (runing) {
			return NULL;
		}

		Thread::mutexLock(&mutexActions);
		if (actions.empty()) {
			Thread::mutexUnlock(&mutexActions);
			return NULL;
		}

		acts = new vector<LinkAction*>(actions);
		Thread::mutexUnlock(&mutexActions);

		events = new vector<FormatterEvent*>;

		for (i = acts->begin(); i != acts->end(); ++i) {
			action = (LinkAction*)(*i);
			actionEvents = action->getEvents();
			if (actionEvents != NULL) {
				for (j = actionEvents->begin(); j != actionEvents->end(); ++j) {
					events->push_back(*j);
				}
				delete actionEvents;
				actionEvents = NULL;
			}
		}

		delete acts;
		if (events->empty()) {
			delete events;
			return NULL;
		}

		return events;
	}

	vector<LinkAction*>* LinkCompoundAction::getImplicitRefRoleActions() {
		vector<LinkAction*>* acts;
		vector<LinkAction*>::iterator i;
		vector<LinkAction*>* assignmentActs;
		vector<LinkAction*>* refActs;
		vector<LinkAction*>::iterator j;

		if (runing) {
			return NULL;
		}

		Thread::mutexLock(&mutexActions);
		if (actions.empty()) {
			Thread::mutexUnlock(&mutexActions);
			return NULL;
		}

		acts = new vector<LinkAction*>(actions);
		Thread::mutexUnlock(&mutexActions);

		refActs = new vector<LinkAction*>;

		for (i = acts->begin(); i != acts->end(); ++i) {
			assignmentActs = (*i)->getImplicitRefRoleActions();
			if (assignmentActs != NULL) {
				for (j = assignmentActs->begin();
						j != assignmentActs->end(); ++j) {

					refActs->push_back(*j);
				}
				delete assignmentActs;
				assignmentActs = NULL;
			}
		}

		delete acts;
		if (refActs->empty()) {
			delete refActs;
			return NULL;
		}

		return refActs;
	}

	void LinkCompoundAction::run() {
		int i, size;
		LinkAction* action = NULL;
		vector<LinkSimpleAction*> simpleActions;

		runing = true;

		LinkAction::run();

		Thread::mutexLock(&mutexActions);
		if (actions.empty()) {
			clog << "LinkCompoundAction::run there is no action to run" << endl;
			Thread::mutexUnlock(&mutexActions);
			return;
		}
		size = actions.size();
		clog << "LinkCompoundAction::run '" << size << "' actions" << endl;
		Thread::mutexUnlock(&mutexActions);

		pendingActions = size;
		hasStart       = false;

		if (op == CompoundAction::OP_PAR) {
			for (i = 0; i < size; i++) {
				try {
					Thread::mutexLock(&mutexActions);
					if (actions.empty()) {
						Thread::mutexUnlock(&mutexActions);
						return;
					}

					action = (LinkAction*)(actions.at(i));
					action->setSatisfiedCondition(satisfiedCondition);
					Thread::mutexUnlock(&mutexActions);
					action->startThread();

				} catch (std::out_of_range& e) {
					clog << "LinkCompoundAction::run catch PAR out of range: ";
					clog << e.what();
					clog << endl;
					Thread::mutexUnlock(&mutexActions);
					continue;
				}
			}

		} else {
			for (i = 0; i < size; i++) {
				try {
					Thread::mutexLock(&mutexActions);
					if (actions.empty()) {
						Thread::mutexUnlock(&mutexActions);
						return;
					}
					action = (LinkAction*)(actions.at(i));
					Thread::mutexUnlock(&mutexActions);
					action->run(satisfiedCondition);

				} catch (std::out_of_range& e) {
					clog << "LinkCompoundAction::run catch SEQ out of range: ";
					clog << e.what();
					clog << endl;
					Thread::mutexUnlock(&mutexActions);
					continue;
				}
			}
		}
	}

	void LinkCompoundAction::actionProcessed(bool start) {
		pendingActions--;
		hasStart = (hasStart || start);
		if (pendingActions == 0) {
			notifyProgressionListeners(hasStart);
		}
	}
}
}
}
}
}
}
}
