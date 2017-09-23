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

#ifndef TRANSITION_H
#define TRANSITION_H

#include "Entity.h"

GINGA_NCL_BEGIN

class Transition : public Entity
{
public:
  static const int TYPE_BARWIPE = 0;
  static const int TYPE_IRISWIPE = 1;
  static const int TYPE_CLOCKWIPE = 2;
  static const int TYPE_SNAKEWIPE = 3;
  static const int TYPE_FADE = 4;

  static const int SUBTYPE_BARWIPE_LEFTTORIGHT = 0;
  static const int SUBTYPE_BARWIPE_TOPTOBOTTOM = 1;

  static const int SUBTYPE_IRISWIPE_RECTANGLE = 20;
  static const int SUBTYPE_IRISWIPE_DIAMOND = 21;

  static const int SUBTYPE_CLOCKWIPE_CLOCKWISETWELVE = 40;
  static const int SUBTYPE_CLOCKWIPE_CLOCKWISETHREE = 41;
  static const int SUBTYPE_CLOCKWIPE_CLOCKWISESIX = 42;
  static const int SUBTYPE_CLOCKWIPE_CLOCKWISENINE = 43;

  static const int SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL = 60;
  static const int SUBTYPE_SNAKEWIPE_TOPLEFTVERTICAL = 61;
  static const int SUBTYPE_SNAKEWIPE_TOPLEFTDIAGONAL = 62;
  static const int SUBTYPE_SNAKEWIPE_TOPRIGHTDIAGONAL = 63;
  static const int SUBTYPE_SNAKEWIPE_BOTTOMRIGHTDIAGONAL = 64;
  static const int SUBTYPE_SNAKEWIPE_BOTTOMLEFTDIAGONAL = 65;

  static const int SUBTYPE_FADE_CROSSFADE = 80;
  static const int SUBTYPE_FADE_FADETOCOLOR = 81;
  static const int SUBTYPE_FADE_FADEFROMCOLOR = 82;

  static const short DIRECTION_FORWARD = 0;
  static const short DIRECTION_REVERSE = 1;

public:
  Transition (NclDocument *, const string &, int);
  virtual ~Transition ();
  GingaColor getBorderColor ();
  int getBorderWidth ();
  short getDirection ();
  GingaTime getDuration ();
  double getEndProgress ();
  GingaColor getFadeColor ();
  int getHorzRepeat ();
  double getStartProgress ();
  int getSubtype ();
  int getType ();
  int getVertRepeat ();
  void setBorderColor (GingaColor);
  void setBorderWidth (int);
  void setDirection (short);
  void setDuration (GingaTime);
  void setEndProgress (double);
  void setFadeColor (GingaColor);
  void setHorzRepeat (int);
  void setStartProgress (double);
  void setSubtype (int);
  void setType (int);
  void setVertRepeat (int);

private:
  int _type;
  int _subtype;
  GingaTime _duration;
  double _startProgress;
  double _endProgress;
  short _direction;
  GingaColor _fadeColor;
  int _horzRepeat;
  int _vertRepeat;
  GingaColor _borderColor;
  int _borderWidth;
};

GINGA_NCL_END

#endif // TRANSITION_H
