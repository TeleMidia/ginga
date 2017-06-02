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
#include "ginga-mime-table.h"

#include "AdapterPlayerManager.h"

GINGA_FORMATTER_BEGIN

AdapterPlayerManager::AdapterPlayerManager () {}

AdapterPlayerManager::~AdapterPlayerManager ()
{
  for (auto &i: _objectPlayers)
    {
      delete i.second; // delete AdapterFormatterPlayer
    }

  _objectPlayers.clear ();
}

bool
AdapterPlayerManager::removePlayer (NclExecutionObject *exObject)
{
  bool removed = false;

  if (NclExecutionObject::hasInstance (exObject, false))
    {
      string objId = exObject->getId ();
      removed = removePlayer (objId);
    }

  return removed;
}

bool
AdapterPlayerManager::removePlayer (const string &objectId)
{
  map<string, AdapterFormatterPlayer *>::iterator i
      = _objectPlayers.find (objectId);

  if (i != _objectPlayers.end ())
    {
      _objectPlayers.erase (i);
      delete i->second; // delete AdapterFormatterPlayer

      return true;
    }

  return false;
}

AdapterFormatterPlayer *
AdapterPlayerManager::initializePlayer (NclExecutionObject *object)
{
  g_assert_nonnull (object);
  string id = object->getId ();

  NodeEntity *entity
      = (NodeEntity *)(object->getDataObject ()->getDataEntity ());
  g_assert_nonnull (entity);

  ContentNode *contentNode = dynamic_cast <ContentNode *> (entity);
  g_assert_nonnull (contentNode);

  if (contentNode->isSettingNode ())
    return NULL;                // nothing to do

  AdapterFormatterPlayer *adapter = new AdapterFormatterPlayer (this);

  _objectPlayers[id] = adapter;

  return adapter;
}

AdapterFormatterPlayer *
AdapterPlayerManager::getObjectPlayer (NclExecutionObject *execObj)
{
  map<string, AdapterFormatterPlayer *>::iterator i;
  AdapterFormatterPlayer *player = nullptr;
  string objId;

  objId = execObj->getId ();
  i = _objectPlayers.find (objId);
  if (i == _objectPlayers.end ())
    {
      player = initializePlayer (execObj);
    }
  else
    {
      player = i->second;
    }

  return player;
}

GINGA_FORMATTER_END
