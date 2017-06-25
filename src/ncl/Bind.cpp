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
#include "Bind.h"

GINGA_NCL_BEGIN

Bind::Bind (Node *node, InterfacePoint *interfPt, GenericDescriptor *desc,
            Role *role)
{
  this->_node = node;
  this->_interfacePoint = interfPt;
  this->_descriptor = desc;
  this->_role = role;
  this->_parameters = new map<string, Parameter *>;

  _typeSet.insert ("Bind");
}

Bind::~Bind ()
{
  map<string, Parameter *>::iterator i;

  if (_parameters != NULL)
    {
      i = _parameters->begin ();
      while (i != _parameters->end ())
        {
          delete i->second;
          ++i;
        }
      delete _parameters;
      _parameters = NULL;
    }
}

bool
Bind::instanceOf (const string &s)
{
  if (_typeSet.empty ())
    return false;
  else
    return (_typeSet.find (s) != _typeSet.end ());
}

GenericDescriptor *
Bind::getDescriptor ()
{
  return _descriptor;
}

InterfacePoint *
Bind::getInterfacePoint ()
{
  return this->_interfacePoint;
}

Node *
Bind::getNode ()
{
  return this->_node;
}

Role *
Bind::getRole ()
{
  return this->_role;
}

void
Bind::setInterfacePoint (InterfacePoint *interfPt)
{
  this->_interfacePoint = interfPt;
}

void
Bind::setNode (Node *node)
{
  this->_node = node;
}

void
Bind::setRole (Role *role)
{
  this->_role = role;
}

void
Bind::setDescriptor (GenericDescriptor *desc)
{
  this->_descriptor = desc;
}

Parameter *
Bind::setParameterValue (const string &propertyLabel, Parameter *propertyValue)
{
  return (*_parameters)[propertyLabel] = propertyValue;
}

void
Bind::addParameter (Parameter *parameter)
{
  if (parameter == NULL || _parameters == NULL)
    {
      return;
    }

  (*_parameters)[parameter->getName ()] = parameter;
}

vector<Parameter *> *
Bind::getParameters ()
{
  if (_parameters->empty ())
    return NULL;

  map<string, Parameter *>::iterator i;
  vector<Parameter *> *params;

  params = new vector<Parameter *>;
  for (i = _parameters->begin (); i != _parameters->end (); ++i)
    params->push_back (i->second);

  return params;
}

Parameter *
Bind::getParameter (const string &paramName)
{
  if (_parameters->empty ())
    return NULL;

  map<string, Parameter *>::iterator i;
  for (i = _parameters->begin (); i != _parameters->end (); ++i)
    if (i->first == paramName)
      return (Parameter *)(i->second);

  return NULL;
}

bool
Bind::removeParameter (Parameter *parameter)
{
  if (_parameters->empty ())
    return false;

  map<string, Parameter *>::iterator i;

  for (i = _parameters->begin (); i != _parameters->end (); ++i)
    {
      if (i->first == parameter->getName ())
        {
          _parameters->erase (i);
          return true;
        }
    }

  return false;
}

vector<Node *> *
Bind::getNodeNesting ()
{
  vector<Node *> *nodeNesting;
  vector<Node *> *nodeSequence;
  vector<Node *>::iterator i;

  nodeNesting = new vector<Node *>;
  nodeNesting->push_back (_node);
  if (_interfacePoint != NULL)
    {
      if (instanceof (Port *, _interfacePoint)
          && !(instanceof (SwitchPort *, _interfacePoint)))
        {
          nodeSequence = ((Port *)_interfacePoint)->getMapNodeNesting ();

          if (nodeSequence != NULL)
            {
              for (i = nodeSequence->begin (); i != nodeSequence->end ();
                   ++i)
                {
                  nodeNesting->push_back (*i);
                }
            }

          delete nodeSequence;
        }
    }

  return nodeNesting;
}

InterfacePoint *
Bind::getEndPointInterface ()
{
  NodeEntity *nodeEntity;
  CompositeNode *compositeNode;
  Port *port;

  nodeEntity = (NodeEntity *)(_node->getDataEntity ());
  if (instanceof (CompositeNode *, nodeEntity)
      && instanceof (Port *, _interfacePoint))
    {
      compositeNode = (CompositeNode *)nodeEntity;
      port = (Port *)_interfacePoint;
      return compositeNode->getMapInterface (port);
    }
  else
    {
      return _interfacePoint;
    }
}

GINGA_NCL_END
