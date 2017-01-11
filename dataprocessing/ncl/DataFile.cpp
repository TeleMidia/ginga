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

#include "dataprocessing/ncl/DataFile.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace ncl {
	DataFile::DataFile(int id) {
		this->structureId  = id;
		this->componentTag = "";
		this->size         = 0;
		this->uri          = "";
	}

	DataFile::~DataFile() {

	}

	int DataFile::getId() {
		return this->structureId;
	}

	void DataFile::setComponentTag(string componentTag) {
		this->componentTag = componentTag;
	}

	string DataFile::getCopmonentTag() {
		return this->componentTag;
	}

	void DataFile::setUri(string uri) {
		this->uri = uri;
	}

	string DataFile::getUri() {
		return this->uri;
	}

	void DataFile::setSize(double size) {
		this->size = size;
	}

	double DataFile::getSize() {
		return this->size;
	}
}
}
}
}
}
}
}
