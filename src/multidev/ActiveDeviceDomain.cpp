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
#include "ActiveDeviceDomain.h"
#include "ActiveDeviceService.h"
#include "MulticastSocketService.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_MULTIDEV_BEGIN

ActiveDeviceDomain::ActiveDeviceDomain (bool useMulticast, int srvPort)
    : DeviceDomain (useMulticast, srvPort)
{
  clog << "ActiveDeviceDomain::ActiveDeviceDomain()" << endl;
  deviceClass = CT_ACTIVE;
  deviceService = NULL;
}

ActiveDeviceDomain::~ActiveDeviceDomain () {}

void
ActiveDeviceDomain::postConnectionRequestTask (int w, int h)
{
  char *task;
  int connReqPayloadSize = 7;
  int taskSize;

  // TODO: offer configuration parameters during requests connection to
  // CT_ACTIVE devs

  // clog << "ActiveDeviceDomain::postConnectionRequestTask";
  // clog << " myIP = " << myIP << endl;
  // clog << endl;

  // prepare frame
  task = mountFrame (myIP, CT_BASE, FT_CONNECTIONREQUEST,
                     connReqPayloadSize);

  task[HEADER_SIZE] = deviceClass;

  task[HEADER_SIZE + 1] = w & 0xFF;
  task[HEADER_SIZE + 2] = (w & 0xFF00) >> 8;

  task[HEADER_SIZE + 3] = h & 0xFF;
  task[HEADER_SIZE + 4] = (h & 0xFF00) >> 8;

  task[HEADER_SIZE + 5] = servicePort & 0xFF;
  task[HEADER_SIZE + 6] = (servicePort & 0xFF00) >> 8;

  taskSize = HEADER_SIZE + connReqPayloadSize;
  broadcastTaskRequest (task, taskSize);
}

void
ActiveDeviceDomain::receiveAnswerTask (arg_unused (char *task))
{
  if (connected)
    {
      clog << "ActiveDeviceDomain::receiveAnswerTask Warning! ";
      clog << "received an answer task in connected state" << endl;
    }

  clog << "ActiveDeviceDomain::receiveAnswerTask Connected with ";
  clog << "base multi-device domain" << endl;
  connected = true;
}

bool
ActiveDeviceDomain::receiveMediaContentTask (arg_unused (char *task))
{
  clog << "ActiveDeviceDomain::receiveMediaContentTask" << endl;
  return false;
}

bool
ActiveDeviceDomain::runControlTask ()
{
  char *task;

  if (taskIndicationFlag)
    {
      task = taskReceive ();
      if (task == NULL)
        {
          taskIndicationFlag = false;
          clog << "ActiveDeviceDomain::runControlTask Warning! ";
          clog << "received a NULL task" << endl;
          return false;
        }

      if (myIP == sourceIp)
        {
          /*
          clog << "ActiveDeviceDomain::runControlTask got my own task ";
          clog << "(size = '" << frameSize << "')" << endl;*/

          delete[] task;
          taskIndicationFlag = false;
          return false;
        }

      if (destClass != deviceClass)
        {
          clog << "ActiveDeviceDomain::runControlTask Task isn't for me!";
          clog << endl;

          delete[] task;
          taskIndicationFlag = false;
          return false;
        }

      if (frameSize + HEADER_SIZE != (unsigned int) bytesRecv)
        {
          delete[] task;
          taskIndicationFlag = false;
          clog << "ActiveDeviceDomain::runControlTask Warning! ";
          clog << "received a wrong size frame '" << frameSize;
          clog << "' bytes received '" << bytesRecv << "'" << endl;
          return false;
        }
      // clog << "ActiveDeviceDomain::runControlTask frame type '";
      // clog << frameType << "'" << endl;

      switch (frameType)
        {
        case FT_ANSWERTOREQUEST:
          if (frameSize != 11)
            {
              clog << "ActiveDeviceDomain::runControlTask Warning!";
              clog << "received an answer to connection request with";
              clog << " wrong size: '" << frameSize << "'" << endl;
              delete[] task;
              taskIndicationFlag = false;
              return false;
            }
          else
            {
              receiveAnswerTask (task);
            }
          break;

        case FT_KEEPALIVE:
          clog << "ActiveDeviceDomain::runControlTask KEEPALIVE";
          clog << endl;
          break;

        default:
          clog << "ActiveDeviceDomain::runControlTask WHAT? FT '";
          clog << frameType << "'" << endl;
          delete[] task;
          taskIndicationFlag = false;
          return false;
        }

      delete[] task;
    }
  else
    {
      clog << "ActiveDeviceDomain::runControlTask Warning! ";
      clog << "task indication flag is false" << endl;
    }

  taskIndicationFlag = false;
  return true;
}

void
ActiveDeviceDomain::checkDomainTasks ()
{
  DeviceDomain::checkDomainTasks ();
}

GINGA_MULTIDEV_END
