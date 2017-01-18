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

#ifndef REFERNODE_H_
#define REFERNODE_H_

#include "GenericDescriptor.h"
#include "Node.h"
#include "Entity.h"

GINGA_NCL_BEGIN

class ReferNode : public Node
{
private:
  string instanceType;
  Entity *referredNode;
  GenericDescriptor *instDesc;

public:
  ReferNode (string id);
  ReferNode (string id, Entity *entity);

  GenericDescriptor *getInstanceDescriptor ();
  void setInstanceDescriptor (GenericDescriptor *descriptor);

  string getInstanceType ();
  void setInstanceType (string instance);

  Entity *getReferredEntity ();
  void setReferredEntity (Entity *entity);
  Entity *getDataEntity ();
};

GINGA_NCL_END

#endif /*REFERNODE_H_*/
