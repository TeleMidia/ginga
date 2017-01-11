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

#include "ncl/components/ReferenceContent.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {
	ReferenceContent::ReferenceContent() : Content() {
		reference = "";
		typeSet.insert("ReferenceContent");
	}

	ReferenceContent::ReferenceContent(string ref) : Content() {
		setReference(ref);
		typeSet.insert("ReferenceContent");
	}

	string ReferenceContent::getReference() {
		return reference;
	}

	string ReferenceContent::getCompleteReferenceUrl() {
		return reference;
	}

	void ReferenceContent::setReference(string ref) {
		reference = ref;
	}

	string ReferenceContent::getType() {
		string src, type, extension;
		string::size_type index, len;

		type = "";
		src = reference;

		// look for type using the mime table
		if (src == "") {
			return "";
		}

		index = src.find_last_of(".");
		if (index != std::string::npos) {
			index++;
			len = src.length();
			if (index < len) {
				extension = src.substr(index, (len - index));
				if (extension != "") {
					type = ContentTypeManager::getInstance()->getMimeType(
							extension);
				}
			}
		}

		return type;
	}
}
}
}
}
}
