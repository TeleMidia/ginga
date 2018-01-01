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
#include "Tuner.h"
#include "FileSystemProvider.h"
#include "IProviderListener.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TUNER_BEGIN

FileSystemProvider::FileSystemProvider (const string &fileName)
{
  this->fileName = fileName;
  this->fileDescriptor = NULL;
  this->capabilities = DPC_CAN_FETCHDATA;
  this->listener = NULL;
}

FileSystemProvider::~FileSystemProvider () {}

void
FileSystemProvider::setListener (ITProviderListener *listener)
{
  this->listener = listener;
}

short
FileSystemProvider::getCaps ()
{
  return capabilities;
}

bool
FileSystemProvider::tune ()
{
  fileDescriptor = fopen (fileName.c_str (), "rb");
  if (fileDescriptor == NULL)
    {
      perror ("openFile");
      return false;
    }

  return true;
}

Channel *
FileSystemProvider::getCurrentChannel ()
{
  return NULL;
}

bool
FileSystemProvider::getSTCValue (unused (guint64 *stc), unused (int *valueType))
{
  return false;
}

bool
FileSystemProvider::changeChannel (unused (int factor))
{
  return false;
}

bool
FileSystemProvider::setChannel (const string &channelValue)
{
  fileName = channelValue;
  return true;
}

int
FileSystemProvider::createPesFilter (unused (int pid), unused (int pesType),
                                     unused (bool compositeFiler))
{
  return -1;
}

string
FileSystemProvider::getPesFilterOutput ()
{
  return "";
}

void
FileSystemProvider::close ()
{
  fclose (fileDescriptor);
}

bool
FileSystemProvider::checkPossiblePacket (char *buff, const int &pos)
{
  if ((buff[pos] == 0x47
       && (buff[pos + 188] == 0x47 || buff[pos + 204] == 0x47)))
    {
      return true;
    }
  else
    {
      return false;
    }
}

int
FileSystemProvider::nextPacket (char *buff)
{
  if (!(checkPossiblePacket (buff, 0)))
    {
      for (int i = 1; i < BUFFSIZE - 204; i++)
        {
          if (checkPossiblePacket (buff, i))
            {
              return i;
            }
        }
    }
  return 0;
}

int
FileSystemProvider::synchBuffer (char *buff, int diff)
{
  memcpy (buff, buff + diff, BUFFSIZE - diff);
  return BUFFSIZE - diff;
}

char *
FileSystemProvider::receiveData (int *len)
{
  bool loop = false;
  int diff, pos;
  char *buff = NULL;
  *len = 0;

  if (fileDescriptor != NULL)
    {
      buff = new char[BUFFSIZE];
      *len = fread ((void *)buff, 1, BUFFSIZE, fileDescriptor);
      if (*len < BUFFSIZE)
        loop = true;

      if (!loop)
        {
          diff = nextPacket (buff);
          if (diff > 0)
            {
              pos = synchBuffer (buff, diff);
              int r = fread (buff + pos, 1, diff, fileDescriptor);
              if (r < diff)
                loop = true;
            }
        }

      if (loop)
        {
          clog << "FileSystemProvider::receiveData" << endl;
          clog << "File is over, set file to begin again!" << endl;
          fseek (fileDescriptor, 0L, SEEK_SET);
          if (listener != NULL)
            {
              listener->receiveSignal (PST_LOOP);
            }
        }
    }

  return buff;
}

GINGA_TUNER_END
