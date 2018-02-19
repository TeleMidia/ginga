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

/**
 * @brief The AnimInfo class.
 */
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
  int getStateNode ();
  void setStateNode (int);
  void update (const string &, double, double, Time);
  bool isDone ();
  bool isInit ();

  void init (double current, Time time);
  void update (Time time);

private:
  string _name;      // property name
  double _current;   // current value
  double _target;    // target value
  Time _duration;    // animation duration
  Time _last_update; // time of the last update
  double _speed;     // animation speed
  bool _done;        // true if animation is done
  bool _init;        // true if animation is initialized
  int _stateNode;    // for complex animations
};

/**
 * @brief The TransitionInfo struct.
 */
struct TransitionInfo
{
  const string type;
  const string subtype;
  const Time dur;
  const gdouble startProgress;
  const gdouble endProgress;
  const string direction;
  const Color fadeColor;
  const guint32 horzRepeat;
  const guint32 vertRepeat;
  const guint32 borderWidth;
  const Color borderColor;

  TransitionInfo (const string &, const string &, Time, gdouble, gdouble,
                  const string &, Color, guint32, guint32, guint32, Color);
};

/**
 * @brief The PlayerAnimator class.
 */
class PlayerAnimator
{
public:
  PlayerAnimator (Formatter *, Time *);
  ~PlayerAnimator ();
  void clear ();
  void schedule (const string &, const string &, const string &, Time);
  void update (Rect *, Color *, guint8 *, list<int> *);
  void setTransitionProperties (const string &, const string &);
  void scheduleTransition (const string &, Rect *, Color *, guint8 *,
                           list<int> *);

private:
  Formatter *_formatter;       // formatter handle
  list<AnimInfo *> _scheduled; // scheduled animations
  TransitionInfo *_transIn;
  TransitionInfo *_transOut;
  Time *_time;

  void doSchedule (const string &, const string &, const string &, Time);
  void updateSchedule (AnimInfo *, const string &, const string &,
                       const string &, Time);
};

GINGA_NAMESPACE_END

#endif // PLAYER_ANIMATOR_H
