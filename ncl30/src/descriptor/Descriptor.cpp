/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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

#include "ncl/descriptor/Descriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace descriptor {
	Descriptor::Descriptor(string id) :
			    GenericDescriptor(id) {

		explicitDuration = NaN();
		presentationTool = "";
		repetitions      = 0;
		freeze           = false;
		region           = NULL;

		keyNavigation    = NULL;
		focusDecoration  = NULL;

		typeSet.insert("Descriptor");
	}

	Descriptor::~Descriptor() {
		map<string, Parameter*>::iterator i;

		if (region != NULL) {
			//deleted through region base
			region = NULL;
		}

		i = parameters.begin();
		while (i != parameters.end()) {
			delete i->second;
			++i;
		}

		if (keyNavigation != NULL) {
			delete keyNavigation;
			keyNavigation = NULL;
		}

		if (focusDecoration != NULL) {
			delete focusDecoration;
			focusDecoration = NULL;
		}
	}

	double Descriptor::getExplicitDuration() {
		return explicitDuration;
	}

	string Descriptor::getPlayerName() {
		return presentationTool;
	}

	LayoutRegion* Descriptor::getRegion() {
		return region;
	}

	long Descriptor::getRepetitions() {
		return repetitions;
	}

	bool Descriptor::isFreeze() {
		return freeze;
	}

	void Descriptor::setFreeze(bool freeze) {
		this->freeze = freeze;
	}

	void Descriptor::setExplicitDuration(double dur) {
		explicitDuration = dur;
	}

	void Descriptor::setPlayerName(string name) {
		presentationTool = name;
	}

	void Descriptor::setRegion(LayoutRegion* someRegion) {
		region = someRegion;
	}

	void Descriptor::setRepetitions(long r) {
		repetitions = r;
	}

	void Descriptor::addParameter(Parameter* parameter) {
		string paramName;
		map<string, Parameter*>::iterator i;

		paramName = parameter->getName();
		i = parameters.find(paramName);
		if (i != parameters.end() && i->second != parameter) {
			delete i->second;
		}

		parameters[paramName] = parameter;
	}

	vector<Parameter*>* Descriptor::getParameters() {
		vector<Parameter*> *ret = new vector<Parameter*>;
		map<string, Parameter*>::iterator it;
		for(it = parameters.begin(); it != parameters.end(); ++it) {
			ret->push_back( it->second );
		}
		return ret;
	}

	Parameter* Descriptor::getParameter(string paramName) {
		if (parameters.count(paramName)==0){
			return NULL;

		} else {
			return parameters[paramName];
		}
	}

	void Descriptor::removeParameter(Parameter* parameter) {
		map<string, Parameter*>::iterator it;

		it = parameters.find(parameter->getName());
		if (it != parameters.end()) {
			parameters.erase(it);
		}
	}

	KeyNavigation* Descriptor::getKeyNavigation() {
		return keyNavigation;
	}

	void Descriptor::setKeyNavigation(KeyNavigation* keyNav) {
		keyNavigation = keyNav;
	}

	FocusDecoration* Descriptor::getFocusDecoration() {
		return focusDecoration;
	}

	void Descriptor::setFocusDecoration(FocusDecoration* focusDec) {
		focusDecoration = focusDec;
	}

	vector<Transition*>* Descriptor::getInputTransitions() {
		return &inputTransitions;
	}

	bool Descriptor::addInputTransition(Transition* transition, int somePos) {
		unsigned int pos;
		pos = (unsigned int)somePos;
		if (pos < 0 || pos > inputTransitions.size() || transition == NULL) {
			return false;
		}

		if (pos == inputTransitions.size()) {
			inputTransitions.push_back(transition);
			return true;
		}

		vector<Transition*>::iterator i;
		i = inputTransitions.begin() + pos;

		inputTransitions.insert(i, transition);
		return true;
	}

	void Descriptor::removeInputTransition(Transition* transition) {
		vector<Transition*>::iterator i;
		i = inputTransitions.begin();
		while (i != inputTransitions.end()) {
			if (*i == transition) {
				inputTransitions.erase(i);
				break;
			}
			++i;
		}
	}

	void Descriptor::removeAllInputTransitions() {
		inputTransitions.clear();
	}

	vector<Transition*>* Descriptor::getOutputTransitions() {
		return &outputTransitions;
	}

	bool Descriptor::addOutputTransition(Transition* transition, int somePos) {
		unsigned int pos;
		pos = (unsigned int)somePos;
		if (pos < 0 || pos > outputTransitions.size() || transition == NULL) {
			return false;
		}

		if (pos == outputTransitions.size()) {
			outputTransitions.push_back(transition);
			return true;
		}

		vector<Transition*>::iterator i;
		i = outputTransitions.begin() + pos;

		outputTransitions.insert(i, transition);
		return true;
	}

	void Descriptor::removeOutputTransition(Transition* transition) {
		vector<Transition*>::iterator i;
		i = outputTransitions.begin();
		while (i != outputTransitions.end()) {
			if (*i == transition) {
				outputTransitions.erase(i);
				break;
			}
			++i;
		}
	}

	void Descriptor::removeAllOutputTransitions() {
		outputTransitions.clear();
	}
}
}
}
}
}
