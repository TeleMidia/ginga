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

#ifndef NCL_AREA_H
#define NCL_AREA_H

#include "NclAnchor.h"

GINGA_NAMESPACE_BEGIN

class NclArea: public NclAnchor
{
public:
  NclArea (NclDocument *, const string &, Time, Time);
  Time getBegin ();
  Time getEnd ();

protected:
  Time _begin;
  Time _end;
};

class NclAreaLambda: public NclArea
{
public:
  NclAreaLambda (NclDocument *doc, const string &id)
    :NclArea (doc, id, 0, GINGA_TIME_NONE) {};
};

GINGA_NAMESPACE_END

#endif // NCL_AREA_H
