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

#include "dataprocessing/dsmcc/carousel/Ior.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	Ior::Ior() {

	}

	void Ior::setTypeId(string typeId) {
		this->typeId = typeId;
	}

	string Ior::getTypeId() {
		return typeId;
	}

	void Ior::setCarouselId(unsigned int carouselId) {
		this->carouselId = carouselId;
	}

	unsigned int Ior::getCarouselId() {
		return carouselId;
	}

	void Ior::setModuleId(unsigned int moduleId) {
		this->moduleId = moduleId;
	}

	unsigned int Ior::getModuleId() {
		return moduleId;
	}

	void Ior::setObjectKey(unsigned int objectKey) {
		this->objectKey = objectKey;
	}

	unsigned int Ior::getObjectKey() {
		return objectKey;
	}

	void Ior::print() {
		clog << "IOR" << endl;
		clog << "typeId = " << typeId.c_str() << endl;
		clog << "carouselId = " << carouselId << endl;
		clog << "moduleId = " << moduleId << endl;
	}
}
}
}
}
}
}
}
