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
  VideoPlayer (GingaInternal *, const string &, const string &);
  virtual ~VideoPlayer ();
  void start () override;
  void stop () override;
  void pause () override;
  void resume () override;
  void redraw (cairo_t *) override;
  bool isFreezeOn ();

protected:
  bool doSetProperty (PlayerProperty, const string &,
                      const string &) override;

private:
  GstElement *_playbin;         // pipeline
  struct {                      // audio pipeline
    GstElement *bin;            // audio bin
    GstElement *volume;         // volume filter
    GstElement *pan;            // balance filter
    GstElement *equalizer;      // equalizer filter
    GstElement *convert;        // convert audio format
    GstElement *sink;           // audio sink
  } _audio;
  struct {                      // video pipeline
    GstElement *bin;            // video bin
    GstElement *caps;           // caps filter
    GstElement *sink;           // app sink
  } _video;
  int _sample_flag;               // true if new sample is available
  GstAppSinkCallbacks _callbacks; // video app-sink callback data

  struct
  {
    bool mute;                  // true if mute is on
    double balance;             // balance sound level
    double volume;              // sound level
    double treble;              // treble level
    double bass;                // bass level
    bool freeze;                // true if freeze is on
  } _prop;

  // GStreamer callbacks.
  static gboolean cb_Bus (GstBus *, GstMessage *, VideoPlayer *);
  static GstFlowReturn cb_NewSample (GstAppSink *, gpointer);
};

GINGA_PLAYER_END

#endif // VIDEO_PLAYER_H
