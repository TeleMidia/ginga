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

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCL_BEGIN

SwitchNode::SwitchNode (const string &id) : CompositeNode (id)
{
  _content = new SwitchContent ();
  _defaultNode = NULL;
}

SwitchNode::~SwitchNode ()
{
  vector<Node *>::iterator i;
  vector<Rule *>::iterator j;

  if (_content != NULL)
    {
      delete _content;
      _content = NULL;
    }

  i = _nodes.begin ();
  j = _ruleList.begin ();
  while (i != _nodes.end ())
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
  _nodes.clear ();
  _ruleList.clear ();
}

bool
SwitchNode::addNode (unsigned int index, Node *node, Rule *rule)
{
  if (node == NULL || rule == NULL || index > _nodes.size ())
    {
      return false;
    }

  if (index == _nodes.size ())
    {
      _nodes.push_back (node);
      _ruleList.push_back (rule);
    }
  else
    {
      _nodes.insert (_nodes.begin () + index, node);
      _ruleList.insert (_ruleList.begin () + index, rule);
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

  _nodes.push_back (node);
  _ruleList.push_back (rule);

  node->setParentComposition (this);
  return true;
}

bool
SwitchNode::addNode (Node *node)
{
  if (!isDocumentNode(node))
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

void
SwitchNode::exchangeNodesAndRules (unsigned int index1, unsigned int index2)
{
  Node *auxNode;
  Rule *auxRule;

  if (index1 >= _nodes.size () || index2 >= _nodes.size ())
    {
      return;
    }

  auxNode = (Node *)(_nodes[index1]);
  auxRule = (Rule *)(_ruleList[index1]);

  _nodes[index1] = _nodes[index2];
  _nodes[index2] = auxNode;

  _ruleList[index1] = _ruleList[index2];
  _ruleList[index2] = auxRule;
}

Node *
SwitchNode::getDefaultNode ()
{
  return _defaultNode;
}

InterfacePoint *
SwitchNode::getMapInterface (Port *port)
{
  if (instanceof (SwitchPort *, port))
    {
      return port;
    }
  else
    {
      return CompositeNode::getMapInterface (port);
    }
}

Node *
SwitchNode::getNode (const string &nodeId)
{
  // verifica se o no' default possui identificador dado por nodeId
  if (_defaultNode != NULL && _defaultNode->getId () == nodeId)
    {
      return _defaultNode;
    }

  return CompositeNode::getNode (nodeId);
}

Node *
SwitchNode::getNode (unsigned int index)
{
  if (index >= _nodes.size ())
    {
      return NULL;
    }

  return (Node *)(_nodes[index]);
}

Node *
SwitchNode::getNode (Rule *rule)
{
  unsigned int index;

  index = indexOfRule (rule);
  if (index > _ruleList.size ())
    {
      return NULL;
    }

  return static_cast<Node *> (_nodes[index]);
}

unsigned int
SwitchNode::getNumRules ()
{
  return (unsigned int) _ruleList.size ();
}

Rule *
SwitchNode::getRule (unsigned int index)
{
  if (index >= _ruleList.size ())
    {
      return NULL;
    }

  return static_cast<Rule *> (_ruleList[index]);
}

unsigned int
SwitchNode::indexOfRule (Rule *rule)
{
  unsigned int i = 0;
  vector<Rule *>::iterator j;

  for (j = _ruleList.begin (); j != _ruleList.end (); ++j)
    {
      if ((*j)->getId () == rule->getId ())
        {
          return i;
        }
      i++;
    }
  return (unsigned int) _ruleList.size () + 10;
}

bool
SwitchNode::recursivelyContainsNode (Node *node)
{
  return recursivelyContainsNode (node->getId ());
}

bool
SwitchNode::recursivelyContainsNode (const string &nodeId)
{
  if (recursivelyGetNode (nodeId) != NULL)
    {
      return true;
    }

  return false;
}

Node *
SwitchNode::recursivelyGetNode (const string &nodeId)
{
  Node *wanted;

  if (_defaultNode != NULL && _defaultNode->getId () == nodeId)
    {
      return _defaultNode;
    }

  wanted = CompositeNode::recursivelyGetNode (nodeId);
  if (wanted == NULL && _defaultNode != NULL
      && instanceof (CompositeNode *, _defaultNode))
    {
      wanted = ((CompositeNode *)_defaultNode)->recursivelyGetNode (nodeId);
    }

  return wanted;
}

bool
SwitchNode::removeNode (Node *node)
{
  int i, size;
  Node *auxNode;

  size = (int) CompositeNode::_nodes.size ();
  for (i = 0; i < size; i++)
    {
      auxNode = (Node *)CompositeNode::_nodes[i];
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

  if (index >= CompositeNode::_nodes.size ())
    {
      return false;
    }

  clog << "SwitchNode::removeNode" << endl;

  node = (Node *)CompositeNode::_nodes[index];
  node->setParentComposition (NULL);

  (CompositeNode::_nodes).erase (CompositeNode::_nodes.begin () + index);
  _ruleList.erase (_ruleList.begin () + index);
  return true;
}

bool
SwitchNode::removeRule (Rule *rule)
{
  unsigned int index;

  index = indexOfRule (rule);
  if (index < _ruleList.size ())
    {
      vector<Node *>::iterator iterNode;
      iterNode = _nodes.begin ();
      iterNode = iterNode + index;

      vector<Rule *>::iterator iterRule;
      iterRule = _ruleList.begin ();
      iterRule = iterRule + index;

      _nodes.erase (iterNode);
      _ruleList.erase (iterRule);
      return true;
    }

  return false;
}

void
SwitchNode::setDefaultNode (Node *node)
{
  _defaultNode = node;
}

GINGA_NCL_END
