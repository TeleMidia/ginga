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

#include "aux-ginga.h"
#include "aux-gl.h"
#include "PlayerSigGen.h"

GINGA_NAMESPACE_BEGIN

#define gstx_element_get_state(elt, st, pend, tout)                        \
  g_assert (gst_element_get_state ((elt), (st), (pend), (tout))            \
            != GST_STATE_CHANGE_FAILURE)

#define gstx_element_get_state_sync(elt, st, pend)                         \
  gstx_element_get_state ((elt), (st), (pend), GST_CLOCK_TIME_NONE)

#define gstx_element_set_state(elt, st)                                    \
  g_assert (gst_element_set_state ((elt), (st)) != GST_STATE_CHANGE_FAILURE)

#define gstx_element_set_state_sync(elt, st)                               \
  G_STMT_START                                                             \
  {                                                                        \
    gstx_element_set_state ((elt), (st));                                  \
    gstx_element_get_state_sync ((elt), nullptr, nullptr);                 \
  }                                                                        \
  G_STMT_END

// Public.

PlayerSigGen::PlayerSigGen (Formatter *formatter, Media *media)
    : Player (formatter, media)
{
  GstBus *bus;
  gulong ret;

  _pipeline = nullptr;
  _audio.src = nullptr;
  _audio.convert = nullptr;
  _audio.tee = nullptr;
  _audio.audioQueue = nullptr;
  _audio.audioSink = nullptr;
  _audio.videoQueue = nullptr;
  _audio.videoScope = nullptr;
  _audio.videoConvert = nullptr;
  _audio.videoSink = nullptr;

  if (!gst_is_initialized ())
    {
      GError *error = nullptr;
      if (unlikely (!gst_init_check (nullptr, nullptr, &error)))
        {
          g_assert_nonnull (error);
          ERROR ("%s", error->message);
          g_error_free (error);
        }
    }

  _pipeline = gst_pipeline_new ("pipeline");
  g_assert_nonnull (_pipeline);

  bus = gst_pipeline_get_bus (GST_PIPELINE (_pipeline));
  g_assert_nonnull (bus);
  ret = gst_bus_add_watch (bus, (GstBusFunc) cb_Bus, this);
  g_assert (ret > 0);
  gst_object_unref (bus);

  // Setup audio pipeline.
  _audio.src = gst_element_factory_make ("audiotestsrc", "audio.src");
  g_assert_nonnull (_audio.src);
  _audio.convert
      = gst_element_factory_make ("audioconvert", "audioconvert");
  g_assert_nonnull (_audio.convert);
  _audio.tee = gst_element_factory_make ("tee", "teesplit");
  g_assert_nonnull (_audio.tee);

  // Audio thread
  _audio.audioQueue = gst_element_factory_make ("queue", "audioqueue");
  g_assert_nonnull (_audio.audioQueue);
  // Try to use ALSA if available.
  _audio.audioSink = gst_element_factory_make ("alsasink", "audio.sink");
  if (_audio.audioSink == nullptr)
    _audio.audioSink
        = gst_element_factory_make ("autoaudiosink", "audio.sink");
  g_assert_nonnull (_audio.audioSink);

  // Video thread
  _audio.videoQueue = gst_element_factory_make ("queue", "videoqueue");
  g_assert_nonnull (_audio.videoQueue);
  _audio.videoScope = gst_element_factory_make ("spectrascope", "scope");
  g_assert_nonnull (_audio.videoScope);
  _audio.videoConvert
      = gst_element_factory_make ("videoconvert", "videoconvert");
  g_assert_nonnull (_audio.videoConvert);
  _audio.videoSink = gst_element_factory_make ("appsink", "videosink");
  g_assert_nonnull (_audio.videoSink);
  g_object_set (_audio.videoSink, "max-buffers", 100, "drop", true,
                nullptr);

  // Pipeline add
  g_assert (gst_bin_add (GST_BIN (_pipeline), _audio.src));
  g_assert (gst_bin_add (GST_BIN (_pipeline), _audio.convert));
  g_assert (gst_bin_add (GST_BIN (_pipeline), _audio.tee));
  g_assert (gst_bin_add (GST_BIN (_pipeline), _audio.audioQueue));
  g_assert (gst_bin_add (GST_BIN (_pipeline), _audio.audioSink));
  g_assert (gst_bin_add (GST_BIN (_pipeline), _audio.videoQueue));
  g_assert (gst_bin_add (GST_BIN (_pipeline), _audio.videoScope));
  g_assert (gst_bin_add (GST_BIN (_pipeline), _audio.videoConvert));
  g_assert (gst_bin_add (GST_BIN (_pipeline), _audio.videoSink));

  // Pipeline common link
  g_assert (gst_element_link (_audio.src, _audio.convert));
  g_assert (gst_element_link (_audio.convert, _audio.tee));

  // Pipeline audio link
  g_assert (gst_element_link (_audio.audioQueue, _audio.audioSink));

  // Pipeline video link
  g_assert (gst_element_link (_audio.videoQueue, _audio.videoScope));
  g_assert (gst_element_link (_audio.videoScope, _audio.videoConvert));
  g_assert (gst_element_link (_audio.videoConvert, _audio.videoSink));

  // Audio pad linking
  _audio.teeAudioPad = gst_element_get_request_pad (_audio.tee, "src_%u");
  g_assert_nonnull (_audio.teeAudioPad);
  _audio.queueAudioPad
      = gst_element_get_static_pad (_audio.audioQueue, "sink");
  g_assert_nonnull (_audio.queueAudioPad);

  if (gst_pad_link (_audio.teeAudioPad, _audio.queueAudioPad)
      != GST_PAD_LINK_OK)
    {
      ERROR ("Tee and audio queue not linked");
    }

  // Video pad linking
  _audio.teeVideoPad = gst_element_get_request_pad (_audio.tee, "src_%u");
  g_assert_nonnull (_audio.teeVideoPad);
  _audio.queueVideoPad
      = gst_element_get_static_pad (_audio.videoQueue, "sink");
  g_assert_nonnull (_audio.queueVideoPad);

  if (gst_pad_link (_audio.teeVideoPad, _audio.queueVideoPad)
      != GST_PAD_LINK_OK)
    {
      ERROR ("Tee and video queue not linked");
    }

  gst_object_unref (_audio.queueAudioPad);
  gst_object_unref (_audio.queueVideoPad);

  // Callbacks.
  _callbacks.eos = nullptr;
  _callbacks.new_preroll = nullptr;
  _callbacks.new_sample = cb_NewSample;
  gst_app_sink_set_callbacks (GST_APP_SINK (_audio.videoSink), &_callbacks,
                              this, nullptr);

  // Initialize handled properties.
  static set<string> handled = {
    "freq",
    "wave",
    "volume",
  };
  this->resetProperties (&handled);
}

PlayerSigGen::~PlayerSigGen ()
{
}

void
PlayerSigGen::start ()
{
  GstCaps *caps;
  GstStructure *st;
  GstStateChangeReturn ret;

  g_assert (_state != OCCURRING);
  TRACE ("starting");

  st = gst_structure_new_empty ("audio/x-raw");
  gst_structure_set (st, "format", G_TYPE_STRING, "BGRA", nullptr);

  caps = gst_caps_new_full (st, nullptr);
  g_assert_nonnull (caps);

  Player::setEOS (false);
  g_atomic_int_set (&_sample_flag, 0);

  // Initialize properties.
  g_object_set (_audio.src, "freq", _prop.freq, "wave", _prop.wave,
                "volume", _prop.volume, nullptr);

  ret = gst_element_set_state (_pipeline, GST_STATE_PLAYING);
  if (unlikely (ret == GST_STATE_CHANGE_FAILURE))
    Player::setEOS (true);

  Player::start ();
  TRACE ("started");
}

void
PlayerSigGen::stop ()
{
  g_assert (_state != SLEEPING);
  TRACE ("stopping");

  gstx_element_set_state_sync (_pipeline, GST_STATE_NULL);
  gst_object_unref (_pipeline);
  Player::stop ();
}

void
PlayerSigGen::pause ()
{
  g_assert (_state != PAUSED && _state != SLEEPING);
  TRACE ("pausing");

  gstx_element_set_state_sync (_pipeline, GST_STATE_PAUSED);
  Player::pause ();
}

void
PlayerSigGen::resume ()
{
  g_assert (_state == PAUSED);
  TRACE ("resuming");

  gstx_element_set_state_sync (_pipeline, GST_STATE_PLAYING);
  Player::resume ();
}

void
PlayerSigGen::redraw (cairo_t *cr)
{
  GstSample *sample;
  GstVideoFrame v_frame;
  GstVideoInfo v_info;
  GstBuffer *buf;
  GstCaps *caps;
  guint8 *pixels;
  int width;
  int height;
  int stride;

  static cairo_user_data_key_t key;
  cairo_status_t status;

  g_assert (_state != SLEEPING);

  if (Player::getEOS ())
    goto done;

  if (!g_atomic_int_compare_and_exchange (&_sample_flag, 1, 0))
    goto done;

  sample = gst_app_sink_pull_sample (GST_APP_SINK (_audio.videoSink));
  if (sample == nullptr)
    goto done;

  buf = gst_sample_get_buffer (sample);
  g_assert_nonnull (buf);

  caps = gst_sample_get_caps (sample);
  g_assert_nonnull (caps);

  g_assert (gst_video_info_from_caps (&v_info, caps));
  g_assert (gst_video_frame_map (&v_frame, &v_info, buf, GST_MAP_READ));

  pixels = (guint8 *) GST_VIDEO_FRAME_PLANE_DATA (&v_frame, 0);
  width = GST_VIDEO_FRAME_WIDTH (&v_frame);
  height = GST_VIDEO_FRAME_HEIGHT (&v_frame);
  stride = (int) GST_VIDEO_FRAME_PLANE_STRIDE (&v_frame, 0);

  if (_opengl)
    {
      if (_gltexture)
        GL::delete_texture (&_gltexture);
      // FIXME: Do not create a new texture for each frame.
      GL::create_texture (&_gltexture, width, height, pixels);
      gst_video_frame_unmap (&v_frame);
      gst_sample_unref (sample);
    }
  else
    {
      if (_surface != nullptr)
        cairo_surface_destroy (_surface);

      _surface = cairo_image_surface_create_for_data (
          pixels, CAIRO_FORMAT_ARGB32, width, height, stride);
      g_assert_nonnull (_surface);
      gst_video_frame_unmap (&v_frame);
      status = cairo_surface_set_user_data (
          _surface, &key, (void *) sample,
          (cairo_destroy_func_t) gst_sample_unref);
      g_assert (status == CAIRO_STATUS_SUCCESS);
    }

done:
  Player::redraw (cr);
}

// Protected.

bool
PlayerSigGen::doSetProperty (Property code, unused (const string &name),
                             const string &value)
{

  switch (code)
    {
    case PROP_FREQ:
      _prop.freq = xstrtodorpercent (value, nullptr);
      if (_state != SLEEPING)
        g_object_set (_audio.src, "freq", _prop.freq, nullptr);
      break;
    case PROP_WAVE:
      TRACE ("Changed wave");
      TRACE ("Evaluation: %s", value.c_str ());

      if (value == "sine")
        _prop.wave = 0;
      else if (value == "square")
        _prop.wave = 1;
      else if (value == "saw")
        _prop.wave = 2;
      else if (value == "triangle")
        _prop.wave = 3;
      else if (value == "silence")
        _prop.wave = 4;
      else if (value == "white-noise")
        _prop.wave = 5;
      else if (value == "pink-noise")
        _prop.wave = 6;
      else if (value == "sine-table")
        _prop.wave = 7;
      else if (value == "ticks")
        _prop.wave = 8;
      else if (value == "gaussian-noise")
        _prop.wave = 9;
      else if (value == "red-noise")
        _prop.wave = 10;
      else if (value == "blue-noise")
        _prop.wave = 11;
      else if (value == "violet-noise")
        _prop.wave = 12;

      if (_state != SLEEPING)
        {
          g_object_set (_audio.src, "wave", _prop.wave, nullptr);
        }
      break;
    case PROP_VOLUME:
      _prop.volume = xstrtodorpercent (value, nullptr);
      TRACE ("Vol: %f", _prop.volume);
      if (_state != SLEEPING)
        g_object_set (_audio.src, "volume", _prop.volume, nullptr);
      break;
    default:
      return Player::doSetProperty (code, name, value);
    }
  return true;
}

// Private: Static (GStreamer callbacks).

gboolean
PlayerSigGen::cb_Bus (GstBus *bus, GstMessage *msg, PlayerSigGen *player)
{
  g_assert_nonnull (bus);
  g_assert_nonnull (msg);
  g_assert_nonnull (player);

  switch (GST_MESSAGE_TYPE (msg))
    {
    case GST_MESSAGE_EOS:
      {
        TRACE ("EOS");
        break;
      }
    case GST_MESSAGE_ERROR:
    case GST_MESSAGE_WARNING:
      {
        GstObject *obj = nullptr;
        GError *error = nullptr;

        obj = GST_MESSAGE_SRC (msg);
        g_assert_nonnull (obj);

        if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR)
          {
            gst_message_parse_error (msg, &error, nullptr);
            g_assert_nonnull (error);
            ERROR ("%s", error->message);
          }
        else
          {
            gst_message_parse_warning (msg, &error, nullptr);
            g_assert_nonnull (error);
            WARNING ("%s", error->message);
          }
        g_error_free (error);
        break;
      }
    default:
      break;
    }
  return TRUE;
}

GstFlowReturn
PlayerSigGen::cb_NewSample (unused (GstAppSink *appsink), gpointer data)
{
  PlayerSigGen *player = (PlayerSigGen *) data;
  g_assert_nonnull (player);
  g_atomic_int_compare_and_exchange (&player->_sample_flag, 0, 1);
  return GST_FLOW_OK;
}

GINGA_NAMESPACE_END
