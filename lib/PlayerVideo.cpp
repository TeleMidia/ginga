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
#include "PlayerVideo.h"

// clang-format off
GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-equal)
// clang-format on

#define _ERROR(fmt, ...)\
  ERROR ("PlayerGStreamer error: " fmt, ## __VA_ARGS__)

#define _WARNING(fmt, ...)\
  WARNING ("PlayerGStreamer warning: " fmt, ## __VA_ARGS__)

#define _TRACE(fmt, ...)\
  __ginga_log (g_print, fmt "\n", ## __VA_ARGS__)

#define _TRACE_MESSAGE(msg, fmt, ...)           \
  _TRACE ("%s: %s: " fmt,                       \
         GST_MESSAGE_SRC_NAME (msg),            \
         GST_MESSAGE_TYPE_NAME (msg),           \
         ## __VA_ARGS__)

#define gstx_element_get_state(elt, state, pending, timeout)            \
  G_STMT_START                                                          \
  {                                                                     \
    if (unlikely (gst_element_get_state                                 \
                  ((elt), (state), (pending), (timeout))                \
                  == GST_STATE_CHANGE_FAILURE))                         \
      {                                                                 \
        _ERROR ("failed to get %s state", gst_element_get_name (elt));  \
      }                                                                 \
  }                                                                     \
  G_STMT_END

#define gstx_element_get_state_sync(elt, state, pending)        \
  G_STMT_START                                                  \
  {                                                             \
    gstx_element_get_state ((elt), (state), (pending),          \
                            GST_CLOCK_TIME_NONE);               \
  }                                                             \
  G_STMT_END

#define gstx_element_set_state(elt, state)                              \
  G_STMT_START                                                          \
  {                                                                     \
    if (unlikely (gst_element_set_state ((elt), (state))                \
                  == GST_STATE_CHANGE_FAILURE))                         \
      {                                                                 \
        _ERROR ("failed to set %s state", gst_element_get_name (elt));  \
      }                                                                 \
  }                                                                     \
  G_STMT_END

#define gstx_element_set_state_sync(elt, state)                 \
  G_STMT_START                                                  \
  {                                                             \
    gstx_element_set_state ((elt), (state));                    \
    gstx_element_get_state_sync ((elt), NULL, NULL);            \
  }                                                             \
  G_STMT_END

namespace ginga {

// Public.

PlayerVideo::PlayerVideo (Formatter *formatter, Media *media)
    : Player (formatter, media)
{
  GstBus *bus;
  gulong ret;

  GstCaps *caps;
  GstStructure *st;

  GstPad *pad;
  GstPad *ghost;

  _TRACE ("");

  // Initialize GStreamer.
  if (!gst_is_initialized ())
    {
      GError *err = NULL;

      if (unlikely (!gst_init_check (NULL, NULL, &err)))
        {
          g_assert_nonnull (err);
          _ERROR ("%s", err->message);
          g_error_free (err);
        }
      g_assert_null (err);
    }

  // Create and initialize playbin.
  _playbin = gst_element_factory_make ("playbin", "playbin");
  if (unlikely (_playbin == NULL))
    {
      _ERROR ("cannot create playbin");
    }
  g_assert_nonnull (_playbin);

  bus = gst_pipeline_get_bus (GST_PIPELINE (_playbin));
  g_assert_nonnull (bus);
  ret = gst_bus_add_watch (bus, (GstBusFunc) cb_Bus, this);
  g_assert (ret > 0);
  gst_object_unref (bus);

  // Setup video the processing bin.
  _video.bin = gst_bin_new ("video.bin");
  g_assert_nonnull (_video.bin);

  _video.caps = gst_element_factory_make ("capsfilter", "video.caps");
  g_assert_nonnull (_video.caps);

  st = gst_structure_new_empty ("video/x-raw");
  gst_structure_set (st, "format", G_TYPE_STRING, "BGRA", nullptr);

  caps = gst_caps_new_full (st, nullptr);
  g_assert_nonnull (caps);
  g_object_set (_video.caps, "caps", caps, nullptr);
  gst_caps_unref (caps);

  _video.sink = gst_element_factory_make ("appsink", "video.sink");
  g_assert_nonnull (_video.sink);

  g_assert (gst_bin_add (GST_BIN (_video.bin), _video.caps));
  g_assert (gst_bin_add (GST_BIN (_video.bin), _video.sink));
  g_assert (gst_element_link (_video.caps, _video.sink));

  pad = gst_element_get_static_pad (_video.caps, "sink");
  g_assert_nonnull (pad);
  ghost = gst_ghost_pad_new ("sink", pad);
  g_assert_nonnull (ghost);
  g_assert (gst_element_add_pad (_video.bin, ghost));
  gst_object_unref (pad);

  g_object_set (G_OBJECT (_playbin), "video-sink", _video.bin, nullptr);
}

PlayerVideo::~PlayerVideo ()
{
  _TRACE ("");
  gstx_element_set_state_sync (_playbin, GST_STATE_NULL);
  gst_object_unref (_playbin);
}

void
PlayerVideo::start ()
{
  g_assert_nonnull (_playbin);

  _TRACE ("");
  gstx_element_set_state (_playbin, GST_STATE_PLAYING);
  Player::start ();
}

void
PlayerVideo::stop ()
{
  g_assert_nonnull (_playbin);

  _TRACE ("");
  gstx_element_set_state (_playbin, GST_STATE_NULL);
  Player::stop ();
}

void
PlayerVideo::pause ()
{
  g_assert_nonnull (_playbin);

  _TRACE ("");
  gstx_element_set_state (_playbin, GST_STATE_PAUSED);
  Player::pause ();
}

void
PlayerVideo::resume ()
{
  g_assert (_state == PAUSED);
  TRACE ("resuming %s", _id.c_str ());

  gstx_element_set_state_sync (_playbin, GST_STATE_PLAYING);
  Player::resume ();
}

void
PlayerVideo::seek (gint64 value)
{
  TRACE ("seek %s to: %" GST_TIME_FORMAT, _id.c_str (),
         GST_TIME_ARGS (value));

  GstState curr, pending;
  GstStateChangeReturn ret;

  if (unlikely (!gst_element_seek (_playbin, _prop.speed, GST_FORMAT_TIME,
                                   GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET,
                                   value, GST_SEEK_TYPE_NONE,
                                   (gint64) GST_CLOCK_TIME_NONE)))
    TRACE ("seek failed");

  ret = gst_element_get_state (_playbin, &curr, &pending,
                               GST_CLOCK_TIME_NONE);
  if (unlikely (ret == GST_STATE_CHANGE_FAILURE))
    {
      string m = gst_element_state_change_return_get_name (ret);
      TRACE ("%s to change state", m.c_str ());
    }
}

void
PlayerVideo::speed (double value)
{
  TRACE ("speed %s to: %f", _id.c_str (), value);
  if (doubleeq (value, 0.))
    {
      return; // nothing to do
    }

  gint64 position = getStreamMediaTime ();
  GstEvent *seek_event;
  if (value > 0)
    {
      seek_event = gst_event_new_seek (
          value, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET,
          position, GST_SEEK_TYPE_NONE, 0);
    }
  else
    {
      seek_event = gst_event_new_seek (
          value, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET, 0,
          GST_SEEK_TYPE_NONE, position);
    }
  if (unlikely (!gst_element_send_event (_video.sink, seek_event)))
    TRACE ("speed failed");
  gst_event_unref (seek_event);
}

void
PlayerVideo::redraw (cairo_t *cr)
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

  // if (Player::getEOS ())
  //   goto done;

  sample = gst_app_sink_try_pull_sample (GST_APP_SINK (_video.sink), 0);
  if (sample == NULL)
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

  if (_surface != NULL)
    cairo_surface_destroy (_surface);

  _surface = cairo_image_surface_create_for_data
    (pixels, CAIRO_FORMAT_ARGB32, width, height, stride);
  g_assert_nonnull (_surface);
  gst_video_frame_unmap (&v_frame);
  status = cairo_surface_set_user_data
    (_surface, &key, (void *) sample,
     (cairo_destroy_func_t) gst_sample_unref);
  g_assert (status == CAIRO_STATUS_SUCCESS);

done:
  Player::redraw (cr);
}

gint64
PlayerVideo::getPipelineTime ()
{
  return (gint64) GST_TIME_AS_NSECONDS (
      gst_clock_get_time (gst_element_get_clock (_playbin)));
}

gint64
PlayerVideo::getStreamMediaTime ()
{
  gint64 cur;
  if (unlikely (
          !gst_element_query_position (_playbin, GST_FORMAT_TIME, &cur)))
    TRACE ("Get %s time failed", _id.c_str ());
  return cur;
}

gint64
PlayerVideo::getStreamMediaDuration ()
{
  gint64 dur;
  if (unlikely (
          !gst_element_query_duration (_playbin, GST_FORMAT_TIME, &dur)))
    TRACE ("Get %s duration failed", _id.c_str ());
  return dur;
}
gboolean
PlayerVideo::cb_Bus (GstBus *bus, GstMessage *msg, PlayerVideo *player)
{
  g_assert_nonnull (bus);
  g_assert_nonnull (msg);
  g_assert_nonnull (player);

  switch (GST_MESSAGE_TYPE (msg))
    {
    case GST_MESSAGE_ASYNC_DONE:
      {
        GstClockTime running_time;

        gst_message_parse_async_done (msg, &running_time);
        _TRACE_MESSAGE (msg, "%" GST_TIME_FORMAT,
                        GST_TIME_ARGS (running_time));
        break;
      }
    case GST_MESSAGE_BUFFERING:
      {
        gint percent = 0;
        gst_message_parse_buffering (msg, &percent);
        _TRACE_MESSAGE (msg, "%d percent", percent);
        break;
      }
    case GST_MESSAGE_DURATION_CHANGED:
      {
        GstFormat fmt;
        const char *name;
        gint64 dur;

        gst_message_parse_duration (msg, &fmt, &dur);
        name = gst_format_get_name (fmt);
        switch (fmt)
          {
          case GST_FORMAT_UNDEFINED:
            _TRACE_MESSAGE (msg, "%s %" G_GINT64_FORMAT " undefined", name,
                            dur);
            break;
          case GST_FORMAT_DEFAULT:
            _TRACE_MESSAGE (msg, "%s %" G_GINT64_FORMAT " default", name,
                            dur);
            break;
          case GST_FORMAT_BYTES:
            _TRACE_MESSAGE (msg, "%s %" G_GINT64_FORMAT " bytes", name,
                            dur);
            break;
          case GST_FORMAT_TIME:
            _TRACE_MESSAGE (msg, "%s %" GST_TIME_FORMAT, name,
                            GST_TIME_ARGS (dur));
            break;
          case GST_FORMAT_BUFFERS:
            _TRACE_MESSAGE (msg, "%s %" G_GINT64_FORMAT " buffers", name,
                            dur);
            break;
          case GST_FORMAT_PERCENT:
            _TRACE_MESSAGE (msg, "%s %" G_GINT64_FORMAT " percent", name,
                            dur);
            break;
          }

        break;
      }
    case GST_MESSAGE_EOS:
      {
        _TRACE_MESSAGE (msg, "");
        // player->stop (); // set eos or use player state to signal eos
        break;
      }
    case GST_MESSAGE_NEW_CLOCK:
      {
        GstClock *clock = NULL;

        gst_message_parse_new_clock (msg, &clock);
        _TRACE_MESSAGE (msg, "%s",
                        (clock ? GST_OBJECT_NAME (clock) : "NULL"));
        break;
      }
    case GST_MESSAGE_STATE_CHANGED:
      {
        GstState old_state;
        GstState new_state;
        GstState pending_state;

        if (GST_ELEMENT (msg->src) != player->_playbin)
          {
            break;
          }

        gst_message_parse_state_changed (msg, &old_state, &new_state,
                                         &pending_state);

        if (pending_state != GST_STATE_VOID_PENDING)
          {
            _TRACE_MESSAGE (msg, "%s->%s (pending %s)",
                            gst_element_state_get_name (old_state),
                            gst_element_state_get_name (new_state),
                            gst_element_state_get_name (pending_state));
          }
        else
          {
            _TRACE_MESSAGE (msg, "%s->%s",
                            gst_element_state_get_name (old_state),
                            gst_element_state_get_name (new_state));
          }
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
            _ERROR ("%s", error->message);
          }
        else
          {
            gst_message_parse_warning (msg, &error, nullptr);
            g_assert_nonnull (error);
            _WARNING ("%s", error->message);
          }
        g_error_free (error);
        gst_object_unref (obj);
        break;
      }
    default:
      {
        _TRACE_MESSAGE (msg, "");
        break;
      }
    }
  return TRUE; // keep callback installed
}

void
PlayerVideo::setURI (const string &uri)
{
  Player::_prop.uri = uri;
  TRACE ("uri = %s\n", Player::_prop.uri.c_str ());
  g_object_set (G_OBJECT (_playbin), "uri", Player::_prop.uri.c_str (), NULL);
}

bool
PlayerVideo::doSetProperty (Property code, unused (const string &name),
                            const string &value)
{
  if (unlikely (_playbin == nullptr))
    return Player::doSetProperty (code, name, value);

  switch (code)
    {
    case PROP_BALANCE:
      {
        _prop.balance = xstrtodorpercent (value, nullptr);
        g_object_set (_audio.pan, "panorama", _prop.balance, nullptr);
        break;
      }
    case PROP_BASS:
      {
        _prop.bass = xstrtodorpercent (value, nullptr);
        g_object_set (_audio.equalizer, "band0", _prop.bass, nullptr);
        break;
      }
    case PROP_FREEZE:
      {
        _prop.freeze = ginga::parse_bool (value);
        break;
      }
    case PROP_MUTE:
      {
        _prop.mute = ginga::parse_bool (value);
        g_object_set (_audio.volume, "mute", _prop.mute, nullptr);
        break;
      }
    case PROP_SPEED:
      {
        if (unlikely (this->getPipelineState () != "PAUSED"
                      && this->getPipelineState () != "PLAYING"))
          {
            stackAction (code, name, value);
            break;
          }

        if (_state != SLEEPING)
          {
            _prop.speed = xstrtod (value);
            speed (_prop.speed);
          }
        break;
      }
    case PROP_TIME:
      {
        if (unlikely (this->getPipelineState () != "PAUSED"
                      && this->getPipelineState () != "PLAYING"))
          {
            stackAction (code, name, value);
            break;
          }

        if (_state != SLEEPING)
          {
            if (value == "indefinite" || value == "")
              break;

            TRACE ("Property value: %s", value.c_str ());
            TRACE ("State: %s", this->getPipelineState ().c_str ());
            Time t;
            gint64 cur, dur, next;

            cur = this->getStreamMediaTime ();
            dur = this->getStreamMediaDuration ();
            TRACE ("time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT,
                   GST_TIME_ARGS (cur), GST_TIME_ARGS (dur));

            try_parse_time (value, &t);
            next = (gint64) (t);
            if (xstrhasprefix (value, "+"))
              {
                if (next + cur >= dur)
                  setEOS (true);

                next += cur;
              }
            else if (xstrhasprefix (value, "-"))
              {
                next = (next - cur < 0) ? 0 : next - cur;
              }
            seek (next);
          }
        break;
      }
    case PROP_TREBLE:
      {
        _prop.treble = xstrtodorpercent (value, nullptr);
        g_object_set (_audio.equalizer, "band1", _prop.treble, "band2",
                      _prop.treble, nullptr);
        break;
      }
    case PROP_URI:
      {
        Player::_prop.uri = value;
        TRACE ("uri = %s\n", value.c_str ());
        g_object_set (G_OBJECT (_playbin), "uri", value.c_str (), nullptr);
        break;
      }
    case PROP_VOLUME:
      {
        _prop.volume = xstrtodorpercent (value, nullptr);
        g_object_set (_audio.volume, "volume", _prop.volume, nullptr);
        break;
      }
    default:
      {
        return Player::doSetProperty (code, name, value);
      }
    }
  return true;
}


void
PlayerVideo::stackAction (Property code, unused (const string &name),
                          const string &value)
{
  PlayerVideoAction act;
  act.code = code;
  act.name = name;
  act.value = value;
  _stack_actions.push_back (act);
}

void
PlayerVideo::doStackedActions ()
{
  while (!_stack_actions.empty ())
    {
      PlayerVideoAction act;
      act = _stack_actions.front ();
      _stack_actions.pop_front ();

      this->doSetProperty (act.code, act.name, act.value);
    }
}

bool
PlayerVideo::getFreeze ()
{
  return _prop.freeze;
}

string
PlayerVideo::getPipelineState ()
{
  GstState curr;
  GstState pending;
  GstStateChangeReturn ret;
  ret = gst_element_get_state (_playbin, &curr, &pending, 0);

  if (unlikely (ret == GST_STATE_CHANGE_FAILURE))
    {
      ERROR("cannot create PlayerVideo for %s", Player::_prop.uri.c_str ());
    }
  return gst_element_state_get_name (curr);
}


}
