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

#define gstx_element_get_state(elt, st, pend, tout)                     \
  G_STMT_START                                                          \
  {                                                                     \
    g_assert (gst_element_get_state ((elt), (st), (pend), (tout))       \
              != GST_STATE_CHANGE_FAILURE);                             \
  }                                                                     \
  G_STMT_END

#define gstx_element_get_state_sync(elt, st, pend)                      \
  G_STMT_START                                                          \
  {                                                                     \
    gstx_element_get_state ((elt), (st), (pend), GST_CLOCK_TIME_NONE);  \
  }                                                                     \
  G_STMT_END

#define gstx_element_set_state(elt, st)                 \
  G_STMT_START                                          \
  {                                                     \
    g_assert (gst_element_set_state ((elt), (st))       \
              != GST_STATE_CHANGE_FAILURE);             \
  }                                                     \
  G_STMT_END

#define gstx_element_set_state_sync(elt, st)            \
  G_STMT_START                                          \
  {                                                     \
    gstx_element_set_state ((elt), (st));               \
    gstx_element_get_state_sync ((elt), NULL, NULL);    \
  }                                                     \
  G_STMT_END

GINGA_PLAYER_BEGIN

VideoPlayer::VideoPlayer (const string &mrl) : Player (mrl)
{
  gchar *uri;
  GstElement *bin;
  GstElement *elt_filter;
  GstElement *elt_scale;
  GstElement *elt_sink;
  GstPad *pad;

  this->texture = nullptr;

  g_rec_mutex_init (&_mutex);
  _eos = false;
  _playbin = nullptr;
  _sample = nullptr;

  if (!gst_is_initialized ())
    {
      GError *error = NULL;
      if (unlikely (!gst_init_check (NULL, NULL, &error)))
        {
          g_assert_nonnull (error);
          ERROR ("%s", error->message);
          g_error_free (error);
        }
    }

  _playbin = gst_element_factory_make ("playbin", NULL);
  g_assert_nonnull (_playbin);

  uri = gst_filename_to_uri (mrl.c_str (), NULL);
  g_assert_nonnull (uri);

  g_object_set (G_OBJECT (_playbin), "uri", uri, NULL);
  g_free (uri);

  bin = gst_bin_new ("videobin");
  g_assert_nonnull (bin);

  elt_filter = gst_element_factory_make ("capsfilter", "filter");
  g_assert_nonnull (elt_filter);

  elt_scale = gst_element_factory_make ("videoscale", "scale");
  g_assert_nonnull (elt_scale);

  elt_sink = gst_element_factory_make ("appsink", "sink");
  g_assert_nonnull (elt_sink);

  g_assert (gst_bin_add (GST_BIN (bin), elt_filter));
  g_assert (gst_bin_add (GST_BIN (bin), elt_scale));
  g_assert (gst_bin_add (GST_BIN (bin), elt_sink));
  g_assert (gst_element_link (elt_filter, elt_scale));
  g_assert (gst_element_link (elt_scale, elt_sink));

  pad = gst_element_get_static_pad (elt_filter, "sink");
  g_assert_nonnull (pad);
  g_assert (gst_element_add_pad (bin, gst_ghost_pad_new ("sink", pad)));
  g_object_set (G_OBJECT (_playbin), "video-sink", bin, NULL);

  _callbacks.eos = cb_eos;
  _callbacks.new_preroll = cb_new_preroll;
  _callbacks.new_sample = cb_new_sample;
  gst_app_sink_set_callbacks (GST_APP_SINK (elt_sink),
                              &_callbacks, this, NULL);
}

VideoPlayer::~VideoPlayer ()
{
  g_rec_mutex_clear (&_mutex);
}

bool
VideoPlayer::play ()
{
  GstCaps *caps;
  GstStructure *st;
  GstElement *bin = NULL;
  GstElement *filter = NULL;

  if (unlikely (this->status == PL_OCCURRING))
    {
      WARNING ("player already occurring");
      return true;
    }

  st = gst_structure_new_empty ("video/x-raw");
  gst_structure_set (st,
                     "format", G_TYPE_STRING, "ARGB",
                     "width", G_TYPE_INT, this->rect.w,
                     "height", G_TYPE_INT, this->rect.h,
                     NULL);

  caps = gst_caps_new_full (st, NULL);
  g_assert_nonnull (caps);

  g_object_get (G_OBJECT (_playbin), "video-sink", &bin, NULL);
  g_assert_nonnull (bin);

  filter = gst_bin_get_by_name (GST_BIN (bin), "filter");
  g_assert_nonnull (filter);

  g_object_set (filter, "caps", caps, NULL);
  gst_caps_unref (caps);
  gst_object_unref (filter);

  gstx_element_set_state_sync (_playbin, GST_STATE_PLAYING);

  g_debug ("starting");
  Player::play ();
  return true;
}

void
VideoPlayer::pause ()
{
  if (unlikely (this->status != PL_OCCURRING))
    {
      WARNING ("player not ocurring");
      return;
    }

  gstx_element_set_state_sync (_playbin, GST_STATE_PAUSED);

  g_debug ("pausing");
  Player::pause ();
}

void
VideoPlayer::stop ()
{
  if (unlikely (this->status == PL_SLEEPING))
    {
      WARNING ("player already sleeping");
      return;
    }
  gstx_element_set_state_sync (_playbin, GST_STATE_NULL);
  gst_object_unref (_playbin);

  g_debug ("stopping");
  Player::stop ();
}

void
VideoPlayer::resume ()
{
  if (unlikely (this->status != PL_PAUSED))
    {
      WARNING ("player is not paused");
      return;
    }

  g_assert (GST_ELEMENT_CAST (_playbin)->current_state == GST_STATE_PAUSED);
  gstx_element_set_state_sync (_playbin, GST_STATE_PLAYING);

  g_debug ("resumming");
  Player::resume ();
}

void
VideoPlayer::redraw (SDL_Renderer *renderer)
{
  GstSample *sample;
  GstVideoFrame v_frame;
  GstVideoInfo v_info;
  GstBuffer *buf;
  GstCaps *caps;
  guint8 *pixels;
  guint stride;

  if (this->getEOS ())
    {
      this->setEOS (false);
      this->forceNaturalEnd (true);
      return;
    }

  sample = this->getSample ();
  if (sample == NULL)
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

  buf = gst_sample_get_buffer (sample);
  g_assert_nonnull (buf);

  caps = gst_sample_get_caps (sample);
  g_assert_nonnull (caps);

  g_assert (gst_video_info_from_caps (&v_info, caps));
  g_assert (gst_video_frame_map (&v_frame, &v_info, buf, GST_MAP_READ));

  pixels = (guint8 *) GST_VIDEO_FRAME_PLANE_DATA (&v_frame, 0);
  stride = (guint) GST_VIDEO_FRAME_PLANE_STRIDE (&v_frame, 0);
  g_assert (SDL_UpdateTexture (this->texture,
                               NULL, pixels, (int) stride) == 0);

  gst_video_frame_unmap (&v_frame);
  gst_sample_unref (sample);

 done:
  Player::redraw (renderer);
}


// Private.

void
VideoPlayer::lock (void)
{
  g_rec_mutex_lock (&_mutex);
}

void
VideoPlayer::unlock (void)
{
  g_rec_mutex_unlock (&_mutex);
}

void
VideoPlayer::setSample (GstSample *sample)
{
  g_assert_nonnull (sample);

  this->lock ();
  if (_sample != NULL)
    gst_sample_unref (_sample);
  _sample = sample;
  this->unlock ();
}

GstSample *
VideoPlayer::getSample (void)
{
  GstSample *sample;
  this->lock ();
  sample = _sample;
  _sample = NULL;
  this->unlock ();
  return sample;
}

void
VideoPlayer::setEOS (bool eos)
{
  this->lock ();
  _eos = eos;
  this->unlock ();
}

bool
VideoPlayer::getEOS (void)
{
  bool eos;
  this->lock ();
  eos = _eos;
  this->unlock ();
  return eos;
}

void
VideoPlayer::cb_eos (arg_unused (GstAppSink *appsink), gpointer data)
{
  VideoPlayer *player = (VideoPlayer *) data;
  player->setEOS (true);
  g_debug ("EOS");
}

GstFlowReturn
VideoPlayer::cb_new_preroll (arg_unused (GstAppSink *appsink),
                             arg_unused (gpointer data))
{
  return GST_FLOW_OK;
}

GstFlowReturn
VideoPlayer::cb_new_sample (GstAppSink *appsink, gpointer data)
{
  VideoPlayer *player = (VideoPlayer *) data;
  player->setSample (gst_app_sink_pull_sample (appsink));
  return GST_FLOW_OK;
}

GINGA_PLAYER_END
