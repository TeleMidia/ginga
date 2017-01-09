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

#include "ncl/switches/DescriptorSwitch.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace switches {
	bool DescriptorSwitch::initMutex = false;
	set<DescriptorSwitch*> DescriptorSwitch::objects;
	pthread_mutex_t DescriptorSwitch::_objMutex;

	void DescriptorSwitch::addInstance(DescriptorSwitch* object) {
		pthread_mutex_lock(&_objMutex);
		objects.insert(object);
		pthread_mutex_unlock(&_objMutex);
	}

	bool DescriptorSwitch::removeInstance(DescriptorSwitch* object) {
		set<DescriptorSwitch*>::iterator i;
		bool removed = false;

		pthread_mutex_lock(&_objMutex);
		i = objects.find(object);
		if (i != objects.end()) {
			objects.erase(i);
			removed = true;
		}
		pthread_mutex_unlock(&_objMutex);

		return removed;
	}

	bool DescriptorSwitch::hasInstance(
			DescriptorSwitch* object, bool eraseFromList) {

		set<DescriptorSwitch*>::iterator i;
		bool hasDSwitch = false;

		if (!initMutex) {
			return false;
		}

		pthread_mutex_lock(&_objMutex);
		i = objects.find(object);
		if (i != objects.end()) {
			if (eraseFromList) {
				objects.erase(i);
			}
			hasDSwitch = true;
		}
		pthread_mutex_unlock(&_objMutex);

		return hasDSwitch;
	}

	DescriptorSwitch::DescriptorSwitch(string id):
		    GenericDescriptor(id) {

		if (!initMutex) {
			initMutex = true;
			pthread_mutex_init(&DescriptorSwitch::_objMutex, NULL);
		}
		descriptorList     = new vector<GenericDescriptor*>;
		ruleList           = new vector<Rule*>;
		defaultDescriptor  = NULL;
		selectedDescriptor = NULL;

		addInstance(this);

		typeSet.insert("DescriptorSwitch");
	}

	DescriptorSwitch::~DescriptorSwitch() {
		vector<GenericDescriptor*>::iterator i;
		vector<Rule*>::iterator j;

		removeInstance(this);

		if (descriptorList != NULL) {
			i = descriptorList->begin();
			while (i != descriptorList->end()) {
				delete *i;
				++i;
			}

			delete descriptorList;
			descriptorList = NULL;
		}

		if (ruleList != NULL) {
			delete ruleList;
			ruleList = NULL;
		}
	}

	bool DescriptorSwitch::addDescriptor(
		    unsigned int index, GenericDescriptor* descriptor, Rule* rule) {

		if (index < 0 || index > descriptorList->size() ||
			    getDescriptor(descriptor->getId()) != NULL ) {

			return false;
		}

		vector<Rule*>::iterator it;
		for (it = ruleList->begin(); it != ruleList->end(); it++) {
			if( *it == rule ) return false;
		}

		if (index == descriptorList->size()) {
			descriptorList->push_back(descriptor);
			ruleList->push_back(rule);

		} else {
			descriptorList->insert(descriptorList->begin() + index, descriptor);
			ruleList->insert(ruleList->begin() + index, rule);
		}
		return true;
	}

	bool DescriptorSwitch::addDescriptor(
		    GenericDescriptor* descriptor, Rule* rule) {

		return addDescriptor(descriptorList->size(), descriptor, rule);
	}

	bool DescriptorSwitch::containsRule(Rule* rule) {
		vector<Rule*>::iterator iterRule;
		for (iterRule = ruleList->begin();
			    iterRule != ruleList->end(); ++iterRule) {

			if ((*iterRule)->getId() == rule->getId()) {
				return true;
			}
		}
		return false;
	}

	void DescriptorSwitch::exchangeDescriptorsAndRules(
		    unsigned int index1, unsigned int index2) {

		if (index1 >= descriptorList->size() ||
			    index2 >= descriptorList->size()) {
			return;
		}

		GenericDescriptor* auxDesc;
		auxDesc = static_cast<GenericDescriptor*>((*descriptorList)[index1]);
		Rule* auxRule = static_cast<Rule*>((*ruleList)[index1]);
		(*descriptorList)[index1] = (*descriptorList)[index2];
		(*descriptorList)[index2] = auxDesc;
		(*ruleList)[index1] = (*ruleList)[index2];
		(*ruleList)[index2] = auxRule;
	}

	int DescriptorSwitch::indexOfRule(Rule *rule) {
		int i=0;
		vector<Rule*>::iterator it;
		for (it=ruleList->begin(); it!=ruleList->end(); it++) {
			if(*it==rule) return i;
			i++;
		}
		return ruleList->size()+1;
	}

	GenericDescriptor* DescriptorSwitch::getDefaultDescriptor() {
		return defaultDescriptor;
	}

	unsigned int DescriptorSwitch::indexOfDescriptor(
		    GenericDescriptor* descriptor) {

		unsigned int i = 0;
		vector<GenericDescriptor*>::iterator iterDescr;

		for (iterDescr = descriptorList->begin();
			    iterDescr != descriptorList->end(); ++iterDescr) {

			if ((*(*iterDescr)).getId() == descriptor->getId())
				return i;
			i++;
		}
		return (descriptorList->size() + 1);
	}

	GenericDescriptor* DescriptorSwitch::getDescriptor(unsigned int index) {
		if (index >= descriptorList->size())
			return NULL;

		return static_cast<GenericDescriptor*>((*descriptorList)[index]);
	}

	GenericDescriptor* DescriptorSwitch::getDescriptor(string descriptorId) {
		int i, size;
		GenericDescriptor *descriptor;

		if (defaultDescriptor != NULL) {
			if (defaultDescriptor->getId() == descriptorId) {
				return defaultDescriptor;
			}
		}

		size = descriptorList->size();
		for (i = 0; i < size; i++) {
			descriptor = (*descriptorList)[i];
			if (descriptor->getId() == descriptorId) {
				return descriptor;
			}
		}

		return NULL;
	}

	GenericDescriptor* DescriptorSwitch::getDescriptor(Rule* rule) {
		unsigned int index;

		index = indexOfRule(rule);
		if (index > ruleList->size())
			return NULL;

		return static_cast<GenericDescriptor*>((*descriptorList)[index]);
	}

	Rule* DescriptorSwitch::getRule(unsigned int index) {
		if (index >= ruleList->size())
			return NULL;

		return static_cast<Rule*>((*ruleList)[index]);
	}

	unsigned int DescriptorSwitch::getNumDescriptors() {
		return descriptorList->size();
	}

	unsigned int DescriptorSwitch::getNumRules() {
		return ruleList->size();
	}

	bool DescriptorSwitch::removeDescriptor(unsigned int index) {
		if (index >= descriptorList->size())
			return false;

		vector<GenericDescriptor*>::iterator iterDescr;
		iterDescr = descriptorList->begin();
		iterDescr = iterDescr + index;
		descriptorList->erase(iterDescr);
		return true;
	}

	bool DescriptorSwitch::removeDescriptor(GenericDescriptor* descriptor) {
		return removeDescriptor(indexOfDescriptor(descriptor));
	}

	bool DescriptorSwitch::removeRule(Rule *rule) {
		int index;

		index = indexOfRule(rule);
		if (index < 0 || index>(int)ruleList->size())
			return false;

		descriptorList->erase(descriptorList->begin() + index);
		ruleList->erase(ruleList->begin() + index);
		return true;
	}

	void DescriptorSwitch::setDefaultDescriptor(GenericDescriptor* descriptor) {
		defaultDescriptor = descriptor;
	}

	void DescriptorSwitch::select(GenericDescriptor* descriptor) {
		vector<GenericDescriptor*>::iterator i;

		i = descriptorList->begin();
		while (i != descriptorList->end()) {
			if (*i == descriptor) {
				selectedDescriptor = descriptor;
				break;
			}
			++i;
		}
	}

	void DescriptorSwitch::selectDefault() {
		if (defaultDescriptor != NULL) {
			selectedDescriptor = defaultDescriptor;
		}
	}

	GenericDescriptor* DescriptorSwitch::getSelectedDescriptor() {
		return selectedDescriptor;
	}
}
}
}
}
}
