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

#ifndef CHANNELPLAYER_H_
#define CHANNELPLAYER_H_

#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

#include "Player.h"

GINGA_PLAYER_BEGIN

class ChannelPlayer : public Player, public IPlayerListener
{
protected:
  map<string, IPlayer *> *objectMap;
  IPlayer *selectedPlayer;
  bool hasParent;

public:
  ChannelPlayer ();
  virtual ~ChannelPlayer ();
  IPlayer *getSelectedPlayer ();
  void setPlayerMap (map<string, IPlayer *> *objs);
  map<string, IPlayer *> *getPlayerMap ();
  IPlayer *getPlayer (const string &objectId);
  void select (IPlayer *selObject);
  guint32 getMediaTime ();

private:
  void setSurfacesParent (SDLWindow* parent);



public:
  bool play ();
  void pause ();
  void resume ();
  void stop ();
  virtual void setPropertyValue (const string &name, const string &value);
  void updateStatus (short code,
                     const string &parameter = "",
                     short type = TYPE_PRESENTATION,
                     const string &value = "");
  virtual void timeShift (arg_unused (const string &direction)){};
};

GINGA_PLAYER_END

#endif /*CHANNELPLAYER_H_*/
