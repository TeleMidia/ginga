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

  this->node = node;
  this->interfacePoint = interfPt;
  this->descriptor = desc;
  this->role = role;
  this->parameters = new map<string, Parameter *>;

  typeSet.insert ("Bind");
}

Bind::~Bind ()
{
  map<string, Parameter *>::iterator i;

  if (parameters != NULL)
    {
      i = parameters->begin ();
      while (i != parameters->end ())
        {
          delete i->second;
          ++i;
        }
      delete parameters;
      parameters = NULL;
    }
}

bool
Bind::instanceOf (string s)
{
  if (typeSet.empty ())
    return false;
  else
    return (typeSet.find (s) != typeSet.end ());
}

GenericDescriptor *
Bind::getDescriptor ()
{
  return descriptor;
}

InterfacePoint *
Bind::getInterfacePoint ()
{
  return this->interfacePoint;
}

Node *
Bind::getNode ()
{
  return this->node;
}

Role *
Bind::getRole ()
{
  return this->role;
}

void
Bind::setInterfacePoint (InterfacePoint *interfPt)
{
  this->interfacePoint = interfPt;
}

void
Bind::setNode (Node *node)
{
  this->node = node;
}

void
Bind::setRole (Role *role)
{
  this->role = role;
}

void
Bind::setDescriptor (GenericDescriptor *desc)
{
  this->descriptor = desc;
}

Parameter *
Bind::setParameterValue (string propertyLabel, Parameter *propertyValue)
{

  return (*parameters)[propertyLabel] = propertyValue;
}

string
Bind::toString ()
{
  return (getRole ()->getLabel () + '/' + getNode ()->getId () + '/'
          + getInterfacePoint ()->getId ());
}

void
Bind::addParameter (Parameter *parameter)
{
  if (parameter == NULL || parameters == NULL)
    {
      return;
    }

  (*parameters)[parameter->getName ()] = parameter;
}

vector<Parameter *> *
Bind::getParameters ()
{
  if (parameters->empty ())
    return NULL;

  map<string, Parameter *>::iterator i;
  vector<Parameter *> *params;

  params = new vector<Parameter *>;
  for (i = parameters->begin (); i != parameters->end (); ++i)
    params->push_back (i->second);

  return params;
}

Parameter *
Bind::getParameter (string paramName)
{
  if (parameters->empty ())
    return NULL;

  map<string, Parameter *>::iterator i;
  for (i = parameters->begin (); i != parameters->end (); ++i)
    if (i->first == paramName)
      return (Parameter *)(i->second);

  return NULL;
}

bool
Bind::removeParameter (Parameter *parameter)
{
  if (parameters->empty ())
    return false;

  map<string, Parameter *>::iterator i;

  for (i = parameters->begin (); i != parameters->end (); ++i)
    {
      if (i->first == parameter->getName ())
        {
          parameters->erase (i);
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
  nodeNesting->push_back (node);
  if (interfacePoint != NULL)
    {
      if (interfacePoint->instanceOf ("Port")
          && !(interfacePoint->instanceOf ("SwitchPort")))
        {

          nodeSequence = ((Port *)interfacePoint)->getMapNodeNesting ();

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

  nodeEntity = (NodeEntity *)(node->getDataEntity ());
  if (nodeEntity->instanceOf ("CompositeNode")
      && interfacePoint->instanceOf ("Port"))
    {

      compositeNode = (CompositeNode *)nodeEntity;
      port = (Port *)interfacePoint;
      return compositeNode->getMapInterface (port);
    }
  else
    {
      return interfacePoint;
    }
}

GINGA_NCL_END
