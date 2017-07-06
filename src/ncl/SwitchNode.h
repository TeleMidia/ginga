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
#include "SwitchPort.h"

#include "Rule.h"

GINGA_NCL_BEGIN

class SwitchNode : public Composition
{
public:
  SwitchNode (const string &_id);
  virtual ~SwitchNode ();
  void addNode (Node *node, Rule *rule);
  void addNode (Node *);
  Node *getDefaultNode ();
  Anchor *getMapInterface (Port *port);
  Node *getNode (const string &nodeId);
  const vector <Rule *> *getRules ();
  void setDefaultNode (Node *node);

private:
  Node *_defaultNode;
  vector<Rule *> _rules;
};

GINGA_NCL_END

#endif //_SWITCHNODE_H_
