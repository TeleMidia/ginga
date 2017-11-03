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

#ifndef SSMLPLAYER_H_
#define SSMLPLAYER_H_

#include "ginga.h"

#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

#include "Player.h"

GINGA_BEGIN

class SsmlPlayer : public Thread, public Player
{
private:
  string content;

public:
  SsmlPlayer (const string &mrl);
  virtual ~SsmlPlayer ();

protected:
  void setFile (const string &mrl);

private:
  void loadSsml ();

public:
  void run ();
  bool play ();
  void stop ();
  void resume ();
  virtual void setPropertyValue (const string &name, const string &value);
};

GINGA_END

#endif /*SSMLPLAYER_H_*/
