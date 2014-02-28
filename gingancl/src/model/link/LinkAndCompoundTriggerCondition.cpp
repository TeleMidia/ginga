/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/
#include "gingancl/model/LinkAndCompoundTriggerCondition.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkAndCompoundTriggerCondition::LinkAndCompoundTriggerCondition() :
		    LinkCompoundTriggerCondition() {

		typeSet.insert("LinkAndCompoundTriggerCondition");
	}

	LinkAndCompoundTriggerCondition::~LinkAndCompoundTriggerCondition() {
		vector<LinkCondition*>::iterator i;
		LinkCondition* l;

		isDeleting = true;
		unsatisfiedConditions.clear();

		i = statements.begin();
		while (i != statements.end()) {
			l = *i;
			if (l != NULL) {
				delete l;
				l = NULL;
			}
			++i;
		}

		statements.clear();
	}

	void LinkAndCompoundTriggerCondition::addCondition(
		    LinkCondition* condition) {

		if (condition == NULL) {
			return;
		}

		if (condition->instanceOf("LinkTriggerCondition")) {
			unsatisfiedConditions.push_back(condition);
			LinkCompoundTriggerCondition::addCondition(condition);

		} else if (condition->instanceOf("LinkStatement")) {
			statements.push_back(condition);

		} else {
			clog << "LinkAndCompoundTriggerCondition::";
			clog << "addCondition Warning! statements ";
			clog << "trying to add a condition !instanceOf(LinkStatement)";
			clog << " and !instanceOf(LinkTriggerCondition)";
			clog << endl;
		}
	}

	void LinkAndCompoundTriggerCondition::conditionSatisfied(
		    void* condition) {

		vector<LinkCondition*>::iterator i;
		i = unsatisfiedConditions.begin();
		while (i != unsatisfiedConditions.end()) {
			if ((*i) == (LinkCondition*)condition) {
				unsatisfiedConditions.erase(i);
				if (unsatisfiedConditions.empty()) {
					break;
				} else {
					i = unsatisfiedConditions.begin();
				}
			} else {
				++i;
			}
		}

		if (unsatisfiedConditions.empty()) {
			for (i = conditions.begin(); i != conditions.end(); ++i) {
				unsatisfiedConditions.push_back(*i);
			}

			for (i = statements.begin(); i != statements.end(); ++i) {
				if (!(((LinkStatement*)(*i))->evaluate())) {
					notifyConditionObservers(
						    LinkTriggerListener::EVALUATION_ENDED);

					return;
				}
			}

			LinkTriggerCondition::conditionSatisfied(condition);

		} else {
			notifyConditionObservers(LinkTriggerListener::EVALUATION_ENDED);
		}
	}

	vector<FormatterEvent*>* LinkAndCompoundTriggerCondition::getEvents() {
		vector<FormatterEvent*>* events;
		vector<FormatterEvent*>* eventsToAdd;

		vector<LinkCondition*>::iterator i;
		vector<FormatterEvent*>::iterator j;

		events = LinkCompoundTriggerCondition::getEvents();
		if (events != NULL) {
			for (i = statements.begin(); i != statements.end(); ++i) {
				eventsToAdd = (*i)->getEvents();
				if (eventsToAdd != NULL) {
					for (j = eventsToAdd->begin();
						    j != eventsToAdd->end(); ++j) {

						events->push_back(*j);
					}
					delete eventsToAdd;
					eventsToAdd = NULL;
				}
			}
		}

		return events;
	}
}
}
}
}
}
}
}
