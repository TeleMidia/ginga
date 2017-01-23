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

#include "ginga.h"
#include "Port.h"
#include "SwitchPort.h"

GINGA_NCL_BEGIN

Port::Port (string id, Node *someNode, InterfacePoint *someInterfacePoint)
    : InterfacePoint (id)
{
  node = someNode;
  interfacePoint = someInterfacePoint;

  typeSet.insert ("Port");
}

Port::~Port () {}

InterfacePoint *
Port::getInterfacePoint ()
{
  return interfacePoint;
}

Node *
Port::getNode ()
{
  return node;
}

Node *
Port::getEndNode ()
{
  if (interfacePoint->instanceOf ("Anchor"))
    return node;
  else
    return ((Port *)(interfacePoint))->getEndNode ();
}

InterfacePoint *
Port::getEndInterfacePoint ()
{
  // Polimorfismo
  if (interfacePoint->instanceOf ("Anchor"))
    {
      return interfacePoint;
    }
  else
    {
      return ((Port *)interfacePoint)->getEndInterfacePoint ();
    }
}

vector<Node *> *
Port::getMapNodeNesting ()
{
  vector<Node *> *nodeSequence;
  vector<Node *> *nodeList;

  nodeSequence = new vector<Node *>;
  nodeSequence->push_back (node);
  if (interfacePoint->instanceOf ("Anchor")
      || interfacePoint->instanceOf ("SwitchPort"))
    {
      return nodeSequence;
    }
  else
    { // Port
      nodeList = ((Port *)interfacePoint)->getMapNodeNesting ();

      vector<Node *>::iterator it;
      for (it = nodeList->begin (); it != nodeList->end (); ++it)
        {
          nodeSequence->push_back (*it);
        }

      delete nodeList;
      return nodeSequence;
    }
}

void
Port::setInterfacePoint (InterfacePoint *someInterfacePoint)
{
  interfacePoint = someInterfacePoint;
}

void
Port::setNode (Node *someNode)
{
  node = someNode;
}

GINGA_NCL_END
