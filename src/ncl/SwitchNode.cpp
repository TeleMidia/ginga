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
#include "SwitchNode.h"

GINGA_NCL_BEGIN

SwitchNode::SwitchNode (string id) : CompositeNode (id)
{
  content = new SwitchContent ();
  defaultNode = NULL;

  typeSet.insert ("SwitchNode");
  typeSet.insert ("DocumentNode");
}

SwitchNode::~SwitchNode ()
{
  vector<Node *>::iterator i;
  vector<Rule *>::iterator j;

  if (content != NULL)
    {
      delete content;
      content = NULL;
    }

  i = nodes.begin ();
  j = ruleList.begin ();
  while (i != nodes.end ())
    {
      if ((*i) != this && (*i)->getParentComposition () == this)
        {
          (*i)->setParentComposition (NULL);
          /*clog << "SwitchNode::~SwitchNode call delete node '";
          clog << (*i)->getId() << "'" << endl;*/
          delete (*i);

          /*clog << "SwitchNode::~SwitchNode call delete rule '";
          clog << (*j)->getId() << "'" << endl;
          delete (*j);*/
        }
      ++i;
      //++j;
    }
  nodes.clear ();
  ruleList.clear ();
}

bool
SwitchNode::addNode (unsigned int index, Node *node, Rule *rule)
{
  if (node == NULL || rule == NULL || index > nodes.size ())
    {
      return false;
    }

  if (index == nodes.size ())
    {
      nodes.push_back (node);
      ruleList.push_back (rule);
    }
  else
    {
      nodes.insert (nodes.begin () + index, node);
      ruleList.insert (ruleList.begin () + index, rule);
    }

  node->setParentComposition (this);
  return true;
}

bool
SwitchNode::addNode (Node *node, Rule *rule)
{
  if (node == NULL || rule == NULL)
    {
      return false;
    }

  nodes.push_back (node);
  ruleList.push_back (rule);

  node->setParentComposition (this);
  return true;
}

bool
SwitchNode::addNode (Node *node)
{
  if (!node->instanceOf ("DocumentNode"))
    {
      return false;
    }

  setDefaultNode (node);
  return true;
}

bool
SwitchNode::addSwitchPortMap (SwitchPort *switchPort, Node *node,
                              InterfacePoint *interfacePoint)
{
  Port *port;

  if (getNode (node->getId ()) == NULL
      || getPort (switchPort->getId ()) == NULL)
    {
      return false;
    }

  port = new Port (switchPort->getId (), node, interfacePoint);
  return switchPort->addPort (port);
}

bool
SwitchNode::addPort (Port *port)
{
  return addPort ((int) portList.size (), port);
}

bool
SwitchNode::addPort (int index, Port *port)
{
  if (!(port->instanceOf ("SwitchPort")))
    {
      return false;
    }

  return CompositeNode::addPort (index, port);
}

void
SwitchNode::exchangeNodesAndRules (unsigned int index1, unsigned int index2)
{
  Node *auxNode;
  Rule *auxRule;

  if (index1 >= nodes.size () || index2 >= nodes.size ())
    {
      return;
    }

  auxNode = (Node *)(nodes[index1]);
  auxRule = (Rule *)(ruleList[index1]);

  nodes[index1] = nodes[index2];
  nodes[index2] = auxNode;

  ruleList[index1] = ruleList[index2];
  ruleList[index2] = auxRule;
}

Node *
SwitchNode::getDefaultNode ()
{
  return defaultNode;
}

InterfacePoint *
SwitchNode::getMapInterface (Port *port)
{
  if (port->instanceOf ("SwitchPort"))
    {
      return port;
    }
  else
    {
      return CompositeNode::getMapInterface (port);
    }
}

Node *
SwitchNode::getNode (string nodeId)
{
  // verifica se o no' default possui identificador dado por nodeId
  if (defaultNode != NULL && defaultNode->getId () == nodeId)
    {
      return defaultNode;
    }

  return CompositeNode::getNode (nodeId);
}

Node *
SwitchNode::getNode (unsigned int index)
{
  if (index >= nodes.size ())
    {
      return NULL;
    }

  return (Node *)(nodes[index]);
}

Node *
SwitchNode::getNode (Rule *rule)
{
  unsigned int index;

  index = indexOfRule (rule);
  if (index > ruleList.size ())
    {
      return NULL;
    }

  return static_cast<Node *> (nodes[index]);
}

unsigned int
SwitchNode::getNumRules ()
{
  return (unsigned int) ruleList.size ();
}

Rule *
SwitchNode::getRule (unsigned int index)
{
  if (index >= ruleList.size ())
    {
      return NULL;
    }

  return static_cast<Rule *> (ruleList[index]);
}

unsigned int
SwitchNode::indexOfRule (Rule *rule)
{
  unsigned int i = 0;
  vector<Rule *>::iterator j;

  for (j = ruleList.begin (); j != ruleList.end (); ++j)
    {
      if ((*j)->getId () == rule->getId ())
        {
          return i;
        }
      i++;
    }
  return (unsigned int) ruleList.size () + 10;
}

bool
SwitchNode::recursivelyContainsNode (Node *node)
{
  return recursivelyContainsNode (node->getId ());
}

bool
SwitchNode::recursivelyContainsNode (string nodeId)
{
  if (recursivelyGetNode (nodeId) != NULL)
    {
      return true;
    }

  return false;
}

Node *
SwitchNode::recursivelyGetNode (string nodeId)
{
  Node *wanted;

  if (defaultNode != NULL && defaultNode->getId () == nodeId)
    {
      return defaultNode;
    }

  wanted = CompositeNode::recursivelyGetNode (nodeId);
  if (wanted == NULL && defaultNode != NULL
      && defaultNode->instanceOf ("CompositeNode"))
    {
      wanted = ((CompositeNode *)defaultNode)->recursivelyGetNode (nodeId);
    }

  return wanted;
}

bool
SwitchNode::removeNode (Node *node)
{
  int i, size;
  Node *auxNode;

  size = (int) CompositeNode::nodes.size ();
  for (i = 0; i < size; i++)
    {
      auxNode = (Node *)CompositeNode::nodes[i];
      if (auxNode->getId () == node->getId ())
        {
          return removeNode (i);
        }
    }
  return false;
}

bool
SwitchNode::removeNode (unsigned int index)
{
  Node *node;

  if (index >= CompositeNode::nodes.size ())
    {
      return false;
    }

  clog << "SwitchNode::removeNode" << endl;

  node = (Node *)CompositeNode::nodes[index];
  node->setParentComposition (NULL);

  (CompositeNode::nodes).erase (CompositeNode::nodes.begin () + index);
  ruleList.erase (ruleList.begin () + index);
  return true;
}

bool
SwitchNode::removeRule (Rule *rule)
{
  unsigned int index;

  index = indexOfRule (rule);
  if (index < ruleList.size ())
    {
      vector<Node *>::iterator iterNode;
      iterNode = nodes.begin ();
      iterNode = iterNode + index;

      vector<Rule *>::iterator iterRule;
      iterRule = ruleList.begin ();
      iterRule = iterRule + index;

      nodes.erase (iterNode);
      ruleList.erase (iterRule);
      return true;
    }

  return false;
}

void
SwitchNode::setDefaultNode (Node *node)
{
  defaultNode = node;
}

GINGA_NCL_END
