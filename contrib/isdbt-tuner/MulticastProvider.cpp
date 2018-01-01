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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "ginga.h"
#include "MulticastProvider.h"
#include "Tuner.h"

GINGA_TUNER_BEGIN

MulticastProvider::MulticastProvider (const string &groupAddr, int port)
{
  clog << "UDP MulticastProvider address '" << groupAddr << ":";
  clog << port << "'" << endl;

  this->addr = groupAddr;
  this->portNumber = port;
  this->capabilities = DPC_CAN_FETCHDATA | DPC_CAN_CTLSTREAM;
}

MulticastProvider::~MulticastProvider ()
{
  if (udpSocket)
    {
      delete udpSocket;
    }
}

int
MulticastProvider::callServer ()
{
  try
    {
      udpSocket = new UDPSocket ((unsigned short)portNumber);
      udpSocket->joinGroup (addr);

      return 1;
    }
  catch (...)
    {
      udpSocket = NULL;
      return 0;
    }
}

char *
MulticastProvider::receiveData (int *len)
{
  char *buff = new char[BUFFSIZE];
  *len = udpSocket->recvFrom (buff, BUFFSIZE, addr,
                              (unsigned short &)portNumber);

  return buff;
}

GINGA_TUNER_END
