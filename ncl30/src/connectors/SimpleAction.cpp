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

#include "ncl/connectors/SimpleAction.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
	SimpleAction::SimpleAction(string role) :
		    Action(), Role() {

		SimpleAction::setLabel(role);
		qualifier = CompoundAction::OP_PAR;
		repeat = "0";
		repeatDelay = "0";
		value = "";
		animation = NULL;
		typeSet.insert("SimpleAction");
		typeSet.insert("Role");
	}

	SimpleAction::~SimpleAction() {
		if (animation != NULL) {
			delete animation;
			animation = NULL;
		}
	}

	void SimpleAction::setLabel(string id) {
		label = id;
		string upLabel = upperCase(label);

		if (upLabel == upperCase("start")) {
			actionType = ACT_START;
			eventType = EventUtil::EVT_PRESENTATION;

		} else if (upLabel == upperCase("stop")) {
			actionType = ACT_STOP;
			eventType = EventUtil::EVT_PRESENTATION;

		} else if (upLabel == upperCase("set")) {
			actionType = ACT_START;
			eventType = EventUtil::EVT_ATTRIBUTION;

		} else if (upLabel == upperCase("abort")) {
			actionType = ACT_ABORT;
			eventType = EventUtil::EVT_PRESENTATION;

		} else if (upLabel == upperCase("pause")) {
			actionType = ACT_PAUSE;
			eventType = EventUtil::EVT_PRESENTATION;

		} else if (upLabel == upperCase("resume")) {
			actionType = ACT_RESUME;
			eventType = EventUtil::EVT_PRESENTATION;
		}
	}

	short SimpleAction::getQualifier() {
		return qualifier;
	}

	void SimpleAction::setQualifier(short qualifier) {
		this->qualifier = qualifier;
	}

	string SimpleAction::getRepeat() {
		return repeat;
	}

	string SimpleAction::getRepeatDelay() {
		return repeatDelay;
	}

	void SimpleAction::setRepeatDelay(string time) {
		repeatDelay = time;
	}

	void SimpleAction::setRepeat(string newRepetitions) {
		repeat = newRepetitions;
	}

	short SimpleAction::getActionType() {
		return actionType;
	}

	void SimpleAction::setActionType(short action) {
		actionType = action;
	}

	string SimpleAction::getValue() {
		return value;
	}

	void SimpleAction::setValue(string value) {
		this->value = value;
	}

	Animation* SimpleAction::getAnimation() {
		return animation;
	}

	void SimpleAction::setAnimation(Animation* animation) {
		this->animation = animation;
	}
}
}
}
}
}
