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

GINGA_NCL_BEGIN

class ContentNode : public NodeEntity
{
public:
  ContentNode (const string &uid, Content *someContent);
  ContentNode (const string &uid, Content *_content, const string &_type);
  virtual ~ContentNode (){}

  bool isSettingNode ();
  bool isTimeNode ();
  string getTypeValue ();
  string getNodeType ();
  void setNodeType (const string &_type);

private:
  string _type;
  bool _isSettingNodeType;
  bool _isTimeNodeType;

  void initialize (const string &_type);
};

GINGA_NCL_END

#endif /* CONTENT_NODE_H */
