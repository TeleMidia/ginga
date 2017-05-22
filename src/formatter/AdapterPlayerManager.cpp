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

#include "AdapterImagePlayer.h"
#include "AdapterLuaPlayer.h"
#include "AdapterNCLPlayer.h"
#include "AdapterVideoPlayer.h"
#include "AdapterSsmlPlayer.h"
#if defined WITH_LIBRSVG && WITH_LIBRSVG
# include "AdapterSvgPlayer.h"
#endif
#if defined WITH_PANGO && WITH_PANGO
# include "AdapterTextPlayer.h"
#endif
#if defined WITH_CEF && WITH_CEF
# include "AdapterHTMLPlayer.h"
#endif

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

bool
AdapterPlayerManager::hasPlayer (AdapterFormatterPlayer *player)
{
  return _playerNames.find (player) != _playerNames.end ();
}

NclPlayerData *
AdapterPlayerManager::getNclPlayerData ()
{
  return _nclPlayerData;
}

void
AdapterPlayerManager::setVisible (const string &objectId,
                                  const string &visible,
                                  NclAttributionEvent *event)
{
  map<string, AdapterFormatterPlayer *>::iterator i;

  i = _objectPlayers.find (objectId);
  if (i != _objectPlayers.end ())
    {
      AdapterFormatterPlayer *player = i->second;
      player->setPropertyValue (event, visible);
      event->stop ();
    }
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

  string classname;
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

  if (false)
    {
    }
#if defined WITH_GSTREAMER && WITH_GSTREAMER
  else if (g_str_has_prefix (mime, "audio")
      || g_str_has_prefix (mime, "video"))
    {
      classname = "AdapterVideoPlayer";
      adapter = new AdapterVideoPlayer ();
    }
#endif
#if WITH_LIBRSVG && WITH_LIBRSVG
  else if (g_str_has_prefix (mime, "image/svg"))
    {
      classname = "AdapterSvgPlayer";
      adapter = new AdapterSvgPlayer ();
    }
#endif
  else if (g_str_has_prefix (mime, "image"))
    {
      classname = "AdapterImagePlayer";
      adapter = new AdapterImagePlayer ();
    }
#if defined WITH_CEF &&  WITH_CEF
  else if (g_str_has_prefix (mime, "text/test-html"))
    {
      classname = "AdapterHTMLPlayer";
      adapter = new AdapterHTMLPlayer();
    }
#endif
#if defined WITH_PANGO && WITH_PANGO
  else if (streq (mime, "text/plain"))
    {
      classname = "AdapterPlayer";
      adapter = new AdapterTextPlayer ();
    }
#endif
  else if (g_strcmp0 (mime, "application/x-ginga-NCLua") == 0)
    {
      classname = "AdapterLuaPlayer";
      adapter = new AdapterLuaPlayer ();
    }
  else if (g_strcmp0 (mime, "application/x-ncl-ncl") == 0
           || g_strcmp0 (mime, "application/x-ginga-ncl") == 0)
    {
      classname = "AdapterNCLPlayer";
      adapter = new AdapterNCLPlayer ();
    }
  else
    {
      g_warning ("adapter: unknown mime-type '%s'", mime);
      return NULL;
    }

  adapter->setAdapterManager (this);
  _objectPlayers[id] = adapter;
  _playerNames[adapter] = classname;

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

  map<string, AdapterFormatterPlayer *>::iterator i;
  map<AdapterFormatterPlayer *, string>::iterator j;
  AdapterFormatterPlayer *player;
  string playerClassName = "";

  i = _deletePlayers.begin ();
  while (i != _deletePlayers.end ())
    {
      player = i->second;

      j = _playerNames.find (player);
      if (j != _playerNames.end ())
        {
          playerClassName = j->second;
          _playerNames.erase (j);
        }

      if (((AdapterFormatterPlayer *)player)->getObjectDevice () == 0)
        {
          dPlayers[playerClassName] = player;
        }

      ++i;
    }
  _deletePlayers.clear ();

  i = dPlayers.begin ();
  while (i != dPlayers.end ())
    {
      player = i->second;
      playerClassName = i->first;

      delete player;

      ++i;
    }
}

GINGA_FORMATTER_END
