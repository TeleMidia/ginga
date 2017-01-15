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

#include "config.h"
#include "ncl/descriptor/DescriptorBase.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace descriptor {
	DescriptorBase::DescriptorBase(string id) : Base(id) {
		descriptorSet = new vector<GenericDescriptor*>;
		typeSet.insert("DescriptorBase");
	}

	DescriptorBase::~DescriptorBase() {
		vector<GenericDescriptor*>::iterator i;

		if (descriptorSet != NULL) {
			i = descriptorSet->begin();
			while (i != descriptorSet->end()) {
				delete *i;
				++i;
			}
			delete descriptorSet;
			descriptorSet = NULL;
		}
	}

	bool DescriptorBase::addDescriptor(GenericDescriptor* descriptor) {
		if (descriptor == NULL)
			return false;

		vector<GenericDescriptor*>::iterator i;
		for (i=descriptorSet->begin(); i!= descriptorSet->end(); ++i) {
			if (*i == descriptor) {
				return false;
			}
		}
		descriptorSet->push_back(descriptor);
		return true;
	}

	bool DescriptorBase::addBase(Base* base, string alias, string location){
		if (base->instanceOf("DescriptorBase")) {
			return Base::addBase(base, alias, location);
		}
		return false;
	}

	void DescriptorBase::clear() {
		descriptorSet->clear();
		Base::clear();
	}

	GenericDescriptor* DescriptorBase::getDescriptorLocally(string descriptorId) {
		vector<GenericDescriptor*>::iterator descriptors;

		descriptors = descriptorSet->begin();
		while (descriptors != descriptorSet->end()) {
			if ((*descriptors)->getId() == descriptorId) {
				return (*descriptors);
			}
			++descriptors;
		}
		return NULL;
	}

	GenericDescriptor* DescriptorBase::getDescriptor(string descriptorId) {
		string::size_type index;
		string prefix, suffix;
		DescriptorBase* base;

		index = descriptorId.find_first_of("#");
		if (index == string::npos) {
			return getDescriptorLocally(descriptorId);
		}
		prefix = descriptorId.substr(0, index);
		index++;
		suffix = descriptorId.substr(index, descriptorId.length() - index);
		if (baseAliases.find(prefix) != baseAliases.end()) {
			base = (DescriptorBase*)(baseAliases[prefix]);
			return base->getDescriptor(suffix);

		} else if (baseLocations.find(prefix) != baseLocations.end()) {
			base = (DescriptorBase*)(baseLocations[prefix]);
			return base->getDescriptor(suffix);

		} else {
			return NULL;
		}
	}

	vector<GenericDescriptor*>* DescriptorBase::getDescriptors() {
		return descriptorSet;
	}

	bool DescriptorBase::removeDescriptor(GenericDescriptor* descriptor) {
		vector<GenericDescriptor*>::iterator i;
		for (i=descriptorSet->begin(); i!=descriptorSet->end(); ++i) {
			if (*i == descriptor) {
				descriptorSet->erase(i);
				return true;
			}
		}
		return false;
	}
}
}
}
}
}
