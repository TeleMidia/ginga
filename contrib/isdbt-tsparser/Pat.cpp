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
#include "Pat.h"

#include "IDemuxer.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

int Pat::defaultProgramNumber = -1;
int Pat::defaultProgramPid = -1;

Pat::Pat () : TransportSection ()
{
  currentPid = 150;
  currentCarouselId = 15;
  carouselComponentTag = 0x21;
  stream = new char[4096];
}

Pat::~Pat ()
{
  set<UnpPmtTime *>::iterator i;

  if (stream != NULL)
    {
      delete (stream);
      stream = NULL;
    }
  for (i = unprocessedPmts.begin (); i != unprocessedPmts.end (); ++i)
    {
      delete (*i);
    }
}

bool
Pat::hasStreamType (short streamType)
{
  map<unsigned int, Pmt *>::iterator i;

  for (i = programs.begin (); i != programs.end (); ++i)
    {
      if (i->second->hasStreamType (streamType))
        {
          return true;
        }
    }

  return false;
}

int
Pat::getPidByStreamType (short streamType)
{
  map<unsigned int, Pmt *>::iterator i;

  for (i = programs.begin (); i != programs.end (); ++i)
    {
      return i->second->getPidByStreamType (streamType);
    }

  return -1;
}

unsigned int
Pat::getNextPid ()
{
  currentPid++;
  while (hasPid (currentPid))
    {
      currentPid++;
    }

  return currentPid;
}

unsigned int
Pat::getNextCarouselId ()
{
  currentCarouselId++;
  return currentCarouselId;
}

unsigned int
Pat::getCarouselComponentTag ()
{
  return carouselComponentTag;
}

bool
Pat::hasProgramNumber (unsigned int programNumber)
{
  map<unsigned int, unsigned int>::iterator i;

  i = pat.begin ();
  while (i != pat.end ())
    {
      if (i->second == programNumber)
        {
          return true;
        }
      ++i;
    }
  return false;
}

bool
Pat::hasPid (unsigned int pid)
{
  map<unsigned int, Pmt *>::iterator i;
  Pmt *pmt;

  i = programs.begin ();
  while (i != programs.end ())
    {
      pmt = i->second;
      if (pmt->hasPid (pid))
        {
          return true;
        }
      ++i;
    }
  return false;
}

Pmt *
Pat::getPmtByProgramNumber (unsigned int programNumber)
{
  map<unsigned int, unsigned int>::iterator i;

  i = pat.begin ();
  while (i != pat.end ())
    {
      if (i->second == programNumber)
        {
          if (programs.count (i->first) != 0)
            {
              return programs[i->first];
            }
        }
      ++i;
    }
  return NULL;
}

unsigned int
Pat::getTSId ()
{
  return this->idExtention;
}

void
Pat::setTSId (unsigned int newTSId)
{
  this->idExtention = newTSId;
}

unsigned int
Pat::getNumOfPrograms ()
{
  return programs.size ();
}

void
Pat::addProgram (unsigned int pid, unsigned int programNumber)
{
  if (pat.count (pid) != 0)
    {
      clog << "Pat::addProgram Warning! Trying to override an existent";
      clog << " program. Pid = '" << pid << "'" << endl;
    }
  else
    {
      pat[pid] = programNumber;
    }
}

void
Pat::addPmt (Pmt *program)
{
  if (program == NULL)
    {
      return;
    }

  set<UnpPmtTime *>::iterator i;
  for (i = unprocessedPmts.begin (); i != unprocessedPmts.end (); ++i)
    {
      if ((*i)->pid == program->getPid ())
        {
          unprocessedPmts.erase (i);
          break;
        }
    }

  if (programs.count (program->getPid ()))
    {
      clog << "Pat::addPmt Warning! Trying to override an existent";
      clog << " program. Pid = '" << program->getPid () << "'";
      clog << endl;
    }
  else
    {
      programs[program->getPid ()] = program;
    }
}

void
Pat::replacePmt (unsigned int pid, Pmt *newPmt)
{
  Pmt *oldPmt;
  if (programs.count (pid) != 0)
    {
      oldPmt = programs[pid];
      delete oldPmt;
      programs[pid] = newPmt;
    }
}

unsigned int
Pat::getFirstProgramNumber ()
{
  if (pat.empty ())
    {
      return 0;
    }
  return pat.begin ()->second;
}

unsigned int
Pat::getProgramNumberByPid (unsigned int pid)
{
  if (pat.count (pid) != 0)
    {
      return pat[pid];
    }
  return 0;
}

short
Pat::getStreamType (unsigned int pid)
{
  if (programs.empty ())
    {
      return 0;
    }

  map<unsigned int, Pmt *>::iterator i;

  Pmt *program = NULL;
  i = programs.begin ();
  while (i != programs.end ())
    {
      program = i->second;
      if (program->hasPid (pid))
        {
          break;
        }

      program = NULL;
      ++i;
    }

  if (program == NULL)
    {
      return 0;
    }

  return program->getStreamTypeValue (pid);
}

bool
Pat::isSectionType (unsigned int pid)
{
  if (pid == 0x00 || pid == 0x01 || pid == 0x03
      || programs.count (pid) != 0)
    {
      return true;
    }

  set<UnpPmtTime *>::iterator it;
  for (it = unprocessedPmts.begin (); it != unprocessedPmts.end (); ++it)
    {
      if ((*it)->pid == pid)
        {
          return true;
        }
    }

  map<unsigned int, Pmt *>::iterator i;

  Pmt *program = NULL;
  i = programs.begin ();
  while (i != programs.end ())
    {
      program = i->second;
      if (program->hasPid (pid))
        {
          break;
        }

      program = NULL;
      ++i;
    }

  if (program == NULL)
    {
      return false;
    }

  return program->isSectionType (pid);
}

bool
Pat::processSectionPayload ()
{
  char *payload;

  if (tableId != 0 || !isConsolidated ())
    {
      return false;
    }

  char *sectionPayload = new char[sectionLength - 9];

  payload = (char *)getPayload ();
  memcpy ((void *)sectionPayload, payload, sectionLength - 9);
  delete payload;

  unsigned int n;
  unsigned int i;
  unsigned int programNumber;
  unsigned int pid;
  n = (unsigned int)((sectionLength - 9) / 4);

  for (i = 0; i < n; i++)
    {
      programNumber = ((sectionPayload[i * 4] & 0xFF) << 8)
                      | (sectionPayload[((i * 4) + 1)] & 0xFF);

      pid = ((sectionPayload[((i * 4) + 2)] & 0x1F) << 8)
            | (sectionPayload[((i * 4) + 3)] & 0xFF);

      /* Ignoring NIT_PIDs */
      if (programNumber != 0)
        {
          if ((pid != (unsigned int)NIT_PID)
              || ((pid == (unsigned int)NIT_PID) && (n == 1)))
            {
              if (defaultProgramNumber == -1
                  || programNumber < (unsigned int) defaultProgramNumber)
                {
                  /*clog << "Pat::process ";
                  clog << "pnumByte1 = '" << (sectionPayload[((i*4) + 0)] &
                  0x1F);
                  clog << "' pnumByte2 = '" << (sectionPayload[((i*4) + 1)]
                  &
                  0xFF);
                  clog << "ppidByte1 = '" << (sectionPayload[((i*4) + 2)] &
                  0x1F);
                  clog << "' ppidByte2 = '" << (sectionPayload[((i*4) + 3)]
                  &
                  0xFF);
                  clog << "' sectionLength = '" << getSectionLength();
                  clog << "'" << endl;*/

                  defaultProgramNumber = programNumber;
                  defaultProgramPid = pid;
                }

              pat[pid] = programNumber;

              UnpPmtTime *upt = new UnpPmtTime ();
              upt->pid = pid;
              upt->time = xruntime_ms ();
              unprocessedPmts.insert (upt);
            }
        }
    }

  delete[] sectionPayload;
  return true;
}

void
Pat::resetPayload (char *payload, int size)
{
  int pointerField = 0;
  int offset;
  unsigned long crcValue;

  if (defaultProgramNumber < 0)
    {
      clog << "Pat::resetPayload Warning! Can't reset: programNum < 0";
      clog << endl;
      return;
    }

  /*if (payload[0] != 0x0) {
          pointerField = payload[0] & 0xFF;
  }*/

  offset = pointerField;

  if (payload[offset] != 0x00)
    {
      clog << "Pat::resetPayload Warning! Pat Table Id != 0x00";
      clog << endl;
    }

  // update section_length
  payload[offset + 1] = payload[offset + 1] & 0xF0;
  payload[offset + 2] = (9 + 4) & 0xFF;

  offset = offset + 8; // escape section_header

  payload[offset] = (defaultProgramNumber & 0x0000FF00) >> 8;
  payload[offset + 1] = (defaultProgramNumber)&0x000000FF;

  if (defaultProgramPid < 255)
    {
      payload[offset + 2] = 0xE0;
    }
  else
    {
      payload[offset + 2] = ((defaultProgramPid & 0x0000FF00) | 0xE0) >> 8;
      payload[offset + 2] = payload[offset + 2] | 0xE0;
    }

  payload[offset + 3] = (defaultProgramPid)&0x000000FF;

  offset = offset + 4; // skipping created program

  crcValue = TransportSection::crc32 (payload + pointerField, 3 + 5 + 4);

  payload[offset] = (crcValue & 0xFF000000) >> 24;
  payload[offset + 1] = (crcValue & 0x00FF0000) >> 16;
  payload[offset + 2] = (crcValue & 0x0000FF00) >> 8;
  payload[offset + 3] = (crcValue & 0x000000FF);

  offset = offset + 4; // skipping crc32
  while (offset < size)
    {
      payload[offset] = 0xFF;
      offset++;
    }
}

bool
Pat::hasUnprocessedPmt ()
{
  set<UnpPmtTime *>::iterator i;
  i = unprocessedPmts.begin ();
  while (i != unprocessedPmts.end ())
    {
      if (((xruntime_ms () - (*i)->time) >= 10000.0)
          && programs.size ())
        {
          cout << "PMT PID = " << (*i)->pid << " has been expired." << endl;
          unprocessedPmts.erase (i);
          i = unprocessedPmts.begin ();
          continue;
        }
      ++i;
    }
  if (unprocessedPmts.empty ())
    {
      return false;
    }
  return true;
}

set<UnpPmtTime *> *
Pat::getUnprocessedPmtPids ()
{
  return &unprocessedPmts;
}

map<unsigned int, Pmt *> *
Pat::getProgramsInfo ()
{
  return &this->programs;
}

void
Pat::checkConsistency ()
{
  map<unsigned int, unsigned int>::iterator i;

  i = pat.begin ();
  while (i != pat.end ())
    {
      if (programs.count (i->first) == 0)
        {
          pat.erase (i);
          if (pat.empty ())
            {
              return;
            }
          i = pat.begin ();
        }
      else
        {
          ++i;
        }
    }
}

unsigned int
Pat::getDefaultProgramPid ()
{
  Pmt *pmt;

  if (programs.empty ())
    {
      return 0;
    }

  pmt = programs.begin ()->second;
  return pmt->getPid ();
}

unsigned int
Pat::getDefaultMainVideoPid ()
{
  Pmt *pmt;

  if (programs.empty ())
    {
      return 0;
    }

  pmt = programs.begin ()->second;
  return pmt->getDefaultMainVideoPid ();
}

unsigned int
Pat::getDefaultMainAudioPid ()
{
  Pmt *pmt;

  if (programs.empty ())
    {
      return 0;
    }

  pmt = programs.begin ()->second;
  return pmt->getDefaultMainAudioPid ();
}

unsigned int
Pat::getDefaultMainCarouselPid ()
{
  Pmt *pmt;

  if (programs.empty ())
    {
      return 0;
    }

  pmt = programs.begin ()->second;
  return pmt->getDefaultMainCarouselPid ();
}

unsigned short
Pat::createPatStreamByProgramPid (unsigned short pid, char **dataStream)
{
  map<unsigned int, unsigned int>::iterator i;
  int pos = 0;

  memset (stream, 0xFF, 4096);
  stream[pos++] = tableId;
  stream[pos] = 0xB0;

  stream[pos] = stream[pos] | ((sectionLength >> 8) & 0x03);
  pos++;
  stream[pos++] = sectionLength & 0xFF;

  stream[pos++] = (idExtention >> 8) & 0xFF;
  stream[pos++] = idExtention & 0xFF;

  stream[pos] = 0xC0;

  stream[pos] = stream[pos] | ((versionNumber << 1) & 0x3E);
  stream[pos] = stream[pos] | (currentNextIndicator & 0x01);
  pos++;
  stream[pos++] = sectionNumber & 0xFF;
  stream[pos++] = lastSectionNumber & 0xFF;

  i = pat.begin ();
  while (i != pat.end ())
    {
      if (i->first == pid)
        {
          stream[pos++] = (i->second >> 8) & 0xFF;
          stream[pos++] = i->second & 0xFF;
          stream[pos] = 0xE0;
          stream[pos] = stream[pos] | ((i->first >> 8) & 0x1F);
          pos++;
          stream[pos++] = i->first & 0xFF;
          break;
        }
      ++i;
    }

  unsigned int crcValue = TransportSection::crc32 (stream, pos);

  stream[pos++] = (crcValue & 0xFF000000) >> 24;
  stream[pos++] = (crcValue & 0x00FF0000) >> 16;
  stream[pos++] = (crcValue & 0x0000FF00) >> 8;
  stream[pos++] = (crcValue & 0x000000FF);

  *dataStream = stream;

  return pos;
}

void
Pat::print ()
{
  map<unsigned int, unsigned int>::iterator i;
  map<unsigned int, Pmt *>::iterator j;

  cout << "Pat::print" << endl;
  cout << "TS id = " << idExtention << endl;

  for (i = pat.begin (); i != pat.end (); ++i)
    {
      cout << "programNumber '" << i->second << "' ";
      cout << " has pid = '" << i->first << "'" << endl;
    }

  for (j = programs.begin (); j != programs.end (); ++j)
    {
      j->second->print ();
    }
}

GINGA_TSPARSER_END
