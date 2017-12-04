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

string
Media::getObjectTypeAsString ()
{
  return "Media";
}

string
Media::toString ()
{
  string str;

  str = Object::toString ();
  if (_mime != "")
    str += "  mime: " + _mime + "\n";
  if (_uri != "")
    str += "  uri: " + _uri + "\n";
  str += xstrbuild ("  player: %p\n", _player);

  return str;
}

void
Media::setProperty (const string &name, const string &value, Time dur)
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

  if (_player == nullptr)
    return;                     // nothing to do

  if (xstrhasprefix (key, "CURSOR_") && _player->isFocused () && press)
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
          settings = _doc->getSettings ();
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
      TRACE ("%s", evt->getFullId ().c_str ());
      if (press)
        _doc->evalAction (evt, Event::START);
      else
        _doc->evalAction (evt, Event::STOP);
    }
}

void
Media::sendTickEvent (Time total, Time diff, Time frame)
{
  Time dur;

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
      TRACE ("eos %s at %" GINGA_TIME_FORMAT, lambda->getFullId ().c_str (),
             GINGA_TIME_ARGS (_time));
      _doc->evalAction (lambda, Event::STOP);
      return;
    }
}

bool
Media::beforeTransition (Event *evt, Event::Transition transition)
{  
  TRACE ("------Init_Transition------\n%s",_uri.c_str());
  
  switch (evt->getType ())
    {
    case Event::PRESENTATION:
      TRACE ("PRESENTATION");
      switch (transition)
        {
        case Event::START:
          if (!evt->isLambda ())
            {
              break;            // nothing to do
            }
          if (evt->getState () == Event::SLEEPING) // create player
            {
              Formatter *fmt;

              g_assert (_doc->getData ("formatter", (void **) &fmt));
              g_assert_null (_player);
              _player = Player::createPlayer (fmt, _id, _uri, _mime);
              if (unlikely (_player == nullptr))
                return false;   // fail

              for (auto it: _properties)
                _player->setProperty (it.first, it.second);
            }
          g_assert_nonnull (_player);
          _player->start ();    // TODO: check failure
          break;

        case Event::PAUSE:
	  TRACE ("PAUSE");
          if (!evt->isLambda ())
            {
              break;            // nothing to do
            }
          g_assert_nonnull (_player);
          for (auto e: _events)
            if (!e->isLambda () && e->getType () == Event::PRESENTATION)
              _doc->evalAction (e, Event::PAUSE);
          _player->pause ();
          break;

        case Event::RESUME:
          if (!evt->isLambda ())
            {
              break;            // nothing to do
            }
          g_assert_nonnull (_player);
          for (auto e: _events)
            if (!e->isLambda () && e->getType () == Event::PRESENTATION)
              _doc->evalAction (e, Event::RESUME);

          _player->resume ();
          break;

        case Event::ABORT:
          TRACE ("ABORT");
          //TODO          
	  ERROR_NOT_IMPLEMENTED ("abort action is not supported");
          break;

        case Event::STOP:
          TRACE ("STOP");
          break;

        default:
          g_assert_not_reached ();
        }
      break;

    case Event::ATTRIBUTION:
      TRACE ("ATTRIBUTION");
      break;
    case Event::SELECTION:
      TRACE ("SELECTION");  //It's possible to select a paused media.
      // if (!this->isOccurring ())
      // {
      //   TRACE ("Not Occurring!");
      //   return false;           // fail
      // }
      break;

    default:
      g_assert_not_reached ();
    }
  return true;
}

bool
Media::afterTransition (Event *evt, Event::Transition transition)
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
                      Time begin, end;
                      e->getInterval (&begin, &end);
                      this->addDelayedAction
                        (e, Event::START, "", begin);
                      this->addDelayedAction
                        (e, Event::STOP, "", end);
                    }
                }
              TRACE ("start %s", evt->getFullId ().c_str ());
            }
          else                  // non-lambda area
            {
              Time begin;
              g_assert (this->isOccurring ());
              evt->getInterval (&begin, nullptr);
              TRACE ("start %s (begin=%" GINGA_TIME_FORMAT
                     ") at %" GINGA_TIME_FORMAT,
                     evt->getFullId ().c_str (),
                     GINGA_TIME_ARGS (begin), GINGA_TIME_ARGS (_time));
            }
          break;

        case Event::PAUSE:
            TRACE ("pause %s", evt->getFullId ().c_str ());
            break;
        case Event::RESUME:
            TRACE ("resume %s", evt->getFullId ().c_str ());
            break;                 // nothing to do

        case Event::STOP:
          if (evt->isLambda ())
            {
              g_assert_nonnull (_player);
              this->doStop ();
              TRACE ("stop %s", evt->getFullId ().c_str ());
            }
          else                  // non-lambda area
            {
              Time end;
              g_assert (this->isOccurring ());
              evt->getInterval (nullptr, &end);
              TRACE ("stop %s (end=%" GINGA_TIME_FORMAT
                     ") at %" GINGA_TIME_FORMAT,
                     evt->getFullId ().c_str (),
                     GINGA_TIME_ARGS (end), GINGA_TIME_ARGS (_time));
            }
          break;

        case Event::ABORT:
          ERROR_NOT_IMPLEMENTED ("abort action is not supported");
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
            Time dur;

            name = evt->getId ();
            evt->getParameter ("value", &value);
            _doc->evalPropertyRef (value, &value);

            if (evt->getParameter ("duration", &s))
              {
                _doc->evalPropertyRef (s, &s);
                dur = ginga::parse_time (s);
              }
            else
              {
                dur = 0;
              }
            this->setProperty (name, value, dur);
            this->addDelayedAction (evt, Event::STOP, value, dur);

            TRACE ("start %s:='%s' (duration=%s)", evt->getFullId ().c_str (),
                   value.c_str (), (s != "") ? s.c_str () : "0s");
            break;
          }

        case Event::STOP:
          TRACE ("stop %s:=...", evt->getFullId ().c_str ());
          break;

        default:
          g_assert_not_reached ();
        }
      break;

    case Event::SELECTION:
      {
        string key;        
        // g_assert (this->isOccurring ());
        TRACE ("Occurring!");
        evt->getParameter ("key", &key);

        switch (transition)
          {
          case Event::START:
            TRACE ("start %s", evt->getFullId ().c_str ());
            break;
          case Event::STOP:
            TRACE ("stop %s", evt->getFullId ().c_str ());
            break;
          default:
            g_assert_not_reached ();
          }
        break;
      }
    default:
      g_assert_not_reached ();
    }
  TRACE ("------End_Transition------");
  return true;
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
