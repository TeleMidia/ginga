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
#include "MulticastSocketService.h"

GINGA_MULTIDEVICE_BEGIN

MulticastSocketService::MulticastSocketService (char *groupAddr,
                                                unsigned int portNumber)
{

  outputBuffer = new vector<struct frame *>;
  port = portNumber;
  groupAddress = groupAddr;

  interfaceIP = 0;

  Thread::mutexInit (&mutexBuffer, false);
  createMulticastGroup ();
}

MulticastSocketService::~MulticastSocketService ()
{
  if (readSocket != NULL)
    {
      try
        {
          readSocket->leaveGroup (groupAddress);
          readSocket->disconnect ();
          delete readSocket;
        }
      catch (SocketException &e)
        {
          clog << e.what () << endl;
        }
    }
  if (writeSocket != NULL)
    {
      try
        {
          writeSocket->disconnect ();
          delete writeSocket;
        }
      catch (SocketException &e)
        {
          clog << e.what () << endl;
        }
    }
  if (outputBuffer != NULL)
    {
      delete outputBuffer;
      outputBuffer = NULL;
    }

  Thread::mutexDestroy (&mutexBuffer);
}

int
MulticastSocketService::createMulticastGroup ()
{
  if (createSocket ())
    {
      if (setSocketOptions ())
        {
          if (tryToBind ())
            {
              if (addToGroup ())
                {
                  return 1;
                }
            }
        }
    }
  return -1;
}

bool
MulticastSocketService::createSocket ()
{
  unsigned char trueVar = 1;
  try
    {
      writeSocket = new UDPSocket ();
      readSocket = new UDPSocket ();
      readSocket->setNonBlocking (true);
    }
  catch (SocketException &e)
    {
      clog << "MulticastSocketService::createSocket()" << endl;
      clog << e.what () << endl;
      return false;
    }

  return true;
}

bool
MulticastSocketService::addToGroup ()
{
  try
    {
      readSocket->joinGroup (groupAddress);
      return true;
    }
  catch (SocketException &e)
    {
      clog << "MulticastSocketService::addToGroup" << endl;
      clog << e.what () << endl;
    }

  return true;
}

bool
MulticastSocketService::setSocketOptions ()
{
  try
    {
      writeSocket->setMulticastTTL (MCAST_TTL);
      writeSocket->setMulticastLoop (true);
      writeSocket->setReuseAddr (true);

      readSocket->setReuseAddr (true);
      readSocket->setMulticastLoop (true);
    }
  catch (SocketException &e)
    {
      clog << "MulticastSocketService::setSocketOptions()" << endl;
      clog << e.what () << endl;
      return false;
    }

  return true;
}

bool
MulticastSocketService::tryToBind ()
{
  try
    {
      readSocket->setLocalAddressAndPort (groupAddress, port);
      interfaceIP = readSocket->getLocalIPAddress ();
    }
  catch (SocketException &e)
    {
      clog << "MulticastSocketService::tryToBind" << endl;
      return false;
    }

  return true;
}

int
MulticastSocketService::getServicePort ()
{
  return port;
}

void
MulticastSocketService::dataRequest (char *data, int taskSize, bool repeat)
{

  struct frame *f;

  Thread::mutexLock (&mutexBuffer);
  f = new struct frame;
  f->data = data;
  f->size = taskSize;
  f->repeat = repeat;
  outputBuffer->push_back (f);
  Thread::mutexUnlock (&mutexBuffer);
}

bool
MulticastSocketService::sendData (struct frame *f)
{
  char *data;
  int taskSize, result, i;

  data = f->data;
  taskSize = f->size;

  clog << "MulticastSocketService::sendData Sending";
  clog << " taskSize = '" << taskSize << "'" << endl;

  for (i = 0; i < NUM_OF_COPIES; i++)
    {
      try
        {
          writeSocket->sendTo (data, taskSize, groupAddress, port);
        }
      catch (SocketException &e)
        {
          clog << "MulticastSocketService::sendData writeSocket sendTo";
          clog << " TASKSIZE = '" << taskSize << "'" << endl;
          clog << e.what () << endl;
          return false;
        }
    }

  return true;
}

bool
MulticastSocketService::checkOutputBuffer ()
{
  vector<struct frame *>::iterator i;
  bool sent = false;
  struct frame *f;

  Thread::mutexLock (&mutexBuffer);
  i = outputBuffer->begin ();
  if (i != outputBuffer->end ())
    {
      f = *i;
      sent = sendData (f);
      if (sent)
        {
          outputBuffer->erase (i);
          delete[] f->data;
          delete f;
        }
    }
  else
    {
      // clog << "MulticastSocketService::checkOutputBuffer ";
      // clog << "empty buffer" << endl;
    }
  Thread::mutexUnlock (&mutexBuffer);

  return sent;
}

bool
MulticastSocketService::checkInputBuffer (char *data, int *size)
{

  int res, recvFrom;
  string null_string;
  unsigned short null_short;

  if (readSocket == NULL)
    {
      clog << "MulticastSocketService::checkInputBuffer readSocket == NULL"
           << endl;
      return false;
    }

  res = readSocket->select_t (0, 0);
  switch (res)
    {
    case -1:
      clog << "MulticastSocketService::checkInputBuffer ";
      clog << "Warning! select ERRNO = " << errno << endl;
      memset (data, 0, MAX_FRAME_SIZE);
      return false;
    case 1:
      {
        memset (data, 0, MAX_FRAME_SIZE);
        try
          {
            *size = readSocket->recvFrom ((void *)data, MAX_FRAME_SIZE,
                                          null_string, null_short);
          }
        catch (SocketException &e)
          {
            memset (data, 0, MAX_FRAME_SIZE);
            clog << e.what () << endl;
            return false;
          }

        if (*size <= HEADER_SIZE)
          {
            clog << "MulticastSocketService::checkInputBuffer ";
            clog << "Warning! Received invalid frame: ";
            clog << "bytes received = '" << *size << "' ";
            clog << "HEADER_SIZE = '" << HEADER_SIZE << "' ";
            clog << endl;

            memset (data, 0, MAX_FRAME_SIZE);
            return false;
          }

        recvFrom = getUIntFromStream (data + 1);

        if (!isValidRecvFrame (recvFrom, interfaceIP, data))
          {
            clog << "MulticastSocketService::checkInputBuffer() "
                    "!isValidRecvFrame"
                 << endl;
            memset (data, 0, MAX_FRAME_SIZE);
            return false;
          }
        break;
      }

    default:
      memset (data, 0, MAX_FRAME_SIZE);
      return false;
    }
  return true;
}

GINGA_MULTIDEVICE_END
