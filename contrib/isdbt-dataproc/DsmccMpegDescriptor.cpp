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
#include "DsmccMpegDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_DATAPROC_BEGIN

DsmccMpegDescriptor::DsmccMpegDescriptor ()
{
  stream = NULL;
  descriptorTag = 0;
  descriptorLength = 0;
  currentSize = 0;
}

DsmccMpegDescriptor::~DsmccMpegDescriptor ()
{
  if (stream != NULL)
    {
      delete (stream);
    }
}

DsmccMpegDescriptor::DsmccMpegDescriptor (unsigned char tag)
{
  stream = NULL;
  descriptorTag = tag;
  descriptorLength = 0;
  currentSize = 0;
}

char
DsmccMpegDescriptor::addData (char *data, unsigned short length)
{
  unsigned short rbytes;

  if (currentSize == 0)
    {
      if (length < 2)
        {
          return -1; // error
        }

      descriptorTag = data[0] & 0xFF;
      descriptorLength = data[1] & 0xFF;

      if (stream != NULL)
        {
          delete (stream);
        }
      try
        {
          stream = new char[descriptorLength + 2];
        }
      catch (...)
        {
          return -1;
        }
    }

  if (currentSize + length >= MAX_DESCRIPTOR_SIZE)
    {
      rbytes = MAX_DESCRIPTOR_SIZE - currentSize;
    }
  else
    {
      rbytes = length;
    }

  memcpy (stream + currentSize, data, rbytes);

  currentSize = currentSize + rbytes;

  if (isConsolidated ())
    {
      if (rbytes > 0)
        {
          process ();
        }
      return 1;
    }
  else
    {
      return 0;
    }
}

int
DsmccMpegDescriptor::process ()
{
  return 2;
}

int
DsmccMpegDescriptor::updateStream ()
{
  unsigned int len;

  if (stream != NULL)
    {
      delete[](stream);
    }
  try
    {
      len = calculateDescriptorSize ();
      stream = new char[len];
    }
  catch (...)
    {
      return -1;
    }

  descriptorLength = len - 2;

  stream[0] = descriptorTag & 0xFF;
  stream[1] = descriptorLength & 0xFF;
  return 2;
}

unsigned int
DsmccMpegDescriptor::calculateDescriptorSize ()
{
  return 2;
}

unsigned char
DsmccMpegDescriptor::isConsolidated ()
{
  unsigned short len = descriptorLength + 2;
  return (len <= currentSize);
}

unsigned char
DsmccMpegDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned char
DsmccMpegDescriptor::getDescriptorLength ()
{
  return descriptorLength;
}

void
DsmccMpegDescriptor::setDescriptorTag (unsigned char tag)
{
  descriptorTag = tag;
}

int
DsmccMpegDescriptor::getStreamSize ()
{
  return calculateDescriptorSize ();
}

int
DsmccMpegDescriptor::getStream (char **dataStream)
{
  int slen = updateStream ();
  if (slen >= 0)
    {
      *dataStream = stream;
      return slen;
    }
  else
    {
      return -1;
    }
}

DsmccMpegDescriptor *
DsmccMpegDescriptor::getDescriptor (
    vector<DsmccMpegDescriptor *> *descriptors, unsigned char Tag)
{
  vector<DsmccMpegDescriptor *>::iterator dit;
  dit = descriptors->begin ();
  while (dit != descriptors->end ())
    {
      if ((*dit)->getDescriptorTag () == Tag)
        {
          return *dit;
        }
      dit++;
    }
  return NULL;
}

vector<DsmccMpegDescriptor *> *
DsmccMpegDescriptor::getDescriptors (
    vector<DsmccMpegDescriptor *> *descriptors, unsigned char Tag)
{
  vector<DsmccMpegDescriptor *> *result;
  vector<DsmccMpegDescriptor *>::iterator dit;
  result = new vector<DsmccMpegDescriptor *>;
  dit = descriptors->begin ();
  while (dit != descriptors->end ())
    {
      if ((*dit)->getDescriptorTag () == Tag)
        {
          result->push_back (*dit);
        }
      dit++;
    }
  if (result->size () == 0)
    {
      delete result;
      result = NULL;
    }
  return result;
}

int
DsmccMpegDescriptor::getDescriptorsLength (
    vector<DsmccMpegDescriptor *> *descriptors)
{
  int len = 0;
  vector<DsmccMpegDescriptor *>::iterator dit;
  dit = descriptors->begin ();
  while (dit != descriptors->end ())
    {
      len = len + (*dit)->getDescriptorLength () + 2;
      dit++;
    }
  return len;
}

GINGA_DATAPROC_END
