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

#include "config.h"
#include "UDPClient.h"

#include <unistd.h>

UDPClient::UDPClient () { sd = 0; }

UDPClient::~UDPClient () { closeSocket (); }

bool
UDPClient::connectSocket (string host, int port)
{
#if defined _MSC_VER || defined __CYGWIN__
  WORD L_Ver;
  WSADATA wsaData;

  L_Ver = MAKEWORD (2, 2);

  int L_Err = WSAStartup (L_Ver, &wsaData);
  if (L_Err != 0)
    {
      cout << " Can not find winsock dll!" << endl;
      return false;
    }

  if (LOBYTE (wsaData.wVersion) < 2 || HIBYTE (wsaData.wVersion) < 2)
    {
      cout << "UDPClient::createSocket Warning!";
      cout << " Winsock dll is too old!" << endl;
      return false;
    }
#endif
  struct protoent *proto;
  proto = getprotobyname ("udp");
  sd = socket (AF_INET, SOCK_DGRAM, proto->p_proto);
  if (sd < 0)
    {
      perror ("UDPClient::createSocket error");
      return false;
    }

  memset ((char *)&myaddr, 0, sizeof (myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  myaddr.sin_port = htons (port);

  if (bind (sd, (struct sockaddr *)&myaddr, sizeof (myaddr)) < 0)
    {
      perror ("bind failed");
      return 0;
    }

  memset (&remaddr, 0, sizeof (remaddr));
  remaddr.sin_family = AF_INET;
  remaddr.sin_port = htons (port);
  remaddr.sin_addr.s_addr = inet_addr (host.c_str ());

  return true;
}

int
UDPClient::send (char *buff, unsigned int size)
{
  if (sd <= 0)
    return -1;
  return sendto (sd, buff, size, 0, (struct sockaddr *)&remaddr,
                 sizeof (remaddr));
}

int
UDPClient::receive (char **buff)
{
  socklen_t size = sizeof (remaddr);

  if (sd <= 0)
    return -1;

  return recvfrom (sd, *buff, 8191, 0, (struct sockaddr *)&remaddr, &size);
}

void
UDPClient::closeSocket ()
{
  if (sd > 0)
    {
#if defined _MSC_VER || defined __CYGWIN__
      closesocket (sd);
#else
      close (sd);
#endif
    }
}

int
UDPClient::dataAvailable (int timeout)
{
  timeval tvtimeout;
  fd_set fds;

  if (sd <= 0)
    return false;

  tvtimeout.tv_sec = timeout / 1000;
  tvtimeout.tv_usec = timeout % 1000;
  FD_ZERO (&fds);
  FD_SET (sd, &fds);

  int nStatus = select (0, &fds, NULL, NULL, &tvtimeout);

  if (nStatus < 0)
    {
      perror ("UDPClient::select");
      return false;
    }
  else if (nStatus > 0)
    {
      return true;
    }

  return false;
}
