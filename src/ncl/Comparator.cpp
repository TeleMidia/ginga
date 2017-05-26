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
#include "Comparator.h"

GINGA_NCL_BEGIN

bool
Comparator::evaluate (const string &first, const string &second, short comparator)
{ // 2 object -> comparable

  if (first == "" || second == "")
    {
      return false;
    }

  int ret;
  ret = first.compare (second);
  switch (comparator)
    {
    case CMP_EQ:
      if (ret == 0)
        return true;
      else
        return false;

    case CMP_NE:
      if (ret == 0)
        return false;
      else
        return true;

    case CMP_LT:
      if (ret < 0)
        return true;
      else
        return false;

    case CMP_LTE:
      if (ret <= 0)
        return true;
      else
        return false;

    case CMP_GT:
      if (ret > 0)
        return true;
      else
        return false;

    case CMP_GTE:
      if (ret >= 0)
        return true;
      else
        return false;

    default:
      return false;
    }
}

bool
Comparator::evaluate (double first, double second, short comparator)
{ // 2 object -> comparable

  int ret;
  if (first > second)
    ret = 1;
  else if (first < second)
    ret = -1;
  else
    ret = 0;

  switch (comparator)
    {
    case CMP_EQ:
      if (ret == 0)
        return true;
      else
        return false;

    case CMP_NE:
      if (ret == 0)
        return false;
      else
        return true;

    case CMP_LT:
      if (ret < 0)
        return true;
      else
        return false;

    case CMP_LTE:
      if (ret <= 0)
        return true;
      else
        return false;

    case CMP_GT:
      if (ret > 0)
        return true;
      else
        return false;

    case CMP_GTE:
      if (ret >= 0)
        return true;
      else
        return false;

    default:
      return false;
    }
}

short
Comparator::fromString (const string &comp)
{
  string comparator = comp;
  for (unsigned i = 0; i < comparator.length (); i++)
    {
      if (comparator[i] >= 'A' && comparator[i] <= 'Z')
        {
          comparator[i] = (char) (comparator[i] - 'A' + 'a');
        }
    }

  if (comparator == "eq")
    {
      return Comparator::CMP_EQ;
    }
  else if (comparator == "ne")
    {
      return Comparator::CMP_NE;
    }
  else if (comparator == "gt")
    {
      return Comparator::CMP_GT;
    }
  else if (comparator == "lt")
    {
      return Comparator::CMP_LT;
    }
  else if (comparator == "gte")
    {
      return Comparator::CMP_GTE;
    }
  else if (comparator == "lte")
    {
      return Comparator::CMP_LTE;
    }
  else
    {
      return Comparator::CMP_EQ;
    }
}

GINGA_NCL_END
