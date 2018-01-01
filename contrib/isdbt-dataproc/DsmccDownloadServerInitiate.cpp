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
#include "DsmccDownloadServerInitiate.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_DATAPROC_BEGIN

DsmccDownloadServerInitiate::DsmccDownloadServerInitiate ()
{
  srgIor = NULL;
  data = NULL;
}

DsmccDownloadServerInitiate::~DsmccDownloadServerInitiate ()
{
  if (data != NULL)
    {
      delete data;
      data = NULL;
    }

  if (srgIor != NULL)
    {
      delete srgIor;
      srgIor = NULL;
    }

  if (header != NULL)
    {
      delete header;
      header = NULL;
    }
}

DsmccIor *
DsmccDownloadServerInitiate::getServiceGatewayIor ()
{
  return this->srgIor;
}

int
DsmccDownloadServerInitiate::processMessage (DsmccMessageHeader *message)
{
  FILE *fd;
  int rval;
  unsigned int privateDataLength;

  header = message;
  // dsmccmessageheader = 12
  idx = header->getAdaptationLength () + 12;

  data = new char[header->getMessageLength () + idx];

  fd = fopen (header->getFileName ().c_str (), "rb");
  if (fd != NULL)
    {
      rval = (int) fread ((void *)&(data[0]), 1,
                    header->getMessageLength () + idx, fd);

      // skip serverId
      idx = idx + 20;

      // compatibilityDescriptor?

      // privateDataLength
      privateDataLength
          = ((data[idx] & 0xFF) << 8) | (data[idx + 1] & 0xFF);

      idx = idx + 2;

      if (privateDataLength == 0x00)
        {
          // skiped bad alignment
          privateDataLength
              = ((data[idx] & 0xFF) << 8) | (data[idx + 1] & 0xFF);

          idx = idx + 2;
        }

      rval = DsmccDownloadServerInitiate::processIor ();

      fclose (fd);
      remove (header->getFileName ().c_str ());
      if (rval < 0)
        return rval;
    }
  else
    {
      clog << "DsmccDownloadServerInitiate::DsmccDownloadServerInitiate - "
              "Message header error: could not open file ";
      clog << header->getFileName ().c_str () << endl;
      return -1;
    }

  return 0;
}

int
DsmccDownloadServerInitiate::processIor ()
{
  unsigned int len;
  char *field;

  // type_id
  len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
        | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

  idx = idx + 4;

  field = new char[len];
  memcpy (field, (void *)&(data[idx]), len);
  srgIor = new DsmccIor ();
  srgIor->setTypeId ((string)field);
  delete[] field;
  idx = idx + len;

  // check Number of TaggedProfiles
  len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
        | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

  idx = idx + 4;
  if (len > 1)
    {
      clog << "Warning: DSI::TaggedProfiles, never reach here!!! TP = ";
      clog << len << endl;
      return -1;
    }

  // get ior profile tag and check it
  len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
        | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

  idx = idx + 4;
  if (len != TAG_BIOP)
    {
      clog << "Warning: DSI::TAG_BIOP, never reach here!!!";
      clog << " TAG = " << len << endl;
      return -2;
    }
  else
    {
      // BIOP Profile Body
      // skip size of body
      len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
            | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

      idx = idx + 4;

      // skip byte_order field
      idx++;

      /*
       * check number of liteComponents
       * shall have objectLocation and connBinder
       */
      len = (data[idx] & 0xFF);
      if (len != 2)
        {
          clog << "Warning: liteComponents, never reach here!!! LC = ";
          clog << len << endl;
          return -3;
        }

      idx++;

      // objectLocation
      // check if tag == "TAG_ObjectLocation"
      len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
            | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

      if (len != TAG_BIOP_OBJECT_LOCATION)
        {
          clog << "Warning: TAG_ObjectLocation, never reach here!!!";
          clog << " TAG = " << len << endl;
          return -4;
        }
      else
        {
          idx = idx + 4;

          // skip size of object data, we don't need it.
          idx++;

          // carousel_id
          len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
                | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

          srgIor->setCarouselId (len);
          idx = idx + 4;

          // module_id
          len = ((data[idx] & 0xFF) << 8) | (data[idx + 1] & 0xFF);
          srgIor->setModuleId (len);
          idx = idx + 2;

          // check version: major == 1 && minor == 0
          if (((data[idx] & 0xFF) != 0x01)
              || ((data[idx + 1] & 0xFF) != 0x00))
            {
              clog << "DSI Warning: DsmccObject version, never reach "
                      "here!!!";
              clog << endl;
              return -5;
            }

          idx = idx + 2;

          // objectKey
          len = (data[idx] & 0xFF);
          idx++;

          // if len > 4 then the unsigned int is not larger enough
          if (len > 4)
            {
              clog << "Warning! Size of";
              clog << " objectKey > 4, never reaches here!";
              clog << endl;

              idx = idx + len;
              return -6;
            }
          else if (len > 0)
            {
              if (len == 4)
                {
                  objectKey = ((data[idx] & 0xFF) << 24)
                              | ((data[idx + 1] & 0xFF) << 16)
                              | ((data[idx + 2] & 0xFF) << 8)
                              | (data[idx + 3] & 0xFF);
                }
              else if (len == 3)
                {
                  objectKey = ((data[idx] & 0xFF) << 16)
                              | ((data[idx + 1] & 0xFF) << 8)
                              | (data[idx + 2] & 0xFF);
                }
              else if (len == 2)
                {
                  objectKey
                      = ((data[idx] & 0xFF) << 8) | (data[idx + 1] & 0xFF);
                }
              else
                {
                  objectKey = (data[idx] & 0xFF);
                }

              srgIor->setObjectKey (objectKey);
              idx = idx + len;
            }
        }

      // connBinder
      // TODO: it will be Usefull?
      // skip connBinder TAG
      idx = idx + 4;

      // get size of connBinder to skip the rest of it
      len = (data[idx] & 0xFF);
      idx++;

      idx = idx + len;
    }

  delete data;
  data = NULL;
  return 0;
}

GINGA_DATAPROC_END
