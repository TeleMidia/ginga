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
#include "SDLWindow.h"

#include "Display.h"

GINGA_MB_BEGIN


// Public methods.

SDLWindow::SDLWindow (int x, int y, int w, int h, int z, int zorder)
{
  this->texture = NULL;
  this->borderWidth = 0;
  this->bgColor = {0, 0, 0, 0};
  this->borderColor = {0, 0, 0, 0};
  this->rect.x = x;
  this->rect.y = y;
  this->rect.w = w;
  this->rect.h = h;
  this->z = z;
  this->zorder = zorder;
  this->ghost = false;
}

SDL_Texture *
SDLWindow::getTexture ()
{
  return this->texture;
}

void
SDLWindow::setTexture (SDL_Texture *texture)
{
  this->texture = texture;
}


SDLWindow::~SDLWindow ()
{
}

SDL_Color
SDLWindow::getBgColor ()
{
  return this->bgColor;
}

void
SDLWindow::setBgColor (SDL_Color c)
{
  this->bgColor = c;
}

SDL_Rect
SDLWindow::getRect ()
{
  return this->rect;
}

void
SDLWindow::setRect (SDL_Rect r)
{
  this->rect = r;
}

void
SDLWindow::setBorder (SDL_Color c, int w)
{
  borderWidth = w;
  borderColor = c;
}

void
SDLWindow::getBorder (SDL_Color *c, int *w)
{
  set_if_nonnull (c, this->borderColor);
  set_if_nonnull (w, this->borderWidth);
}

void
SDLWindow::setBounds (int posX, int posY, int w, int h)
{
  this->rect.x = posX;
  this->rect.y = posY;
  this->rect.w = w;
  this->rect.h = h;
}

void
SDLWindow::moveTo (int posX, int posY)
{
  this->rect.x = posX;
  this->rect.y = posY;
}

void
SDLWindow::resize (int width, int height)
{
  this->rect.w = width;
  this->rect.h = height;
}

int
SDLWindow::getX ()
{
  return this->rect.x;
}

int
SDLWindow::getY ()
{
  return this->rect.y;
}

int
SDLWindow::getW ()
{
  return this->rect.w;
}

int
SDLWindow::getH ()
{
  return this->rect.h;
}

void
SDLWindow::getZ (int *z, int *zorder)
{
  set_if_nonnull (z, this->z);
  set_if_nonnull (zorder, this->zorder);
}

void
SDLWindow::setX (int x)
{
  this->rect.x = x;
}

void
SDLWindow::setY (int y)
{
  this->rect.y = y;
}

void
SDLWindow::setW (int w)
{
  this->rect.w = w;
}

void
SDLWindow::setH (int h)
{
  this->rect.h = h;
}

void
SDLWindow::setZ (gint z)
{
  this->z = z;
}

bool
SDLWindow::isGhostWindow ()
{
  return ghost;
}

void
SDLWindow::setGhostWindow (bool ghost)
{
  this->ghost = ghost;
}

GINGA_MB_END
