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

#ifndef _COMPOSITENODE_H_
#define _COMPOSITENODE_H_

#include "InterfacePoint.h"
#include "Port.h"
#include "Anchor.h"

#include "Node.h"

GINGA_NCL_BEGIN

class CompositeNode : public NodeEntity
{
protected:
  vector<Node *> nodes;
  vector<Port *> portList;

public:
  CompositeNode (const string &id);
  virtual ~CompositeNode ();
  bool addAnchor (int index, Anchor *anchor);
  bool addAnchor (Anchor *anchor);

  // virtual to all
  virtual bool addNode (Node *node) = 0;

  // virtual to SwitchNode
  virtual bool addPort (unsigned int index, Port *port);

  // virtual to SwitchNode
  virtual bool addPort (Port *port);

  void clearPorts ();
  unsigned int getNumPorts ();
  Port *getPort (const string &portId);
  Port *getPort (unsigned int index);
  vector<Port *> *getPorts ();
  unsigned int indexOfPort (Port *port);
  bool removePort (Port *port);

  // virtual to SwitchNode
  virtual InterfacePoint *getMapInterface (Port *port);

  // virtual to SwitchNode
  virtual Node *getNode (const string &nodeId);

  vector<Node *> *getNodes ();
  unsigned int getNumNodes ();

  // virtual to SwitchNode
  virtual bool recursivelyContainsNode (const string &nodeId);

  // virtual to SwitchNode
  virtual bool recursivelyContainsNode (Node *node);

  // virtual to SwitchNode
  virtual Node *recursivelyGetNode (const string &nodeId);

  // virtual to SwitchNode
  virtual bool removeNode (Node *node);

  // virtual to ContextNode
  virtual GenericDescriptor *getNodeDescriptor (Node *node);

  // virtual to ContextNode
  virtual bool setNodeDescriptor (const string &nodeId,
                                  GenericDescriptor *descriptor);
};

GINGA_NCL_END

#endif //_COMPOSITENODE_H_
