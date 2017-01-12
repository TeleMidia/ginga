/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef _ICOLOR_H_
#define _ICOLOR_H_

#include "util/functions.h"

extern "C" {
#include <stdint.h>
}

#include <string>
using namespace std;

TELEMIDIA_UTIL_BEGIN_DECLS

class IColor {
	public:
		virtual ~IColor(){};
		virtual void setColor(string color)=0;
		virtual void setColor(int red, int green, int blue)=0;
		virtual int getR()=0;
		virtual int getG()=0;
		virtual int getB()=0;
		virtual int getAlpha()=0;
		virtual uint32_t getRGBA()=0;
		virtual uint32_t getARGB()=0;
};

TELEMIDIA_UTIL_END_DECLS

#endif //_ICOLOR_H_
