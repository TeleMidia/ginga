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

#ifndef ANIMATION_H
#define ANIMATION_H

#include "ginga.h"

GINGA_NCL_BEGIN

class Animation
{
private:
  string duration;
  string by;

public:
  Animation();
  void setDuration(string dur);
  string getDuration();
  void setBy(string by);
  string getBy();
};

GINGA_NCL_END

#endif /* ANIMATION_H */
