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
#include "ContentAnchor.h"

GINGA_NCL_BEGIN

const short ContentAnchor::CAT_NONE = 0x00;
const short ContentAnchor::CAT_ALL = 0x01;
const short ContentAnchor::CAT_TIME = 0x02;
const short ContentAnchor::CAT_SAMPLES = 0x03;
const short ContentAnchor::CAT_FRAMES = 0x04;
const short ContentAnchor::CAT_NPT = 0x05;

ContentAnchor::ContentAnchor (string id) : Anchor (id)
{
  typeSet.insert ("ContentAnchor");
}

GINGA_NCL_END
