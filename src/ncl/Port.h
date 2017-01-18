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

#ifndef _PORT_H_
#define _PORT_H_

#include "NodeEntity.h"
using namespace ::ginga::ncl;

#include "InterfacePoint.h"

#include <vector>
#include <string>
using namespace std;

GINGA_NCL_BEGIN

	class Port : public InterfacePoint {
		protected:
			Node* node;
			InterfacePoint* interfacePoint;

		public:
			Port(string id, Node* someNode,
				    InterfacePoint* someInterfacePoint);

			virtual ~Port();
			InterfacePoint* getInterfacePoint();
			Node* getNode();
			virtual Node* getEndNode();
			virtual InterfacePoint* getEndInterfacePoint();
			vector<Node*>* getMapNodeNesting();
			void setInterfacePoint(InterfacePoint* someInterfacePoint);
			void setNode(Node* someNode);
	};

GINGA_NCL_END
#endif //_PORT_H_
