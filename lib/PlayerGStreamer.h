/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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

#ifndef GINGA_PLAYER_GSTREAMER_H
#define GINGA_PLAYER_GSTREAMER_H

#include "Player.h"

GINGA_NAMESPACE_BEGIN

class PlayerGStreamer : public Player
{
public:

  PlayerGStreamer (Media *);

  ~PlayerGStreamer ();

  void setURI (const string &uri) override;

  void start () override;

  void pause () override;

  void stop () override;

  void redraw (cairo_t *) override;

private:

  /// The GStreamer playbin (pipeline) of this player.
  GstElement *_playbin;

  /// The elements that process the video output of playbin.
  struct {
    GstElement *bin;            // bin
    GstElement *caps;           // capsfilter
    GstElement *sink;           // appsink
  } _video;

  /// The bus-watch callback.  Called whenever a message is posted in
  /// playbin's bus.
  static gboolean busWatch (GstBus *bus, GstMessage *msg,
                            PlayerGStreamer *player);
};

GINGA_NAMESPACE_END

#endif // GINGA_PLAYER_GSTREAMER_H
