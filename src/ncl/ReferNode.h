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

#ifndef REFER_NODE_H
#define REFER_NODE_H

#include "Node.h"
#include "GenericDescriptor.h"

GINGA_NCL_BEGIN

class Node;
class ReferNode : public Node
{
public:
  ReferNode (const string &);
  ReferNode (const string &, Entity *);

  GenericDescriptor *getInstanceDescriptor ();
  void setInstanceDescriptor (GenericDescriptor *);

  string getInstanceType ();
  void setInstanceType (const string &);

  Entity *getReferredEntity ();
  void setReferredEntity (Entity *);

private:
  string _instanceType;
  Entity *_referredNode;
  GenericDescriptor *_instDesc;
};

GINGA_NCL_END

#endif // REFER_NODE_H
