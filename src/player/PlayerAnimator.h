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
   void addProperty(string dur, string name, string value);
   void update(SDL_Rect* rect);

   private:
   GList *properties; 
   void updateList(string dur, string name, string value);
   void updatePosition(SDL_Rect* rect, ANIM_PROPERTY* pr);
   void calculePosition(gint32 * value, ANIM_PROPERTY* pr,gint32 dir);
   bool calculeVelocity(gint32 * value, ANIM_PROPERTY* pr);
   gdouble cvtTimeIntToDouble(guint32 value);
   gdouble getAnimationVelocity(gdouble initPos,
                                gdouble finalPos,
                                gdouble duration);

};

#endif /* PLAYER_ANIMATOR_H */
