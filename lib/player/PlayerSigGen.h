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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef PLAYER_SIGGEN_H
#define PLAYER_SIGGEN_H

#include "Player.h"

GINGA_NAMESPACE_BEGIN

class PlayerSigGen: public Player
{
public:
  PlayerSigGen (Formatter *, const string &, const string &);
  ~PlayerSigGen();
  void start () override;
  void stop () override;
  void pause () override;
  void resume () override;

protected:
  bool doSetProperty (PlayerProperty, const string &,
                      const string &) override;

private:
  GstElement *_pipeline;         // pipeline
  struct {                      // audio pipeline
    GstElement *src;            // Audio Test Src format
    GstElement *convert;        // convert audio format
    GstElement *sink;           // audio sink
  } _audio;
  int _sample_flag;             // true if new sample is available

  struct
  {
    double freq;                 // frequency
  } _prop;

  // GStreamer callbacks.
  static gboolean cb_Bus (GstBus *, GstMessage *, PlayerSigGen *);
  static GstFlowReturn cb_NewSample (GstAppSink *, gpointer);
};

GINGA_NAMESPACE_END

#endif // PLAYER_SIGGEN_H