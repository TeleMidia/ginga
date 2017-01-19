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
#include "ReferNode.h"

#include "NodeEntity.h"

GINGA_NCL_BEGIN

ReferNode::ReferNode (string id) : Node (id)
{
  typeSet.insert ("ReferNode");
  typeSet.insert ("DocumentNode");
  typeSet.insert ("ReferEntity");
  instanceType = "new";
  referredNode = NULL;
  instDesc = NULL;
}

ReferNode::ReferNode (string id, Entity *entity) : Node (id)
{
  typeSet.insert ("ReferNode");
  typeSet.insert ("DocumentNode");
  typeSet.insert ("ReferEntity");
  instanceType = "new";
  instDesc = NULL;

  setReferredEntity (entity);
}

GenericDescriptor *
ReferNode::getInstanceDescriptor ()
{
  GenericDescriptor *desc = NULL;

  if (instanceType == "new")
    {
      desc = instDesc;
    }

  return desc;
}

void
ReferNode::setInstanceDescriptor (GenericDescriptor *descriptor)
{
  if (instanceType == "new")
    {
      instDesc = descriptor;
    }
}

string
ReferNode::getInstanceType ()
{
  return instanceType;
}

void
ReferNode::setInstanceType (string instance)
{
  if (instance != this->instanceType)
    {
      // if it was new
      if (this->instanceType == "new")
        {
          this->instanceType = instance;
          if (referredNode != NULL
              && referredNode->instanceOf ("NodeEntity"))
            {

              ((NodeEntity *)referredNode)->addSameInstance (this);
            }
        }
      else
        {
          this->instanceType = instance;
          if (referredNode != NULL
              && referredNode->instanceOf ("NodeEntity"))
            {

              if (instance == "new")
                {
                  ((NodeEntity *)referredNode)->removeSameInstance (this);
                }
            }
        }
    }
}

Entity *
ReferNode::getReferredEntity ()
{
  return referredNode;
}

void
ReferNode::setReferredEntity (Entity *entity)
{
  if (referredNode != entity)
    {
      if (referredNode != NULL && referredNode->instanceOf ("NodeEntity"))
        {

          ((NodeEntity *)referredNode)->removeSameInstance (this);
        }

      referredNode = entity;

      if (referredNode != NULL && referredNode->instanceOf ("NodeEntity")
          && instanceType != "new")
        {

          ((NodeEntity *)referredNode)->addSameInstance (this);
        }
    }
}

Entity *
ReferNode::getDataEntity ()
{
  return referredNode;
}

GINGA_NCL_END
