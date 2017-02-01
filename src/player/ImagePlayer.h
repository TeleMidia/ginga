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

#ifndef IMAGE_PLAYER_H
#define IMAGE_PLAYER_H

#include "Player.h"

GINGA_PLAYER_BEGIN

class ImagePlayer : public Player
{
private:
  GRecMutex mutex;              // sync access to image player
  bool display_job_done;        // signals that display job is done
  GMutex display_job_mutex;     // sync access to done flag
  GCond display_job_cond;       // sync access to done flag

  void lock (void);
  void unlock (void);

  static void displayJobWrapper (SDL_Renderer *, void *);
  void displayJob (SDL_Renderer *);

public:
  ImagePlayer (const string &mrl);
  ~ImagePlayer (void);
  bool play (void);
};

GINGA_PLAYER_END

#endif /* IMAGE_PLAYER_H */
