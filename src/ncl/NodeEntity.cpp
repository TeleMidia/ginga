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

NodeEntity::NodeEntity (string uid, Content *someContent) : Node (uid)
{
  descriptor = NULL;
  anchorList.push_back (new LambdaAnchor (uid));
  this->content = someContent;
  typeSet.insert ("NodeEntity");
}

NodeEntity::~NodeEntity ()
{
  vector<Anchor *>::iterator i;
  set<ReferNode *>::iterator j;
  Anchor *anchor;

  if (descriptor != NULL)
    {
      // descriptor is deleted in descriptor base
      descriptor = NULL;
    }

  if (content != NULL)
    {
      delete content;
      content = NULL;
    }

  for (j = instSameInstances.begin (); j != instSameInstances.end (); ++j)
    {
      if ((Node *)(*j) != (Node *)this && Entity::hasInstance ((*j), true))
        {
          delete (*j);
        }
    }
  instSameInstances.clear ();

  for (j = gradSameInstances.begin (); j != gradSameInstances.end (); ++j)
    {
      delete (*j);
    }
  gradSameInstances.clear ();

  i = anchorList.begin ();
  while (i != anchorList.end ())
    {
      anchor = (*i);
      if (Entity::hasInstance (anchor, true))
        {
          delete (*i);
        }
      ++i;
    }
  anchorList.clear ();
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

LambdaAnchor *
NodeEntity::getLambdaAnchor ()
{
  LambdaAnchor *lambda;
  lambda = static_cast<LambdaAnchor *> (*(anchorList.begin ()));
  return lambda;
}

void
NodeEntity::setId (string id)
{
  LambdaAnchor *anchor;

  Entity::setId (id);
  anchor = getLambdaAnchor ();
  anchor->setId (id);
}

bool
NodeEntity::removeAnchor (int index)
{
  if (index == 0)
    {
      return false;
    }
  return Node::removeAnchor (index);
}

GenericDescriptor *
NodeEntity::getDescriptor ()
{
  return descriptor;
}

void
NodeEntity::setDescriptor (GenericDescriptor *someDescriptor)
{
  descriptor = someDescriptor;
}

Content *
NodeEntity::getContent ()
{
  return content;
}

void
NodeEntity::setContent (Content *someContent)
{
  content = someContent;
}

bool
NodeEntity::addAnchor (Anchor *anchor)
{
  return Node::addAnchor (anchor);
}

bool
NodeEntity::removeAnchor (Anchor *anchor)
{
  return Node::removeAnchor (anchor);
}

set<ReferNode *> *
NodeEntity::getInstSameInstances ()
{
  if (instSameInstances.empty ())
    {
      return NULL;
    }

  return &instSameInstances;
}

set<ReferNode *> *
NodeEntity::getGradSameInstances ()
{
  if (gradSameInstances.empty ())
    {
      return NULL;
    }

  return &gradSameInstances;
}

bool
NodeEntity::addSameInstance (ReferNode *node)
{
  if (node->getInstanceType () == "instSame")
    {
      if (instSameInstances.count (node) != 0)
        {
          return false;
        }

      instSameInstances.insert (node);
    }
  else if (node->getInstanceType () == "gradSame")
    {
      if (gradSameInstances.count (node) != 0)
        {
          return false;
        }

      gradSameInstances.insert (node);
    }
  else
    {
      return false;
    }

  return true;
}

void
NodeEntity::removeSameInstance (ReferNode *node)
{
  set<ReferNode *>::iterator i;

  i = gradSameInstances.find (node);
  if (i != gradSameInstances.end ())
    {
      gradSameInstances.erase (i);
    }

  i = instSameInstances.find (node);
  if (i != instSameInstances.end ())
    {
      instSameInstances.erase (i);
    }
}

GINGA_NCL_END
