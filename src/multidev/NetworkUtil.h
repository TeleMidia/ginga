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

#ifndef NETWORK_UTIL_H
#define NETWORK_UTIL_H

GINGA_MULTIDEV_BEGIN

struct frame
{
  char *data;
  int size;
  bool repeat;
};

static G_GNUC_UNUSED const unsigned int BROADCAST_PORT = 41000;
static G_GNUC_UNUSED const int BASE_WRITE_BCAST_PORT = 51000;
static G_GNUC_UNUSED const int SECO_WRITE_BCAST_PORT = 61000;
static G_GNUC_UNUSED const string INTERFACE_NAME_A = "eth0";
static G_GNUC_UNUSED const string INTERFACE_NAME_B = "wlan0";
static G_GNUC_UNUSED const int MCAST_TTL = 31;
static G_GNUC_UNUSED const int HEADER_SIZE = 11;
static G_GNUC_UNUSED const int MAX_FRAME_SIZE = 400000;
static G_GNUC_UNUSED const int NUM_OF_COPIES = 1;

bool isValidRecvFrame (int recvFrom, int myIP, char *frame);
bool isControlFrame (int frameType);
char getFrameId (char *frame);
int getFrameType (char *frame);
unsigned int getUIntFromStream (const char *stream);
char *getStreamFromUInt (unsigned int uint);
string getStrIP (unsigned int someIp);
char *mountFrame (int sourceIp, int destDevClass, int mountFrameType,
                  unsigned int payloadSize);

GINGA_MULTIDEV_END

#endif /* NETWORK_UTIL_H */
