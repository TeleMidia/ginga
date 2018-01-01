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
#include "DsmccDownloadDataBlock.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_DATAPROC_BEGIN

DsmccDownloadDataBlock::DsmccDownloadDataBlock (DsmccMessageHeader *message)
{
  header = message;
  moduleId = 0;
  moduleVersion = 0;
}

DsmccDownloadDataBlock::~DsmccDownloadDataBlock ()
{
  if (header != NULL)
    {
      delete header;
      header = NULL;
    }
}

int
DsmccDownloadDataBlock::processDataBlock (
    map<unsigned int, DsmccModule *> *ocModules)
{
  FILE *fd;
  int rval, trval;
  unsigned int i, blockNumber;
  unsigned int messageId, adaptationLength, messageLength;
  char headerBytes[12];
  char *bytes;
  DsmccModule *mod;

  map<unsigned int, DsmccModule *> mods;

  mods.insert (ocModules->begin (), ocModules->end ());

  trval = 0;
  rval = 1;
  fd = fopen (header->getFileName ().c_str (), "rb");

  while (rval > 0 && fd != NULL)
    {
      rval = (int) fread ((void *)&(headerBytes[0]), 1, 12, fd);
      trval = trval + rval;

      if (rval == 0)
        {
          break;
        }

      messageId = ((headerBytes[2] & 0xFF) << 8) | (headerBytes[3] & 0xFF);

      adaptationLength = (headerBytes[9] & 0xFF);
      messageLength
          = ((headerBytes[10] & 0xFF) << 8) | (headerBytes[11] & 0xFF);

      if (messageId == 0x1003)
        {
          i = adaptationLength;

          bytes = new char[messageLength];
          memset (bytes, 0, messageLength);
          rval = (int) fread ((void *)&(bytes[0]), 1, messageLength, fd);
          trval = trval + rval;
          if (rval == (int)messageLength)
            {
              moduleId = ((bytes[i] & 0xFF) << 8) | (bytes[i + 1] & 0xFF);

              if (mods.count (moduleId) != 0)
                {
                  mod = mods[moduleId];
                  if (!(mod->isConsolidated ()))
                    {
                      // TODO: offer update version support
                      moduleVersion = (bytes[i + 2] & 0xFF);

                      // checking reserved
                      if ((bytes[i + 3] & 0xFF) != 0xFF)
                        {
                          clog << "DsmccDownloadDataBlock::"
                                  "processDataBlock ";
                          clog << "Warning! Reserved DDB shall be set";
                          clog << "to 0xFF" << endl;
                        }

                      blockNumber = ((bytes[i + 4] & 0xFF) << 8)
                                    | (bytes[i + 5] & 0xFF);

                      mod->pushDownloadData (blockNumber,
                                             (void *)&(bytes[i + 6]),
                                             (messageLength - (i + 6)));
                    }
                }
              else
                {
                  clog << "DsmccDownloadDataBlock::processDataBlock ";
                  clog << "DDB Warning! modId '" << moduleId << "' not";
                  clog << " found!" << endl;
                  return -1;
                }
            }
          else
            {
              clog << "DsmccDownloadDataBlock::processDataBlock Warning!!"
                   << endl;
              return -2;
            }

          delete[] bytes;
          bytes = NULL;
        }
      else
        {
          clog << "Warning! Unknown DDB MessageId: ";
          clog << hex << messageId << endl;
          return -3;
        }
    }
  if (fd != NULL)
    {
      fclose (fd);
    }

  remove (header->getFileName ().c_str ());
  return 0;
}

unsigned int
DsmccDownloadDataBlock::getModuleId ()
{
  return moduleId;
}

unsigned int
DsmccDownloadDataBlock::getModuleVersion ()
{
  return moduleVersion;
}

void
DsmccDownloadDataBlock::print ()
{
  clog << "messageLength " << header->getMessageLength () << endl;
  clog << "datablock moduleId " << moduleId << endl;
  clog << "datablock moduleVersion " << moduleVersion << endl;
}

GINGA_DATAPROC_END
