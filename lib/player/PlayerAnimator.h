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

#ifndef PLAYER_ANIMATOR_H
#define PLAYER_ANIMATOR_H

#include "GingaInternal.h"

GINGA_PLAYER_BEGIN

// Entry in scheduled animations list.
class AnimInfo
{
public:
  AnimInfo (const string &, double, double, GingaTime);
  ~AnimInfo ();

  string getName ();
  double getCurrent ();
  double getTarget ();
  GingaTime getDuration ();
  double getSpeed ();
  bool isDone ();
  bool isInit ();

  void init (double current);
  void update (void);

private:
  string _name;                  // property name
  double _current;               // current value
  double _target;                // target value
  GingaTime _duration;           // animation duration
  GingaTime _last_update;        // time of the last update
  double _speed;                 // animation speed
  bool _done;                    // true if animation is done
  bool _init;                    // true if animation is initialized
};

class PlayerAnimator
{
public:
  PlayerAnimator(GingaInternal *);
  ~PlayerAnimator();
  void clear ();
  void schedule (const string &, const string &, const string &, GingaTime);
  void update (GingaRect *, GingaColor *, guint8 *);

private:
  GingaInternal *_ginga;        // ginga handle
  list <AnimInfo *> _scheduled; // scheduled animations
  void doSchedule (const string &, const string &,
                   const string &, GingaTime);
};

GINGA_PLAYER_END

#endif // PLAYER_ANIMATOR_H
