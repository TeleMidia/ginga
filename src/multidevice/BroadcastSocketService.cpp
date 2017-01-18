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
#include "BroadcastSocketService.h"

#ifndef _MSC_VER
#endif

#ifdef _MSC_VER
typedef int socklen_t;
#endif

#include "system/SystemCompat.h"
#include "system/PracticalSocket.h"
using namespace ::ginga::system;

GINGA_MULTIDEVICE_BEGIN

static UDPSocket *udpSocket;

BroadcastSocketService::BroadcastSocketService ()
{
  interfaceIP = 0;
  broadcastIPAddr = "0.0.0.0";
  outputBuffer = new vector<struct frame *>;
  udpSocket = NULL;

  pthread_mutex_init (&mutexBuffer, NULL);

  if (buildDomainAddress ())
    {
      try
        {
          broadcastIPAddr = udpSocket->getBroadcastAddress ();
          interfaceIP = udpSocket->getLocalIPAddress ();
          clog << endl << "broadcastIPAddr= " << broadcastIPAddr << endl;
          clog << "interfaceIP= " << interfaceIP << endl;
          // udpSocket->setLocalAddressAndPort(broadcastIPAddr,port);
        }
      catch (SocketException &e)
        {
          clog << e.what () << endl;
          clog << " BroadcastSocketService::getBroadcastAddress() error! "
               << endl;
        }
    }
  else
    {
      try
        {
          // TODO: configure port
          udpSocket = new UDPSocket (42088);
          udpSocket->setNonBlocking (true);
          clog << "BroadcastSocketService: using UDP port 42088 for device "
                  "search"
               << endl;
          broadcastIPAddr = udpSocket->getBroadcastAddress ();
          interfaceIP = udpSocket->getLocalIPAddress ();
          clog << endl << "broadcastIPAddr= " << broadcastIPAddr << endl;
          clog << "interfaceIP= " << interfaceIP << endl;
        }
      catch (SocketException &e)
        {
          clog << e.what () << endl;
          clog << " BroadcastSocketService::getLocalIPAddress() error! "
               << endl;
        }
    }
}

BroadcastSocketService::~BroadcastSocketService ()
{
  if (udpSocket != NULL)
    {
      udpSocket->disconnect ();
      // udpSocket->cleanUp();
      delete udpSocket;
    }

  if (outputBuffer != NULL)
    {
      delete outputBuffer;
      outputBuffer = NULL;
    }
  Thread::mutexDestroy (&mutexBuffer);
}

bool
BroadcastSocketService::buildDomainAddress ()
{
  try
    {
      udpSocket = new UDPSocket ();
      udpSocket->setReuseAddr (true);
      udpSocket->setLocalPort (port);
      udpSocket->setNonBlocking (true);
      return true;
    }
  catch (SocketException &e)
    {
      clog << e.what () << endl;
      clog << "BroadcastSocketService::buildClientAddress Warning!" << endl;
      return false;
    }
  return true;
}

unsigned int
BroadcastSocketService::discoverBroadcastAddress ()
{
  clog << "BroadcastSocketService::discoverBroadcastAddress Warning!";
  clog << " can't discover broadcast address" << endl;
  return 0;
}

unsigned int
BroadcastSocketService::getInterfaceIPAddress ()
{
  return interfaceIP;
}

int
BroadcastSocketService::getServicePort ()
{
  return port;
}

void
BroadcastSocketService::dataRequest (char *data, int taskSize, bool repeat)
{

  struct frame *f;
  pthread_mutex_lock (&mutexBuffer);
  f = new struct frame;
  f->data = data;
  f->size = taskSize;
  f->repeat = repeat;
  outputBuffer->push_back (f);
  pthread_mutex_unlock (&mutexBuffer);
}

bool
BroadcastSocketService::sendData (struct frame *f)
{

  char *data;
  int taskSize, result, i;

  data = f->data;
  taskSize = f->size;

  // clog << "BroadcastSocketService::sendData Sending to "<< broadcastIPAddr;
  // clog << " taskSize = " << taskSize << endl; //<< "' and headerSize = '";
  // clog << headerSize << "'" << endl;

  try
    {
      for (i = 0; i < NUM_OF_COPIES; i++)
        {
          udpSocket->sendTo (data, taskSize, broadcastIPAddr, port);
        }
    }
  catch (SocketException &e)
    {
      clog << e.what () << endl;
      clog << "BroadcastSocketService::sendData Error!!";
      return false;
    }
  return true;
}

bool
BroadcastSocketService::checkOutputBuffer ()
{
  vector<struct frame *>::iterator i;
  bool sent = false;
  struct frame *f = NULL;
  // clog << "=========== checkOutputBuffer ==========="<<endl;
  pthread_mutex_lock (&mutexBuffer);
  i = outputBuffer->begin ();
  if (i != outputBuffer->end ())
    {
      f = *i;
      sent = sendData (f);
      if (sent)
        {
          outputBuffer->erase (i);
          delete[] f->data;
          f->data = NULL;
          delete f;
          f = NULL;
        }
    }
  else
    {
      /*clog << "BroadcastSocketService::checkOutputBuffer ";
      clog << "empty buffer" << endl;*/
    }
  pthread_mutex_unlock (&mutexBuffer);

  return sent;
}

bool
BroadcastSocketService::checkInputBuffer (char *data, int *size)
{
  int res = 0;
  int recvFrom = 0;
  int sz = 0;
  char recvString[MAX_FRAME_SIZE + 1];
  string sourceAddress = "";
  unsigned short sourcePort = 0;
  // TODO: checkInput loop fix

  if (udpSocket == NULL)
    {

      return false;
    }

  res = udpSocket->select_t (0, 0);

  // clog << "=========== checkInputBuffer ============"<<endl;
  switch (res)
    {
    case -1:
      clog << "BroadcastSocketService::checkInputBuffer ERROR res=-1" << endl;
      // clog << "Warning! select ERRNO = " << errno << endl;
      memset (data, 0, MAX_FRAME_SIZE);
      return false;

    case 1:
      {
        clog << "BroadcastSocketService::checkInputBuffer ";
        clog << "receiving data ..." << endl;

        memset (data, 0, MAX_FRAME_SIZE);
        memset (recvString, 0, MAX_FRAME_SIZE);
        try
          {
            sz = udpSocket->recvFrom (recvString, MAX_FRAME_SIZE,
                                      sourceAddress, sourcePort);

            if (sz > 0)
              {
                recvString[sz] = '\0';
                clog << "BroadcastSocketService::udpSocket->recvFrom "
                     << sourceAddress;
                clog << " " << sz << " bytes" << endl;
              }
            else
              {
                return false;
              }
            // if (sourceAddress == interfaceIP)
          }
        catch (SocketException &e)
          {
            clog << e.what () << endl;
            memset (data, 0, MAX_FRAME_SIZE);
            memset (recvString, 0, MAX_FRAME_SIZE);
            return false;
          }
        if (sz <= HEADER_SIZE)
          {
            clog << "BroadcastSocketService::checkInputBuffer ";
            clog << "Warning! Received invalid frame: ";
            clog << "bytes received = '" << sz << "' ";
            clog << "HEADER_SIZE = '" << HEADER_SIZE << "' ";
            clog << endl;
            memset (data, 0, MAX_FRAME_SIZE);
            memset (recvString, 0, MAX_FRAME_SIZE);
            return false;
          }

        *size = sz;
        memcpy (data, recvString, (int)(*size));

        recvFrom = getUIntFromStream (data + 1);

        if (!isValidRecvFrame (recvFrom, interfaceIP, data))
          {
            clog << "BroadcastSocketService::checkInputBuffer if "
                    "(!isValidRecvFrame())"
                 << endl;
            memset (data, 0, MAX_FRAME_SIZE);
            memset (recvString, 0, MAX_FRAME_SIZE);

            return false;
          }
        else
          {
            clog << "BroadcastSocketService::checkInputBuffer if "
                    "(isValidRecvFrame())"
                 << endl;
            memset (recvString, 0, MAX_FRAME_SIZE);
          }
        break;
      }

    default:
      memset (data, 0, MAX_FRAME_SIZE);
      memset (recvString, 0, MAX_FRAME_SIZE);
      return false;
    }
  return true;
}

GINGA_MULTIDEVICE_END
