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

#ifndef COMPOSITE_NODE_H
#define COMPOSITE_NODE_H

#include "NodeEntity.h"
#include "Port.h"

GINGA_NCL_BEGIN

class CompositeNode : public NodeEntity
{
public:
  CompositeNode (const string &);
  virtual ~CompositeNode ();

  // Virtual to all.
  virtual bool addNode (Node *) = 0;

  // Virtual to SwitchNode.
  virtual void addPort (Port *);

  Port *getPort (const string &);
  const vector<Node *> *getNodes ();
  const vector<Port *> *getPorts ();

  // Virtual to SwitchNode.
  virtual InterfacePoint *getMapInterface (Port *);

  // Virtual to SwitchNode.
  virtual Node *getNode (const string &nodeId);

  // Virtual to SwitchNode.
  virtual bool recursivelyContainsNode (const string &);

  // Virtual to SwitchNode.
  virtual bool recursivelyContainsNode (Node *);

  // Virtual to SwitchNode.
  virtual Node *recursivelyGetNode (const string &);

protected:
  vector<Node *> _nodes;
  vector<Port *> _ports;
  static bool
  isDocumentNode (Node *node);
};

GINGA_NCL_END

#endif // COMPOSITE_NODE_H
