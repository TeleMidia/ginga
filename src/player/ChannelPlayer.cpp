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
#include "ChannelPlayer.h"

GINGA_PLAYER_BEGIN

ChannelPlayer::ChannelPlayer (GingaScreenID screenId)
    : Player (screenId, "")
{
  this->objectMap = NULL;
  this->selectedPlayer = NULL;
  this->hasParent = false;
}

ChannelPlayer::~ChannelPlayer ()
{
  map<string, IPlayer *>::iterator players;
  IPlayer *objectPlayer;

  if (objectMap != NULL)
    {
      players = objectMap->begin ();
      while (players != objectMap->end ())
        {
          objectPlayer = players->second;
          delete objectPlayer;
          objectPlayer = NULL;
          ++players;
        }

      objectMap->clear ();
      delete objectMap;
      objectMap = NULL;
    }
}

void
ChannelPlayer::setPlayerMap (map<string, IPlayer *> *objs)
{
  objectMap = objs;
}

IPlayer *
ChannelPlayer::getSelectedPlayer ()
{
  if (selectedPlayer == NULL)
    {
      clog << "ChannelPlayer::getSelectedPlayerAdd == NULL";
      clog << endl;
      return NULL;
    }
  return selectedPlayer;
}

map<string, IPlayer *> *
ChannelPlayer::getPlayerMap ()
{
  if (objectMap == NULL || objectMap->empty ())
    {
      return NULL;
    }

  return objectMap;
}

IPlayer *
ChannelPlayer::getPlayer (string objectId)
{
  IPlayer *newSelected;

  if (objectMap->count (objectId) != 0)
    {
      newSelected = (*objectMap)[objectId];
      if (newSelected != NULL && newSelected != selectedPlayer)
        {
          return newSelected;
        }
    }

  return NULL;
}

void
ChannelPlayer::select (IPlayer *selObject)
{
  if (selectedPlayer != NULL)
    {
      selectedPlayer->removeListener (this);
    }

  this->selectedPlayer = selObject;
  if (selectedPlayer != NULL)
    {
      selectedPlayer->addListener (this);
    }
}

double
ChannelPlayer::getMediaTime ()
{
  return selectedPlayer->getMediaTime ();
}

void
ChannelPlayer::setSurfacesParent (GingaWindowID parent)
{
  map<string, IPlayer *>::iterator players;
  IPlayer *avPlayer;
  GingaSurfaceID s = 0;

  players = objectMap->begin ();
  while (players != objectMap->end ())
    {
      avPlayer = players->second;
      s = ((Player *)avPlayer)->getSurface ();
      if (s != 0 && Ginga_Display_M->getSurfaceParentWindow (s) != parent)
        {
          Ginga_Display_M->setSurfaceParentWindow (myScreen, s, parent);
        }
      ++players;
    }
  hasParent = true;
}

GingaSurfaceID
ChannelPlayer::getSurface ()
{
  if (selectedPlayer != NULL)
    {
      return ((Player *)selectedPlayer)->getSurface ();
    }
  return 0;
}

bool
ChannelPlayer::play ()
{
  GingaSurfaceID s;

  if (selectedPlayer != NULL)
    {
      s = ((Player *)selectedPlayer)->getSurface ();
      if (!hasParent && s != 0 && Ginga_Display_M->getSurfaceParentWindow (s) != 0)
        {
          GingaWindowID parentWindow = Ginga_Display_M->getSurfaceParentWindow (s);
          setSurfacesParent (parentWindow);
        }
      selectedPlayer->play ();
    }

  return Player::play ();
}

void
ChannelPlayer::pause ()
{
  if (selectedPlayer != NULL)
    {
      selectedPlayer->pause ();
    }
  Player::pause ();
}

void
ChannelPlayer::resume ()
{
  if (selectedPlayer != NULL)
    {
      selectedPlayer->resume ();
    }
  Player::resume ();
}

void
ChannelPlayer::stop ()
{
  IPlayer *objectPlayer;
  map<string, IPlayer *>::iterator players;

  players = objectMap->begin ();
  while (players != objectMap->end ())
    {
      objectPlayer = players->second;
      objectPlayer->stop ();
      ++players;
    }
  hasParent = false;
  Player::stop ();
}

void
ChannelPlayer::updateStatus (short code, string parameter, short type,
                             string value)
{
  notifyPlayerListeners (code, parameter, type, value);
}

void
ChannelPlayer::setPropertyValue (string name, string value)
{
  IPlayer *objectPlayer;
  map<string, IPlayer *>::iterator players;

  // TODO: set brightness, ...
  if (name == "soundLevel")
    {
      players = objectMap->begin ();
      while (players != objectMap->end ())
        {
          objectPlayer = players->second;
          objectPlayer->setPropertyValue (name, value);
          ++players;
        }
    }

  Player::setPropertyValue (name, value);
}

GINGA_PLAYER_END
