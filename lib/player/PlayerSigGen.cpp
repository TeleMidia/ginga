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
#include <string.h>

GINGA_PRAGMA_DIAG_IGNORE (-Wunused-variable)

GINGA_NAMESPACE_BEGIN

#define gstx_element_get_state(elt, st, pend, tout)             \
  g_assert (gst_element_get_state ((elt), (st), (pend), (tout)) \
            != GST_STATE_CHANGE_FAILURE)

#define gstx_element_get_state_sync(elt, st, pend)\
  gstx_element_get_state ((elt), (st), (pend), GST_CLOCK_TIME_NONE)

#define gstx_element_set_state(elt, st)         \
  g_assert (gst_element_set_state ((elt), (st)) \
            != GST_STATE_CHANGE_FAILURE)

#define gstx_element_set_state_sync(elt, st)                    \
  G_STMT_START                                                  \
  {                                                             \
    gstx_element_set_state ((elt), (st));                       \
    gstx_element_get_state_sync ((elt), nullptr, nullptr);      \
  }                                                             \
  G_STMT_END


// Public.

PlayerSigGen::PlayerSigGen (Formatter *formatter, const string &id,
                          const string &uri)
  :Player (formatter, id, uri)
{
  GstBus *bus;
  gulong ret;
  char *buf;

  GstPad *pad;
  GstPad *ghost;

  _pipeline = nullptr;
  _audio.src = nullptr;
  _audio.convert = nullptr;
  _audio.sink = nullptr;

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
  _audio.convert = gst_element_factory_make ("audioconvert", "convert");
  g_assert_nonnull (_audio.convert);

  // Try to use ALSA if available.
  _audio.sink = gst_element_factory_make ("alsasink", "audio.sink");
  if (_audio.sink == nullptr)
    _audio.sink = gst_element_factory_make ("autoaudiosink", "audio.sink");
  g_assert_nonnull (_audio.sink);


  g_assert (gst_bin_add (GST_BIN (_pipeline), _audio.src));
  g_assert (gst_bin_add (GST_BIN (_pipeline), _audio.convert));
  g_assert (gst_bin_add (GST_BIN (_pipeline), _audio.sink));
  g_assert (gst_element_link (_audio.src, _audio.convert));
  g_assert (gst_element_link (_audio.convert, _audio.sink));
  

  // Initialize handled properties.
  static set<string> handled =
    {
     "freq",
     "wave",
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
  g_object_set (_audio.src,
                "freq", _prop.freq,
                "wave", _prop.wave,
                nullptr);
  TRACE("Wave Value: %d\n",_prop.wave);
  TRACE("Freq Value: %f\n",_prop.freq);

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


// Protected.

bool
PlayerSigGen::doSetProperty (PlayerProperty code,
                            unused (const string &name),
                            const string &value)
{
 
  switch (code)
    {
      case PROP_FREQ:
        _prop.freq = xstrtodorpercent (value, nullptr);
        if (_state != SLEEPING)
          g_object_set (_audio.src,
                        "freq", _prop.freq,
                        nullptr);
        break;
      case PROP_WAVE:
      TRACE("Changed wave");
      TRACE("Evaluation: %s", value.c_str());
        if (_state != SLEEPING){
          if (strcmp(value.c_str(),"sine")==0)
            _prop.wave = 0;
          else if (strcmp(value.c_str(),"square")==0){
            TRACE("SQUARED");
            _prop.wave = 1;
          }
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

          g_object_set (_audio.src,
                        "wave", _prop.wave,
                        nullptr);
        }
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
