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

#ifndef _PLAYERADAPTERMANAGER_H_
#define _PLAYERADAPTERMANAGER_H_

#include "system/Thread.h"
using namespace ::ginga::system;

#include "ncl/NodeEntity.h"
#include "ncl/ContentNode.h"
using namespace ::ginga::ncl;

#include "NclCascadingDescriptor.h"
#include "NclExecutionObject.h"

#include "player/INCLPlayer.h"
using namespace ::ginga::player;

#include "AdapterFormatterPlayer.h"

GINGA_FORMATTER_BEGIN

class AdapterPlayerManager
{
public:
  AdapterPlayerManager (NclPlayerData *data);
  virtual ~AdapterPlayerManager ();

  AdapterFormatterPlayer *getObjectPlayer (NclExecutionObject *execObj);
  NclPlayerData *getNclPlayerData ();

  bool removePlayer (NclExecutionObject *object);
  static bool isEmbeddedApp (NodeEntity *dataObject);

private:
  map<string, AdapterFormatterPlayer *> _objectPlayers;
  map<string, AdapterFormatterPlayer *> _deletePlayers;
  NclPlayerData *_nclPlayerData;

  AdapterFormatterPlayer *initializePlayer (NclExecutionObject *object);
  static bool isEmbeddedAppMediaType (const string &mediaType);

  bool removePlayer (const string &objectId);
  void clear ();
  void clearDeletePlayers ();
};

GINGA_FORMATTER_END

#endif //_PLAYERADAPTERMANAGER_H_
