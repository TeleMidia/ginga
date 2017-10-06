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
#include "TransitionUtil.h"

GINGA_NCL_BEGIN

int
TransitionUtil::getTypeCode (const string &type)
{
  if (type == "barWipe")
    {
      return Transition::TYPE_BARWIPE;
    }
  else if (type == "irisWipe")
    {
      return Transition::TYPE_IRISWIPE;
    }
  else if (type == "clockWipe")
    {
      return Transition::TYPE_CLOCKWIPE;
    }
  else if (type == "snakeWipe")
    {
      return Transition::TYPE_SNAKEWIPE;
    }
  else if (type == "fade")
    {
      return Transition::TYPE_FADE;
    }
  else
    {
      return -1;
    }
}

string
TransitionUtil::getTypeName (int type)
{
  switch (type)
    {
    case Transition::TYPE_BARWIPE:
      return "barWipe";

    case Transition::TYPE_IRISWIPE:
      return "irisWipe";

    case Transition::TYPE_CLOCKWIPE:
      return "clockWipe";

    case Transition::TYPE_SNAKEWIPE:
      return "snakeWipe";

    case Transition::TYPE_FADE:
      return "fade";

    default:
      g_assert_not_reached ();
    }
}

int
TransitionUtil::getSubtypeCode (int type, const string &subtype)
{
  switch (type)
    {
    case Transition::TYPE_BARWIPE:
      if (subtype == "leftToRight")
        {
          return Transition::SUBTYPE_BARWIPE_LEFTTORIGHT;
        }
      else if (subtype == "topToBottom")
        {
          return Transition::SUBTYPE_BARWIPE_TOPTOBOTTOM;
        }

      break;

    case Transition::TYPE_IRISWIPE:
      if (subtype == "rectangle")
        {
          return Transition::SUBTYPE_IRISWIPE_RECTANGLE;
        }
      else if (subtype == "diamond")
        {
          return Transition::SUBTYPE_IRISWIPE_DIAMOND;
        }

      break;

    case Transition::TYPE_CLOCKWIPE:
      if (subtype == "clockwiseTwelve")
        {
          return Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETWELVE;
        }
      else if (subtype == "clockwiseThree")
        {
          return Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETHREE;
        }
      else if (subtype == "clockwiseSix")
        {
          return Transition::SUBTYPE_CLOCKWIPE_CLOCKWISESIX;
        }
      else if (subtype == "clockwiseNine")
        {
          return Transition::SUBTYPE_CLOCKWIPE_CLOCKWISENINE;
        }

      break;

    case Transition::TYPE_SNAKEWIPE:
      if (subtype == "topLeftHorizontal")
        {
          return Transition::SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL;
        }
      else if (subtype == "topLeftVertical")
        {
          return Transition::SUBTYPE_SNAKEWIPE_TOPLEFTVERTICAL;
        }
      else if (subtype == "topLeftDiagonal")
        {
          return Transition::SUBTYPE_SNAKEWIPE_TOPLEFTDIAGONAL;
        }
      else if (subtype == "topRightDiagonal")
        {
          return Transition::SUBTYPE_SNAKEWIPE_TOPRIGHTDIAGONAL;
        }
      else if (subtype == "bottomRightDiagonal")
        {
          return Transition::SUBTYPE_SNAKEWIPE_BOTTOMRIGHTDIAGONAL;
        }
      else if (subtype == "bottomLeftDiagonal")
        {
          return Transition::SUBTYPE_SNAKEWIPE_BOTTOMLEFTDIAGONAL;
        }

      break;

    case Transition::TYPE_FADE:
      if (subtype == "crossfade")
        {
          return Transition::SUBTYPE_FADE_CROSSFADE;
        }
      else if (subtype == "fadeToColor")
        {
          return Transition::SUBTYPE_FADE_FADETOCOLOR;
        }
      else if (subtype == "fadeFromColor")
        {
          return Transition::SUBTYPE_FADE_FADEFROMCOLOR;
        }

      break;

    default:
      g_assert_not_reached ();
    }

  return -1;
}

string
TransitionUtil::getSubtypeName (int type, int subtype)
{
  string subtypeName = "";

  switch (type)
    {
    case Transition::TYPE_BARWIPE:
      switch (subtype)
        {
        case Transition::SUBTYPE_BARWIPE_LEFTTORIGHT:
          subtypeName = "leftToRight";
          break;

        case Transition::SUBTYPE_BARWIPE_TOPTOBOTTOM:
          subtypeName = "topToBottom";
          break;

        default:
          break;
        }
      break;

    case Transition::TYPE_IRISWIPE:
      switch (subtype)
        {
        case Transition::SUBTYPE_IRISWIPE_RECTANGLE:
          subtypeName = "rectangle";
          break;

        case Transition::SUBTYPE_IRISWIPE_DIAMOND:
          subtypeName = "diamond";
          break;

        default:
          break;
        }
      break;

    case Transition::TYPE_CLOCKWIPE:
      switch (subtype)
        {
        case Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETWELVE:
          subtypeName = "clockwiseTwelve";
          break;

        case Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETHREE:
          subtypeName = "clockwiseThree";
          break;

        case Transition::SUBTYPE_CLOCKWIPE_CLOCKWISESIX:
          subtypeName = "clockwiseSix";
          break;

        case Transition::SUBTYPE_CLOCKWIPE_CLOCKWISENINE:
          subtypeName = "clockwiseNine";
          break;

        default:
          break;
        }
      break;

    case Transition::TYPE_SNAKEWIPE:
      switch (subtype)
        {
        case Transition::SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL:
          subtypeName = "topLeftHorizontal";
          break;

        case Transition::SUBTYPE_SNAKEWIPE_TOPLEFTVERTICAL:
          subtypeName = "topLeftVertical";
          break;

        case Transition::SUBTYPE_SNAKEWIPE_TOPLEFTDIAGONAL:
          subtypeName = "topLeftDiagonal";
          break;

        case Transition::SUBTYPE_SNAKEWIPE_TOPRIGHTDIAGONAL:
          subtypeName = "topRightDiagonal";
          break;

        case Transition::SUBTYPE_SNAKEWIPE_BOTTOMRIGHTDIAGONAL:
          subtypeName = "bottomRigthDiagonal";
          break;

        case Transition::SUBTYPE_SNAKEWIPE_BOTTOMLEFTDIAGONAL:
          subtypeName = "bottomLeftDiagonal";
          break;

        default:
          break;
        }
      break;

    case Transition::TYPE_FADE:
      switch (subtype)
        {
        case Transition::SUBTYPE_FADE_CROSSFADE:
          subtypeName = "crossfade";
          break;

        case Transition::SUBTYPE_FADE_FADETOCOLOR:
          subtypeName = "fadeToColor";
          break;

        case Transition::SUBTYPE_FADE_FADEFROMCOLOR:
          subtypeName = "fadeFromColor";
          break;

        default:
          break;
        }
      break;

    default:
      break;
    }

  return (subtypeName);
}

short
TransitionUtil::getDirectionCode (const string &direction)
{
  if (direction == "forward")
    {
      return Transition::DIRECTION_FORWARD;
    }
  else if (direction == "reverse")
    {
      return Transition::DIRECTION_REVERSE;
    }
  else
    {
      return -1;
    }
}

string
TransitionUtil::getDirectionName (short direction)
{
  switch (direction)
    {
    case Transition::DIRECTION_FORWARD:
      return "forward";

    case Transition::DIRECTION_REVERSE:
      return "reverse";

    default:
      g_assert_not_reached ();
    }
}

int
TransitionUtil::getDefaultSubtype (int type)
{
  switch (type)
    {
    case Transition::TYPE_BARWIPE:
      return Transition::SUBTYPE_BARWIPE_LEFTTORIGHT;

    case Transition::TYPE_IRISWIPE:
      return Transition::SUBTYPE_IRISWIPE_RECTANGLE;

    case Transition::TYPE_CLOCKWIPE:
      return Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETWELVE;

    case Transition::TYPE_SNAKEWIPE:
      return Transition::SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL;

    case Transition::TYPE_FADE:
      return Transition::SUBTYPE_FADE_CROSSFADE;

    default:
      return Transition::SUBTYPE_FADE_CROSSFADE;
    }
}

GINGA_NCL_END
