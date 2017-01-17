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

#ifndef KEY_NAVIGATION_H
#define KEY_NAVIGATION_H

#include "namespaces.h"

BR_PUCRIO_TELEMIDIA_NCL_NAVIGATION_BEGIN

class KeyNavigation
{
private:
  string focusIndex;
  string moveUp;
  string moveDown;
  string moveLeft;
  string moveRight;

public:
  KeyNavigation();
  string getFocusIndex();
  void setFocusIndex(string index);
  string getMoveUp();
  void setMoveUp(string index);
  string getMoveDown();
  void setMoveDown(string index);
  string getMoveRight();
  void setMoveRight(string index);
  string getMoveLeft();
  void setMoveLeft(string index);
};

BR_PUCRIO_TELEMIDIA_NCL_NAVIGATION_END

#endif /* KEY_NAVIGATION_H */
