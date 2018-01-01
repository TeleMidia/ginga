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
#include "DsmccMessageProcessor.h"

#ifndef DII_MESSAGE
#define DII_MESSAGE 0x1002
#define DDB_MESSAGE 0x1003
#define DSI_MESSAGE 0x1006
#endif

GINGA_DATAPROC_BEGIN

DsmccMessageProcessor::DsmccMessageProcessor (unsigned short pid)
{
  sd = NULL;
  dii = NULL;
  dsi = NULL;
  this->pid = pid;
  Thread::mutexInit (&msgMutex, true);
}

DsmccMessageProcessor::~DsmccMessageProcessor ()
{
  if (sd != NULL)
    {
      delete sd;
      sd = NULL;
    }

  if (dii != NULL)
    {
      delete dii;
      dii = NULL;
    }

  if (dsi != NULL)
    {
      delete dsi;
      dsi = NULL;
    }

  Thread::mutexLock (&msgMutex);
  vector<DsmccMessageHeader *>::iterator it;
  it = msgs.begin ();
  while (it != msgs.end ())
    {
      delete *it;
      ++it;
    }
  Thread::mutexUnlock (&msgMutex);
  Thread::mutexDestroy (&msgMutex);
}

DsmccServiceDomain *
DsmccMessageProcessor::pushMessage (DsmccMessageHeader *message)
{
  unsigned int messageId;

  if (message == NULL)
    {
      clog << "DsmccMessageProcessor::pushMessage " << endl;
      clog << "Warning! Try to push NULL message" << endl;
    }
  else
    {
      messageId = message->getMessageId ();
      if (messageId == DSI_MESSAGE)
        {
          return processDSIMessage (message);
        }
      else if (messageId == DII_MESSAGE)
        {
          return processDIIMessage (message);
        }
      else if (messageId == DDB_MESSAGE)
        {
          Thread::mutexLock (&msgMutex);
          msgs.push_back (message);
          Thread::mutexUnlock (&msgMutex);
        }
    }

  processDDBMessages ();
  return NULL;
}

DsmccServiceDomain *
DsmccMessageProcessor::processDSIMessage (DsmccMessageHeader *message)
{
  if (dsi == NULL)
    {
      clog << "Message Processor dsi done!" << endl;
      dsi = new DsmccDownloadServerInitiate ();
      if (dsi->processMessage (message) < 0)
        {
          delete dsi;
          dsi = NULL;
          return NULL;
        }
      if (dii != NULL && sd == NULL)
        {
          clog << "Creating SD" << endl;
          sd = new DsmccServiceDomain (dsi, dii, pid);
          return sd;
        }
    }
  else
    {
      // TODO: received another DSI (is it an OC update)
    }
  return NULL;
}

DsmccServiceDomain *
DsmccMessageProcessor::processDIIMessage (DsmccMessageHeader *message)
{
  if (dii == NULL)
    {
      dii = new DsmccDownloadInfoIndication ();
      if (dii->processMessage (message) < 0)
        {
          delete dii;
          dii = NULL;
          return NULL;
        }
      /*
       * TODO: start process all DII file?
       * or has every DII file less then 4066
       * bytes?
       */
      clog << "Message Processor dii done!" << endl;
      if (dsi != NULL && sd == NULL)
        {
          clog << "Creating SD" << endl;
          sd = new DsmccServiceDomain (dsi, dii, pid);

          return sd;
        }
    }
  else
    {
      // TODO: received another DII (is it an OC update)
    }
  return NULL;
}

void
DsmccMessageProcessor::processDDBMessages ()
{
  DsmccMessageHeader *msg;
  DsmccDownloadDataBlock *ddb;

  if (sd != NULL)
    {
      Thread::mutexLock (&msgMutex);
      while (!msgs.empty ())
        {
          msg = *(msgs.begin ());
          ddb = new DsmccDownloadDataBlock (msg);
          if (sd->receiveDDB (ddb) < 0)
            {
              clog << "DsmccMessageProcessor::processDDBMessages - error"
                   << endl;
            }
          delete ddb;

          msgs.erase (msgs.begin ());
        }
      Thread::mutexUnlock (&msgMutex);
    }
}

DsmccServiceDomain *
DsmccMessageProcessor::getServiceDomain ()
{
  return sd;
}

void
DsmccMessageProcessor::checkTasks ()
{
  processDDBMessages ();
}

GINGA_DATAPROC_END
