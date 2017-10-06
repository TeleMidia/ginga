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

#include "aux-ginga.h"
#include "Transition.h"
#include "TransitionUtil.h"

GINGA_NCL_BEGIN

Transition::Transition (NclDocument *ncl, const string &id, int type)
  : Entity (ncl, id)
{
  setType (type);

  this->_duration = 1 * GINGA_SECOND;
  this->_startProgress = 0.;
  this->_endProgress = 1.;
  this->_direction = Transition::DIRECTION_FORWARD;
  this->_horzRepeat = 1;
  this->_vertRepeat = 1;
  this->_fadeColor = {0, 0, 0, 1.};
  this->_borderColor = {0, 0, 0, 0};
  this->_borderWidth = 0;
}

Transition::~Transition ()
{
}

GingaColor
Transition::getBorderColor ()
{
  return _borderColor;
}

int
Transition::getBorderWidth ()
{
  return _borderWidth;
}

short
Transition::getDirection ()
{
  return _direction;
}

GingaTime
Transition::getDuration ()
{
  return _duration;
}

double
Transition::getEndProgress ()
{
  return _endProgress;
}

GingaColor
Transition::getFadeColor ()
{
  return _fadeColor;
}

int
Transition::getHorzRepeat ()
{
  return _horzRepeat;
}

double
Transition::getStartProgress ()
{
  return _startProgress;
}

int
Transition::getSubtype ()
{
  return _subtype;
}

int
Transition::getType ()
{
  return _type;
}

int
Transition::getVertRepeat ()
{
  return _vertRepeat;
}

void
Transition::setBorderColor (GingaColor color)
{
  this->_borderColor = color;
}

void
Transition::setBorderWidth (int width)
{
  if (width >= 0)
    {
      this->_borderWidth = width;
    }
}

void
Transition::setDirection (short dir)
{
  if (dir >= Transition::DIRECTION_FORWARD
      && dir <= Transition::DIRECTION_REVERSE)
    {
      this->_direction = dir;
    }
}

void
Transition::setDuration (GingaTime duration)
{
  this->_duration = duration;
}

void
Transition::setEndProgress (double ep)
{
  if (ep >= 0 && ep <= 1 && ep >= _startProgress)
    {
      this->_endProgress = ep;
    }
}

void
Transition::setFadeColor (GingaColor color)
{
  this->_fadeColor = color;
}

void
Transition::setHorzRepeat (int num)
{
  if (num > 0)
    {
      this->_horzRepeat = num;
    }
}

void
Transition::setStartProgress (double sp)
{
  if (sp >= 0 && sp <= 1 && sp <= _endProgress)
    {
      this->_startProgress = sp;
    }
}

void
Transition::setSubtype (int subtype)
{
  switch (_type)
    {
    case Transition::TYPE_BARWIPE:
      if (subtype >= Transition::SUBTYPE_BARWIPE_LEFTTORIGHT
          && subtype <= Transition::SUBTYPE_BARWIPE_TOPTOBOTTOM)
        {
          this->_subtype = subtype;
        }

      break;

    case Transition::TYPE_IRISWIPE:
      if (subtype >= Transition::SUBTYPE_IRISWIPE_RECTANGLE
          && subtype <= Transition::SUBTYPE_IRISWIPE_DIAMOND)
        {
          this->_subtype = subtype;
        }

      break;

    case Transition::TYPE_CLOCKWIPE:
      if (subtype >= Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETWELVE
          && subtype <= Transition::SUBTYPE_CLOCKWIPE_CLOCKWISENINE)
        {
          this->_subtype = subtype;
        }

      break;

    case Transition::TYPE_SNAKEWIPE:
      if (subtype >= Transition::SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL
          && subtype <= Transition::SUBTYPE_SNAKEWIPE_BOTTOMLEFTDIAGONAL)
        {
          this->_subtype = subtype;
        }

      break;

    case Transition::TYPE_FADE:
    default:
      if (subtype >= Transition::SUBTYPE_FADE_CROSSFADE
          && subtype <= Transition::SUBTYPE_FADE_FADEFROMCOLOR)
        {
          this->_subtype = subtype;
        }
      break;
    }
}

void
Transition::setType (int type)
{
  if (type >= Transition::TYPE_BARWIPE && type <= Transition::TYPE_FADE)
    {
      this->_type = type;
      _subtype = TransitionUtil::getDefaultSubtype (type);
    }
}

void
Transition::setVertRepeat (int num)
{
  if (num > 0)
    {
      this->_vertRepeat = num;
    }
}

GINGA_NCL_END
