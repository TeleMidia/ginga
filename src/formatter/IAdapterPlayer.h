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

#ifndef IAdapterPlayer_H_
#define IAdapterPlayer_H_

#include "NclAttributionEvent.h"

#include "player/IPlayer.h"
using namespace ::ginga::player;

using namespace ::ginga::system;

GINGA_FORMATTER_BEGIN

class IAdapterPlayer
{
public:
  virtual ~IAdapterPlayer (){};

  virtual void setAdapterManager (void *manager) = 0;

public:
  virtual bool instanceOf (string s) = 0;

protected:
  virtual void createPlayer () = 0;

public:
  virtual IPlayer *getPlayer () = 0;

  virtual double getOutTransTime () = 0;

  virtual bool setPropertyValue (NclAttributionEvent *event, string value)
      = 0;

  virtual string getPropertyValue (string name) = 0;
};

GINGA_FORMATTER_END

#endif /*IAdapterPlayer_H_*/
