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
#include "SectionFilter.h"

#include "isdbt-tsparser/TransportSection.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_DATAPROC_BEGIN

SectionFilter::SectionFilter ()
{
  this->listener = NULL;
  this->lastPid = -1;

  Thread::mutexInit (&stlMutex, true);
}

SectionFilter::~SectionFilter ()
{
  map<int, ITransportSection *>::iterator i;
  map<unsigned int, SectionHandler *>::iterator j;

  clog << "SectionFilter::~SectionFilter" << endl;
  Thread::mutexLock (&stlMutex);

  processedSections.clear ();

  i = hFilteredSections.begin ();
  while (i != hFilteredSections.end ())
    {
      delete i->second;
      ++i;
    }
  hFilteredSections.clear ();

  /*Clear the allocated structs */
  j = sectionPidSelector.begin ();
  while (j != sectionPidSelector.end ())
    {
      delete j->second->section;
      delete j->second;
      ++j;
    }
  sectionPidSelector.clear ();

  Thread::mutexUnlock (&stlMutex);
  Thread::mutexDestroy (&stlMutex);
}

void
SectionFilter::setListener (IFilterListener *listener)
{
  this->listener = listener;
}

// Set manipulation functions.

bool
SectionFilter::checkProcessedSections (const string &sectionName)
{
  bool checked;

  Thread::mutexLock (&stlMutex);
  if (processedSections.count (sectionName) == 0)
    {
      checked = false;
    }
  else
    {
      checked = true;
    }
  Thread::mutexUnlock (&stlMutex);

  return checked;
}

void
SectionFilter::addProcessedSection (const string &sectionName)
{
  Thread::mutexLock (&stlMutex);
  processedSections.insert (sectionName);
  Thread::mutexUnlock (&stlMutex);
}

void
SectionFilter::removeProcessedSection (const string &sectionName)
{
  set<string>::iterator i;

  Thread::mutexLock (&stlMutex);
  i = processedSections.find (sectionName);
  if (i != processedSections.end ())
    {
      processedSections.erase (i);
    }
  Thread::mutexUnlock (&stlMutex);
}

void
SectionFilter::clearProcessedSections ()
{
  Thread::mutexLock (&stlMutex);
  processedSections.clear ();
  Thread::mutexUnlock (&stlMutex);
}

bool
SectionFilter::checkSectionVersion (ITransportSection *section)
{
  unsigned int tableId;
  string sectionName;

  tableId = section->getTableId ();
  if (section->getVersionNumber () > 0)
    {
      if (tableId == 0x42 || tableId == 0x4E || tableId == 0xC8)
        {
          xstrassign (sectionName, "%s/ginga/epg/data/%u%u%u%u",
                      g_get_tmp_dir (),
                      section->getESId (),
                      section->getTableId (),
                      section->getExtensionId (),
                      section->getVersionNumber () - 1);
        }
      else
        {
          xstrassign (sectionName,
                      "%s/ginga/carousel/modules/%u%u%d%u%u",
                      g_get_tmp_dir (),
                      section->getESId (),
                      section->getTableId (),
                      ((TransportSection *)section)->getSection ()[11],
                      section->getExtensionId (),
                      section->getVersionNumber () - 1);
        }

      if (checkProcessedSections (sectionName))
        {
          // there is an old version
          // TODO: do something
          return true;
        }
    }
  return false;
}

// Delivers consolidated section packets to listener.  Invalid
// packets are ignored.
void
SectionFilter::receiveTSPacket (ITSPacket *pack)
{
  unsigned int counter;
  unsigned int packPID;
  int last;
  SectionHandler *handler;
  packPID = pack->getPid ();
  handler = getSectionHandler (packPID);
  if (handler == NULL)
    {
      handler = new SectionHandler;
      handler->section = NULL;

      resetHandler (handler);
      sectionPidSelector[packPID] = handler;
    }

  /*if (pack->getStartIndicator()) {
          clog << "SectionFilter::receiveTSPacket number '" << recvPack;
          clog << "'" << endl;
  }*/
  handler->recvPack++;

  counter = pack->getContinuityCounter ();
  last = handler->lastContinuityCounter;

  // updates the Continuity
  handler->lastContinuityCounter = counter;

  // Check if the TS has a beginning of a new section.
  if (pack->getStartIndicator ())
    {
      // Consolidates previous section.
      if (handler != NULL)
        {
          /*clog << "SectionFilter::receiveTSPacket ";
          clog << "Consolidates previous section.";
          clog << endl;*/
          verifyAndAddData (pack, true);
        }

      // Create a new section.

      if (!verifyAndCreateSection (pack))
        {
          /*clog << "SectionFilter::receiveTSPacket - Failed to create ";
          clog << "Section, perhaps header is not complete yet!" << endl;
          */
        }

      // Continuation of a previous section.
    }
  else
    {
      if (handler->section == NULL)
        {
          /* Tries to create a continuation of section
           * which the header was not ready at the moment */
          if (!verifyAndCreateSection (pack))
            {
              /*clog << "SectionFilter::receiveTSPacket - Receive a ";
              clog << "continuation but failed to create ";
              clog << "Section, perhaps header is not complete yet!";
              clog << endl;*/
            }
        }
      else
        {
          bool isValidCounter = false;

          if (pack->getAdaptationFieldControl () == 2
              || pack->getAdaptationFieldControl () == 0)
            {
              if (last == (int) counter)
                {
                  isValidCounter = true;
                }
            }
          else if ((last == 15 && counter == 0) || (counter - last == 1))
            {
              isValidCounter = true;
            }

          if (isValidCounter)
            {
              /* The section is OK */
              verifyAndAddData (pack, false);
            }
          else
            { // Discontinuity, ignore section.
              clog << "SectionFilter::receiveTSPacket: ";
              clog << "Discontinuity, last = '" << last << "'";
              clog << " counter = '" << counter << "'";
              clog << " ignoring section.";
              clog << endl;

              ignore (pack->getPid ());
            }
        }
    }
}

void
SectionFilter::receiveSection (char *buf, int len, IFrontendFilter *filter)
{
  // clog << "SectionFilter::receiveSection '" << len << "'" << endl;
  ITransportSection *filteredSection = NULL;
  map<int, ITransportSection *>::iterator i;
  int pid;

  if (listener == NULL)
    {
      clog << "SectionFilter::receiveSection Warning!";
      clog << " NULL listener" << endl;
      return;
    }

  pid = filter->getPid ();
  lastPid = pid;

  i = hFilteredSections.find (pid);
  if (i == hFilteredSections.end ())
    {
      filteredSection = new TransportSection (buf, len);
      filteredSection->setESId (pid);
      hFilteredSections[pid] = filteredSection;
      i = hFilteredSections.find (pid);
    }
  else
    {
      filteredSection = i->second;
      filteredSection->addData (buf, len);
    }

  if (filteredSection->isConsolidated ())
    {
      hFilteredSections.erase (i);

      if (!checkProcessedSections (filteredSection->getSectionName ()))
        {
          listener->receiveSection (filteredSection);
        }
      else
        {
          delete filteredSection;
        }

      filteredSection = NULL;
    }
}

void
SectionFilter::receivePes (unused (char *buf), unused (int len), unused (IFrontendFilter *filter))
{
}

SectionHandler *
SectionFilter::getSectionHandler (unsigned int pid)
{
  if (sectionPidSelector.count (pid) != 0)
    {
      return sectionPidSelector[pid];
    }

  return NULL;
}

// Process and frees SECTION.
void
SectionFilter::process (unused (ITransportSection *section), unsigned int pid)
{
  SectionHandler *handler;

  handler = getSectionHandler (pid);
  assert (handler != NULL);
  assert (handler->section != NULL);

  if (listener != NULL)
    {
      if (!checkProcessedSections (handler->section->getSectionName ()))
        {
          listener->receiveSection (handler->section);
          handler->section = NULL;
        }
    }

  resetHandler (handler);
}

void
SectionFilter::resetHandler (SectionHandler *handler)
{
  if (handler->section != NULL)
    {
      delete handler->section;
    }

  handler->section = NULL;
  handler->lastContinuityCounter = -1;
  handler->headerSize = 0;
  handler->recvPack = 0;

  memset (handler->sectionHeader, 0, G_N_ELEMENTS (handler->sectionHeader));
}

// Ignore SECTION.
void
SectionFilter::ignore (unsigned int pid)
{
  map<unsigned int, SectionHandler *>::iterator i;

  i = sectionPidSelector.find (pid);
  if (i != sectionPidSelector.end ())
    {
      resetHandler (i->second);
    }
}

void
SectionFilter::verifyAndAddData (ITSPacket *pack, bool lastPacket)
{
  char data[184];
  SectionHandler *handler;
  int payloadSize = pack->getPayloadSize ();
  /* Get the freespace in Section */
  int freespace;

  handler = getSectionHandler (pack->getPid ());

  assert (handler != NULL);

  if ((handler->section == NULL) && (handler->headerSize == 0))
    return;

  if (lastPacket)
    {
      if (pack->getPayloadSize2 ())
        {
          payloadSize = pack->getPayloadSize2 ();
        }
    }

  memset (data, 0, sizeof (data));

  if (lastPacket)
    {
      if (pack->getPayloadSize2 ())
        {
          pack->getPayload2 (data);
        }
      else
        {
          pack->getPayload (data);
        }
    }
  else
    {
      pack->getPayload (data);
    }

  if (handler->section == NULL)
    {
      int headerSizeLeft = 8 - handler->headerSize;
      if (headerSizeLeft > 0)
        {
          memcpy (handler->sectionHeader + handler->headerSize, data,
                  headerSizeLeft);
          handler->headerSize += headerSizeLeft;
          if (handler->headerSize == 8)
            {
              handler->section = new TransportSection (
                  handler->sectionHeader, handler->headerSize);

              handler->section->addData (data + headerSizeLeft,
                                         payloadSize - headerSizeLeft);
              setSectionParameters (pack);
              return;
            }
          else
            {
              handler->headerSize = 0;
              return;
            }
        }
      else
        {
          return;
        }
    }

  freespace = handler->section->getSectionLength () + 3
              - handler->section->getCurrentSize ();

  /* If the freeSpace is bigger than payLoadSize then
  * add just the payloadSize      */

  if (freespace > payloadSize)
    {
      freespace = payloadSize;
    }

  assert (freespace > 0);
  handler->section->addData (data, freespace);

  if (handler->section->isConsolidated ())
    {
      handler->section->setESId (pack->getPid ());
      process (handler->section, pack->getPid ());
    }
}

bool
SectionFilter::verifyAndCreateSection (ITSPacket *pack)
{
  unsigned int payloadSize;
  char data[ITSPacket::TS_PAYLOAD_SIZE];
  char *buffer;
  SectionHandler *handler;

  handler = getSectionHandler (pack->getPid ());
  assert (handler != NULL);

  payloadSize = pack->getPayloadSize ();

  if (payloadSize > ITSPacket::TS_PAYLOAD_SIZE)
    {
      clog << "SectionFilter::verifyAndCreateSection Warning! ";
      clog << "invalid TS Packet" << endl;
      pack->print ();

      return false;
    }

  pack->getPayload (data);
  /* The payload has only a part of the header */
  if (payloadSize
      < (G_N_ELEMENTS (handler->sectionHeader) - handler->headerSize))
    {
      /*clog << "SectionFilter::verifyAndCreateSection ";
      clog << "Creating Section header, currentSize is '";
      clog << handler->headerSize << " and dataSize is '";
      clog << diff << "'";
      clog << endl;*/

      memcpy ((void *)&handler->sectionHeader[handler->headerSize], data,
              payloadSize);

      handler->headerSize = handler->headerSize + payloadSize;
      handler->lastContinuityCounter = pack->getContinuityCounter ();
      return false;

      /* Needs to copy the header */
    }
  else if (handler->headerSize > 0)
    {
      /*clog << "Pointer field: " << pack->getPointerField();
      clog << " PayloadSize: " << pack->getPayloadSize() << endl;
      clog << "Appending Section header '" << handler->headerSize << "'";
      clog << " to data '" << diff << "'" << endl;*/

      /* Creates the new data buffer */
      buffer = new char[handler->headerSize + payloadSize];

      /* Copies the header to buffer */
      memcpy ((void *)&buffer[0], (void *)&handler->sectionHeader,
              handler->headerSize);

      /* Copies the payload to buffer */
      memcpy ((void *)&buffer[handler->headerSize], data, payloadSize);

      /* Creates the new section */
      handler->section = new TransportSection (
          buffer, payloadSize + handler->headerSize);

      delete[] buffer;
      buffer = NULL;

      /* The Header is ready */
    }
  else if (pack->getStartIndicator ())
    {
      /*clog << "SectionFilter::verifyAndCreateSection PUSI = 1";
      clog << " for PID = '" << pack->getPid() << "'";
      clog << "Header is ready already!" << endl;*/

      handler->section = new TransportSection (data, payloadSize);
    }
  else
    {
      /*clog << "SectionFilter::verifyAndCreateSection ";
      clog << "nothing to do current header size is '";
      clog << handler->headerSize << "' current section address is '";
      clog << handler->section << "' TS packet: ";
      pack->print();
      clog << endl;*/

      return false;
    }

  return setSectionParameters (pack);
}

bool
SectionFilter::setSectionParameters (ITSPacket *pack)
{
  SectionHandler *handler;

  handler = getSectionHandler (pack->getPid ());

  assert (handler != NULL);
  assert (handler->section != NULL);

  /* Verifies if the TransportSection has been created! */
  if (!(handler->section->isConstructionFailed ()))
    {
      clog << "SectionFilter::receiveTSPacket failed to create Section!";
      clog << endl;

      ignore (pack->getPid ());
      return false;
    }

  /*clog <<
                  "SectionFilter::setSectionParameters "
                  "Section %s created with secNUm=%d, lasSec=%d and
     secLen=%d\n",
                  handler->section->getSectionName().c_str(),
                  handler->section->getSectionNumber(),
                  handler->section->getLastSectionNumber(),
                  handler->section->getSectionLength();*/

  if (handler->section->isConsolidated ())
    {
      handler->section->setESId (pack->getPid ());
      process (handler->section, pack->getPid ());
    }
  else
    {
      handler->lastContinuityCounter = pack->getContinuityCounter ();
    }

  return true;
}

GINGA_DATAPROC_END
