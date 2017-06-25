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

Port::Port (const string &id, Node *someNode, InterfacePoint *someInterfacePoint)
    : InterfacePoint (id)
{
  _node = someNode;
  _interfacePoint = someInterfacePoint;
}

Port::~Port () {}

InterfacePoint *
Port::getInterfacePoint ()
{
  return _interfacePoint;
}

Node *
Port::getNode ()
{
  return _node;
}

Node *
Port::getEndNode ()
{
  if (instanceof (Anchor *, _interfacePoint))
    return _node;
  else
    return ((Port *)(_interfacePoint))->getEndNode ();
}

InterfacePoint *
Port::getEndInterfacePoint ()
{
  // Polimorfismo
  if (instanceof (Anchor *, _interfacePoint))
    {
      return _interfacePoint;
    }
  else
    {
      return ((Port *)_interfacePoint)->getEndInterfacePoint ();
    }
}

vector<Node *> *
Port::getMapNodeNesting ()
{
  vector<Node *> *nodeSequence;
  vector<Node *> *nodeList;

  nodeSequence = new vector<Node *>;
  nodeSequence->push_back (_node);
  if (instanceof (Anchor *, _interfacePoint)
      || instanceof (SwitchPort *, _interfacePoint))
    {
      return nodeSequence;
    }
  else
    { // Port
      nodeList = ((Port *)_interfacePoint)->getMapNodeNesting ();

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
  _interfacePoint = someInterfacePoint;
}

void
Port::setNode (Node *someNode)
{
  _node = someNode;
}

GINGA_NCL_END
