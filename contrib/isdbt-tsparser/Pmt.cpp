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
#include "Pmt.h"
#include "IDemuxer.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

Pmt::Pmt (unsigned int pid, unsigned int programNumber)
    : TransportSection ()
{
  processed = false;

  streamTypeNames[STREAM_TYPE_VIDEO_MPEG1] = "ISO/IEC 11172-2 Video";
  streamTypeNames[STREAM_TYPE_VIDEO_MPEG2] = "ISO/IEC 13818-2 Video";
  streamTypeNames[STREAM_TYPE_AUDIO_MPEG1] = "ISO/IEC 11172-3 Audio";
  streamTypeNames[STREAM_TYPE_AUDIO_MPEG2] = "ISO/IEC 13818-3 Audio";
  streamTypeNames[STREAM_TYPE_PRIVATE_SECTION]
      = "ISO/IEC 13818 Private Sections";

  streamTypeNames[STREAM_TYPE_PRIVATE_DATA] = "ISO/IEC 13818 Private Data";

  streamTypeNames[STREAM_TYPE_MHEG] = "ISO/IEC 13522 MHEG";
  streamTypeNames[STREAM_TYPE_DSMCC_TYPE_A] = "ISO/IEC 13818-6 type A";
  streamTypeNames[STREAM_TYPE_DSMCC_TYPE_B] = "ISO/IEC 13818-6 type B";
  streamTypeNames[STREAM_TYPE_DSMCC_TYPE_C] = "ISO/IEC 13818-6 type C";
  streamTypeNames[STREAM_TYPE_DSMCC_TYPE_D] = "ISO/IEC 13818-6 type D";
  streamTypeNames[STREAM_TYPE_AUDIO_AAC] = "ISO/IEC 13818-7 Audio AAC";
  streamTypeNames[STREAM_TYPE_VIDEO_MPEG4] = "ISO/IEC 14496-2 Visual";
  streamTypeNames[STREAM_TYPE_AUDIO_MPEG4] = "ISO/IEC 14496-3 Audio";
  streamTypeNames[STREAM_TYPE_VIDEO_H264] = "ISO/IEC 14496-10 h.264";
  streamTypeNames[STREAM_TYPE_AUDIO_AC3] = "audio AC3";
  streamTypeNames[STREAM_TYPE_AUDIO_DTS] = "audio DTS";

  this->pid = pid;
  this->programNumber = programNumber;
  this->pcrPid = 0;
}

Pmt::~Pmt () {}

bool
Pmt::hasStreamType (short streamType)
{
  map<unsigned int, short>::iterator i;

  for (i = streams.begin (); i != streams.end (); ++i)
    {
      if (i->second == streamType)
        {
          return true;
        }
    }

  return false;
}

int
Pmt::getPidByStreamType (short streamType)
{
  map<unsigned int, short>::iterator i;

  for (i = streams.begin (); i != streams.end (); ++i)
    {
      if (i->second == streamType)
        {
          return i->first;
        }
    }

  return -1;
}

void
Pmt::addElementaryStream (unsigned int pid, short esType)
{
  clog << "pid = '" << pid << "' esType = '" << esType;
  clog << "' " << endl;
  streams[pid] = esType;
}

unsigned int
Pmt::getPid ()
{
  return this->pid;
}

vector<unsigned int> *
Pmt::copyPidsByTid (unsigned int tid)
{
  vector<unsigned int> *pids;
  map<unsigned int, short>::iterator i;
  short streamType;

  streamType = IDemuxer::getStreamTypeFromTableId (tid);

  pids = new vector<unsigned int>;
  i = streams.begin ();
  while (i != streams.end ())
    {
      if (streamType == i->second)
        {
          pids->push_back (i->first);
        }
      ++i;
    }

  return pids;
}

void
Pmt::setPid (unsigned int pid)
{
  this->pid = pid;
}

unsigned int
Pmt::getProgramNumber ()
{
  return this->programNumber;
}

void
Pmt::setProgramNumber (unsigned int programNumber)
{
  this->programNumber = programNumber;
}

bool
Pmt::hasPid (unsigned int somePid)
{
  if (somePid == this->pid)
    {
      return true;
    }

  return (streams.count (somePid) != 0);
}

short
Pmt::getStreamTypeValue (unsigned int somePid)
{
  if (streams.count (somePid) == 0)
    {
      return -1;
    }

  return streams[somePid];
}

string
Pmt::getStreamType (unsigned int somePid)
{
  if (streams.count (somePid) == 0)
    {
      return "";
    }

  return getStreamTypeName (streams[somePid]);
}

bool
Pmt::isSectionType (unsigned int pid)
{
  short value;
  value = getStreamTypeValue (pid);

  if (value == STREAM_TYPE_PRIVATE_SECTION
      || value == STREAM_TYPE_PRIVATE_DATA
      || (value >= STREAM_TYPE_DSMCC_TYPE_A
          && value <= STREAM_TYPE_DSMCC_TYPE_D))
    {
      return true;
    }
  return false;
}

string
Pmt::getStreamTypeName (short streamType)
{
  if (streamTypeNames.count (streamType) == 0)
    {
      string s;
      xstrassign (s, "%x", (unsigned int) streamType);
      return "unknown type '" + s + "'";
    }
  return streamTypeNames[streamType];
}

bool
Pmt::processSectionPayload ()
{
  if (processed)
    {
      clog << "Warning! Pmt::processSectionPayload() - Try to process a ";
      clog << "already processed PMT " << endl;
      return false;
    }

  if (tableId != (unsigned int)PMT_TID || !isConsolidated ())
    {
      clog << "Pmt::processSectionPayload Warning! inconsistency found";
      clog << " tableId = " << tableId << " and isConsolidated = ";
      clog << isConsolidated () << endl;
      return false;
    }

  if (programNumber != idExtention)
    {
      clog << "Pmt::processSectionPayload Warning! programNumber !=";
      clog << " idExtension." << endl;
    }

  char *sectionPayload = new char[sectionLength - 9];
  memcpy ((void *)sectionPayload, getPayload (), sectionLength - 9);

  pcrPid = ((sectionPayload[0] & 0x1F) << 8) | (sectionPayload[1] & 0xFF);

  unsigned int programInfoLength;
  programInfoLength
      = ((sectionPayload[2] & 0x0F) << 8) | (sectionPayload[3] & 0xFF);

  unsigned int i;
  short streamType;
  unsigned int elementaryPid;
  unsigned int esInfoLength, esInfoPos;
  unsigned char descriptorTag, descriptorSize;
  StreamIdentifierDescriptor *sid;

  i = 4 + programInfoLength; // jumping descriptors
  while (i < (sectionLength - 9))
    {
      streamType = (short)(sectionPayload[i] & 0xFF);
      i++;
      elementaryPid = ((sectionPayload[i] & 0x1F) << 8)
                      | (sectionPayload[i + 1] & 0xFF);

      streams[elementaryPid] = streamType;

      i += 2;
      esInfoLength = ((sectionPayload[i] & 0x0F) << 8)
                     | (sectionPayload[i + 1] & 0xFF);
      i += 2;
      esInfoPos = 0;
      while (esInfoPos < esInfoLength)
        {
          descriptorTag = (sectionPayload[i] & 0xFF);
          descriptorSize = (sectionPayload[i + 1] & 0xFF) + 2;
          switch (descriptorTag)
            {
            case 0x52:
              sid = new StreamIdentifierDescriptor ();
              sid->process (sectionPayload, i);
              componentTags[elementaryPid] = sid->getComponentTag ();
              delete sid;
              break;
            default:
              break;
            }
          esInfoPos += descriptorSize;
          i += descriptorSize;
        }
    }

  processed = true;

  delete[] sectionPayload;
  return processed;
}

bool
Pmt::hasProcessed ()
{
  return processed;
}

map<unsigned int, short> *
Pmt::getStreamsInformation ()
{
  return &streams;
}

map<unsigned short, unsigned char> *
Pmt::getComponentTags ()
{
  return &componentTags;
}

unsigned int
Pmt::getPCRPid ()
{
  return pcrPid;
}

unsigned int
Pmt::getDefaultMainVideoPid ()
{
  map<unsigned int, short>::iterator i;

  i = streams.begin ();
  while (i != streams.end ())
    {
      switch (i->second)
        {
        case STREAM_TYPE_VIDEO_MPEG1:
        case STREAM_TYPE_VIDEO_MPEG2:
        case STREAM_TYPE_VIDEO_MPEG4:
        case STREAM_TYPE_VIDEO_H264:
          return i->first;

        default:
          break;
        }
      ++i;
    }

  return 0;
}

unsigned int
Pmt::getDefaultMainCarouselPid ()
{
  map<unsigned int, short>::iterator i;

  i = streams.begin ();
  while (i != streams.end ())
    {
      if (i->second == STREAM_TYPE_DSMCC_TYPE_B)
        {
          return i->first;
        }
      ++i;
    }

  return 0;
}

unsigned int
Pmt::getDefaultMainAudioPid ()
{
  map<unsigned int, short>::iterator i;

  i = streams.begin ();
  while (i != streams.end ())
    {
      switch (i->second)
        {
        case STREAM_TYPE_AUDIO_MPEG1:
        case STREAM_TYPE_AUDIO_MPEG2:
        case STREAM_TYPE_AUDIO_MPEG4:
        case STREAM_TYPE_AUDIO_AAC:
        case STREAM_TYPE_AUDIO_AC3:
        case STREAM_TYPE_AUDIO_DTS:
          return i->first;

        default:
          break;
        }
      ++i;
    }

  return 0;
}

int
Pmt::getTaggedVideoPid (unsigned char idx)
{
  short st;
  map<unsigned short, unsigned char>::iterator itTag;
  map<unsigned char, unsigned short>::iterator it;
  map<unsigned char, unsigned short> videoTags;

  itTag = componentTags.begin ();
  while (itTag != componentTags.end ())
    {
      st = getStreamTypeValue (itTag->first);
      if ((st == STREAM_TYPE_VIDEO_MPEG1) || (st == STREAM_TYPE_VIDEO_MPEG2)
          || (st == STREAM_TYPE_VIDEO_MPEG4)
          || (st == STREAM_TYPE_VIDEO_H264))
        {
          videoTags[itTag->second] = itTag->first;
        }
      ++itTag;
    }
  if (idx < videoTags.size ())
    {
      unsigned char cnt = 0;
      it = videoTags.begin ();
      while (it != videoTags.end ())
        {
          if (cnt == idx)
            return it->second;
          cnt++;
          ++it;
        }
    }
  return -1;
}

int
Pmt::getTaggedAudioPid (unsigned char idx)
{
  short st;
  map<unsigned short, unsigned char>::iterator itTag;
  map<unsigned char, unsigned short>::iterator it;
  map<unsigned char, unsigned short> audioTags;

  itTag = componentTags.begin ();
  while (itTag != componentTags.end ())
    {
      st = getStreamTypeValue (itTag->first);
      if ((st == STREAM_TYPE_AUDIO_MPEG1) || (st == STREAM_TYPE_AUDIO_MPEG2)
          || (st == STREAM_TYPE_AUDIO_MPEG4)
          || (st == STREAM_TYPE_AUDIO_AAC) || (st == STREAM_TYPE_AUDIO_AC3)
          || (st == STREAM_TYPE_AUDIO_DTS))
        {
          audioTags[itTag->second] = itTag->first;
        }
      ++itTag;
    }
  if (idx < audioTags.size ())
    {
      unsigned char cnt = 0;
      it = audioTags.begin ();
      while (it != audioTags.end ())
        {
          if (cnt == idx)
            return it->second;
          cnt++;
          ++it;
        }
    }
  return -1;
}

void
Pmt::print ()
{
  map<unsigned short, unsigned char>::iterator it;

  cout << "Pmt::print" << endl;
  cout << "Program number = " << idExtention << endl;
  cout << "streams:" << endl;
  map<unsigned int, short>::iterator i;
  for (i = streams.begin (); i != streams.end (); ++i)
    {
      cout << "streamType '" << getStreamTypeName (i->second) << "' ";
      cout << " has pid = '" << i->first << "'";
      it = componentTags.find (i->first);
      if (it != componentTags.end ())
        {
          cout << " with ctag = '" << (it->second & 0xFF) << "'";
        }
      cout << endl;
    }
}

GINGA_TSPARSER_END
