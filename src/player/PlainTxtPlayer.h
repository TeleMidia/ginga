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

#ifndef TXTPLAINPLAYER_H_
#define TXTPLAINPLAYER_H_

#include "TextPlayer.h"

GINGA_PLAYER_BEGIN

class PlainTxtPlayer : public TextPlayer
{
private:
  pthread_mutex_t mutex;
  string content;

public:
  PlainTxtPlayer (const string &mrl);
  virtual ~PlainTxtPlayer ();

protected:
  void setFile (const string &mrl);

private:
  void loadTxt ();
  void setContent (const string &content);
  void setTextAlign (const string &align);

public:
  bool play ();
  void stop ();
  virtual void setPropertyValue (const string &name, const string &value);
};

GINGA_PLAYER_END

#endif /*TXTPLAINPLAYER_H_*/
