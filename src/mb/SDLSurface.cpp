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

#include "ginga.h"
#include "SDLSurface.h"


#include "system/Thread.h"
using namespace ::ginga::system;


//#include "IFontProvider.h" removed
#include "Display.h"
#include "SDLWindow.h"

GINGA_MB_BEGIN

SDLSurface::SDLSurface ()
{
  
}

SDLSurface::SDLSurface (void *underlyingSurface)
{

}

SDLSurface::~SDLSurface (){

}



void
SDLSurface::setChromaColor (SDL_Color color)
{
  SDLWindow *w = (SDLWindow *)parent;

  this->chromaColor = new  SDL_Color();
  this->chromaColor->r = color.r;
  this->chromaColor->g = color.g;
  this->chromaColor->b = color.b;
  this->chromaColor->a = color.a;

  Thread::mutexLock (&sMutex);
  if (sur != NULL)
    {
      Thread::mutexLock (&pMutex);
      
      Thread::mutexUnlock (&pMutex);
    }

  if (parent != NULL)
    {
      w->setColorKey (color);
    }

  Thread::mutexUnlock (&sMutex);
}

void
SDLSurface::setBgColor (SDL_Color color)
{
 
  this->bgColor = new  SDL_Color();
  this->bgColor->r = color.r;
  this->bgColor->g = color.g;
  this->bgColor->b = color.b;
  this->bgColor->a = color.a;

}

SDL_Color *
SDLSurface::getBgColor ()
{
  return bgColor;
}

void
SDLSurface::setColor (SDL_Color color)
{
  this->surfaceColor = new  SDL_Color();
  this->surfaceColor->r = color.r;
  this->surfaceColor->g = color.g;
  this->surfaceColor->b = color.b;
  this->surfaceColor->a = color.a;
}

SDL_Color *
SDLSurface::getColor ()
{
  return surfaceColor;
}




GINGA_MB_END
