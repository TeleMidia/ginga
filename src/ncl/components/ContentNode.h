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

#ifndef CONTENT_NODE_H
#define CONTENT_NODE_H

#include "ginga.h"

#include "NodeEntity.h"
#include "Content.h"
using namespace ::br::pucrio::telemidia::ncl::components;

BR_PUCRIO_TELEMIDIA_NCL_COMPONENTS_BEGIN

class ContentNode : public NodeEntity
{
private:
  string type;
  bool isSettingNodeType;
  bool isTimeNodeType;

public:
  ContentNode(string uid, Content* someContent);
  ContentNode(string uid, Content* content, string type);
  virtual ~ContentNode(){};

private:
  void initialize(string type);

public:
  bool isSettingNode();
  bool isTimeNode();
  string getTypeValue();
  string getNodeType();
  void setNodeType(string type);
};

BR_PUCRIO_TELEMIDIA_NCL_COMPONENTS_END

#endif /* CONTENT_NODE_H */
