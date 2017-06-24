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

#include "Player.h"

GINGA_PLAYER_BEGIN

class VideoPlayer : public Player
{
public:
  VideoPlayer (const string &);
  virtual ~VideoPlayer ();
  void start () override;
  void stop () override;
  void pause () override;
  void resume () override;
  void redraw (SDL_Renderer *) override;

private:
  GRecMutex _mutex;               // sync access to player data
  GstElement *_playbin;           // pipeline
  bool _playbin_eos;              // true if playbin generated an EOS
  GstSample *_sample;             // last sample seen
  GstAppSinkCallbacks _callbacks; // app-sink callback data

  void lock ();
  void unlock ();
  bool getPlaybinEOS (void);
  void setPlaybinEOS (bool);
  GstSample *getSample (void);
  void setSample (GstSample *);

  // Pipeline callbacks.
  static gboolean cb_Bus (GstBus *, GstMessage *, VideoPlayer *);

  // AppSink callbacks.
  static void cb_EOS (GstAppSink *, gpointer);
  static GstFlowReturn cb_NewPreroll (GstAppSink *, gpointer);
  static GstFlowReturn cb_NewSample (GstAppSink *, gpointer);
};

GINGA_PLAYER_END

#endif /* VIDEO_PLAYER_H */
