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
#include "PlayerGStreamer.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-equal)

GINGA_NAMESPACE_BEGIN

#define _ERROR(fmt, ...)\
  ERROR ("PlayerGStreamer error: " fmt, ## __VA_ARGS__)

#define _WARNING(fmt, ...)\
  WARNING ("PlayerGStreamer warning: " fmt, ## __VA_ARGS__)

#define _TRACE(fmt, ...)\
  g_print ("PlayerGStreamer: " fmt "\n", ## __VA_ARGS__)

#define _TRACE_MESSAGE(msg, fmt, ...)           \
  _TRACE ("%s: %s: " fmt,                       \
    GST_MESSAGE_SRC_NAME (msg),                 \
    GST_MESSAGE_TYPE_NAME (msg),                \
    ## __VA_ARGS__)

PlayerGStreamer::PlayerGStreamer (Media *media) : Player (media)
{
  GstBus *bus;
  gulong ret;

  GstCaps *caps;
  GstStructure *st;

  GstPad *pad;
  GstPad *ghost;

  TRACE ("");

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
  ret = gst_bus_add_watch (bus, (GstBusFunc) busWatch, this);
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

PlayerGStreamer::~PlayerGStreamer ()
{
  TRACE ("");
  gst_object_unref (_playbin);
}

void
PlayerGStreamer::setURI (const string &uri)
{
  Player::setURI (uri);
  g_object_set (G_OBJECT (_playbin), "uri", _uri.c_str (), NULL);
}

void
PlayerGStreamer::start ()
{
  GstStateChangeReturn ret;

  g_assert_nonnull (_playbin);

  ret = gst_element_set_state (_playbin, GST_STATE_PLAYING);
  if (unlikely (ret == GST_STATE_CHANGE_FAILURE))
    _ERROR ("state change failed");

  Player::start ();
}

void
PlayerGStreamer::pause ()
{
  GstStateChangeReturn ret;

  g_assert_nonnull (_playbin);

  ret = gst_element_set_state (_playbin, GST_STATE_PAUSED);
  if (unlikely (ret == GST_STATE_CHANGE_FAILURE))
    _ERROR ("state change failed");

  Player::pause ();
}

void
PlayerGStreamer::stop ()
{
  GstStateChangeReturn ret;

  g_assert_nonnull (_playbin);

  ret = gst_element_set_state (_playbin, GST_STATE_NULL);
  if (unlikely (ret == GST_STATE_CHANGE_FAILURE))
    ERROR ("state change failed");

  Player::stop ();
}

void
PlayerGStreamer::redraw (cairo_t *cr)
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

static void G_GNUC_UNUSED
print_tag (const GstTagList * list, const gchar *tag,
           unused (gpointer unused))
{
  gint i, count;

  count = gst_tag_list_get_tag_size (list, tag);

  for (i = 0; i < count; i++) {
    gchar *str = NULL;

    if (gst_tag_get_type (tag) == G_TYPE_STRING) {
      if (!gst_tag_list_get_string_index (list, tag, i, &str)) {
        g_warning ("Couldn't fetch string for %s tag", tag);
        g_assert_not_reached ();
      }
    } else if (gst_tag_get_type (tag) == GST_TYPE_SAMPLE) {
      GstSample *sample = NULL;

      if (gst_tag_list_get_sample_index (list, tag, i, &sample)) {
        GstBuffer *img = gst_sample_get_buffer (sample);
        GstCaps *caps = gst_sample_get_caps (sample);

        if (img) {
          if (caps) {
            gchar *caps_str;

            caps_str = gst_caps_to_string (caps);
            str = g_strdup_printf ("buffer of %" G_GSIZE_FORMAT " bytes, "
                "type: %s", gst_buffer_get_size (img), caps_str);
            g_free (caps_str);
          } else {
            str = g_strdup_printf ("buffer of %" G_GSIZE_FORMAT " bytes",
                gst_buffer_get_size (img));
          }
        } else {
          str = g_strdup ("NULL buffer");
        }
      } else {
        g_warning ("Couldn't fetch sample for %s tag", tag);
        g_assert_not_reached ();
      }
      gst_sample_unref (sample);
    } else if (gst_tag_get_type (tag) == GST_TYPE_DATE_TIME) {
      GstDateTime *dt = NULL;

      gst_tag_list_get_date_time_index (list, tag, i, &dt);
      if (!gst_date_time_has_time (dt)) {
        str = gst_date_time_to_iso8601_string (dt);
      } else {
        gdouble tz_offset = gst_date_time_get_time_zone_offset (dt);
        gchar tz_str[32];

        if (tz_offset != 0.0) {
          g_snprintf (tz_str, sizeof (tz_str), "(UTC %s%gh)",
              (tz_offset > 0.0) ? "+" : "", tz_offset);
        } else {
          g_snprintf (tz_str, sizeof (tz_str), "(UTC)");
        }

        str = g_strdup_printf ("%04d-%02d-%02d %02d:%02d:%02d %s",
            gst_date_time_get_year (dt), gst_date_time_get_month (dt),
            gst_date_time_get_day (dt), gst_date_time_get_hour (dt),
            gst_date_time_get_minute (dt), gst_date_time_get_second (dt),
            tz_str);
      }
      gst_date_time_unref (dt);
    } else {
      str =
          g_strdup_value_contents (gst_tag_list_get_value_index (list, tag, i));
    }

    if (str) {
      _TRACE ("%16s: %s\n", i == 0 ? gst_tag_get_nick (tag) : "", str);
      g_free (str);
    }
  }
}

gboolean
PlayerGStreamer::busWatch (GstBus *bus, GstMessage *msg,
                           PlayerGStreamer *player)
{
  g_assert_nonnull (bus);
  g_assert_nonnull (msg);
  g_assert_nonnull (player);

  switch (GST_MESSAGE_TYPE (msg))
    {
    case GST_MESSAGE_ASYNC_DONE:
      {
        GstClockTime running_time;
        // GstState target;

        gst_message_parse_async_done (msg, &running_time);
        //        gst_message_parse_request_state (msg, &target);
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
            _TRACE_MESSAGE (msg, "%s %" G_GINT64_FORMAT " undefined",
                            name, dur);
            break;
          case GST_FORMAT_DEFAULT:
            _TRACE_MESSAGE (msg, "%s %" G_GINT64_FORMAT " default",
                            name, dur);
            break;
          case GST_FORMAT_BYTES:
            _TRACE_MESSAGE (msg, "%s %" G_GINT64_FORMAT " bytes",
                            name, dur);
            break;
          case GST_FORMAT_TIME:
            _TRACE_MESSAGE (msg, "%s %" GST_TIME_FORMAT, name,
                            GST_TIME_ARGS (dur));
            break;
          case GST_FORMAT_BUFFERS:
            _TRACE_MESSAGE (msg, "%s %" G_GINT64_FORMAT " buffers",
                            name, dur);
            break;
          case GST_FORMAT_PERCENT:
            _TRACE_MESSAGE (msg, "%s %" G_GINT64_FORMAT " percent",
                            name, dur);
            break;
          }

        break;
      }
    case GST_MESSAGE_EOS:
      {
        _TRACE_MESSAGE (msg, "");
        //player->stop (); // set eos or use player state to signal eos
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
#if 0
    case GST_MESSAGE_TAG:
      {
        GstTagList *tag_list;

        // if (GST_IS_ELEMENT (GST_MESSAGE_SRC (message))) {
        //   PRINT (_("FOUND TAG      : found by element \"%s\".\n"),
        //          GST_MESSAGE_SRC_NAME (message));
        // } else if (GST_IS_PAD (GST_MESSAGE_SRC (message))) {
        //   PRINT (_("FOUND TAG      : found by pad \"%s:%s\".\n"),
        //          GST_DEBUG_PAD_NAME (GST_MESSAGE_SRC (message)));
        // } else if (GST_IS_OBJECT (GST_MESSAGE_SRC (message))) {
        //   PRINT (_("FOUND TAG      : found by object \"%s\".\n"),
        //          GST_MESSAGE_SRC_NAME (message));
        // } else {
        //   PRINT (_("FOUND TAG\n"));
        // }

        gst_message_parse_tag (msg, &tag_list);
        gst_tag_list_foreach (tag_list, print_tag, NULL);
        gst_tag_list_unref (tag_list);
        break;
      }
#endif
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
  return TRUE;                  // keep callback installed
}

GINGA_NAMESPACE_END
