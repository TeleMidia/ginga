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

#ifndef ADAPTER_PLAYER_MANAGER_H
#define ADAPTER_PLAYER_MANAGER_H

#include "AdapterFormatterPlayer.h"
#include "NclExecutionObject.h"

GINGA_FORMATTER_BEGIN

class AdapterPlayerManager
{
public:
  explicit AdapterPlayerManager ();
  virtual ~AdapterPlayerManager ();

  AdapterFormatterPlayer *getObjectPlayer (NclExecutionObject *execObj);
  bool removePlayer (NclExecutionObject *object);

private:
  map<string, AdapterFormatterPlayer *> _objectPlayers;

  AdapterFormatterPlayer *initializePlayer (NclExecutionObject *object);

  bool removePlayer (const string &objectId);
  void clearDeletePlayers ();
};

GINGA_FORMATTER_END

#endif // ADAPTER_PLAYER_MANAGER_H
