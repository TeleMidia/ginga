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
#include "PlayerAnimator.h"

GINGA_PLAYER_BEGIN


// PlayerAnimator: Public.

/**
 * @brief Creates a new player animator.
 */
PlayerAnimator::PlayerAnimator (Formatter *ginga)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;
}

/**
 * @brief Destroys player animator.
 */
PlayerAnimator::~PlayerAnimator ()
{
  this->clear ();
}

/**
 * @brief Removes all scheduled animations.
 */
void
PlayerAnimator::clear ()
{
  _scheduled.clear ();
}

/**
 * @brief Schedules property animation.
 * @param name Property name.
 * @param from Current value.
 * @param to Target value.
 * @param dur Duration of the animation.
 */
void
PlayerAnimator::schedule (const string &name, const string &from,
                          const string &to, GingaTime dur)
{
  vector<string> pre = {"", "", "", ""};
  vector<string> pos;

  if (name == "bounds")
    {
      if (from != "")
        pre = ginga_parse_list (from, ',', 4, 4);
      pos = ginga_parse_list (to, ',', 4, 4);
      this->doSchedule ("left", pre[0], pos[0], dur);
      this->doSchedule ("top", pre[1], pos[1], dur);
      this->doSchedule ("width", pre[2], pos[2], dur);
      this->doSchedule ("height", pre[3], pos[3], dur);
    }
  else if (name == "location")
    {
      if (from != "")
        pre = ginga_parse_list (from, ',', 2, 2);
      pos = ginga_parse_list (to, ',', 2, 2);
      this->doSchedule ("left", pre[0], pos[0], dur);
      this->doSchedule ("top", pre[1], pos[1], dur);
    }
  else if (name == "size")
    {
      if (from != "")
        pre = ginga_parse_list (from, ',', 2, 2);
      pos = ginga_parse_list (to, ',', 2, 2);
      this->doSchedule ("width", pre[0], pos[0], dur);
      this->doSchedule ("height", pre[1], pos[1], dur);
    }
  else if (name == "background")
    {
      GingaColor _pos;
      if (from != "")
        {
          GingaColor _pre = ginga_parse_color (from);
          pre = {xstrbuild ("%d", (int) CLAMP (_pre.red * 255, 0, 255)),
                 xstrbuild ("%d", (int) CLAMP (_pre.green * 255, 0, 255)),
                 xstrbuild ("%d", (int) CLAMP (_pre.blue * 255, 0, 255))};
        }
      _pos = ginga_parse_color (to);
      pos = {xstrbuild ("%d", (int) CLAMP (_pos.red * 255, 0, 255)),
             xstrbuild ("%d", (int) CLAMP (_pos.green * 255, 0, 255)),
             xstrbuild ("%d", (int) CLAMP (_pos.blue * 255, 0, 255))};
      this->doSchedule ("background:r", pre[0], pos[0], dur);
      this->doSchedule ("background:g", pre[1], pos[1], dur);
      this->doSchedule ("background:b", pre[2], pos[2], dur);
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

/**
 * @brief Updates scheduled animations.
 * @param rect Variable to store the resulting bounds.
 * @param bgColor Variable to store the resulting background color.
 * @param alpha Variable to store the resulting duration.
 */
void
PlayerAnimator::update (GingaRect *rect, GingaColor *bgColor, guint8 *alpha)
{

#define UPDATE(info, Type, var, rnd, min, max)                          \
  G_STMT_START                                                          \
    {                                                                   \
      if (!(info)->isInit ())                                           \
        (info)->init (var);                                             \
      (info)->update ();                                                \
      var = (Type) CLAMP (rnd ((info)->getCurrent ()), min, max);       \
    }                                                                   \
  G_STMT_END

  g_assert_nonnull (rect);
  g_assert_nonnull (bgColor);
  g_assert_nonnull (alpha);

  for (AnimInfo *info: _scheduled)
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


// PlayerAnimator: Private.

void
PlayerAnimator::doSchedule (const string &name, const string &from,
                            const string &to, GingaTime dur)
{
  AnimInfo *info;
  double current;
  double target;
  int width;
  int height;

  current = 0;
  width = _ginga->getOptionInt ("width");
  height = _ginga->getOptionInt ("height");

  if (name == "top" || name == "height")
    {
      if (from != "")
        current = ginga_parse_percent (from, height, 0, G_MAXINT);
      target = ginga_parse_percent (to, height, 0, G_MAXINT);
    }
  else if (name == "left" || name == "width")
    {
      if (from != "")
        current = ginga_parse_percent (from, width, 0, G_MAXINT);
      target = ginga_parse_percent (to, width, 0, G_MAXINT);
    }
  else if (name == "transparency" || name == "background")
    {
      if (from != "")
        current = ginga_parse_percent (from, 255, 0, 255);
      target = ginga_parse_percent (to, 255, 0, 255);
    }
  else
    {
      if (from != "")
        current = ginga_parse_percent (from, 100, 0, G_MAXINT);
      target = ginga_parse_percent (to, 100, 0, G_MAXINT);
    }

  info = new AnimInfo (name, current, target, dur);
  if (from != "")
    info->init (current);

  _scheduled.push_back (info);
}


// AnimInfo.

/**
 * @brief Creates animation info.
 * @param name Property name.
 * @param from Current value.
 * @param to Target value.
 * @param dur Duration of the animation.
 */
AnimInfo::AnimInfo (const string &name, double from,
                    double to, GingaTime dur)
{
  _name = name;
  _current = from;
  _target = to;
  _duration = dur;
  _done = false;
  _init = false;
}

/**
 * @brief Destroys animation info.
 */
AnimInfo::~AnimInfo ()
{
}

/**
 * @brief Gets animation name.
 */
string
AnimInfo::getName ()
{
  return _name;
}

/**
 * @brief Gets animation current value.
 */
double
AnimInfo::getCurrent ()
{
  return _current;
}

/**
 * @brief Gets animation target value.
 */
double
AnimInfo::getTarget ()
{
  return _target;
}

/**
 * @brief Gets animation  duration.
 */
GingaTime
AnimInfo::getDuration ()
{
  return _duration;
}

/**
 * @brief Tests if animation is done.
 */
bool
AnimInfo::isDone ()
{
  return _done;
}

/**
 * @brief Tests if animation is initialized.
 */
bool
AnimInfo::isInit ()
{
  return _init;
}

/**
 * @brief Initializes animation.
 */
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
  _last_update = ginga_gettime ();
}

/**
 * @brief Updates animation.
 */
void
AnimInfo::update (void)
{
  GingaTime _current_time = ginga_gettime ();
  int dir;

  g_assert (_init);
  g_assert (!_done);

  dir = (_current < _target) ? 1 : -1;
  _current += dir * _speed * (double) (_current_time - _last_update);
  _last_update = _current_time;

  if (_duration == 0
      || (dir > 0 && _current >= _target)
      || (dir < 0 && _current <= _target))
    {
      _done = true;
    }
}

GINGA_PLAYER_END
