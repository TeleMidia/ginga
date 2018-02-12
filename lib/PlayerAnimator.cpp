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
#include "PlayerAnimator.h"

GINGA_NAMESPACE_BEGIN

// PlayerAnimator: Public.

PlayerAnimator::PlayerAnimator (Formatter *formatter, Time *time)
{
  g_assert_nonnull (formatter);
  _formatter = formatter;
  _transIn = NULL;
  _transOut = NULL;
  _time = time;
}

PlayerAnimator::~PlayerAnimator ()
{
  delete _transIn;
  delete _transOut;
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

  TRACE ("%s from '%s' to '%s' in %" GINGA_TIME_FORMAT, name.c_str (),
         from.c_str (), to.c_str (), GINGA_TIME_ARGS (dur));

  list<string> list_pre = {"", "", "", ""};
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
      Color pre = {0, 0, 0, 255};
      Color pos;

      string pre_r, pre_g, pre_b;
      string pos_r, pos_g, pos_b;

      pre = ginga::parse_color (from);
      pre_r = xstrbuild ("%d", (int) CLAMP (pre.red * 255, 0, 255));
      pre_g = xstrbuild ("%d", (int) CLAMP (pre.green * 255, 0, 255));
      pre_b = xstrbuild ("%d", (int) CLAMP (pre.blue * 255, 0, 255));

      pos = ginga::parse_color (to);
      pos_r = xstrbuild ("%d", (int) CLAMP (pos.red * 255, 0, 255));
      pos_g = xstrbuild ("%d", (int) CLAMP (pos.green * 255, 0, 255));
      pos_b = xstrbuild ("%d", (int) CLAMP (pos.blue * 255, 0, 255));

      this->doSchedule ("background:r", pre_r, pos_r, dur);
      this->doSchedule ("background:g", pre_g, pos_g, dur);
      this->doSchedule ("background:b", pre_b, pos_b, dur);
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
PlayerAnimator::update (Rect *rect, Color *bgColor, guint8 *alpha,
                        list<int> *cropPolygon)
{

#define UPDATE(info, Type, var, rnd, min, max)                             \
  G_STMT_START                                                             \
  {                                                                        \
    if (!(info)->isInit ())                                                \
      (info)->init (var, *_time);                                          \
    (info)->update (*_time);                                               \
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
      else if (name == "barwipe:topToBottom")
        {
          list<int>::iterator it = cropPolygon->begin ();
          advance (it, 1);
          UPDATE (info, int, *it, lround, G_MININT, G_MAXINT);
          advance (it, 2);
          UPDATE (info, int, *it, lround, G_MININT, G_MAXINT);

          if (info->isDone ())
            cropPolygon->clear ();
        }
      else if (name == "barwipe:leftToRight")
        {
          list<int>::iterator it = cropPolygon->begin ();
          UPDATE (info, int, *it, lround, G_MININT, G_MAXINT);
          advance (it, 6);
          UPDATE (info, int, *it, lround, G_MININT, G_MAXINT);

          if (info->isDone ())
            cropPolygon->clear ();
        }
      else if (name == "diagonalwipe:topLeft:x")
        {
          list<int>::iterator it = cropPolygon->begin ();
          if (info->getStateNode () == 1)
            advance (it, 3);
          else
            advance (it, 2);

          UPDATE (info, int, *it, lround, G_MININT, G_MAXINT);

          if ((info->getStateNode () == 0)
              && (*it >= (rect->x + rect->width)))
            {
              info->setStateNode (1);
              this->updateSchedule (
                  info, "diagonalwipe:topLeft:x", to_string (rect->y),
                  to_string (rect->y + rect->height), info->getDuration ());
            }

          if (info->isDone ())
            cropPolygon->clear ();
        }
      else if (name == "diagonalwipe:topLeft:y")
        {
          list<int>::iterator it = cropPolygon->begin ();
          if (info->getStateNode () == 0)
            advance (it, 1);

          UPDATE (info, int, *it, lround, G_MININT, G_MAXINT);

          if ((info->getStateNode () == 0)
              && (*it >= (rect->y + rect->height)))
            {
              info->setStateNode (1);
              this->updateSchedule (
                  info, "diagonalwipe:topLeft:y", to_string (rect->x),
                  to_string (rect->x + rect->width), info->getDuration ());
            }

          if (info->isDone ())
            cropPolygon->clear ();
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
  // WARNING ("property '%s': value '%s'", name.c_str (), value.c_str ());
  map<string, string> tab = ginga::parse_table (value);

  string type = tab["type"];
  string subtype = tab["subtype"];
  Time dur = ginga::parse_time (tab["dur"]);
  gdouble startProgress = stod (tab["startProgress"]);
  gdouble endProgress = stod (tab["endProgress"]);
  string direction = tab["direction"];
  Color fadeColor = ginga::parse_color (tab["fadeColor"]);
  guint horzRepeat = xstrtouint (tab["horzRepeat"], 10);
  guint vertRepeat = xstrtouint (tab["vertRepeat"], 10);
  guint borderWidth = xstrtouint (tab["borderWidth"], 10);
  Color borderColor = ginga::parse_color (tab["borderColor"]);

  if (name == "transIn")
    {
      _transIn = new TransitionInfo (
          type, subtype, dur, startProgress, endProgress, direction,
          fadeColor, horzRepeat, vertRepeat, borderWidth, borderColor);
    }
  else if (name == "transOut")
    {
      _transOut = new TransitionInfo (
          type, subtype, dur, startProgress, endProgress, direction,
          fadeColor, horzRepeat, vertRepeat, borderWidth, borderColor);
    }
}

static bool
createPolygon (string name, Rect *rect, list<int> *cropPoly)
{
  if (name == "barWipe")
    {
      cropPoly->insert (cropPoly->end (), rect->x); // dot1 (x,y)
      cropPoly->insert (cropPoly->end (), rect->y); //
      cropPoly->insert (cropPoly->end (),
                        rect->x + rect->width);     // dot1 (x+w,y)
      cropPoly->insert (cropPoly->end (), rect->y); //
      cropPoly->insert (cropPoly->end (),
                        rect->x + rect->width); // dot3 (x+w,y+h)
      cropPoly->insert (cropPoly->end (), rect->y + rect->height); //
      cropPoly->insert (cropPoly->end (), rect->x); // dot4 (x,y+h)
      cropPoly->insert (cropPoly->end (), rect->y + rect->height); //
    }
  else if (name == "diagonalWipe")
    {
      cropPoly->insert (cropPoly->end (), rect->x); // dot1 (x,y)
      cropPoly->insert (cropPoly->end (), rect->y); //
      cropPoly->insert (cropPoly->end (), rect->x); // dot2 (x,y)
      cropPoly->insert (cropPoly->end (), rect->y); //
      cropPoly->insert (cropPoly->end (),
                        rect->x + rect->width);     // dot3 (x+w,y)
      cropPoly->insert (cropPoly->end (), rect->y); //
      cropPoly->insert (cropPoly->end (),
                        rect->x + rect->width); // dot4 (x+w,y+h)
      cropPoly->insert (cropPoly->end (), rect->y + rect->height); //
      cropPoly->insert (cropPoly->end (), rect->x); // dot5 (x,y+h)
      cropPoly->insert (cropPoly->end (), rect->y + rect->height); //
    }

  return true;
}

void
PlayerAnimator::scheduleTransition (const string &notificationType,
                                    Rect *rect, unused (Color *bgColor),
                                    guint8 *alpha, list<int> *cropPoly)
{
  cropPoly->clear ();

  if (notificationType == "start")
    {
      if (_transIn == NULL)
        return;

      if (_transIn->type == "barWipe")
        {
          createPolygon (_transIn->type, rect, cropPoly);

          if (_transIn->subtype == "topToBottom")
            {
              this->schedule ("barwipe:topToBottom", to_string (rect->y),
                              to_string (rect->y + rect->height),
                              _transIn->dur);
            }
          else
            {
              this->schedule ("barwipe:leftToRight", to_string (rect->x),
                              to_string (rect->x + rect->width),
                              _transIn->dur);
            }
        }
      else if (_transIn->type == "diagonalWipe")
        {
          createPolygon (_transIn->type, rect, cropPoly);

          this->schedule ("diagonalwipe:topLeft:x", to_string (rect->x),
                          to_string (rect->x + rect->width),
                          _transIn->dur / 2);
          this->schedule ("diagonalwipe:topLeft:y", to_string (rect->y),
                          to_string (rect->y + rect->height),
                          _transIn->dur / 2);
        }
      else
        {
          this->schedule ("transparency", "0", to_string (*alpha),
                          _transIn->dur);
        }
    }
  else
    {
      if (_transOut == NULL)
        return;
    }
}

// PlayerAnimator: Private.

void
PlayerAnimator::updateSchedule (AnimInfo *info, const string &name,
                                const string &from, const string &to,
                                Time dur)
{
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

  info->update (name, current, target, dur);
}

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

  TRACE ("---> from %s", from.c_str ());
  TRACE ("---> to %s", to.c_str ());

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
    info->init (current, *_time);

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
  _stateNode = 0;
  _speed = 1.0;
}

void
AnimInfo::update (const string &name, double from, double to, Time dur)
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

int
AnimInfo::getStateNode ()
{
  return _stateNode;
}

void
AnimInfo::setStateNode (int state)
{
  _stateNode = state;
}

void
AnimInfo::init (double current, Time time)
{
  g_assert (!_init);
  _current = current;
  if (_duration > 0)
    _speed = fabs (_target - current) / (double) _duration;
  else
    _speed = 0;
  _init = true;
  _last_update = time;
}

void
AnimInfo::update (Time time)
{
  Time _current_time = time; // micro to mili
  int dir;

  g_assert (_init);
  // g_assert (!_done);

  dir = (_current < _target) ? 1 : -1;
  _current += dir * _speed * (double) (_current_time - _last_update);

  _last_update = _current_time;

  if (_duration == 0 || (dir > 0 && _current >= _target)
      || (dir < 0 && _current <= _target))
    {
      _current = _target;
      _done = true;
    }
}

// Transition Info

TransitionInfo::TransitionInfo (const string &type, const string &subtype,
                                Time dur, gdouble startProgress,
                                gdouble endProgres, const string &direction,
                                Color fadeColor, guint32 horzRepeat,
                                guint32 vertRepeat, guint32 borderWidth,
                                Color borderColor)
    : type (type), subtype (subtype), dur (dur),
      startProgress (startProgress), endProgress (endProgres),
      direction (direction), fadeColor (fadeColor), horzRepeat (horzRepeat),
      vertRepeat (vertRepeat), borderWidth (borderWidth),
      borderColor (borderColor)
{
}

GINGA_NAMESPACE_END
