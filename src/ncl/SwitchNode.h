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

#ifndef _SWITCHNODE_H_
#define _SWITCHNODE_H_

#include "Context.h"
#include "Composition.h"
#include "Node.h"

#include "Port.h"
#include "Interface.h"
#include "SwitchPort.h"

#include "Rule.h"

GINGA_NCL_BEGIN

class SwitchNode : public Composition
{
public:
  SwitchNode (const string &_id);
  virtual ~SwitchNode ();
  void addNode (unsigned int index, Node *node, Rule *rule);
  void addNode (Node *node, Rule *rule);

  // virtual from Composition
  void addNode (Node *);

  bool addSwitchPortMap (SwitchPort *switchPort, Node *node,
                         Interface *interfacePoint);

  Node *getDefaultNode ();

  // virtual from Composition
  Interface *getMapInterface (Port *port);

  // virtual from Composition
  Node *getNode (const string &nodeId);

  Node *getNode (unsigned int index);
  Node *getNode (Rule *rule);
  unsigned int getNumRules ();
  Rule *getRule (unsigned int index);
  unsigned int indexOfRule (Rule *rule);

  // virtual from Composition
  bool recursivelyContainsNode (Node *node);

  // virtual from Composition
  bool recursivelyContainsNode (const string &nodeId);

  // virtual from Composition
  Node *recursivelyGetNode (const string &nodeId);

  // virtual from Composition
  bool removeNode (Node *node);

  bool removeNode (unsigned int index);
  bool removeRule (Rule *rule);
  void setDefaultNode (Node *node);

private:
  Node *_defaultNode;
  vector<Rule *> _ruleList;
};

GINGA_NCL_END

#endif //_SWITCHNODE_H_
