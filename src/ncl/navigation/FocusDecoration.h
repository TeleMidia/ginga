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

#ifndef FOCUS_DECORATION_H
#define FOCUS_DECORATION_H

#include "namespaces.h"

#include "util/Color.h"
using namespace ::br::pucrio::telemidia::util;

BR_PUCRIO_TELEMIDIA_NCL_NAVIGATION_BEGIN

class FocusDecoration
{
private:
  Color* focusBorderColor;
  double focusBorderTransparency;
  string focusSrc;
  int focusBorderWidth;
  Color* selBorderColor;
  string focusSelSrc;

public:
  FocusDecoration();
  virtual ~FocusDecoration();
  Color* getFocusBorderColor();
  double getFocusBorderTransparency();
  string getFocusSelSrc();
  string getFocusSrc();
  int getFocusBorderWidth();
  Color* getSelBorderColor();
  void setFocusBorderColor(Color* color);
  void setFocusBorderTransparency(double alpha);
  void setFocusBorderWidth(int width);
  void setFocusSelSrc(string src);
  void setFocusSrc(string src);
  void setSelBorderColor(Color* color);
};

BR_PUCRIO_TELEMIDIA_NCL_NAVIGATION_END

#endif /* FOCUS_DECORATION_H */
