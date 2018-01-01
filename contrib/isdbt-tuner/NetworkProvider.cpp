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
#include "Tuner.h"
#include "NetworkProvider.h"

GINGA_TUNER_BEGIN

NetworkProvider::NetworkProvider (const string &address, int port, const string &protocol)
{
  clog << "UDP MulticastProvider address '" << address << ":";
  clog << port << "'" << endl;

  this->addr = address;
  this->portNumber = port;
  this->capabilities = DPC_CAN_FETCHDATA | DPC_CAN_CTLSTREAM;
  this->protocol = protocol;
  this->udpSocket = NULL;
}

NetworkProvider::~NetworkProvider ()
{
  if (udpSocket)
    {
      delete udpSocket;
    }
}

int
NetworkProvider::callServer ()
{
  try
    {
      if (protocol == "udp_multicast")
        {
          udpSocket = new UDPSocket ((unsigned short)portNumber);
          udpSocket->joinGroup (addr);
        }
      else if (protocol == "udp_unicast")
        {
          udpSocket = new UDPSocket (addr, (unsigned short)portNumber);
        }

      return 1;
    }
  catch (...)
    {
      udpSocket = NULL;
      return 0;
    }
}

char *
NetworkProvider::receiveData (int *len)
{
  char *buff = new char[BUFFSIZE];
  *len = udpSocket->recvFrom (buff, BUFFSIZE, addr,
                              (unsigned short &)portNumber);

  return buff;
}

GINGA_TUNER_END
