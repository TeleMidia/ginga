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

#include "ncl/switches/CompositeRule.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace switches {
	CompositeRule::CompositeRule(string id, short someOperator) : Rule(id) {
		rules = new vector<Rule*>;
		setOperator(someOperator);
		typeSet.insert("CompositeRule");
	}

	CompositeRule::CompositeRule(
		    string id,
		    Rule* firstRule,
		    Rule* secondRule,
		    short someOperator) : Rule(id) {

		rules = new vector<Rule*>;
		setOperator(someOperator);
		rules->push_back(firstRule);
		rules->push_back(secondRule);
		typeSet.insert("CompositeRule");
	}

	CompositeRule::~CompositeRule() {
		vector<Rule*>::iterator i;

		if (rules != NULL) {
			i = rules->begin();
			while (i != rules->begin()) {
				delete *i;
				++i;
			}

			delete rules;
			rules = NULL;
		}
	}

	bool CompositeRule::addRule(Rule* rule) {
		rules->push_back(rule);
		return true;
	}

	vector<Rule*>* CompositeRule::getRules() {
		if (rules->begin() == rules->end())
			return NULL;

		return rules;
	}

	unsigned int CompositeRule::getNumRules() {
		return rules->size();
	}

	short CompositeRule::getOperator() {
		return ruleOperator;
	}

	bool CompositeRule::removeRule(Rule* rule) {
		vector<Rule*>::iterator iterRule;

		iterRule = rules->begin();
		while (iterRule != rules->end()) {
			if ((*(*iterRule)).getId() == rule->getId()) {
				rules->erase(iterRule);
				return true;
			}
			++iterRule;
		}
		return false;
	}

	string CompositeRule::toString() {
		string result;
		vector<Rule*>::iterator ruleIterator;
		Rule* rule;

		result = "(";

		for (ruleIterator = rules->begin(); ruleIterator != rules->end();
			    ++ruleIterator) {

			rule = (Rule*)(*ruleIterator);
			result = result + (rule->getId()) + " " + opStr + " ";
		}

		if ( (result.length() - opStr.length() - 2) > 0)
			result = result.substr(0, result.length() - opStr.length() - 2);

		return result + ")";
	}

	void CompositeRule::setOperator(short op) {
		switch (op) {
			case OP_OR:
				ruleOperator = OP_OR;
				opStr = "OR";
				break;

			case OP_AND:
				ruleOperator = OP_AND;
				opStr = "AND";
				break;

			default:
				ruleOperator = OP_AND;
				opStr = "AND";
				break;
		}
	}
}
}
}
}
}
