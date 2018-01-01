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

#ifndef ITPROVIDERLISTENER_H_
#define ITPROVIDERLISTENER_H_

GINGA_TUNER_BEGIN

static G_GNUC_UNUSED const short PST_LOOP = 0x01;

class ITProviderListener
{
public:
  virtual ~ITProviderListener (){};
  virtual void receiveSignal (short signalCode) = 0;
};

GINGA_TUNER_END

#endif /*ITPROVIDERLISTENER_H_*/
