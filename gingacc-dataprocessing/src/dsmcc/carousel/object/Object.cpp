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

#include "dataprocessing/dsmcc/carousel/object/Object.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	Object::Object() {

	}

	Object::~Object() {
		vector<Binding*>::iterator i;

		i = bindings.begin();
		while (i != bindings.end()) {
			delete (*i);

			++i;
		}
	}

	void Object::setCarouselId(unsigned int objectCarouselId) {
		carouselId = objectCarouselId;
	}

	void Object::setModuleId(unsigned int objectModuleId) {
		moduleId = objectModuleId;
	}

	void Object::setKey(unsigned int objectKey) {
		key = objectKey;
	}

	void Object::setKind(string objectKind) {
		kind = objectKind;
	}

	void Object::addBinding(Binding* binding) {
		bindings.push_back(binding);
	}

	void Object::setData(char* fileData) {
		data = fileData;
	}

	void Object::setDataSize(unsigned int size) {
		dataSize = size;
	}

	string Object::getObjectId() {
		return itos(carouselId) + itos(moduleId) + itos(key);
	}

	unsigned int Object::getCarouselId() {
		return carouselId;
	}

	unsigned int Object::getModuleId() {
		return moduleId;
	}

	unsigned int Object::getKey() {
		return key;
	}

	string Object::getKind() {
		return kind;
	}

	vector<Binding*>* Object::getBindings() {
		return &bindings;
	}

	char* Object::getData() {
		return data;
	}

	unsigned int Object::getDataSize() {
		return dataSize;
	}

	void Object::print() {
		clog << "Object" << endl;
		clog << "carouselId = " << carouselId << endl;
		clog << "moduleId = " << moduleId << endl;
		clog << "key = " << key << endl;
		clog << "kind = " << kind << endl;
		clog << "name = " << name << endl;
	}
}
}
}
}
}
}
}
