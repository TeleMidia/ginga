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
#include "AdapterSsmlPlayer.h"
#include "player/SsmlPlayer.h"

GINGA_FORMATTER_BEGIN

AdapterSsmlPlayer::AdapterSsmlPlayer () : AdapterFormatterPlayer ()
{

  typeSet.insert ("AdapterSsmlPlayer");
}

void
AdapterSsmlPlayer::createPlayer ()
{
  clog << "AdapterSsmlPlayer::createPlayer '" << mrl << "'" << endl;

  string paramValue;
  NclCascadingDescriptor *descriptor;

  bool hasVisual = false;

  player = new SsmlPlayer (myScreen, mrl.c_str ());

  clog << "AdapterSsmlPlayer::createPlayer '";
  clog << mrl << "' ALL DONE" << endl;

  AdapterFormatterPlayer::createPlayer ();
}

GINGA_FORMATTER_END
