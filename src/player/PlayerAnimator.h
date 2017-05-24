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

#include "ginga.h"

typedef struct
{
  string name;
  gdouble duration;
  gdouble velocity;
  gdouble targetValue;
  gdouble curValue;  
} ANIM_PROPERTY;

class PlayerAnimator
{
public:
  PlayerAnimator();
  ~PlayerAnimator();
  void addProperty(const string &dur, const string &name, const string &value);
  void update(SDL_Rect* rect, guint8* r, guint8* g, guint8* b, guint8* alpha);

private:
  GList *properties;
  void updateList(const string &dur, const string &name, const string &value);
  void updatePosition(SDL_Rect* rect, ANIM_PROPERTY* pr);
  void updateColor(guint8* alpha,  guint8* red,
                   guint8* green,guint8* blue,
                   ANIM_PROPERTY* pr);
  void calculatePosition(gint32* value, ANIM_PROPERTY* pr,gint32 dir);
  void calculateColor(guint8* value, ANIM_PROPERTY* pr,gint32 dir);
  bool calculateVelocity(gint32* value, ANIM_PROPERTY* pr);
  bool calculateVelocity(guint8* value, ANIM_PROPERTY* pr);
  gdouble cvtTimeIntToDouble(guint32 value);
  gdouble getAnimationVelocity(gdouble initPos, gdouble finalPos, gdouble dur);

};

#endif /* PLAYER_ANIMATOR_H */
