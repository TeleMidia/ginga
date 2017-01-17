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
#include "Binding.h"

BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_CAROUSEL_BEGIN

	Binding::Binding() {
		ior = NULL;
	}

	Binding::~Binding() {
		releaseIor();
	}

	void Binding::releaseIor() {
		if (ior != NULL) {
			delete ior;
			ior = NULL;
		}
	}

	void Binding::setId(string id) {
		this->id = id;
	}

	string Binding::getId() {
		return id;
	}

	void Binding::setKind(string kind) {
		this->kind = kind;
	}

	string Binding::getKind() {
		return kind;
	}

	void Binding::setType(unsigned int type) {
		this->type = type;
	}

	unsigned int Binding::getType() {
		return type;
	}

	void Binding::setIor(Ior* ior) {
		releaseIor();

		this->ior = ior;
	}

	Ior* Binding::getIor() {
		return ior;
	}

	void Binding::setObjectInfo(string objectInfo) {
		this->objectInfo = objectInfo;
	}

	string Binding::getObjectInfo() {
		return this->objectInfo;
	}

	void Binding::print() {
		clog << "BINDING" << endl;
		clog << "id = " << id.c_str() << endl;
		clog << "kind = " << kind.c_str() << endl;
		clog << "type = " << type << endl;
		clog << " info = " << objectInfo.c_str() << endl;
	}

BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_CAROUSEL_END
