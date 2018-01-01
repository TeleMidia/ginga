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
#include "DsmccDownloadInfoIndication.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_DATAPROC_BEGIN

DsmccDownloadInfoIndication::DsmccDownloadInfoIndication () {}

DsmccDownloadInfoIndication::~DsmccDownloadInfoIndication ()
{
  map<unsigned int, DsmccModule *>::iterator i;

  i = modules.begin ();
  while (i != modules.end ())
    {
      delete i->second;

      ++i;
    }

  if (header != NULL)
    {
      delete header;
      header = NULL;
    }

  modules.clear ();
}

int
DsmccDownloadInfoIndication::processMessage (DsmccMessageHeader *message)
{
  FILE *fd;
  unsigned int i, moduleId, moduleSize, moduleVersion, moduleInfoLength;
  DsmccModule *module;

  header = message;

  // dsmccmessageheader = 12
  i = header->getAdaptationLength () + 12;

  char *bytes = new char[(header->getMessageLength () + i)];

  fd = fopen (header->getFileName ().c_str (), "rb");
  if (fd != NULL)
    {
      this->downloadId
          = ((bytes[i] & 0xFF) << 24) | ((bytes[i + 1] & 0xFF) << 16)
            | ((bytes[i + 2] & 0xFF) << 8) | (bytes[i + 3] & 0xFF);

      this->blockSize
          = ((bytes[i + 4] & 0xFF) << 8) | (bytes[i + 5] & 0xFF);

      // jump 10 bytes of unused fields
      i = i + 10;

      // checking compatibilityDescriptor()
      unsigned int compatDesc;
      compatDesc = ((bytes[i + 6] & 0xFF) << 8) | (bytes[i + 7] & 0xFF);
      if ((compatDesc) != 0)
        {
          i = i + compatDesc;
        }

      this->numberOfModules
          = ((bytes[i + 8] & 0xFF) << 8) | (bytes[i + 9] & 0xFF);

      i = i + 10;

      unsigned int j;
      for (j = 0; j < numberOfModules; j++)
        {
          moduleId = ((bytes[i] & 0xFF) << 8) | (bytes[i + 1] & 0xFF);
          moduleSize = ((bytes[i + 2] & 0xFF) << 24)
                       | ((bytes[i + 3] & 0xFF) << 16)
                       | ((bytes[i + 4] & 0xFF) << 8)
                       | (bytes[i + 5] & 0xFF);

          moduleVersion = (bytes[i + 6] & 0xFF);
          moduleInfoLength = (bytes[i + 7] & 0xFF);

          module = new DsmccModule (moduleId);
          module->setESId (header->getESId ());
          module->setSize (moduleSize);
          module->setVersion (moduleVersion);
          module->setInfoLength (moduleInfoLength);
          module->setCarouselId (downloadId);
          module->openFile ();
          modules[moduleId] = module;

          i = i + 8;
          i = i + moduleInfoLength;
          module = NULL;
        }
      fclose (fd);

      remove (header->getFileName ().c_str ());
    }
  else
    {
      clog
          << "DsmccDownloadInfoIndication::processMessage - Message header "
             "error: could not open file ";
      clog << header->getFileName ().c_str () << endl;
      return -1;
    }

  delete[] bytes;

  return 0;
}

unsigned int
DsmccDownloadInfoIndication::getDonwloadId ()
{
  return downloadId;
}

unsigned int
DsmccDownloadInfoIndication::getBlockSize ()
{
  return blockSize;
}

unsigned int
DsmccDownloadInfoIndication::getNumberOfModules ()
{
  return numberOfModules;
}

void
DsmccDownloadInfoIndication::getInfo (
    map<unsigned int, DsmccModule *> *ocInfo)
{
  ocInfo->insert (modules.begin (), modules.end ());
}

void
DsmccDownloadInfoIndication::print ()
{
  clog << "downloadId = " << this->downloadId << endl;
  clog << "blockSize = " << this->blockSize << endl;
  clog << "numberOfModules = " << this->numberOfModules << endl;
}

GINGA_DATAPROC_END
