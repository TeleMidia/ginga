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
#include "ConnectorBase.h"

GINGA_NCL_BEGIN

ConnectorBase::ConnectorBase (const string &id) : Base (id)
{
}

ConnectorBase::~ConnectorBase ()
{
  _connectorSet.clear ();
}

bool
ConnectorBase::addConnector (Connector *connector)
{
  if (connector == NULL || containsConnector (connector))
    {
      return false;
    }

  _connectorSet.insert (connector);
  return true;
}

bool
ConnectorBase::addBase (Base *base, const string &alias, const string &location)
{
  if (Base::hasInstance (base, false) && instanceof (ConnectorBase *, base))
    {
      return Base::addBase (base, alias, location);
    }

  return false;
}

void
ConnectorBase::clear ()
{
  _connectorSet.clear ();
  Base::clear ();
}

bool
ConnectorBase::containsConnector (const string &connectorId)
{
  if (getConnectorLocally (connectorId) != NULL)
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool
ConnectorBase::containsConnector (Connector *connector)
{
  set<Connector *>::iterator i;

  i = _connectorSet.find (connector);
  if (i != _connectorSet.end ())
    {
      return true;
    }
  return false;
}

Connector *
ConnectorBase::getConnectorLocally (const string &connectorId)
{
  set<Connector *>::iterator i;

  i = _connectorSet.begin ();
  while (i != _connectorSet.end ())
    {
      if ((*i)->getId () == connectorId)
        {
          return (*i);
        }

      ++i;
    }
  return NULL;
}

Connector *
ConnectorBase::getConnector (const string &connectorId)
{
  string::size_type index;
  string prefix, suffix;
  ConnectorBase *base;
  Connector *conn = NULL;
  map<string, Base *>::iterator i;

  index = connectorId.find_first_of ("#");
  if (index == string::npos)
    {
      return getConnectorLocally (connectorId);
    }

  prefix = connectorId.substr (0, index);
  index++;
  suffix = connectorId.substr (index, connectorId.length () - index);

  i = _baseAliases.find (prefix);
  if (i != _baseAliases.end ())
    {
      base = (ConnectorBase *)(i->second);
      conn = base->getConnector (suffix);
    }

  if (conn == NULL)
    {
      i = _baseLocations.find (prefix);
      if (i != _baseLocations.end ())
        {
          base = (ConnectorBase *)(i->second);
          conn = base->getConnector (suffix);
        }
    }

  if (conn == NULL)
    {
      cout << "ConnectorBase::getConnector can't find connector '";
      cout << connectorId << "'";
      cout << " using prefix = '" << prefix << "'";
      cout << " and suffix = '" << suffix << "'";
      cout << " baseAli has = '" << _baseAliases.size () << "' aliases";
      cout << " baseLoc has = '" << _baseLocations.size () << "' locations";
      cout << endl;
    }
  return conn;
}

bool
ConnectorBase::removeConnector (const string &connectorId)
{
  Connector *connector;

  connector = getConnectorLocally (connectorId);
  return removeConnector (connector);
}

bool
ConnectorBase::removeConnector (Connector *connector)
{
  set<Connector *>::iterator i;

  i = _connectorSet.find (connector);
  if (i != _connectorSet.end ())
    {
      _connectorSet.erase (i);
      return true;
    }
  return false;
}

GINGA_NCL_END
