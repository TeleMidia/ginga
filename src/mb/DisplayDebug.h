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

#ifndef DISPLAY_DEBUG_H
#define DISPLAY_DEBUG_H

#include "ginga.h"

#include "IContinuousMediaProvider.h"
#include "IDiscreteMediaProvider.h"
#include "SDLSurface.h"
#include "SDLWindow.h"

GINGA_MB_BEGIN

class DisplayDebug{
   
   private:
   int width;
   int height;
   gdouble iniTime;
   guint32 accTime;
   gdouble totalTime;
   SDL_Texture* texture;
   SDL_Rect rect;
   gboolean isActive;
   
   SDL_Texture * updateTexture(SDL_Renderer * renderer, SDL_Rect rect,gchar * fps_str);

   public:
   DisplayDebug (int width, int height);
   ~DisplayDebug ();

   void toggle(void);
   void update(gdouble elapsedTime);
   void draw(SDL_Renderer* renderer, guint32 elapsedTime);

};


GINGA_MB_END

#endif /* DISPLAY_H */