/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
#include "PipeFilter.h"

#include "Pat.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_TSPARSER_BEGIN

PipeFilter::PipeFilter (unsigned int pid) : Thread ()
{
  this->pid = pid;
  this->dataReceived = false;
  this->packetsReceived = 0;
  debugBytesWritten = 0;

  this->srcIsAPipe = false;
  this->srcUri = "";
  this->dstUri = "";
  this->dstPipeCreated = false;

  this->running = false;

  this->pids.clear ();
  clog << "PipeFilter::PipeFilter all done" << endl;
}

PipeFilter::~PipeFilter () { pids.clear (); }

void
PipeFilter::addPid (int pid)
{
  clog << "PipeFilter::addPid '" << pid << "'" << endl;
  pids[pid] = 0;
}

bool
PipeFilter::hasData ()
{
  return dataReceived;
}

void
PipeFilter::receiveTSPacket (ITSPacket *pack)
{
  int ppid;
  int contCounter;
  int bytesWritten = 0;
  char *packData;

  // memset(packData, 0, ITSPacket::TS_PACKET_SIZE);
  ppid = pack->getPid ();

  if (!pids.empty ())
    {
      if (pids.count (ppid) == 0)
        {
          return;
        }

      contCounter = pids[ppid];
      pack->setContinuityCounter (contCounter);
      if (pack->getAdaptationFieldControl () != 2
          && pack->getAdaptationFieldControl () != 0)
        {
          if (contCounter == 15)
            {
              contCounter = -1;
            }
          pids[ppid] = contCounter + 1;
        }
    }

  pack->getPacketData (&packData);

  dataReceived = true;

  if (!dstPipeCreated)
    {
      if (!running && dstUri != "")
        {
          Thread::startThread ();
        }

      while (!dstPipeCreated)
        {
          g_usleep (10000);
        }
    }

  bytesWritten = SystemCompat::writePipe (dstPd, packData,
                                          ITSPacket::TS_PACKET_SIZE);

  assert (bytesWritten == ITSPacket::TS_PACKET_SIZE);
}

void
PipeFilter::receiveSection (unused (char *buf), unused (int len), unused (IFrontendFilter *filter))
{
}

void
PipeFilter::receivePes (unused (char *buf), unused (int len), unused (IFrontendFilter *filter))
{
}

void
PipeFilter::setSourceUri (const string &srcUri, bool isPipe)
{
  this->srcUri = srcUri;
  this->srcIsAPipe = isPipe;
}

string
PipeFilter::setDestinationUri (const string &dstUri)
{
  this->dstUri = SystemCompat::checkPipeName (dstUri);

  if (!running)
    {
      Thread::startThread ();
    }

  return this->dstUri;
}

void
PipeFilter::run ()
{
  int rval;
  int buffSize = 188 * 1024;
  char *buff = new char[buffSize];
  string cmd;

  running = true;
  clog << "PipeFilter::run(" << this << ")" << endl;

  if (!SystemCompat::createPipe (dstUri, &dstPd))
    {
      clog << "PipeFilter::run(" << this << ")";
      clog << " can't create '" << dstUri;
      clog << "'" << endl;

      running = false;
      delete[] buff;
      return;
    }

  dstPipeCreated = true;

  clog << "PipeFilter::run(" << this << ") pipe '";
  clog << dstUri << "' created" << endl;

  if (srcIsAPipe)
    {
      clog << "PipeFilter::run(" << this << ") reader" << endl;

      if (!SystemCompat::openPipe (srcUri, &srcPd))
        {
          clog << "PipeFilter::run(" << this << ")";
          clog << " can't open '" << srcUri;
          clog << "'" << endl;

          delete[] buff;
          return;
        }

      clog << "PipeFilter::run(" << this << ") '" << srcUri;
      clog << "' OPENED" << endl;

      while (srcIsAPipe)
        {
          rval = SystemCompat::readPipe (srcPd, buff, buffSize);
          if (rval > 0)
            {
              SystemCompat::writePipe (dstPd, buff, rval);
            }
        }

      clog << "PipeFilter::run(" << this << ") reader all done!" << endl;
    }

  running = false;
  delete[] buff;
}

GINGA_TSPARSER_END
