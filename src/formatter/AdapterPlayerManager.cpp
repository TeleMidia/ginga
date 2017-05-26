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
#include "AdapterLuaPlayer.h"

GINGA_FORMATTER_BEGIN

AdapterPlayerManager::AdapterPlayerManager (NclPlayerData *data)
{
  _nclPlayerData = data;
}

AdapterPlayerManager::~AdapterPlayerManager ()
{
  clear ();
  clearDeletePlayers ();
}

NclPlayerData *
AdapterPlayerManager::getNclPlayerData ()
{
  return _nclPlayerData;
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
  map<string, AdapterFormatterPlayer *>::iterator i;

  i = _objectPlayers.find (objectId);
  if (i != _objectPlayers.end ())
    {
      _deletePlayers[objectId] = i->second;
      _objectPlayers.erase (i);
      return true;
    }

  return false;
}

void
AdapterPlayerManager::clear ()
{
  map<string, AdapterFormatterPlayer *>::iterator i;

  i = _objectPlayers.begin ();
  while (i != _objectPlayers.end ())
    {
      if (removePlayer (i->first))
        {
          i = _objectPlayers.begin ();
        }
      else
        {
          ++i;
        }
    }
  _objectPlayers.clear ();
}

AdapterFormatterPlayer *
AdapterPlayerManager::initializePlayer (NclExecutionObject *object)
{
  NodeEntity *entity;
  Content *content;
  string id;
  string buf;
  const char *mime;

  AdapterFormatterPlayer *adapter = NULL;

  g_assert_nonnull (object);
  id = object->getId ();

  entity = (NodeEntity *)(object->getDataObject ()->getDataEntity ());
  g_assert_nonnull (entity);
  g_assert (entity->instanceOf ("ContentNode"));

  if (((ContentNode *)entity)->isSettingNode ())
    return NULL;                // nothing to do

  content = entity->getContent ();
  g_assert_nonnull (content);

  buf = ((ContentNode *)entity)->getNodeType ();
  mime = buf.c_str ();
  g_assert_nonnull (mime);

  if (g_strcmp0 (mime, "application/x-ginga-NCLua") == 0)
    {
      adapter = new AdapterLuaPlayer (this);
    }
  else
    {
      adapter = new AdapterFormatterPlayer (this);
    }

  _objectPlayers[id] = adapter;

  return adapter;
}

AdapterFormatterPlayer *
AdapterPlayerManager::getObjectPlayer (NclExecutionObject *execObj)
{
  map<string, AdapterFormatterPlayer *>::iterator i;
  AdapterFormatterPlayer *player;
  string objId;

  objId = execObj->getId ();
  i = _objectPlayers.find (objId);
  if (i == _objectPlayers.end ())
    {
      i = _deletePlayers.find (objId);
      if (i == _deletePlayers.end ())
        {
          player = initializePlayer (execObj);
        }
      else
        {
          player = i->second;
          _deletePlayers.erase (i);
          _objectPlayers[objId] = player;
        }
    }
  else
    {
      player = i->second;
    }

  return player;
}

bool
AdapterPlayerManager::isEmbeddedApp (NodeEntity *dataObject)
{
  string mediaType = "";
  string url = "";
  string::size_type pos;
  Descriptor *descriptor;
  Content *content;

  // first, descriptor
  descriptor = (Descriptor *)(dataObject->getDescriptor ());
  if (descriptor != NULL && !descriptor->instanceOf ("DescriptorSwitch"))
    {
      mediaType = descriptor->getPlayerName ();
      if (mediaType == "AdapterLuaPlayer"
          || mediaType == "AdapterNCLPlayer")
        {
          return true;
        }
    }

  // second, media type
  if (dataObject->instanceOf ("ContentNode"))
    {
      mediaType = ((ContentNode *)dataObject)->getNodeType ();
      if (mediaType != "")
        {
          return isEmbeddedAppMediaType (mediaType);
        }
    }

  // finally, content file extension
  content = dataObject->getContent ();
  if (content != NULL)
    {
      if (content->instanceOf ("ReferenceContent"))
        {
          url = ((ReferenceContent *)(content))->getCompleteReferenceUrl ();

          if (url != "")
            {
              pos = url.find_last_of (".");
              if (pos != std::string::npos)
                {
                  gboolean status;
                  const char *s;
                  string extension;
                  string mime;

                  extension = url.substr (pos, url.length () - (pos + 1));
                  status = ginga_mime_table_index (extension.c_str (), &s);
                  mime = (likely (status)) ? string (s) : "";
                  return isEmbeddedAppMediaType (mime);
                }
            }
        }
    }

  return false;
}

bool
AdapterPlayerManager::isEmbeddedAppMediaType (const string &mediaType)
{
  string upMediaType = xstrup (mediaType);

  if (upMediaType == "APPLICATION/X-GINGA-NCLUA"
      || upMediaType == "APPLICATION/X-GINGA-NCLET"
      || upMediaType == "APPLICATION/X-GINGA-NCL"
      || upMediaType == "APPLICATION/X-NCL-NCL"
      || upMediaType == "APPLICATION/X-NCL-NCLUA")
    {
      return true;
    }

  return false;
}

void
AdapterPlayerManager::clearDeletePlayers ()
{
  map<string, AdapterFormatterPlayer *> dPlayers;
  AdapterFormatterPlayer *adapter;
  string playerClassName = "";

  for (auto it: _deletePlayers)
    {
      adapter = it.second;
      dPlayers[playerClassName] = adapter;
    }
  _deletePlayers.clear ();

  for (auto it: dPlayers)
    {
      adapter = it.second;
      delete adapter;
    }
}

GINGA_FORMATTER_END
