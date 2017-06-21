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

#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include "ginga.h"

#include "Player.h"
#include "mb/Display.h"
#include "mb/SDLWindow.h"

using namespace ginga::mb;

GINGA_PLAYER_BEGIN

class VideoPlayer : public Player
{
public:
  VideoPlayer (const string &);
  virtual ~VideoPlayer ();
  bool play () override;
  void pause () override;
  void stop () override;
  void resume () override;
  void redraw (SDL_Renderer *) override;

private:
  GRecMutex _mutex;               // sync access to player data
  GstElement *_playbin;           // pipeline
  GstSample *_sample;             // last sample seen
  bool _eos;                      // true if EOS has been seen
  GstAppSinkCallbacks _callbacks; // app-sink callback data

  void lock ();
  void unlock ();
  void setSample (GstSample *);
  GstSample *getSample (void);
  void setEOS (bool);
  bool getEOS ();

  // Pipeline callbacks.
  static gboolean cb_bus (GstBus *, GstMessage *, VideoPlayer *);

  // AppSink callbacks.
  static void cb_eos (GstAppSink *, gpointer);
  static GstFlowReturn cb_new_preroll (GstAppSink *, gpointer);
  static GstFlowReturn cb_new_sample (GstAppSink *, gpointer);
};

GINGA_PLAYER_END

#endif /* VIDEO_PLAYER_H */
