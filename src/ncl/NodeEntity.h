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

#ifndef _NODEENTITY_H_
#define _NODEENTITY_H_

#include "Anchor.h"
#include "LambdaAnchor.h"
#include "ReferNode.h"

#include "Node.h"
#include "Content.h"

#include "GenericDescriptor.h"

GINGA_NCL_BEGIN

class NodeEntity : public Node
{
public:
  NodeEntity (const string &uid, Content *someContent);
  virtual ~NodeEntity ();

  LambdaAnchor *getLambdaAnchor ();
  GenericDescriptor *getDescriptor ();
  void setDescriptor (GenericDescriptor *someDescriptor);
  Content *getContent ();
  void setContent (Content *_content);
  bool addAnchor (Anchor *anchor);
  bool addAnchor (int index, Anchor *anchor);
  void setId (const string &_id);
  bool removeAnchor (int index);
  bool removeAnchor (Anchor *anchor);
  set<ReferNode *> *getInstSameInstances ();
  set<ReferNode *> *getGradSameInstances ();
  bool addSameInstance (ReferNode *node);
  void removeSameInstance (ReferNode *node);

protected:
  GenericDescriptor *_descriptor;
  Content *_content;

private:
  set<ReferNode *> _instSameInstances;
  set<ReferNode *> _gradSameInstances;
};

GINGA_NCL_END

#endif //_NODEENTITY_H_
