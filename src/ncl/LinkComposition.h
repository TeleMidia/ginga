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

#ifndef _LINKCOMPOSITION_H_
#define _LINKCOMPOSITION_H_

#include "Link.h"

#include <set>
using namespace std;

GINGA_NCL_BEGIN

	class LinkComposition {
		public:
			virtual ~LinkComposition(){};
			virtual bool instanceOf(string className)=0;
			virtual bool addLink(Link* link)=0;
			virtual void clearLinks()=0;
			virtual bool containsLink(Link* link)=0;
			virtual set<Link*>* getLinks()=0;
			virtual int getNumLinks()=0;
			virtual bool removeLink(Link* link)=0;
	};

GINGA_NCL_END
#endif //_LINKCOMPOSITION_H_
