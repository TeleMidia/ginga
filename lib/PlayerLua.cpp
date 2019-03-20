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

#include "PlayerLua.h"
#include "Media.h"

GINGA_NAMESPACE_BEGIN

// Event handling.
// #define evt_ncl_send_attribution(nw, action, name, value)
//   ncluaw_send_ncl_event (nw, "attribution", action, name, value)

#define evt_ncl_send_presentation(nw, action, name)                        \
  ncluaw_send_ncl_event (nw, "presentation", action, name, NULL)

// #define evt_ncl_send_selection(nw, action, name)
//   ncluaw_send_ncl_event (nw, "selection", action, name, NULL)
// #define evt_key_send ncluaw_send_key_event

/// Conversion from NCLua actions to NCL transitions
static map<string, Event::Transition> nclua_act_to_ncl = {
  { "start", Event::START },   { "pause", Event::PAUSE },
  { "resume", Event::RESUME }, { "stop", Event::STOP },
  { "abort", Event::ABORT },
};

// Public.

PlayerLua::PlayerLua (Media *media)
    : Player (media)
{
  _nw = NULL;
  _init_rect = { 0, 0, 0, 0 };
}

PlayerLua::~PlayerLua ()
{
}

void
PlayerLua::start ()
{
  char *errmsg;

  g_assert (_state != Player::PLAYING);
  g_assert_null (_nw);
  GError *err;
  char *filename = g_filename_from_uri (_uri.c_str (), NULL, &err);
  if (filename == NULL)
    {
      ERROR ("%s.", err->message);
      g_error_free (err);
    }

  this->pwdSave (filename);
  _init_rect = _rect;
  _nw = ncluaw_open (filename, _init_rect.width, _init_rect.height,
                     &errmsg);
  g_free (filename);

  if (unlikely (_nw == nullptr))
    ERROR ("%s", errmsg);
  this->pwdRestore ();

  evt_ncl_send_presentation (_nw, "start", "");
  Player::start ();
}

void
PlayerLua::stop ()
{
  g_assert (_state != Player::STOPPED);
  g_assert_nonnull (_nw);
  TRACE ("stopping");

  evt_ncl_send_presentation (_nw, "stop", "");

  this->pwdSave ();
  ncluaw_cycle (_nw);
  this->pwdRestore ();

  ncluaw_close (_nw);
  _nw = nullptr;

  Player::stop ();
}

// void
// PlayerLua::sendKeyEvent (const string &key, bool press)
// {
//   g_assert_nonnull (_nw);
//   evt_key_send (_nw, press ? "press" : "release", key.c_str ());
// }

// void
// PlayerLua::sendPresentationEvent (const string &action, const string &label)
// {
//   g_assert_nonnull (_nw);
//   evt_ncl_send_presentation (_nw, action.c_str (), label.c_str ());
// }

void
PlayerLua::draw (cairo_t *cr)
{
  cairo_surface_t *sfc;
  ncluaw_event_t *evt;

  g_assert (_state != Player::STOPPED);
  g_assert_nonnull (_nw);

  this->pwdSave ();
  ncluaw_cycle (_nw);
  this->pwdRestore ();

  sfc = (cairo_surface_t *) ncluaw_debug_get_surface (_nw);
  g_assert_nonnull (sfc);

  _surface = sfc;

  Player::draw (cr);

  // Get events posted from NCLua.
  while ((evt = ncluaw_receive (_nw)) != nullptr)
    {
      if (evt->cls != NCLUAW_EVENT_NCL)
        {
          ncluaw_event_free (evt);
          continue; // nothing to do
        }

      if (g_str_equal (evt->u.ncl.type, "presentation"))
        {
          Event *nclEvt;
          std::string label = evt->u.ncl.name;

          if (label == "")
            {
              nclEvt = _media->getLambda ();
            }
          else
            {
              set<Event *> events;

              _media->getEvents (&events);
              for (auto &evt: events)
                {
                  if (evt->getType () == Event::PRESENTATION
                      && evt->getLabel () == label)
                    {
                      nclEvt = evt;
                      break;
                    }
                }
            }

          g_assert_nonnull (nclEvt);
          g_assert (nclua_act_to_ncl.count (evt->u.ncl.action) != 0);
          // _media->addDelayedAction (
          //     nclEvt, nclua_act_to_ncl.at (evt->u.ncl.action));
        }
      else if (g_str_equal (evt->u.ncl.type, "attribution"))
        {
          Event *nclEvt;

          nclEvt = _media->getEvent (Event::ATTRIBUTION, evt->u.ncl.name);
          g_assert_nonnull (nclEvt);

          g_assert (nclua_act_to_ncl.count (evt->u.ncl.action));
          // _media->addDelayedAction (nclEvt,
          //                           nclua_act_to_ncl.at (evt->u.ncl.action),
          //                           evt->u.ncl.value);
        }
      ncluaw_event_free (evt);
    }
}

// Protected.

// bool
// PlayerLua::doSetProperty (Property code, const string &name,
//                           const string &value)
// {
//   if (_nw != nullptr && _state == Player::PLAYING)
//     {
//       const char *k = name.c_str ();
//       const char *v = value.c_str ();
//       evt_ncl_send_attribution (_nw, "start", k, v);
//       evt_ncl_send_attribution (_nw, "stop", k, v);
//     }
//   return Player::doSetProperty (code, name, value);
// }

// Private.

static void
do_chdir (string dir)
{
  if (g_chdir (dir.c_str ()) < 0)
    ERROR ("cannot chdir to '%s': %s", dir.c_str (), g_strerror (errno));
}

void
PlayerLua::pwdSave (const string &path)
{
  gchar *cwd;
  gchar *dir;

  cwd = g_get_current_dir ();
  g_assert_nonnull (cwd);

  dir = g_path_get_dirname (path.c_str ());
  g_assert_nonnull (dir);

  _saved_pwd = string (cwd);
  _pwd = string (dir);
  do_chdir (_pwd);

  g_free (cwd);
  g_free (dir);
}

void
PlayerLua::pwdSave ()
{
  do_chdir (_pwd);
}

void
PlayerLua::pwdRestore ()
{
  do_chdir (_saved_pwd);
}

GINGA_NAMESPACE_END
