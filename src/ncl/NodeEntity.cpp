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
#include "NodeEntity.h"
#include "CompositeNode.h"

GINGA_NCL_BEGIN

NodeEntity::NodeEntity (const string &uid, Content *someContent) : Node (uid)
{
  _descriptor = NULL;
  _anchorList.push_back (new AreaLambda (uid));
  this->_content = someContent;
}

NodeEntity::~NodeEntity ()
{
  vector<Anchor *>::iterator i;
  set<ReferNode *>::iterator j;

  if (_descriptor != NULL)
    {
      // descriptor is deleted in descriptor base
      _descriptor = NULL;
    }

  if (_content != NULL)
    {
      delete _content;
      _content = NULL;
    }

  _instSameInstances.clear ();
  _anchorList.clear ();
}

bool
NodeEntity::addAnchor (int index, Anchor *anchor)
{
  if (index == 0)
    {
      return false;
    }
  return Node::addAnchor (index, anchor);
}

AreaLambda *
NodeEntity::getAreaLambda ()
{
  AreaLambda *lambda;
  lambda = static_cast<AreaLambda *> (*(_anchorList.begin ()));
  return lambda;
}

Descriptor *
NodeEntity::getDescriptor ()
{
  return _descriptor;
}

void
NodeEntity::setDescriptor (Descriptor *someDescriptor)
{
  _descriptor = someDescriptor;
}

Content *
NodeEntity::getContent ()
{
  return _content;
}

void
NodeEntity::setContent (Content *someContent)
{
  _content = someContent;
}

bool
NodeEntity::addAnchor (Anchor *anchor)
{
  return Node::addAnchor (anchor);
}

set<ReferNode *> *
NodeEntity::getInstSameInstances ()
{
  return &_instSameInstances;
}

bool
NodeEntity::addSameInstance (ReferNode *node)
{
  if (node->getInstanceType () == "instSame")
    {
      if (_instSameInstances.count (node) != 0)
        {
          return false;
        }

      _instSameInstances.insert (node);
    }
  return true;
}

void
NodeEntity::removeSameInstance (ReferNode *node)
{
  set<ReferNode *>::iterator i;

  i = _instSameInstances.find (node);
  if (i != _instSameInstances.end ())
    {
      _instSameInstances.erase (i);
    }
}

GINGA_NCL_END
