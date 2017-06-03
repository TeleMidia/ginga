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
#include "Transition.h"
#include "TransitionUtil.h"

GINGA_NCL_BEGIN

Transition::Transition (const string &id, int type) : Entity (id)
{
  setType (type);

  this->dur = 1000; /* 1 second */
  this->startProgress = 0.0;
  this->endProgress = 1.0;
  this->direction = Transition::DIRECTION_FORWARD;
  this->horzRepeat = 1;
  this->vertRepeat = 1;
  this->fadeColor = {0, 0, 0, 255};
  this->borderColor = {0, 0, 0, 0};
  this->borderWidth = 0;
}

Transition::~Transition ()
{
}

SDL_Color
Transition::getBorderColor ()
{
  return borderColor;
}

int
Transition::getBorderWidth ()
{
  return borderWidth;
}

short
Transition::getDirection ()
{
  return direction;
}

double
Transition::getDur ()
{
  return dur;
}

double
Transition::getEndProgress ()
{
  return endProgress;
}

SDL_Color
Transition::getFadeColor ()
{
  return fadeColor;
}

int
Transition::getHorzRepeat ()
{
  return horzRepeat;
}

double
Transition::getStartProgress ()
{
  return startProgress;
}

int
Transition::getSubtype ()
{
  return subtype;
}

int
Transition::getType ()
{
  return type;
}

int
Transition::getVertRepeat ()
{
  return vertRepeat;
}

void
Transition::setBorderColor (SDL_Color color)
{
  this->borderColor = color;
}

void
Transition::setBorderWidth (int width)
{
  if (width >= 0)
    {
      this->borderWidth = width;
    }
}

void
Transition::setDirection (short dir)
{
  if (dir >= Transition::DIRECTION_FORWARD
      && dir <= Transition::DIRECTION_REVERSE)
    {
      this->direction = dir;
    }
}

void
Transition::setDur (double dur)
{
  if (dur >= 0)
    {
      this->dur = dur;
    }
}

void
Transition::setEndProgress (double ep)
{
  if (ep >= 0 && ep <= 1 && ep >= startProgress)
    {
      this->endProgress = ep;
    }
}

void
Transition::setFadeColor (SDL_Color color)
{
  this->fadeColor = color;
}

void
Transition::setHorzRepeat (int num)
{
  if (num > 0)
    {
      this->horzRepeat = num;
    }
}

void
Transition::setStartProgress (double sp)
{
  if (sp >= 0 && sp <= 1 && sp <= endProgress)
    {
      this->startProgress = sp;
    }
}

void
Transition::setSubtype (int subtype)
{
  switch (type)
    {
    case Transition::TYPE_BARWIPE:
      if (subtype >= Transition::SUBTYPE_BARWIPE_LEFTTORIGHT
          && subtype <= Transition::SUBTYPE_BARWIPE_TOPTOBOTTOM)
        {
          this->subtype = subtype;
        }

      break;

    case Transition::TYPE_IRISWIPE:
      if (subtype >= Transition::SUBTYPE_IRISWIPE_RECTANGLE
          && subtype <= Transition::SUBTYPE_IRISWIPE_DIAMOND)
        {
          this->subtype = subtype;
        }

      break;

    case Transition::TYPE_CLOCKWIPE:
      if (subtype >= Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETWELVE
          && subtype <= Transition::SUBTYPE_CLOCKWIPE_CLOCKWISENINE)
        {
          this->subtype = subtype;
        }

      break;

    case Transition::TYPE_SNAKEWIPE:
      if (subtype >= Transition::SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL
          && subtype <= Transition::SUBTYPE_SNAKEWIPE_BOTTOMLEFTDIAGONAL)
        {
          this->subtype = subtype;
        }

      break;

    case Transition::TYPE_FADE:
    default:
      if (subtype >= Transition::SUBTYPE_FADE_CROSSFADE
          && subtype <= Transition::SUBTYPE_FADE_FADEFROMCOLOR)
        {
          this->subtype = subtype;
        }
      break;
    }
}

void
Transition::setType (int type)
{
  if (type >= Transition::TYPE_BARWIPE && type <= Transition::TYPE_FADE)
    {
      this->type = type;
      subtype = TransitionUtil::getDefaultSubtype (type);
    }
}

void
Transition::setVertRepeat (int num)
{
  if (num > 0)
    {
      this->vertRepeat = num;
    }
}

GINGA_NCL_END
