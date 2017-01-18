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
#include "TcpClientConnection.h"

GINGA_MULTIDEVICE_BEGIN

const static int MAX_MSG_SIZE = 1024;

TCPClientConnection::TCPClientConnection (unsigned int devid,
                                          unsigned int index, char *hostname,
                                          char *port_str,
                                          IRemoteDeviceListener *srv)
{

  try
    {
      deviceId = devid;
      orderId = index;
      counter = 0;
      tcpSocket = new TCPSocket (string (hostname), atoi (port_str));
      ////
      // TODO: improve (create setIndex e getIndex methods) so index does not
      // change

      char *set_index = g_strdup_printf ("%d %s %s=%d\n", 0, "SET",
                                         "child.index", orderId);
      this->post (set_index);
      ////
      running = true;
      resrv = srv;
    }
  catch (SocketException &e)
    {
      clog << "TCPClientConnection:: creation error" << endl;
      clog << e.what () << endl;
    }
}

TCPClientConnection::~TCPClientConnection ()
{
  this->release ();
  if (tcpSocket != NULL)
    {
      delete tcpSocket;
    }
}

bool
TCPClientConnection::post (char *str)
{
  char *com;

  if (tcpSocket == NULL)
    {
      return false;
    }
  com = g_strdup_printf ("%d", counter);
  string s_com = string (com) + " " + string (str);
  counter++;

  try
    {
      // tcpSocket->send(com, strlen(com));
      tcpSocket->send ((char *)s_com.c_str (), (int)s_com.size ());
      return true;
    }
  catch (SocketException &e)
    {
      clog << "TCPClientConnection::post send error" << endl;
      clog << e.what () << endl;
      this->release ();
      return false;
    }
  return false;
}

void
TCPClientConnection::run ()
{
  // TODO: verify memcpy for windows (windows has memcpy_s)
  char buf[MAX_MSG_SIZE]; // max event string size
  char msgType[4];
  char evtType[5];
  int nr;
  while (running)
    {

      memset (buf, 0, MAX_MSG_SIZE);
      memset (msgType, 0, 4);
      memset (evtType, 0, 5);
      try
        {
          nr = tcpSocket->recv (buf, MAX_MSG_SIZE);
        }
      catch (SocketException &e)
        {
          clog << e.what () << endl;
          this->release ();
        }

      if (nr > 3)
        {
          buf[nr] = '\0';
        }
      else
        {
          break;
        }

      // strncpy(msgType,buf,3);
      memcpy (msgType, buf, 3);
      msgType[3] = '\0';

      // strncpy(buf,buf+4,nr);
      memcpy (buf, buf + 4, nr);
      if ((strcmp (msgType, "EVT")) == 0)
        {
          // strncpy(evtType,buf,4);
          memcpy (evtType, buf, 4);
          evtType[4] = '\0';
          // strncpy(buf,buf+5,nr);
          memcpy (buf, buf + 5, nr);

          if ((strcmp (evtType, "ATTR")) == 0)
            {
              if (resrv != NULL)
                {
                  resrv->receiveRemoteEvent (2, 5, buf);
                }
            }
        } // end if strcmp(msgtype)

    } // end while running
}

void
TCPClientConnection::release ()
{
  running = false;
}

GINGA_MULTIDEVICE_END
