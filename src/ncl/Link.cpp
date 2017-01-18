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

#include "config.h"
#include "ncl/Link.h"

#include "ncl/ContextNode.h"
using namespace ::ginga::ncl;

GINGA_NCL_BEGIN

Link::Link (string id, Connector *connector) : Entity (id)
{
  roleBinds = new map<string, vector<Bind *> *>;
  binds = new vector<Bind *>;
  this->connector = connector;
  parameters = new map<string, Parameter *>;
  composition = NULL;
  typeSet.insert ("Link");
}

Link::~Link ()
{
  map<string, vector<Bind *> *>::iterator i;
  vector<Bind *>::iterator j;
  map<string, Parameter *>::iterator k;
  vector<Bind *> *rmBinds;

  if (roleBinds != NULL)
    {
      i = roleBinds->begin ();
      while (i != roleBinds->end ())
        {
          rmBinds = i->second;
          rmBinds->clear ();
          delete rmBinds;
          ++i;
        }
      delete roleBinds;
      roleBinds = NULL;
    }

  if (binds != NULL)
    {
      j = binds->begin ();
      while (j != binds->end ())
        {
          delete *j;
          ++j;
        }
      delete binds;
      binds = NULL;
    }

  if (parameters != NULL)
    {
      k = parameters->begin ();
      while (k != parameters->end ())
        {
          delete k->second;
          ++k;
        }
      delete parameters;
      parameters = NULL;
    }
}

Bind *
Link::bind (Node *node, InterfacePoint *interfPt, GenericDescriptor *desc,
            string roleId)
{

  Role *role;

  role = connector->getRole (roleId);
  if (role == NULL)
    {
      clog << "Link::bind Warning! Can't find role '" << roleId;
      clog << "'" << endl;
      return NULL;
    }

  return bind (node, interfPt, desc, role);
}

Bind *
Link::bind (Node *node, InterfacePoint *interfPt, GenericDescriptor *desc,
            Role *role)
{

  Bind *bind;
  vector<Bind *> *roleBindList;
  int maxConn;
  string label;

  label = role->getLabel ();
  if (roleBinds->count (label) == 0)
    {
      roleBindList = new vector<Bind *>;
      (*roleBinds)[label] = roleBindList;
    }
  else
    {
      roleBindList = (*roleBinds)[label];
    }

  // binds only if the max attribute is equal or greater than the
  // number of link participants.
  // If the link is inconsistent, it must be ignored
  maxConn = role->getMaxCon ();

  if (maxConn != Role::UNBOUNDED && (int)(roleBindList->size ()) >= maxConn)
    {

      clog << "Link::bind Warning! Bind overflows maxConn(";
      clog << maxConn;
      clog << ")" << endl;
    }

  bind = new Bind (node, interfPt, desc, role);
  roleBindList->push_back (bind);
  binds->push_back (bind);
  return bind;
}

bool
Link::isConsistent ()
{
  vector<Role *>::iterator i;
  Role *role;
  int minConn, maxConn;
  vector<Role *> *roles;

  roles = connector->getRoles ();
  i = roles->begin ();
  while (i != roles->end ())
    {
      role = *i;
      // if (role instanceof ICardinalityRole) {
      minConn = role->getMinCon ();
      maxConn = role->getMaxCon ();
      //}
      // else {
      //	minConn = 1;
      //	maxConn = 1;
      //}
      if ((int)getNumRoleBinds (role) < minConn
          || (maxConn != Role::UNBOUNDED
              && (int)getNumRoleBinds (role) > maxConn))
        {

          delete roles;
          return false;
        }
      ++i;
    }
  delete roles;
  return true;
}

Bind *
Link::getBind (Node *node, InterfacePoint *interfPt,
               GenericDescriptor *desc, Role *role)
{

  map<string, vector<Bind *> *>::iterator i;

  bool containsKey = false;
  for (i = roleBinds->begin (); i != roleBinds->end (); ++i)
    if (i->first == role->getLabel ())
      containsKey = true;

  if (!containsKey)
    return NULL;

  Bind *bind;
  vector<Bind *>::iterator bindIterator;

  vector<Bind *> *roleBindList;
  roleBindList = (*roleBinds)[role->getLabel ()];

  for (bindIterator = roleBindList->begin ();
       bindIterator != roleBindList->end (); ++roleBindList)
    {

      bind = (*bindIterator);
      if ((bind->getNode () == node)
          && (bind->getInterfacePoint () == interfPt)
          && (bind->getDescriptor () == desc))
        return bind;
    }

  return NULL;
}

vector<Bind *> *
Link::getBinds ()
{
  if (binds->empty ())
    return NULL;

  return binds;
}

Connector *
Link::getConnector ()
{
  return connector;
}

void
Link::setParentComposition (LinkComposition *composition)
{
  if (composition == NULL
      || ((ContextNode *)composition)->containsLink (this))
    {

      this->composition = composition;
    }
}

LinkComposition *
Link::getParentComposition ()
{
  return composition;
}

unsigned int
Link::getNumBinds ()
{
  return binds->size ();
}

unsigned int
Link::getNumRoleBinds (Role *role)
{
  map<string, vector<Bind *> *>::iterator i;

  i = roleBinds->find (role->getLabel ());
  if (i == roleBinds->end ())
    {
      return 0;
    }

  return i->second->size ();
}

vector<Bind *> *
Link::getRoleBinds (Role *role)
{
  vector<Bind *> *roleBindList;
  map<string, vector<Bind *> *>::iterator i;

  i = roleBinds->find (role->getLabel ());
  if (i == roleBinds->end ())
    {
      return NULL;
    }

  return i->second;
}

bool
Link::isMultiPoint ()
{
  if (binds->size () > 2)
    return true;
  else
    return false;
}

string
Link::toString ()
{
  string linkStr;
  vector<Bind *>::iterator iterator;
  Bind *bind;

  linkStr = (getId ()) + "\n" + ((this->getConnector ())->getId ()) + "\n";

  iterator = binds->begin ();
  while (iterator++ != binds->end ())
    {
      bind = (*iterator);
      linkStr += ((bind->getRole ())->getLabel ()) + " "
                 + ((bind->getNode ())->getId ()) + " "
                 + ((bind->getInterfacePoint ())->getId ()) + "\n"
                 + ((bind->getRole ())->getLabel ());
      ++iterator;
    }
  return linkStr;
}

void
Link::setConnector (Connector *connector)
{
  this->connector = connector;

  roleBinds->clear ();
  binds->clear ();
}

bool
Link::unBind (Bind *bind)
{
  vector<Bind *> *roleBindList;
  bool containsBind = false;

  vector<Bind *>::iterator it;
  for (it = binds->begin (); it != binds->end (); ++it)
    {
      if (bind == *it)
        {
          containsBind = true;
          binds->erase (it);
          break;
        }
    }

  if (!containsBind)
    {
      return false;
    }

  if (roleBinds->count (bind->getRole ()->getLabel ()) == 1)
    {
      roleBindList = (*roleBinds)[bind->getRole ()->getLabel ()];
      vector<Bind *>::iterator i;
      for (i = roleBindList->begin (); i != roleBindList->end (); ++i)
        {
          if (*i == bind)
            {
              roleBindList->erase (i);
              break;
            }
        }
    }
  return true;
}

void
Link::addParameter (Parameter *parameter)
{
  if (parameter == NULL)
    return;

  (*parameters)[parameter->getName ()] = parameter;
}

vector<Parameter *> *
Link::getParameters ()
{
  if (parameters->empty ())
    return NULL;

  vector<Parameter *> *params;
  params = new vector<Parameter *>;

  map<string, Parameter *>::iterator i;
  for (i = parameters->begin (); i != parameters->end (); ++i)
    params->push_back (i->second);

  return params;
}

Parameter *
Link::getParameter (string name)
{
  if (parameters->empty ())
    return NULL;

  map<string, Parameter *>::iterator i;
  for (i = parameters->begin (); i != parameters->end (); ++i)
    if (i->first == name)
      return (Parameter *)(i->second);

  return NULL;
}

void
Link::removeParameter (Parameter *parameter)
{
  if (parameters->empty ())
    return;

  map<string, Parameter *>::iterator i;
  for (i = parameters->begin (); i != parameters->end (); ++i)
    {
      if (i->first == parameter->getName ())
        {
          parameters->erase (i);
          return;
        }
    }
}

void
Link::updateConnector (Connector *newConnector)
{
  int i, size;
  Bind *bind;
  Role *newRole;

  if (this->connector == NULL)
    {
      // TODO test if the set of roles is identical
      return;
    }

  size = binds->size ();
  for (i = 0; i < size; i++)
    {
      bind = (Bind *)(*binds)[i];
      newRole = newConnector->getRole (bind->getRole ()->getLabel ());
      bind->setRole (newRole);
    }
  this->connector = newConnector;
}

bool
Link::containsNode (Node *node, GenericDescriptor *descriptor)
{
  return containsNode (node, descriptor, getBinds ());
}

bool
Link::containsNode (Node *node, GenericDescriptor *descriptor,
                    vector<Bind *> *binds)
{

  Bind *bind;
  Node *bindNode;
  InterfacePoint *bindInterface;
  GenericDescriptor *bindDescriptor;

  NodeEntity *nodeEntity;
  GenericDescriptor *entityDesc;

  vector<Bind *>::iterator i;

  clog << "Link::containsNode searching '" << node->getId () << "' ";
  clog << "inside link '" << getId () << "' (" << binds->size ();
  clog << " condition binds)" << endl;

  i = binds->begin ();
  while (i != binds->end ())
    {
      bind = (*i);
      bindInterface = bind->getInterfacePoint ();
      if (bindInterface != NULL && bindInterface->instanceOf ("Port"))
        {

          bindNode = ((Port *)bindInterface)->getEndNode ();
        }
      else
        {
          bindNode = bind->getNode ();
        }

      clog << "Link::containsNode link '" << getId () << "' has '";
      clog << bindNode->getId () << "' with descriptor = '";
      if (descriptor != NULL)
        {
          clog << descriptor->getId () << "'" << endl;
        }
      else
        {
          clog << "NULL'" << endl;
        }

      if (node == bindNode)
        {
          bindDescriptor = bind->getDescriptor ();
          nodeEntity = (NodeEntity *)node->getDataEntity ();

          if (bindDescriptor != NULL)
            {
              if (bindDescriptor == descriptor)
                {
                  return true;
                }
            }
          else if (descriptor == NULL)
            {
              return true;
            }
          else if (nodeEntity != NULL)
            {
              entityDesc = nodeEntity->getDescriptor ();
              if (entityDesc == descriptor)
                {
                  return true;
                }
              else if (descriptor != NULL
                       && descriptor->getId ().find ("dummy")
                              != std::string::npos)
                {

                  return true;
                }
              else if (entityDesc != NULL
                       && entityDesc->getId ().find ("dummy")
                              != std::string::npos)
                {

                  return true;
                }
            }
        }
      ++i;
    }
  return false;
}

GINGA_NCL_END
