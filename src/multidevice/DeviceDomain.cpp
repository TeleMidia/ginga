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
#include "BaseDeviceDomain.h"
#include "ActiveDeviceService.h"
#include "PassiveDeviceService.h"
#include "RemoteEventService.h"

#include "util/functions.h"
using namespace ::ginga::util;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#ifndef _MSC_VER
#endif

GINGA_MULTIDEVICE_BEGIN

char DeviceDomain::headerStream[HEADER_SIZE];
char *DeviceDomain::mdFrame = new char[MAX_FRAME_SIZE];
unsigned int DeviceDomain::myIP = 0;
bool DeviceDomain::taskIndicationFlag = false;

DeviceDomain::DeviceDomain (bool useMulticast, int srvPort)
{
  deviceClass = -1;
  deviceWidth = -1;
  deviceHeight = -1;
  schedulePost = -1;
  schedDevClass = -1;
  sentTimeStamp = -1;
  receivedTimeStamp = -1;
  deviceService = NULL;
  newAnswerPosted = false;
  connected = false;
  enableMulticast = useMulticast;
  servicePort = srvPort;

  clearHeader ();

  broadcastService = new BroadcastSocketService ();
  myIP = broadcastService->getInterfaceIPAddress ();

  /*
   * TODO: change remote event service constructor in order to
   *       accomplish the addition of the device classes based on
   *       a configuration file
   */
  RemoteEventService *nres = new RemoteEventService ();
  nres->addDeviceClass (1);
  nres->addDeviceClass (2);
  res = nres;
}

DeviceDomain::~DeviceDomain ()
{
  if (deviceService != NULL)
    {
      delete deviceService;
      deviceService = NULL;
    }

  if (broadcastService != NULL)
    {
      delete broadcastService;
      broadcastService = NULL;
    }
}

bool
DeviceDomain::isConnected ()
{
  return connected;
}

void
DeviceDomain::clearHeader ()
{
  sourceIp = 0;
  destClass = -1;
  frameType = -1;
  frameSize = 0;
}

bool
DeviceDomain::broadcastTaskRequest (char *data, int taskSize)
{
  //	if (deviceSearch)
  broadcastService->dataRequest (data, taskSize);
  return true;
}

char *
DeviceDomain::taskReceive ()
{
  char *data = NULL;

  memset (headerStream, 0, HEADER_SIZE);
  memcpy (headerStream, mdFrame, HEADER_SIZE);
  parseTaskHeader ();
  printTaskHeader ();

  if (frameSize == 0)
    {
      memset (headerStream, 0, HEADER_SIZE);
      if (frameType != FT_KEEPALIVE)
        {
          clog << "DeviceDomain::taskReceive Warning! ";
          clog << " empty payload in a non keep alive frame" << endl;
        }
      else
        {
          clog << "DeviceDomain::taskReceive Keep Alive! ";
          clog << endl;
        }
      return NULL;
    }

  try
    {
      data = new char[frameSize];
    }
  catch (bad_alloc &e)
    {
      clog << "DeviceDomain::taskReceive Warning! ";
      clog << "can't alloc '" << frameSize << "' bytes." << endl;
      return NULL;
    }

  memset (data, 0, frameSize);
  memcpy (data, mdFrame + HEADER_SIZE, frameSize);

  return data;
}

void
DeviceDomain::parseTaskHeader ()
{
  clearHeader ();

  sourceIp = getUIntFromStream (headerStream + 1);
  destClass = (((unsigned char)headerStream[5]) & 0xFF);
  frameType = (((unsigned char)headerStream[6]) & 0xFF);
  frameSize = getUIntFromStream (headerStream + 7);
}

void
DeviceDomain::printTaskHeader ()
{
  clog << "FrameId = '";
  clog << (int)(unsigned char)headerStream[0];
  clog << "' SourceIp = '" << sourceIp << "', which means '";
  clog << getStrIP (sourceIp);
  clog << "', destClase = '" << destClass << "' header[5] = '";
  clog << (int)(unsigned char)headerStream[5];
  clog << "', frameType = '" << frameType << "' header[6] = '";
  clog << (int)(unsigned char)headerStream[6];
  clog << "', frameSize = '" << frameSize << "' header[7] = '";
  clog << (int)(unsigned char)headerStream[7];
  clog << "', header[8] = '";
  clog << (int)(unsigned char)headerStream[8];
  clog << "' header[9] = '";
  clog << (int)(unsigned char)headerStream[9];
  clog << "', header[10] = '";
  clog << (int)(unsigned char)headerStream[10];
  clog << "'" << endl;
}

bool
DeviceDomain::addDevice (int reqDeviceClass, int width, int height,
                         int srvPort)
{

  bool added = false;

  if (reqDeviceClass == 2)
    {
      clog << "DeviceDomain::addDevice adding new device - class 2...";
      clog << endl;

      ((RemoteEventService *)res)
          ->addDevice (reqDeviceClass, (sourceIp + srvPort),
                       (char *)getStrIP (sourceIp).c_str (), srvPort,
                       (sourceIp == myIP));
    }

  if (deviceService != NULL)
    {
      added = deviceService->addDevice (sourceIp, reqDeviceClass, width,
                                        height);
    }

  return added;
}

void
DeviceDomain::postConnectionRequestTask ()
{
  postConnectionRequestTask (deviceWidth, deviceHeight);
}

void
DeviceDomain::postEventTask (int destDevClass, int frameType, char *payload,
                             int payloadSize)
{

  char *task;
  string _doc;
  int taskSize;

  // prepare frame
  if (destDevClass == DeviceDomain::CT_ACTIVE)
    {
      if (frameType == DeviceDomain::FT_PRESENTATIONEVENT)
        {
          if (strstr (payload, "start::") != NULL)
            {
              _doc.assign (payload + 7, payloadSize - 7);
              clog << "DeviceDomain::postEventTask calling ";
              clog << "startDocument with doc = '";
              clog << _doc << "'" << endl;
              ((RemoteEventService *)res)
                  ->startDocument (2, (char *)(_doc.c_str ()));
            }
          else if (strstr (payload, "stop::") != NULL)
            {
              clog << "DeviceDomain::postEventTask calling ";
              clog << "stopDocument" << endl;

              _doc.assign (payload + 6, payloadSize - 6);
              ((RemoteEventService *)res)
                  ->stopDocument (2, (char *)(_doc.c_str ()));
            }
        }
    }
  else
    {
      if (frameType == DeviceDomain::FT_SELECTIONEVENT)
        {
          clog
              << "DeviceDomain frameType == DeviceDomain::FT_SELECTIONEVENT"
              << endl;
        }
      task = mountFrame (myIP, destDevClass, frameType, payloadSize);

      memcpy (task + HEADER_SIZE, payload, payloadSize);

      taskSize = HEADER_SIZE + payloadSize;
      taskRequest (destDevClass, task, taskSize);
    }
}

void
DeviceDomain::setDeviceInfo (int width, int height,
                             string base_device_ncl_path)
{
  this->deviceWidth = width;
  this->deviceHeight = height;
  ((RemoteEventService *)this->res)
      ->setBaseDeviceNCLPath (base_device_ncl_path);
}

int
DeviceDomain::getDeviceClass ()
{
  return deviceClass;
}

void
DeviceDomain::checkDomainTasks ()
{
  int tmpClass, res;
  double receivedElapsedTime;

  if (deviceClass < 0)
    {
      clog << "DeviceDomain::checkDomainTasks ";
      clog << "Warning! deviceClass = " << deviceClass << endl;
      return;
    }

  if (schedulePost >= 0)
    {
      tmpClass = schedDevClass;
      res = schedulePost;

      schedulePost = -1; // Modificado por Roberto
      schedDevClass = -1;

      switch (res)
        {
        case FT_ANSWERTOREQUEST:
          postAnswerTask (tmpClass, true);
          newAnswerPosted = true;
          break;

        default:
          clog << "DeviceDomain::checkDomainTasks RES = '";
          clog << res << "'" << endl;
          break;
        }
    }

  if ((!taskIndicationFlag))
    {
      if (broadcastService->checkInputBuffer (mdFrame, &bytesRecv))
        {
          taskIndicationFlag = true;
          if (runControlTask ())
            {
              receivedTimeStamp = getCurrentTimeMillis ();
            }
        }

    } /*else if (deviceSearch) {
            clog << "DeviceDomain::checkDomainTasks can't process input ";
            clog << "buffer: task indication flag is true" << endl;
    }*/

  // if (deviceSearch)
  broadcastService->checkOutputBuffer ();
}

void
DeviceDomain::addDeviceListener (IRemoteDeviceListener *listener)
{
  if (deviceService != NULL)
    {
      deviceService->addListener (listener);
    }
}

void
DeviceDomain::removeDeviceListener (IRemoteDeviceListener *listener)
{
  if (deviceService != NULL)
    {
      deviceService->removeListener (listener);
    }
}

GINGA_MULTIDEVICE_END
