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

AdapterPlayerManager::AdapterPlayerManager (NclPlayerData *data) : Thread ()
{
  nclPlayerData = data;

  editingCommandListener = NULL;
  epgFactoryAdapter = NULL;
  timeBaseProvider = NULL;

  Thread::mutexInit (&mutexPlayer, false);

  running = true;
  startThread ();
}

AdapterPlayerManager::~AdapterPlayerManager ()
{
  isDeleting = true;
  running = false;
  unlockConditionSatisfied ();

  clear ();
  clearDeletePlayers ();

  Thread::mutexLock (&mutexPlayer);
  Thread::mutexUnlock (&mutexPlayer);
  Thread::mutexDestroy (&mutexPlayer);

  clog << "AdapterPlayerManager::~AdapterPlayerManager all done" << endl;
}

bool
AdapterPlayerManager::hasPlayer (IAdapterPlayer *player)
{
  bool hasInstance = false;

  Thread::mutexLock (&mutexPlayer);
  if (playerNames.find (player) != playerNames.end ())
    {
      hasInstance = true;
    }
  Thread::mutexUnlock (&mutexPlayer);

  return hasInstance;
}

NclPlayerData *
AdapterPlayerManager::getNclPlayerData ()
{
  return nclPlayerData;
}

void
AdapterPlayerManager::setTimeBaseProvider (
    ITimeBaseProvider *timeBaseProvider)
{
  this->timeBaseProvider = timeBaseProvider;
}

ITimeBaseProvider *
AdapterPlayerManager::getTimeBaseProvider ()
{
  return timeBaseProvider;
}

void
AdapterPlayerManager::setVisible (const string &objectId, const string &visible,
                                  NclAttributionEvent *event)
{
  map<string, IAdapterPlayer *>::iterator i;
  AdapterFormatterPlayer *player;

  Thread::mutexLock (&mutexPlayer);
  i = objectPlayers.find (objectId);
  if (i != objectPlayers.end ())
    {
      player = (AdapterFormatterPlayer *)(i->second);
      player->setPropertyValue (event, visible);
      event->stop ();
    }
  Thread::mutexUnlock (&mutexPlayer);
}

bool
AdapterPlayerManager::removePlayer (void *exObject)
{
  NclExecutionObject *object;
  bool removed = false;
  string objId;

  object = (NclExecutionObject *)exObject;
  Thread::mutexLock (&mutexPlayer);
  if (NclExecutionObject::hasInstance (object, false))
    {
      objId = object->getId ();
      removed = removePlayer (objId);
    }
  Thread::mutexUnlock (&mutexPlayer);

  return removed;
}

bool
AdapterPlayerManager::removePlayer (const string &objectId)
{
  map<string, IAdapterPlayer *>::iterator i;
  AdapterFormatterPlayer *player;

  i = objectPlayers.find (objectId);
  if (i != objectPlayers.end ())
    {
      player = (AdapterFormatterPlayer *)(i->second);
      if (!player->instanceOf ("AdapterProgramAVPlayer"))
        {
          deletePlayers[objectId] = player;
        }
      objectPlayers.erase (i);
      unlockConditionSatisfied ();
      return true;
    }

  return false;
}

void
AdapterPlayerManager::clear ()
{
  map<string, IAdapterPlayer *>::iterator i;

  Thread::mutexLock (&mutexPlayer);
  i = objectPlayers.begin ();
  while (i != objectPlayers.end ())
    {
      if (removePlayer (i->first))
        {
          i = objectPlayers.begin ();
        }
      else
        {
          ++i;
        }
    }
  objectPlayers.clear ();
  Thread::mutexUnlock (&mutexPlayer);
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
  IAdapterPlayer *adapter = NULL;

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

  g_debug ("mime %s", mime);
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
  else if (g_strcmp0 (mime, "application/x-ginga-NCLua") == 0
           || g_strcmp0 (mime, "application/x-ginga-EPGFactory") == 0)
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
      g_warning ("unknown mime \"%s\", skipping object id=%s", mime,
                 id.c_str ());
      return NULL;
    }

  adapter->setAdapterManager (this);
  objectPlayers[id] = adapter;
  playerNames[adapter] = classname;

  g_debug ("%s allocated for object id=%s) ", classname.c_str (),
           id.c_str ());

  return (AdapterFormatterPlayer *)adapter;
}

void *
AdapterPlayerManager::getObjectPlayer (void *eObj)
{
  map<string, IAdapterPlayer *>::iterator i;
  AdapterFormatterPlayer *player;
  string objId;
  NclExecutionObject *execObj = (NclExecutionObject *)eObj;

  Thread::mutexLock (&mutexPlayer);
  objId = execObj->getId ();
  i = objectPlayers.find (objId);
  if (i == objectPlayers.end ())
    {
      i = deletePlayers.find (objId);
      if (i == deletePlayers.end ())
        {
          player = initializePlayer (execObj);
        }
      else
        {
          player = (AdapterFormatterPlayer *)(i->second);
          deletePlayers.erase (i);
          objectPlayers[objId] = player;
        }
    }
  else
    {
      player = (AdapterFormatterPlayer *)(i->second);
    }
  Thread::mutexUnlock (&mutexPlayer);

  return player;
}

string
AdapterPlayerManager::getMimeTypeFromSchema (arg_unused (const string &url))
{
  return "";
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
  map<string, IAdapterPlayer *> dPlayers;

  map<string, IAdapterPlayer *>::iterator i;
  map<IAdapterPlayer *, string>::iterator j;
  IAdapterPlayer *player;
  string playerClassName = "";

  Thread::mutexLock (&mutexPlayer);
  i = deletePlayers.begin ();
  while (i != deletePlayers.end ())
    {
      player = i->second;

      j = playerNames.find (player);
      if (j != playerNames.end ())
        {
          playerClassName = j->second;
          playerNames.erase (j);
        }

      if (((AdapterFormatterPlayer *)player)->getObjectDevice () == 0)
        {
          dPlayers[playerClassName] = player;
        }

      ++i;
    }
  deletePlayers.clear ();
  Thread::mutexUnlock (&mutexPlayer);

  i = dPlayers.begin ();
  while (i != dPlayers.end ())
    {
      player = i->second;
      playerClassName = i->first;

      delete player;

      ++i;
    }
}

void
AdapterPlayerManager::run ()
{
  set<IAdapterPlayer *>::iterator i;

  while (running)
    {
      if (!isDeleting)
        {
          Thread::mutexLock (&mutexPlayer);
          if (deletePlayers.empty ())
            {
              Thread::mutexUnlock (&mutexPlayer);
              waitForUnlockCondition ();
            }
          else
            {
              Thread::mutexUnlock (&mutexPlayer);
            }
        }

      if (!running)
        {
          return;
        }

      if (isDeleting)
        {
          break;
        }

      Thread::mSleep (1000);
      if (running)
        {
          clearDeletePlayers ();
        }
    }

  clog << "AdapterPlayerManager::run all done" << endl;
}

GINGA_FORMATTER_END
