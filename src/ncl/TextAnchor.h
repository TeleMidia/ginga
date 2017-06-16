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

#ifndef TEXTANCHOR_H_
#define TEXTANCHOR_H_

#include "Entity.h"
#include "Anchor.h"

GINGA_NCL_BEGIN

class TextAnchor : public Anchor
{
public:
  TextAnchor (const string &id, const string &t, int p);
  int getPosition ();
  string getText ();
  void setPosition (int p);
  void setText (const string &t);

protected:
  string _text;
  int _position;
};

GINGA_NCL_END

#endif /*TEXTANCHOR_H_*/
