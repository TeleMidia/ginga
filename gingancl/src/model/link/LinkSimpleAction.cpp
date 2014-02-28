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

#include "gingancl/model/LinkSimpleAction.h"
#include "gingancl/model/LinkAssignmentAction.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkSimpleAction::LinkSimpleAction(
			FormatterEvent* event, short type) : LinkAction() {

		this->event = event;
		actionType  = type;
		listener    = NULL;

		typeSet.insert("LinkSimpleAction");
	}

	LinkSimpleAction::~LinkSimpleAction() {
		clog << "LinkSimpleAction::~LinkSimpleAction" << endl;
		if (listener != NULL) {
			listener->removeAction(this);
		}

		listener = NULL;
		event    = NULL;
	}

	FormatterEvent* LinkSimpleAction::getEvent() {
		return event;
	}

	short LinkSimpleAction::getType() {
		return actionType;
	}

	void LinkSimpleAction::setSimpleActionListener(
		    ILinkActionListener* listener) {

		if (listener != NULL) {
			listener->addAction(this);
		}

		this->listener = listener;
	}

	vector<FormatterEvent*>* LinkSimpleAction::getEvents() {
		if (event == NULL)
			return NULL;

		vector<FormatterEvent*>* events;
		events = new vector<FormatterEvent*>;

		events->push_back(event);
		return events;
	}

	vector<LinkAction*>* LinkSimpleAction::getImplicitRefRoleActions() {
		vector<LinkAction*>* actions;
		string attVal = "", durVal = "", byVal = "";
		Animation* anim;

		actions = new vector<LinkAction*>;

		if (this->instanceOf("LinkAssignmentAction")) {
			attVal = ((LinkAssignmentAction*)this)->getValue();
			anim   = ((LinkAssignmentAction*)this)->getAnimation();

			if (anim != NULL) {
				durVal = anim->getDuration();
				byVal  = anim->getBy();
			}

			if ((byVal != "" && byVal.substr(0, 1) == "$") ||
					(durVal != "" && durVal.substr(0, 1) == "$") ||
					(attVal != "" && attVal.substr(0, 1) == "$")) {

				if (event->instanceOf("AttributionEvent")) {
					actions->push_back((LinkAction*)this);
				}
			}
		}

		if (actions->empty()) {
			delete actions;
			return NULL;
		}

		return actions;
	}

	void LinkSimpleAction::run() {
		LinkAction::run();

		if (listener != NULL) {
			listener->scheduleAction(satisfiedCondition, (void*)this);
		}

		if (actionType == SimpleAction::ACT_START) {
			/*clog << "LinkSimpleAction::run notify action INIT ";
			if (event != NULL) {
				clog << "'" << event->getId() << "'";
			}
			clog << endl;*/
			notifyProgressionListeners(true);

		} else {
			/*clog << "LinkSimpleAction::run notify action END ";
			if (event != NULL) {
				clog << "'" << event->getId() << "'";
			}
			clog << endl;*/
			notifyProgressionListeners(false);
		}
	}

}
}
}
}
}
}
}
