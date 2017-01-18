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
#include "ncl/Content.h"

GINGA_NCL_BEGIN

	Content::Content(string someType, long someSize) {
		type = someType;
		size = someSize;
		typeSet.insert("Content");
	}

	Content::Content() {
		type = "";
		size = -1;
		typeSet.insert("Content");
	}

	Content::~Content() {

	}

	bool Content::instanceOf(string s) {
		/*clog << "Content instanceOf for " << s << " with the following set:" << endl;
		for(set<string>::iterator it = typeSet.begin(); it!=typeSet.end(); it++) {
			clog << "[" << *it << "] ";
		}
		clog << ((typeSet.find(s) != typeSet.end()) ? "true" : "false") << endl;*/
		if(typeSet.empty())
			return false;
		else
			return (typeSet.find(s) != typeSet.end());
	}

	long Content::getSize() {
		return size;
	}

	string Content::getType() {
		return type;
	}

	void Content::setSize(long someSize) {
		size = someSize;
	}

	void Content::setType(string someType) {
		type = someType;
	}

GINGA_NCL_END
