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
#include "CompositeNode.h"

GINGA_NCL_BEGIN

CompositeNode::CompositeNode (string id) : NodeEntity (id, NULL)
{
  typeSet.insert ("CompositeNode");
}

CompositeNode::~CompositeNode ()
{
  vector<Port *>::iterator i;

  nodes.clear ();

  i = portList.begin ();
  while (i != portList.end ())
    {
      delete *i;
      ++i;
    }
  portList.clear ();
}

bool
CompositeNode::addAnchor (int index, Anchor *anchor)
{
  if (anchor == NULL)
    {
      clog << "CompositeNode::addAnchor Warning! Trying to add a";
      clog << " NULL anchor" << endl;
      return false;
    }

  string anchorId;
  anchorId = anchor->getId ();
  if (getPort (anchorId) != NULL)
    {
      return false;
    }

  return NodeEntity::addAnchor (index, anchor);
}

bool
CompositeNode::addAnchor (Anchor *anchor)
{
  return CompositeNode::addAnchor ((int) anchorList.size (), anchor);
}

bool
CompositeNode::addPort (unsigned int index, Port *port)
{
  if (index > portList.size () || port == NULL
      || NodeEntity::getAnchor (port->getId ()) != NULL
      || getPort (port->getId ()) != NULL)
    {

      /*clog << "CompositeNode::addPort Warning! Can't add port '";
      clog << port->getId() << "' inside '" << getId() << "'" << endl;*/
      return false;
    }

  if (index == portList.size ())
    {
      portList.push_back (port);
    }
  else
    {
      portList.insert (portList.begin () + index, port);
    }

  return true;
}

bool
CompositeNode::addPort (Port *port)
{
  return addPort ((int) portList.size (), port);
}

void
CompositeNode::clearPorts ()
{
  portList.clear ();
}

unsigned int
CompositeNode::getNumPorts ()
{
  return (int) portList.size ();
}

Port *
CompositeNode::getPort (string portId)
{
  if (portId == "")
    {
      return NULL;
    }

  vector<Port *>::iterator i;

  for (i = portList.begin (); i != portList.end (); ++i)
    {
      if ((*i)->getId () == portId)
        {
          return (*i);
        }
    }
  return NULL;
}

Port *
CompositeNode::getPort (unsigned int index)
{
  if (index >= portList.size ())
    return NULL;

  return portList[index];
}

vector<Port *> *
CompositeNode::getPorts ()
{
  if (portList.empty ())
    return NULL;

  return &portList;
}

unsigned int
CompositeNode::indexOfPort (Port *port)
{
  unsigned int i = 0;
  vector<Port *>::iterator it;

  for (it = portList.begin (); it != portList.end (); it++)
    {
      if ((*it)->getId () == port->getId ())
        {
          return i;
        }
      i++;
    }
  return (int) portList.size () + 10;
}

bool
CompositeNode::removePort (Port *port)
{
  vector<Port *>::iterator it;
  for (portList.begin (); it != portList.end (); it++)
    {
      if (*it == port)
        {
          portList.erase (it);
          return true;
        }
    }
  return false;
}

InterfacePoint *
CompositeNode::getMapInterface (Port *port)
{
  Node *node;
  CompositeNode *compositeNode;
  InterfacePoint *interfacePoint;

  node = port->getNode ();
  interfacePoint = port->getInterfacePoint ();
  if (interfacePoint->instanceOf ("Port"))
    {
      compositeNode = (CompositeNode *)node->getDataEntity ();
      return compositeNode->getMapInterface ((Port *)interfacePoint);
    }
  else
    {
      return (Anchor *)interfacePoint;
    }
}

Node *
CompositeNode::getNode (string nodeId)
{
  vector<Node *>::iterator i;

  for (i = nodes.begin (); i != nodes.end (); ++i)
    {
      if ((*i)->getId () == nodeId)
        {
          return (*i);
        }
    }

  return NULL;
}

vector<Node *> *
CompositeNode::getNodes ()
{
  return &nodes;
}

unsigned int
CompositeNode::getNumNodes ()
{
  return (unsigned int) nodes.size ();
}

bool
CompositeNode::recursivelyContainsNode (string nodeId)
{
  if (recursivelyGetNode (nodeId) != NULL)
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool
CompositeNode::recursivelyContainsNode (Node *node)
{
  vector<Node *>::iterator it;
  Node *childNode;
  CompositeNode *compositeNode;
  unsigned int i;

  if (nodes.empty ())
    {
      return false;
    }

  for (i = 0; i < nodes.size (); i++)
    {
      childNode = nodes[i];
      if (childNode == node)
        {
          return true;
        }
    }

  for (it = nodes.begin (); it != nodes.end (); ++it)
    {
      childNode = (Node *)*it;
      if (childNode->instanceOf ("CompositeNode"))
        {
          compositeNode = (CompositeNode *)childNode;
          if (compositeNode->recursivelyContainsNode (node))
            {
              return true;
            }
        }
      else if (childNode->instanceOf ("ReferNode"))
        {
          childNode
              = (Node *)(((ReferNode *)childNode)->getReferredEntity ());

          if (childNode == node)
            {
              return true;
            }
          else if (childNode->instanceOf ("CompositeNode"))
            {
              compositeNode = (CompositeNode *)childNode;
              if (compositeNode->recursivelyContainsNode (node))
                {
                  return true;
                }
            }
        }
    }
  return false;
}

Node *
CompositeNode::recursivelyGetNode (string nodeId)
{
  Node *node;
  vector<Node *>::iterator i;

  if (nodes.empty ())
    {
      return NULL;
    }

  for (i = nodes.begin (); i != nodes.end (); ++i)
    {
      if (((*i)->getId ()).compare (nodeId) == 0)
        {
          return (*i);
        }

      if ((*i)->instanceOf ("CompositeNode"))
        {
          node = ((CompositeNode *)(*i))->recursivelyGetNode (nodeId);
          if (node != NULL)
            {
              return node;
            }
        }
    }

  return NULL;
}

bool
CompositeNode::removeNode (Node *node)
{
  vector<Node *>::iterator it;

  clog << "CompositeNode::removeNode" << endl;

  if (nodes.empty ())
    {
      return false;
    }

  for (it = nodes.begin (); it != nodes.end (); ++it)
    {
      if (*it == node)
        {
          break;
        }
    }

  if (it != nodes.end ())
    {
      node->setParentComposition (NULL);
      nodes.erase (it);
      return true;
    }
  else
    {
      return false;
    }
}

GenericDescriptor *
CompositeNode::getNodeDescriptor (arg_unused (Node *node))
{
  return NULL;
}

bool
CompositeNode::setNodeDescriptor (arg_unused (string nodeId),
                                  arg_unused (GenericDescriptor *descriptor))
{
  return false;
}

GINGA_NCL_END
