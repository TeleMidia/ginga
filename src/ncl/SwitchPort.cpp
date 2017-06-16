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
#include "SwitchPort.h"
#include "SwitchNode.h"

GINGA_NCL_BEGIN

SwitchPort::SwitchPort (const string &id, SwitchNode *switchNode)
    : Port (id, switchNode, NULL)
{
  _portList = new vector<Port *>;
  _typeSet.insert ("SwitchPort");
}

SwitchPort::~SwitchPort ()
{
  vector<Port *>::iterator i;

  if (_portList != NULL)
    {
      i = _portList->begin ();
      while (i != _portList->end ())
        {
          delete *i;
          ++i;
        }

      delete _portList;
      _portList = NULL;
    }
}

bool
SwitchPort::addPort (Port *port)
{
  if (containsMap (port->getNode ()))
    return false;

  _portList->push_back (port);
  return true;
}

bool
SwitchPort::containsMap (Node *node)
{
  vector<Port *>::iterator i;
  Port *port;

  for (i = _portList->begin (); i < _portList->end (); ++i)
    {
      port = (Port *)(*i);
      if (port->getNode ()->getId () == node->getId ())
        return true;
    }
  return false;
}

vector<Port *> *
SwitchPort::getPorts ()
{
  return _portList;
}

bool
SwitchPort::removePort (Port *port)
{
  vector<Port *>::iterator i;

  for (i = _portList->begin (); i != _portList->end (); ++i)
    {
      if ((*i)->getId () == port->getId ())
        {
          _portList->erase (i);
          return true;
        }
    }
  return false;
}

Node *
SwitchPort::getEndNode ()
{
  return _node;
}

InterfacePoint *
SwitchPort::getEndInterfacePoint ()
{
  // Polimorfismo
  return (InterfacePoint *)(this);
}

GINGA_NCL_END
