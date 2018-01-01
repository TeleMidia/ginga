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
#include "DsmccModule.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_DATAPROC_BEGIN

DsmccModule::DsmccModule (unsigned int moduleId)
{
  this->pid = 0;
  this->id = moduleId;
  currentDownloadSize = 0;
  overflowNotification = true;
}

void
DsmccModule::setESId (unsigned int pid)
{
  this->pid = pid;
}

unsigned int
DsmccModule::getESId ()
{
  return this->pid;
}

void
DsmccModule::openFile ()
{
  moduleFd = fopen (getModuleFileName ().c_str (), "wb");
}

void
DsmccModule::setCarouselId (unsigned int id)
{
  carouselId = id;
}

void
DsmccModule::setSize (unsigned int size)
{
  this->size = size;
}

void
DsmccModule::setVersion (unsigned int version)
{
  this->version = version;
}

void
DsmccModule::setInfoLength (unsigned int length)
{
  this->infoLength = length;
}

bool
DsmccModule::isConsolidated ()
{
  if (currentDownloadSize > size)
    {
      if (overflowNotification)
        {
          clog << "DsmccModule::isConsolidated Warning! ";
          clog << "MODULE '" << getModuleFileName () << "' ";
          clog << "OVERFLOW!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
          clog << " SIZE='" << size << "' RCVD='";
          clog << currentDownloadSize << "'" << endl;
          overflowNotification = false;
        }
    }

  return (currentDownloadSize == size);
}

unsigned int
DsmccModule::getId ()
{
  return id;
}

unsigned int
DsmccModule::getCarouselId ()
{
  return carouselId;
}

unsigned int
DsmccModule::getSize ()
{
  return size;
}

unsigned int
DsmccModule::getVersion ()
{
  return version;
}

unsigned int
DsmccModule::getInfoLength ()
{
  return infoLength;
}

string
DsmccModule::getModuleFileName ()
{
  return xstrbuild ("%s/ginga/carousel/modules/%u%u%u.mod",
                    g_get_tmp_dir (), pid, id, version);
}

void
DsmccModule::pushDownloadData (unsigned int blockNumber, void *data,
                               unsigned int dataSize)
{
  unsigned int bytesSaved;

  if (!isConsolidated ())
    {
      if (blocks.find (blockNumber) != blocks.end ())
        {
          return;
        }

      blocks.insert (blockNumber);
      if (moduleFd != NULL)
        {
          bytesSaved = (int) fwrite (data, 1, dataSize, moduleFd);
          if (bytesSaved != dataSize)
            {
              clog << "DsmccModule::pushDownloadData Warning!";
              clog << " size of data is '" << dataSize;
              clog << "' saved only '" << bytesSaved << "'";
              clog << endl;
            }

          currentDownloadSize = currentDownloadSize + bytesSaved;
        }
      else
        {
          clog << "DsmccModule Warning! File not open." << endl;
        }
    }

  if (isConsolidated () && moduleFd != NULL)
    {
      fclose (moduleFd);
    }
}

void
DsmccModule::print ()
{
  clog << endl << endl;
  clog << "id: " << id << endl;
  clog << "currentDownloadSize: " << currentDownloadSize << endl;
  clog << "size: " << size << endl;
  clog << "version: " << version << endl;
  clog << "infolengh: " << infoLength << endl;
  clog << endl << endl;
}

GINGA_DATAPROC_END
