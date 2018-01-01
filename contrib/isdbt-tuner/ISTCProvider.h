/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef ISTC_PROVIDER_H
#define ISTC_PROVIDER_H

#include "ginga.h"

GINGA_TUNER_BEGIN

static G_GNUC_UNUSED const int ST_90KHz = 0x01;
static G_GNUC_UNUSED const int ST_27MHz = 0x02;

class ISTCProvider
{
public:
  virtual ~ISTCProvider (){};
  virtual bool getSTCValue (guint64 *stc, int *valueType) = 0;
};

GINGA_TUNER_END

#endif /* ISTC_PROVIDER_H */
