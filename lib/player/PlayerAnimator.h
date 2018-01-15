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

#ifndef PLAYER_ANIMATOR_H
#define PLAYER_ANIMATOR_H

#include "Formatter.h"

GINGA_NAMESPACE_BEGIN

// Entry in scheduled animations list.
class AnimInfo
{
public:
  AnimInfo (const string &, double, double, Time);
  ~AnimInfo ();

  string getName ();
  double getCurrent ();
  double getTarget ();
  Time getDuration ();
  double getSpeed ();
  bool isDone ();
  bool isInit ();

  void init (double current);
  void update ();

private:
  string _name;      // property name
  double _current;   // current value
  double _target;    // target value
  Time _duration;    // animation duration
  Time _last_update; // time of the last update
  double _speed;     // animation speed
  bool _done;        // true if animation is done
  bool _init;        // true if animation is initialized
};

class TransitionInfo
{
public:
  TransitionInfo (const string &, const string &, Time, gdouble, gdouble,
                  const string &, Color, guint32, guint32, guint32, Color);
  ~TransitionInfo ();

  string getType();
  string getSubType();
  Time getDur();
  gdouble getStartProgress();
  gdouble getEndProgress();
  string getDirection();
  Color getFadeColor();
  guint32 getHorzRepeat();
  guint32 getVertRepeat();
  guint32 getBorderWidth();
  Color getBorderColor();

private:
  string _type;
  string _subtype;
  Time _dur;
  gdouble _startProgress;
  gdouble _endProgress;
  string _direction;
  Color _fadeColor;
  guint32 _horzRepeat;
  guint32 _vertRepeat;
  guint32 _borderWidth;
  Color _borderColor;
};

class PlayerAnimator
{
public:
  PlayerAnimator (Formatter *);
  ~PlayerAnimator ();
  void clear ();
  void schedule (const string &, const string &, const string &, Time);
  void update (Rect *, Color *, guint8 *);
  void setTransitionProperties (const string &, const string &);
  void notifyPlayerStartOrStop(const string&, Rect *, Color *, guint8 *);

private:
  Formatter *_formatter;       // formatter handle
  list<AnimInfo *> _scheduled; // scheduled animations
  TransitionInfo *_transIn;
  TransitionInfo *_transOut;

  void doSchedule (const string &, const string &, const string &, Time);
};

GINGA_NAMESPACE_END

#endif // PLAYER_ANIMATOR_H
