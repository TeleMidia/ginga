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

#include "gingancl/model/FormatterCausalLink.h"

#include "gingancl/model/CompositeExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	FormatterCausalLink::FormatterCausalLink(
		    LinkTriggerCondition* condition,
		    LinkAction* action,
		    Link* ncmLink,
			void* parentObject) :
				FormatterLink(ncmLink, parentObject) {

		typeSet.insert("FormatterCausalLink");
		this->condition = condition;
		this->action = action;

		if (this->condition != NULL) {
			this->condition->setTriggerListener(this);
		}

		if (this->action != NULL) {
			this->action->addActionProgressionListener(this);
		}
	}

	FormatterCausalLink::~FormatterCausalLink() {
		if (condition != NULL) {
			delete condition;
			condition = NULL;
		}

		if (action != NULL) {
			delete action;
			action = NULL;
		}
	}

    LinkAction* FormatterCausalLink::getAction() {
    	return action;
    }

    LinkTriggerCondition* FormatterCausalLink::getTriggerCondition() {
    	return condition;
    }

    void FormatterCausalLink::conditionSatisfied(void* condition) {
    	if (!suspend) {
    		action->run(condition);
    	}
    }

    vector<FormatterEvent*>* FormatterCausalLink::getEvents() {
    	vector<FormatterEvent*>* events;
    	vector<FormatterEvent*>* actEvents;
    	vector<FormatterEvent*>::iterator i;

    	events = condition->getEvents();
		actEvents = action->getEvents();

		if (actEvents == NULL) {
			return events;
		}

    	if (events == NULL) {
    		return actEvents;
    	}

		for (i = actEvents->begin(); i != actEvents->end(); ++i) {
			events->push_back(*i);
		}

		delete actEvents;
		actEvents = NULL;

		if (events->empty()) {
			delete events;
			return NULL;
		}

		return events;
    }

    void FormatterCausalLink::evaluationStarted() {
		/*clog << endl;
		clog << "FormatterCausalLink::evaluationStarted(" << ncmLink->getId();
		clog << ")" << endl << endl;*/
		((CompositeExecutionObject*)parentObject)->linkEvaluationStarted(this);
	}

	void FormatterCausalLink::evaluationEnded() {
		/*clog << endl;
		clog << "FormatterCausalLink::evaluationEnded(" << ncmLink->getId();
		clog << ")" << endl << endl;*/
		((CompositeExecutionObject*)parentObject)->
			    linkEvaluationFinished(this, false);
	}

	void FormatterCausalLink::actionProcessed(bool start) {
		/*clog << endl;
		clog << "FormatterCausalLink::actionProcessed(";
		if (!start) {
			clog << "start:" << ncmLink->getId();
		} else {
			clog << "nostart:" << ncmLink->getId();
		}
		clog << ")" << endl << endl;*/
		((CompositeExecutionObject*)parentObject)->
			    linkEvaluationFinished(this, start);
	}
}
}
}
}
}
}
}
