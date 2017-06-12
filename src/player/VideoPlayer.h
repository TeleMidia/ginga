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

GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wcast-qual)
GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>
GINGA_PRAGMA_DIAG_POP ()

#include "Player.h"
#include "mb/Display.h"
#include "mb/SDLWindow.h"

using namespace ginga::mb;

GINGA_PLAYER_BEGIN

class VideoPlayer : public Player
{
private:
  GINGA_MUTEX_DEFN ()

  GstElement *playbin;
  GstElement *binVideo;
  GstElement *filterVideo;

  GstSample *sample;            // last sample seen
  bool eosSeen;                 // true if EOS has been seen
  double soundLevel;            // sound level

private:
  void setEOS (bool);
  bool getEOS ();

public:
  VideoPlayer (const string &mrl); //works
  virtual ~VideoPlayer ();

  bool play (); //works
  void pause (); //works
  void stop (); //works
  void resume (); //works

  virtual string getPropertyValue (const string &);
  virtual void setPropertyValue (const string &, const string &);

  string getMrl ();
  bool isPlaying ();
  bool isRunning ();
  void setOutWindow (SDLWindow *);

 private:
  static void eosCB (GstAppSink *, gpointer);
  static GstFlowReturn newPrerollCB (GstAppSink *, gpointer);
  static GstFlowReturn newSampleCB (GstAppSink *, gpointer);

  static bool displayJobCallbackWrapper (DisplayJob *,
                                         SDL_Renderer *, void *); //works
  bool displayJobCallback (DisplayJob *, SDL_Renderer *); //works


  GstStateChangeReturn ret;
  GstAppSinkCallbacks callbacks;

  void createPipeline ();
  void eos();
  void printPipelineState ();

};

GINGA_PLAYER_END

#endif /* VIDEO_PLAYER_H */
