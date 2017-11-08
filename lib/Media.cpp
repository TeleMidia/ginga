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

#include "aux-ginga.h"
#include "Object.h"

#include "Context.h"
#include "MediaSettings.h"
#include "Switch.h"
#include "Event.h"
#include "ncl/Ncl.h"
#include "player/Player.h"

GINGA_NAMESPACE_BEGIN

static map<string, string> mime_table =
{
  {"ac3", "audio/ac3"},
  {"avi", "video/x-msvideo"},
  {"bmp", "image/bmp"},
  {"bpg", "image/x-bpg"},
  {"class", "application/x-ginga-NCLet"},
  {"css", "text/css"},
  {"gif", "image/gif"},
  {"htm", "text/html"},
  {"html", "text/html"},
  {"jpeg", "image/jpeg"},
  {"jpg", "image/jpeg"},
  {"lua", "application/x-ginga-NCLua"},
  {"mov", "video/quicktime"},
  {"mp2", "audio/mp2"},
  {"mp3", "audio/mp3"},
  {"mp4", "video/mp4"},
  {"mpa", "audio/mpa"},
  {"mpeg", "video/mpeg"},
  {"mpg", "video/mpeg"},
  {"mpv", "video/mpv"},
  {"ncl", "application/x-ginga-ncl"},
  {"oga", "audio/ogg"},
  {"ogg", "audio/ogg"},
  {"ogv", "video/ogg"},
  {"opus", "audio/ogg"},
  {"png", "image/png"},
  {"smil", "application/smil"},
  {"spx", "audio/ogg"},
  {"srt", "text/srt"},
  {"ssml", "application/ssml+xml"},
  {"svg", "image/svg+xml"},
  {"svgz", "image/svg+xml"},
  {"ts", "video/mpeg"},
  {"txt", "text/plain"},
  {"wav", "audio/basic"},
  {"webp", "image/x-webp"},
  {"wmv", "video/x-ms-wmv"},
  {"xlet", "application/x-ginga-NCLet"},
  {"xlt", "application/x-ginga-NCLet"},
  {"xml", "text/xml"},
};

static bool
mime_table_index (const string &key, string *result)
{
  map<string, string>::iterator it;
  if ((it = mime_table.find (key)) == mime_table.end ())
    return false;
  tryset (result, it->second);
  return true;
}


// Public.

Media::Media (const string &id, const string &mime, const string &uri)
  :Object (id)
{
  _mime = mime;
  _uri = uri;

  if (_mime == "" && _uri != "")
    {
      string::size_type index, len;
      index = _uri.find_last_of (".");
      if (index != std::string::npos)
        {
          index++;
          len = _uri.length ();
          if (index < len)
            {
              string extension = _uri.substr (index, (len - index));
              if (extension != "")
                mime_table_index (extension, &_mime);
            }
        }
    }

  if (_mime == "")
    _mime = "application/x-ginga-timer";

  _player = nullptr;
}

Media::~Media ()
{
  this->doStop ();
}


// Public: Object.

void
Media::setProperty (const string &name, const string &value,
                    GingaTime dur)
{
  string from = this->getProperty (name);
  Object::setProperty (name, value, dur);
  if (_player == nullptr)
    return;

  g_assert (GINGA_TIME_IS_VALID (dur));
  if (dur > 0)
    _player->schedulePropertyAnimation (name, from, value, dur);
  else
    _player->setProperty (name, value);
}

void
Media::sendKeyEvent (const string &key, bool press)
{
  list<Event *> buf;

  if (!press)
    return;                     // nothing to do
  if (_player == nullptr)
    return;                     // nothing to do

  if (xstrhasprefix (key, "CURSOR_") && _player->isFocused ())
    {
      string next;
      if ((key == "CURSOR_UP"
           && (next = _player->getProperty ("moveUp")) != "")
          || ((key == "CURSOR_DOWN"
               && (next = _player->getProperty ("moveDown")) != ""))
          || ((key == "CURSOR_LEFT"
               && (next = _player->getProperty ("moveLeft")) != ""))
          || ((key == "CURSOR_RIGHT"
               && (next = _player->getProperty ("moveRight")) != "")))
        {
          MediaSettings *settings;
          settings = _formatter->getSettings ();
          g_assert_nonnull (settings);
          settings->scheduleFocusUpdate (next);
        }
    }

  // Pass key to player.
  if (_player->isFocused ())
    _player->sendKeyEvent (key, press);

  // Collect the events to be triggered.
  for (auto evt: _events)
    {
      string expected;

      if (evt->getType () != Event::SELECTION)
        continue;

      expected = "";
      evt->getParameter ("key", &expected);
      if (!((expected == "" && key == "ENTER" && _player->isFocused ())
            || (expected != "" && key == expected)))
        {
          continue;
        }
      buf.push_back (evt);
    }

  // Run collected events.
  for (Event *evt: buf)
    {
      TRACE ("%s<%s>", _id.c_str (), evt->getId ().c_str ());
      _formatter->evalAction (evt, Event::START);
      _formatter->evalAction (evt, Event::STOP);
    }
}

void
Media::sendTickEvent (GingaTime total, GingaTime diff,
                      GingaTime frame)
{
  GingaTime dur;

  // Update object time.
  Object::sendTickEvent (total, diff, frame);

  if (_player == nullptr)
    return;                     // nothing to do.

  // Update player time.
  g_assert_nonnull (_player);
  _player->incTime (diff);

  // Check EOS.
  if (_player->getEOS ()
      || (GINGA_TIME_IS_VALID (dur = _player->getDuration ())
          && _time > dur))
    {
      Event *lambda = this->getLambda ();
      g_assert_nonnull (lambda);
      TRACE ("eos %s@lambda at %" GINGA_TIME_FORMAT, _id.c_str (),
             GINGA_TIME_ARGS (_time));
      _formatter->evalAction (lambda, Event::STOP);
      return;
    }
}

bool
Media::startTransition (Event *evt, Event::Transition transition)
{
  switch (evt->getType ())
    {
    case Event::PRESENTATION:
      switch (transition)
        {
        case Event::START:
          if (evt->isLambda ())
            {
              g_assert_null (_player);
              _player = Player::createPlayer (_formatter, _id, _uri, _mime);
              if (unlikely (_player == nullptr))
                return false;       // fail

              for (auto it: _properties)
                _player->setProperty (it.first, it.second);

              _player->start ();    // TODO: check failure
            }
          break;

        case Event::STOP:
          break;
        default:
          g_assert_not_reached ();
        }
      break;

    case Event::ATTRIBUTION:
    case Event::SELECTION:
      if (!this->isOccurring ())
        return false;           // fail
      break;

    default:
      g_assert_not_reached ();
    }
  return true;
}

void
Media::endTransition (Event *evt, Event::Transition transition)
{
  switch (evt->getType ())
    {
    case Event::PRESENTATION:
      switch (transition)
        {
        case Event::START:
          if (evt->isLambda ())
            {
              g_assert_nonnull (_player);
              Object::doStart ();
              for (auto e: _events) // schedule time anchors
                {
                  if (!e->isLambda ()
                      && e->getType () == Event::PRESENTATION)
                    {
                      GingaTime begin, end;
                      e->getInterval (&begin, &end);
                      this->addDelayedAction
                        (e, Event::START, "", begin);
                      this->addDelayedAction
                        (e, Event::STOP, "", end);
                    }
                }
              TRACE ("start %s@lambda", _id.c_str ());
            }
          else                  // non-lambda area
            {
              GingaTime begin;
              g_assert (this->isOccurring ());
              evt->getInterval (&begin, nullptr);
              TRACE ("start %s@%s (begin=%" GINGA_TIME_FORMAT
                     ") at %" GINGA_TIME_FORMAT,
                     _id.c_str (), evt->getId ().c_str (),
                     GINGA_TIME_ARGS (begin), GINGA_TIME_ARGS (_time));
            }
          break;

        case Event::STOP:
          if (evt->isLambda ())
            {
              g_assert_nonnull (_player);
              this->doStop ();
              TRACE ("stop %s@lambda", _id.c_str ());
            }
          else                  // non-lambda area
            {
              GingaTime end;
              g_assert (this->isOccurring ());
              evt->getInterval (nullptr, &end);
              TRACE ("stop %s@%s (end=%" GINGA_TIME_FORMAT
                     ") at %" GINGA_TIME_FORMAT,
                     _id.c_str (), evt->getId ().c_str (),
                     GINGA_TIME_ARGS (end), GINGA_TIME_ARGS (_time));
            }
          break;

        default:
          g_assert_not_reached ();
        }
      break;

    case Event::ATTRIBUTION:
      switch (transition)
        {
        case Event::START:
          {
            string name;
            string value;
            string s;
            GingaTime dur;

            name = evt->getId ();
            evt->getParameter ("value", &value);
            if (value[0] == '$')
              _formatter->getObjectPropertyByRef (value, &value);

            if (evt->getParameter ("duration", &s))
              {
                if (s[0] == '$')
                  _formatter->getObjectPropertyByRef (s, &s);
                dur = ginga_parse_time (s);
              }
            else
              {
                dur = 0;
              }
            this->setProperty (name, value, dur);
            _formatter->evalAction (evt, Event::STOP);

            TRACE ("start %s.%s:=%s (duration=%s)", _id.c_str (),
                   name.c_str (), value.c_str (), s.c_str ());
            break;
          }
        case Event::STOP:
          TRACE ("stop %s.%s:=...", _id.c_str (), evt->getId ().c_str ());
          break;
        default:
          g_assert_not_reached ();
        }
      break;

    case Event::SELECTION:
      {
        string key;

        g_assert (this->isOccurring ());
        evt->getParameter ("key", &key);

        switch (transition)
          {
          case Event::START:
            TRACE ("start %s<%s>", _id.c_str (), key.c_str ());
            break;
          case Event::STOP:
            TRACE ("stop %s<%s>", _id.c_str (), key.c_str ());
            break;
          default:
            g_assert_not_reached ();
          }
        break;
      }
    default:
      g_assert_not_reached ();
    }
}


// Public.

bool
Media::isFocused ()
{
  if (!this->isOccurring ())
    return false;
  g_assert_nonnull (_player);
  return _player->isFocused ();
}

bool
Media::getZ (int *z, int *zorder)
{
  if (this->isSleeping () || _player == nullptr)
    return false;               // nothing to do
  g_assert_nonnull (_player);
  _player->getZ (z, zorder);
  return true;
}

void
Media::redraw (cairo_t *cr)
{
  if (this->isSleeping () || _player == nullptr)
    return;                     // nothing to do
  _player->redraw (cr);
}


// Protected.

void
Media::doStop ()
{
  if (_player != nullptr)
    {
      if (_player->getState () != Player::SLEEPING)
        _player->stop ();
      delete _player;
      _player = nullptr;
    }
  Object::doStop ();
}

GINGA_NAMESPACE_END
