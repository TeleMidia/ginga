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

#ifndef _LINK_H_
#define _LINK_H_

#include "Bind.h"
#include "Connector.h"
#include "ContextNode.h"
#include "Entity.h"
#include "Descriptor.h"
#include "InterfacePoint.h"
#include "NodeEntity.h"
#include "Parameter.h"
#include "Role.h"
#include "SwitchPort.h"

GINGA_NCL_BEGIN

class ContextNode;
class Link : public Entity
{
public:
  Link (const string &_id, Connector *connector);
  virtual ~Link ();

  Bind *bind (Node *node, InterfacePoint *interfPt, Descriptor *desc,
              string roleId);

  Bind *bind (Node *node, InterfacePoint *interfPt, Descriptor *desc,
              Role *role);

  bool isConsistent ();
  Bind *getBind (Node *node, InterfacePoint *interfPt,
                 Descriptor *desc, Role *role);

  vector<Bind *> *getBinds ();
  Connector *getConnector ();
  void setParentComposition (ContextNode *composition);
  ContextNode *getParentComposition ();
  unsigned int getNumBinds ();
  unsigned int getNumRoleBinds (Role *role);
  vector<Bind *> *getRoleBinds (Role *role);
  bool isMultiPoint ();
  void setConnector (Connector *connector);
  bool unBind (Bind *bind);
  void addParameter (Parameter *parameter);
  vector<Parameter *> *getParameters ();
  Parameter *getParameter (const string &name);
  void removeParameter (Parameter *parameter);
  void updateConnector (Connector *newConnector);
  bool containsNode (Node *node, Descriptor *descriptor);

protected:
  Connector *_connector;
  map<string, vector<Bind *> *> *_roleBinds;
  vector<Bind *> *_binds;
  map<string, Parameter *> *_parameters;

  bool containsNode (Node *node, Descriptor *descriptor,
                     vector<Bind *> *_binds);

private:
  ContextNode *_composition;
};

GINGA_NCL_END

#endif //_LINK_H_
