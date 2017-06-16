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
#include "ContentNode.h"

GINGA_NCL_BEGIN

ContentNode::ContentNode (const string &uid, Content *someContent)
    : NodeEntity (uid, someContent)
{
  initialize ("");
}

ContentNode::ContentNode (const string &uid, Content *someContent, const string &type)
    : NodeEntity (uid, someContent)
{
  initialize (type);
}

void
ContentNode::initialize (const string &type)
{
  _typeSet.insert ("ContentNode");
  _typeSet.insert ("DocumentNode");

  this->_type = type;

  // must set to false before a new isSettingNode call
  _isSettingNodeType = false;
  _isSettingNodeType = isSettingNode ();

  // must set to false before a new isTimeNode call
  _isTimeNodeType = false;
  _isTimeNodeType = isTimeNode ();
}

bool
ContentNode::isSettingNode ()
{
  string nodeType = getNodeType ();

  if (_isSettingNodeType)
    {
      return true;
    }

  if (nodeType == "")
    {
      return false;
    }

  if (xstrcaseeq (nodeType, "application/x-ginga-settings")
      || xstrcaseeq (nodeType, "application/x-ncl-settings"))
    {
      return true;
    }

  return false;
}

bool
ContentNode::isTimeNode ()
{
  string nodeType = getNodeType ();

  if (_isTimeNodeType)
    {
      return true;
    }

  if (nodeType == "")
    {
      return false;
    }

  if (xstrcaseeq (nodeType, "application/x-ginga-time")
      || xstrcaseeq (nodeType, "application/x-ncl-time"))
    {
      return true;
    }

  return false;
}

string
ContentNode::getTypeValue ()
{
  return _type;
}

string
ContentNode::getNodeType ()
{
  if (_type != "")
    {
      return _type;
    }
  else if (_content != NULL)
    {
      return _content->getType ();
    }
  else
    {
      return "";
    }
}

void
ContentNode::setNodeType (const string &type)
{
  this->_type = type;

  // must set to false before a new isSettingNode call
  _isSettingNodeType = false;
  _isSettingNodeType = isSettingNode ();
}

GINGA_NCL_END
