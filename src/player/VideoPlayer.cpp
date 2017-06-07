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

#include "ginga.h"
#include "VideoPlayer.h"

#define TRACE() g_debug ("%s",G_STRFUNC);

GINGA_PLAYER_BEGIN

VideoPlayer::VideoPlayer (const string &mrl) : Player (mrl)
{
  this->soundLevel = 1.0;
  this->texture = NULL;
  this->playbin = NULL;
  this->eosSeen = false;
  this->binVideo = NULL;
  this->filterVideo = NULL;
  this->sample = NULL;
  this->mutexInit ();
  createPipeline ();
}

VideoPlayer::~VideoPlayer ()
{
  this->mutexClear ();
}

void
VideoPlayer::createPipeline ()
{
  GstElement *scale;
  GstElement *sinkVideo;

  GstPad *padVideo;

  char *uri;

  gst_init (NULL, NULL);

  this->playbin = gst_element_factory_make ("playbin", NULL);
  g_assert_nonnull (playbin);

  uri = gst_filename_to_uri (mrl.c_str (), NULL);
  g_assert_nonnull (uri);

  g_object_set (G_OBJECT (this->playbin), "uri", uri, NULL);
  g_free (uri);

  this->binVideo = gst_bin_new (NULL);
  g_assert_nonnull (this->binVideo);

  this->filterVideo = gst_element_factory_make ("capsfilter", NULL);
  g_assert_nonnull (this->filterVideo);

  scale = gst_element_factory_make ("videoscale", NULL);
  g_assert_nonnull (scale);

  sinkVideo = gst_element_factory_make ("appsink", NULL);
  g_assert_nonnull (sinkVideo);

  g_assert (gst_bin_add (GST_BIN (this->binVideo), this->filterVideo));
  g_assert (gst_bin_add (GST_BIN (this->binVideo), scale));
  g_assert (gst_bin_add (GST_BIN (this->binVideo), sinkVideo));

  g_assert (gst_element_link (this->filterVideo, scale));
  g_assert (gst_element_link (scale, sinkVideo));

  padVideo = gst_element_get_static_pad (this->filterVideo, "sink");
  gst_element_add_pad (this->binVideo, gst_ghost_pad_new ("sink", padVideo));

  g_object_set (G_OBJECT (this->playbin), "video-sink", this->binVideo, NULL);

  callbacks.eos = eosCB;
  callbacks.new_preroll = newPrerollCB;
  callbacks.new_sample = newSampleCB;
  gst_app_sink_set_callbacks (GST_APP_SINK (sinkVideo), &callbacks, this, NULL);
}

void
VideoPlayer::eosCB (arg_unused (GstAppSink *appsink), gpointer data)
{
  VideoPlayer *player = (VideoPlayer *) data;
  g_debug ("EOS!!!!!");
  player->setEOS (true);
}

GstFlowReturn
VideoPlayer::newPrerollCB (arg_unused (GstAppSink *appsink),
                           arg_unused (gpointer data))
{
  return GST_FLOW_OK;
}

GstFlowReturn
VideoPlayer::newSampleCB (GstAppSink *appsink, gpointer data)
{
  VideoPlayer *player = (VideoPlayer *) data;

  player->lock();
  if (player->sample != NULL)
    {
      gst_sample_unref (player->sample);
      player->sample = NULL;
    }
  player->sample = gst_app_sink_pull_sample (appsink);
  g_assert_nonnull (player->sample);
  player->unlock ();

  return GST_FLOW_OK;
}

bool
VideoPlayer::displayJobCallbackWrapper (DisplayJob *job,
                                        SDL_Renderer *renderer,
                                        void *self)
{
  return ((VideoPlayer *) self)->displayJobCallback (job, renderer);
}

bool
VideoPlayer::displayJobCallback (arg_unused (DisplayJob *job),
                                 SDL_Renderer *renderer)
{
  GstVideoFrame v_frame;
  GstVideoInfo v_info;
  GstBuffer *buf;
  GstCaps *caps;
  guint8 *pixels;
  guint stride;

  this->lock ();

  if (this->getEOS ())
    {
      this->setEOS (false);
      this->unlock ();
      this->forceNaturalEnd (true);
      return false;             // remove job
    }

  if (this->status == PL_SLEEPING)
    {
      this->unlock ();
      return false;             // remove job
    }

  if (this->sample == NULL)
    goto done;

  if (this->texture == NULL)
    {
      this->texture = SDL_CreateTexture (renderer,
                                         SDL_PIXELFORMAT_ARGB32,
                                         SDL_TEXTUREACCESS_TARGET,
                                         this->rect.w,
                                         this->rect.h);
      g_assert_nonnull (this->texture);
    }

  buf = gst_sample_get_buffer (this->sample);
  g_assert_nonnull (buf);

  caps = gst_sample_get_caps (this->sample);
  g_assert_nonnull (caps);

  g_assert (gst_video_info_from_caps(&v_info, caps));
  g_assert (gst_video_frame_map (&v_frame, &v_info, buf, GST_MAP_READ));

  pixels = (guint8 *) GST_VIDEO_FRAME_PLANE_DATA (&v_frame, 0);
  stride = (guint) GST_VIDEO_FRAME_PLANE_STRIDE (&v_frame, 0);
  g_assert (SDL_UpdateTexture(this->texture,
                              NULL, pixels, (int) stride) == 0);

  gst_video_frame_unmap (&v_frame);

  gst_sample_unref (this->sample);
  this->sample = NULL;

 done:
  this->unlock ();
  return true; // keep job
}

guint32
VideoPlayer::getMediaTime ()
{
  if(this->playbin==NULL)
      return 0;

  GstClock *clock = gst_element_get_clock (this->playbin);

  if(clock==NULL)
     return 0;

  guint32 time = (guint32)(GST_TIME_AS_MSECONDS (gst_clock_get_time (clock))
    - GST_TIME_AS_MSECONDS (gst_element_get_base_time (this->playbin)));

  gst_object_unref (clock);

  return time;
}

void
VideoPlayer::setMediaTime (arg_unused(guint32 pos))
{
  TRACE ();
}

bool
VideoPlayer::play ()
{
  if ( GST_ELEMENT_CAST(this->playbin)->current_state == GST_STATE_PLAYING )
  {
    return true;
  }

  g_object_set (G_OBJECT (this->playbin), "volume", this->soundLevel, NULL);

  Player::play ();

  this->ret = gst_element_set_state (this->playbin, GST_STATE_PLAYING);
  g_assert (this->ret != GST_STATE_CHANGE_FAILURE);

  g_debug ("\nVideoPlayer::play()\n");
  //clog << "\n\n\n>>VideoPlayer::play() - " << this->mrl << "\n\n\n" << endl;
  printPipelineState ();

  GstStateChangeReturn retWait = gst_element_get_state (this->playbin,
                                                          NULL,
                                                          NULL,
                                                          GST_CLOCK_TIME_NONE);
  gint n_video;
  gint n_audio;
  g_object_get (this->playbin, "n-video", &n_video, NULL); //Number of video streams
  g_object_get (this->playbin, "n-audio", &n_audio, NULL); //Number of audio streams
  g_debug ("%d video stream(s), %d audio stream(s)", n_video, n_audio);

  if ( retWait == GST_STATE_CHANGE_SUCCESS )
    {
      if (n_video > 0 )
        {
          Ginga_Display->addJob (displayJobCallbackWrapper, this);
        }
      return true;
    }

  g_print ("failed to play the file\n");
  return false;
}

void
VideoPlayer::pause ()
{
  this->lock ();
  if ( GST_ELEMENT_CAST(this->playbin)->current_state != GST_STATE_PLAYING )
  {
    this->unlock ();
    return;
  }

  Player::pause ();

  this->ret = gst_element_set_state (this->playbin, GST_STATE_PAUSED);
  g_assert (this->ret != GST_STATE_CHANGE_FAILURE);

  g_debug ("\nVideoPlayer::pause()\n");
  printPipelineState ();

  GstStateChangeReturn retWait = gst_element_get_state (this->playbin, NULL, NULL, GST_CLOCK_TIME_NONE);

  printPipelineState ();

  if ( retWait != GST_STATE_CHANGE_SUCCESS )
  {
    g_print ("failed to pause the file\n");
  }
  this->unlock ();
}

void
VideoPlayer::stop ()
{
  this->lock ();

  // when stops with natural end
  if( forcedNaturalEnd )
  {
    Player::stop ();
    gst_object_unref (this->playbin);
    gst_object_unref (this->binVideo);
    this->unlock ();
    return;
  }

  if ( GST_ELEMENT_CAST(this->playbin)->current_state != GST_STATE_PLAYING &&
       GST_ELEMENT_CAST(this->playbin)->current_state != GST_STATE_PAUSED )
  {
    this->unlock ();
    return;
  }

  this->ret = gst_element_set_state (this->playbin, GST_STATE_READY);
  //this->ret = gst_element_set_state (this->playbin, GST_STATE_NULL);
  g_assert (this->ret != GST_STATE_CHANGE_FAILURE);

  g_debug ("\nVideoPlayer::stop()\n");
  //clog << "\n\n\n>>VideoPlayer::stop() - " << this->mrl << "\n\n\n" << endl;

  printPipelineState ();

  GstStateChangeReturn retWait = gst_element_get_state (this->playbin, NULL, NULL, GST_CLOCK_TIME_NONE);

  if ( retWait != GST_STATE_CHANGE_SUCCESS )
  {
    g_print ("failed to stop the file\n");
  }

  Player::stop ();
  gst_object_unref (this->playbin);
  gst_object_unref (this->binVideo);
  this->unlock ();
}

void
VideoPlayer::resume ()
{
  Player::resume ();

  g_debug ("\nVideoPlayer::resume()\n");
  printPipelineState ();

  if (GST_ELEMENT_CAST(this->playbin)->current_state == GST_STATE_PAUSED)
  {
    this->play ();
  }
}

string
VideoPlayer::getPropertyValue (const string &name)
{
  return Player::getPropertyValue(name);
}

void
VideoPlayer::setPropertyValue (const string &name, const string &value)
{
  if (value.length () == 0)
    return;

  if (name == "soundLevel")
  {
      this->soundLevel = xstrtodorpercent (value, NULL);
      g_debug ("video: setting soundLevel to %f\n", this->soundLevel);
      g_object_set (G_OBJECT (this->playbin), "volume",
                    this->soundLevel, NULL);
  }

  //TODO
  //balanceLevel (Gstreamer audiopanorama)
  //trebleLevel
  //bassLevel

  Player::setPropertyValue (name, value);
}

string
VideoPlayer::getMrl ()
{
  return this->mrl;
}

bool
VideoPlayer::isPlaying ()
{
  GstElement *elt = GST_ELEMENT_CAST (this->playbin);
  return elt->current_state == GST_STATE_PLAYING;
}

void
VideoPlayer::setOutWindow (SDLWindow* windowId)
{
  GstCaps *caps;
  GstStructure *st;

  this->window = windowId;
  if(windowId!=NULL)
  {
      this->rect = windowId->getRect();
      windowId->getZ (&this->z, &this->zorder);
  }

  st = gst_structure_new_empty ("video/x-raw");
  gst_structure_set (st, "format", G_TYPE_STRING, "ARGB",
                      "width", G_TYPE_INT, this->rect.w,
                      "height", G_TYPE_INT, this->rect.h,
                      NULL);

  caps = gst_caps_new_full (st, NULL);
  g_assert_nonnull (caps);

  g_object_set (this->filterVideo, "caps", caps, NULL);
  gst_caps_unref (caps);
}

bool
VideoPlayer::isRunning ()
{
  return this->isPlaying ();
}

void
VideoPlayer::printPipelineState()
{
  if (GST_ELEMENT_CAST(this->playbin)->current_state == GST_STATE_PAUSED)
  {
    g_debug ("PIPELINE::PAUSED\n");
  }
  else if (GST_ELEMENT_CAST(this->playbin)->current_state == GST_STATE_PLAYING)
  {
    g_debug ("PIPELINE::PLAYING\n");
  }
  else if (GST_ELEMENT_CAST(this->playbin)->current_state == GST_STATE_READY)
  {
    g_debug ("PIPELINE::READY\n");
  }
  else if (GST_ELEMENT_CAST(this->playbin)->current_state == GST_STATE_NULL)
  {
    g_debug ("PIPELINE::NULL\n");
  }
  else if (GST_ELEMENT_CAST(this->playbin)->current_state == GST_STATE_VOID_PENDING)
  {
    g_debug ("PIPELINE::PENDING\n");
  }
}

void
VideoPlayer::setEOS (bool eos)
{
  this->lock ();
  this->eosSeen = eos;
  this->unlock ();
}

bool
VideoPlayer::getEOS (void)
{
  bool eos;
  this->lock ();
  eos = this->eosSeen;
  this->unlock ();
  return eos;
}

GINGA_PLAYER_END
