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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "PlayerAnimator.h"

GINGA_NAMESPACE_BEGIN

// PlayerAnimator: Public.

PlayerAnimator::PlayerAnimator (Formatter *formatter)
{
  g_assert_nonnull (formatter);
  _formatter = formatter;
}

PlayerAnimator::~PlayerAnimator ()
{
  this->clear ();
}

void
PlayerAnimator::clear ()
{
  _scheduled.clear ();
}

void
PlayerAnimator::schedule (const string &name, const string &from,
                          const string &to, Time dur)
{
  list<string> list_pre = { "", "", "", "" };
  list<string> list_pos;

  if (name == "bounds")
    {
      if (from != "")
        list_pre = ginga::parse_list (from, ',', 4, 4);
      list_pos = ginga::parse_list (to, ',', 4, 4);
      auto pre = list_pre.begin ();
      auto pos = list_pos.end ();
      this->doSchedule ("left", *pre++, *pos++, dur);
      this->doSchedule ("top", *pre++, *pos++, dur);
      this->doSchedule ("width", *pre++, *pos++, dur);
      this->doSchedule ("height", *pre++, *pos++, dur);
    }
  else if (name == "location")
    {
      if (from != "")
        list_pre = ginga::parse_list (from, ',', 2, 2);
      list_pos = ginga::parse_list (to, ',', 2, 2);
      auto pre = list_pre.begin ();
      auto pos = list_pos.end ();
      this->doSchedule ("left", *pre++, *pos++, dur);
      this->doSchedule ("top", *pre++, *pos++, dur);
    }
  else if (name == "size")
    {
      if (from != "")
        list_pre = ginga::parse_list (from, ',', 2, 2);
      list_pos = ginga::parse_list (to, ',', 2, 2);
      auto pre = list_pre.begin ();
      auto pos = list_pos.end ();
      this->doSchedule ("width", *pre++, *pos++, dur);
      this->doSchedule ("height", *pre++, *pos++, dur);
    }
  else if (name == "background")
    {
      Color _pos;
      if (from != "")
        {
          Color _pre = ginga::parse_color (from);
          list_pre
              = { xstrbuild ("%d", (int) CLAMP (_pre.red * 255, 0, 255)),
                  xstrbuild ("%d", (int) CLAMP (_pre.green * 255, 0, 255)),
                  xstrbuild ("%d", (int) CLAMP (_pre.blue * 255, 0, 255)) };
        }
      _pos = ginga::parse_color (to);
      list_pos
          = { xstrbuild ("%d", (int) CLAMP (_pos.red * 255, 0, 255)),
              xstrbuild ("%d", (int) CLAMP (_pos.green * 255, 0, 255)),
              xstrbuild ("%d", (int) CLAMP (_pos.blue * 255, 0, 255)) };
      auto pre = list_pre.begin ();
      auto pos = list_pos.end ();
      this->doSchedule ("background:r", *pre++, *pos++, dur);
      this->doSchedule ("background:g", *pre++, *pos++, dur);
      this->doSchedule ("background:b", *pre++, *pos++, dur);
    }
  else
    {
      this->doSchedule (name, from, to, dur);
    }
}

static bool
isDone (AnimInfo *info)
{
  return info->isDone ();
}

void
PlayerAnimator::update (Rect *rect, Color *bgColor, guint8 *alpha)
{

#define UPDATE(info, Type, var, rnd, min, max)                             \
  G_STMT_START                                                             \
  {                                                                        \
    if (!(info)->isInit ())                                                \
      (info)->init (var);                                                  \
    (info)->update ();                                                     \
    var = (Type) CLAMP (rnd ((info)->getCurrent ()), min, max);            \
  }                                                                        \
  G_STMT_END

  g_assert_nonnull (rect);
  g_assert_nonnull (bgColor);
  g_assert_nonnull (alpha);

  for (AnimInfo *info : _scheduled)
    {
      string name;

      g_assert_nonnull (info);
      g_assert (!info->isDone ());

      name = info->getName ();
      if (name == "top")
        {
          UPDATE (info, int, rect->y, lround, G_MININT, G_MAXINT);
        }
      else if (name == "left")
        {
          UPDATE (info, int, rect->x, lround, G_MININT, G_MAXINT);
        }
      else if (name == "width")
        {
          UPDATE (info, int, rect->width, lround, G_MININT, G_MAXINT);
        }
      else if (name == "height")
        {
          UPDATE (info, int, rect->height, lround, G_MININT, G_MAXINT);
        }
      else if (name == "background:r")
        {
          UPDATE (info, double, bgColor->red, round, 0., 1.);
        }
      else if (name == "background:g")
        {
          UPDATE (info, double, bgColor->green, round, 0., 1.);
        }
      else if (name == "background:b")
        {
          UPDATE (info, double, bgColor->blue, round, 0., 1.);
        }
      else if (name == "transparency")
        {
          UPDATE (info, guint8, *alpha, lround, 0, 255);
        }
      else
        {
          WARNING ("do not know how to animate property '%s'",
                   name.c_str ());
        }
    }

  _scheduled.remove_if (isDone);
}

void
PlayerAnimator::setTransitionProperties (const string &name,
                                         const string &value)
{
  WARNING ("property '%s': value '%s'", name.c_str (), value.c_str ());

  // if(name == "transIn")

}

// PlayerAnimator: Private.

void
PlayerAnimator::doSchedule (const string &name, const string &from,
                            const string &to, Time dur)
{
  AnimInfo *info;
  double current;
  double target;
  int width;
  int height;

  current = 0;
  width = _formatter->getOptionInt ("width");
  height = _formatter->getOptionInt ("height");

  if (name == "top" || name == "height")
    {
      if (from != "")
        current = ginga::parse_percent (from, height, 0, G_MAXINT);
      target = ginga::parse_percent (to, height, 0, G_MAXINT);
    }
  else if (name == "left" || name == "width")
    {
      if (from != "")
        current = ginga::parse_percent (from, width, 0, G_MAXINT);
      target = ginga::parse_percent (to, width, 0, G_MAXINT);
    }
  else if (name == "transparency" || name == "background")
    {
      if (from != "")
        current = ginga::parse_percent (from, 255, 0, 255);
      target = ginga::parse_percent (to, 255, 0, 255);
    }
  else
    {
      if (from != "")
        current = ginga::parse_percent (from, 100, 0, G_MAXINT);
      target = ginga::parse_percent (to, 100, 0, G_MAXINT);
    }

  info = new AnimInfo (name, current, target, dur);
  if (from != "")
    info->init (current);

  _scheduled.push_back (info);
}

// AnimInfo.

AnimInfo::AnimInfo (const string &name, double from, double to, Time dur)
{
  _name = name;
  _current = from;
  _target = to;
  _duration = dur;
  _done = false;
  _init = false;
}

AnimInfo::~AnimInfo ()
{
}

string
AnimInfo::getName ()
{
  return _name;
}

double
AnimInfo::getCurrent ()
{
  return _current;
}

double
AnimInfo::getTarget ()
{
  return _target;
}

Time
AnimInfo::getDuration ()
{
  return _duration;
}

bool
AnimInfo::isDone ()
{
  return _done;
}

bool
AnimInfo::isInit ()
{
  return _init;
}

void
AnimInfo::init (double current)
{
  g_assert (!_init);
  _current = current;
  if (_duration > 0)
    _speed = fabs (_target - current) / (double) _duration;
  else
    _speed = 0;
  _init = true;
  _last_update = (Time) g_get_monotonic_time (); // fixme
}

void
AnimInfo::update ()
{
  Time _current_time = (Time) g_get_monotonic_time (); // fixme
  int dir;

  g_assert (_init);
  g_assert (!_done);

  dir = (_current < _target) ? 1 : -1;
  _current += dir * _speed * (double) (_current_time - _last_update);
  _last_update = _current_time;

  if (_duration == 0 || (dir > 0 && _current >= _target)
      || (dir < 0 && _current <= _target))
    {
      _done = true;
    }
}

GINGA_NAMESPACE_END
