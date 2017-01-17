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

#ifndef COMPARATOR_H
#define COMPARATOR_H

#include "ginga.h"

BR_PUCRIO_TELEMIDIA_NCL_UTIL_BEGIN

class Comparator
{
public:
  static const short CMP_EQ = 0;
  static const short CMP_NE = 1;
  static const short CMP_LT = 2;
  static const short CMP_LTE = 3;
  static const short CMP_GT = 4;
  static const short CMP_GTE = 5;
  static bool evaluate(string first, string second, short comparator);
  static bool evaluate(float first, float second, short comparator);
  static string toString(short comparator);
  static short fromString(string comp);
};

BR_PUCRIO_TELEMIDIA_NCL_UTIL_END

#endif /* COMPARATOR_H */
