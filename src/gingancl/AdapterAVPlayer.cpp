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

#include "config.h"
#include "AdapterAVPlayer.h"
#include "player/AVPlayer.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_AV_BEGIN

AdapterAVPlayer::AdapterAVPlayer () : AdapterFormatterPlayer ()
{
  typeSet.insert ("AdapterAVPlayer");
}

void
AdapterAVPlayer::createPlayer ()
{
  CascadingDescriptor *descriptor;
  string soundLevel;

  clog << "AdapterAVPlayer::createPlayer for '" << mrl << "'" << endl;

  if (mrl != "")
    {
      player = new AVPlayer (myScreen, mrl.c_str ());
    }

  AdapterFormatterPlayer::createPlayer ();

  clog << "AdapterAVPlayer::createPlayer for '" << mrl << "'" << endl;
}

bool
AdapterAVPlayer::setPropertyValue (AttributionEvent *event, string value)
{

  return AdapterFormatterPlayer::setPropertyValue (event, value);
}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_AV_END
