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

/**
 * @file   AdapterHTMLPlayer.cpp
 * @author Eduardo Cruz Araújo (edcaraujo@gmail.com)
 * @date   April, 2017
 *
 * @brief  Source file of HTML player adapter.
 *
 * This is the source file for the AdapterHTMLPlayer class.
 */

#include "AdapterHTMLPlayer.h"

#include "player/HTMLPlayer.h"

GINGA_FORMATTER_BEGIN

AdapterHTMLPlayer::AdapterHTMLPlayer () 
  : AdapterFormatterPlayer ()
{
  _typeSet.insert ("AdapterHTMLPlayer");
}

AdapterHTMLPlayer::~AdapterHTMLPlayer() 
{
  
}

void 
AdapterHTMLPlayer::createPlayer()
{
  _player = new HTMLPlayer(_mrl);
  
  AdapterFormatterPlayer::createPlayer();
}

GINGA_FORMATTER_END
