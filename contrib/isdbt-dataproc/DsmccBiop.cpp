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
#include "DsmccBiop.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_DATAPROC_BEGIN

DsmccBiop::DsmccBiop (DsmccModule *module, DsmccObjectProcessor *processor)
{
  Thread::mutexInit (&dataMutex, false);

  currentSize = 0;
  isValidHdr = false;
  hasMoreBiopMessage = false;
  objectKind = "";
  objectInfo = "";
  data = NULL;

  this->module = module;
  this->processor = processor;

  if (fileExists (module->getModuleFileName ()))
    {
      moduleFd = fopen (module->getModuleFileName ().c_str (), "r+b");
      if (!moduleFd)
        {
          clog << "DsmccBiop::init - Cannot open file: "
               << module->getModuleFileName ().c_str () << endl;
          throw 1;
        }
    }
  else
    {
      abortProcess ("File not found: " + module->getModuleFileName ());
      throw 2;
    }
}

DsmccBiop::~DsmccBiop ()
{
  Thread::mutexLock (&dataMutex);
  releaseData ();
  closeModule ();
  Thread::mutexUnlock (&dataMutex);
  Thread::mutexDestroy (&dataMutex);
}

void
DsmccBiop::closeModule ()
{
  if (moduleFd != NULL)
    {
      fclose (moduleFd);
      moduleFd = NULL;
    }
}

void
DsmccBiop::createData (unsigned int dataSize)
{
  assert (dataSize > 0);

  releaseData ();
  data = new char[dataSize];
  memset (data, 0, dataSize);
}

void
DsmccBiop::releaseData ()
{
  if (data != NULL)
    {
      delete data;
      data = NULL;
    }
}

string
DsmccBiop::getStringFromData (unsigned int offset, unsigned int len)
{
  string strData = "";

  assert (len > 0);

  char *field = new char[len];
  memcpy (field, (void *)&(data[offset]), len);
  strData = (string)field;

  delete[] field;

  return strData;
}

string
DsmccBiop::getObjectKind ()
{
  return objectKind;
}

string
DsmccBiop::getObjectInfo ()
{
  return objectInfo;
}

void
DsmccBiop::abortProcess (const string &warningText)
{
  clog << "Warning! " << warningText.c_str () << endl;
  Thread::mutexLock (&dataMutex);
  closeModule ();
  releaseData ();
  Thread::mutexUnlock (&dataMutex);
}

bool
DsmccBiop::processMessageHeader ()
{
  int rval;
  unsigned int i;

  createData (12);
  i = 0;

  // BIOP::MessageHeader
  // Check magic Field == BIOP
  rval = (int) fread ((void *)&(data[0]), 1, 12, moduleFd);
  if (rval != 12)
    {
      ::abort ();
      return false;
    }

  if ((data[i] & 0xFF) != 0x42 || (data[i + 1] & 0xFF) != 0x49
      || (data[i + 2] & 0xFF) != 0x4F || (data[i + 3] & 0xFF) != 0x50)
    {
      ::abort ();
      return false;
    }

  i = i + 4;

  // Check biop_version field
  if ((data[i] & 0xFF) != 0x01 || (data[i + 1] & 0xFF) != 0x00)
    {
      abortProcess ("DsmccBiop::processMessageHeader Wrong biop_version");
      return false;
    }
  i = i + 2;

  // check byte_order field
  if ((data[i] & 0xFF) != 0x00)
    {
      abortProcess ("DsmccBiop::processMessageHeader Wrong byte_order");
      return false;
    }
  i++;

  // check message_type field
  if ((data[i] & 0xFF) != 0x00)
    {
      abortProcess ("DsmccBiop::processMessageHeader Wrong message_type");
      return false;
    }
  i++;

  // get message_size
  this->messageSize = ((data[i] & 0xFF) << 24)
                      | ((data[i + 1] & 0xFF) << 16)
                      | ((data[i + 2] & 0xFF) << 8) | (data[i + 3] & 0xFF);

  idx = 0;

  currentSize = currentSize + (messageSize + 12);
  if (currentSize >= module->getSize ())
    {
      hasMoreBiopMessage = false;
    }
  else
    {
      hasMoreBiopMessage = true;
    }

  releaseData ();
  return true;
}

int
DsmccBiop::processMessageSubHeader ()
{
  int rval;
  unsigned int len;

  createData (messageSize + 12);
  rval = (int) fread ((void *)&(data[0]), 1, messageSize, moduleFd);

  if ((unsigned int)rval != messageSize)
    {
      ::abort ();
      return -1;
    }

  // BIOP::MessageSubHeader

  // objectKey
  len = (data[idx] & 0xFF);
  idx++;

  if (len > 4)
    {
      clog << "Warning! Size of objectKey > 4, never reaches here!";
      clog << endl;
      idx = idx + len;
      return -2;
    }
  else if (len > 0)
    {
      if (len == 4)
        {
          objectKey
              = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
                | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);
        }
      else if (len == 3)
        {
          objectKey = ((data[idx] & 0xFF) << 16)
                      | ((data[idx + 1] & 0xFF) << 8)
                      | (data[idx + 2] & 0xFF);
        }
      else if (len == 2)
        {
          objectKey = ((data[idx] & 0xFF) << 8) | (data[idx + 1] & 0xFF);
        }
      else
        {
          objectKey = (data[idx] & 0xFF);
        }

      idx = idx + len;
    }

  // objectKind
  len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
        | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

  idx = idx + 4;

  if (len > 0)
    {
      this->objectKind = getStringFromData (idx, len);
      idx = idx + len;
    }
  else
    {
      clog << "Warning! No kind. Should never reaches here!" << endl;
      objectKind = "";
      return -3;
    }

  // objectInfo
  len = ((data[idx] & 0xFF) << 8) | (data[idx + 1] & 0xFF);
  idx = idx + 2;

  if (len > 0)
    {
      this->objectInfo = getStringFromData (idx, len);

      idx = idx + len;
    }
  else
    {
      objectInfo = "";
    }

  // skip last SubHeader field, named service_context
  idx++;
  return 0;
}

int
DsmccBiop::skipObject ()
{
  unsigned int len;

  // size of messageBody
  len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
        | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

  idx = idx + 4;

  // skip
  idx = idx + len;

  if (hasMoreBiopMessage)
    {
      isValidHdr = processMessageHeader ();
      if (isValidHdr)
        {
          if (processMessageSubHeader () < 0)
            {
              clog << "DsmccBiop::skipObject Warning! Invalid SubHeader"
                   << endl;
              return -1;
            }
        }
      else
        {
          clog << "DsmccBiop::skipObject Warning! Invalid Header" << endl;
          return -2;
        }
    }
  return 0;
}

int
DsmccBiop::processServiceGateway (unsigned int srgObjectKey)
{
  unsigned int i, len;
  DsmccBinding *binding;
  DsmccObject *carouselObject;

  Thread::mutexLock (&dataMutex);
  this->isValidHdr = processMessageHeader ();

  if (isValidHdr)
    {
      if (processMessageSubHeader () < 0)
        {
          Thread::mutexUnlock (&dataMutex);
          return -2;
        }
    }
  else
    {
      clog << "BIOP process SRG Warning! Invalid HDR" << endl;
      Thread::mutexUnlock (&dataMutex);
      return -1;
    }

  carouselObject = new DsmccObject ();

  while (objectKey != srgObjectKey)
    {
      if (skipObject () < 0)
        {
          clog << "error: object skipped." << endl;
        }
    }

  carouselObject->setCarouselId (module->getCarouselId ());
  carouselObject->setModuleId (module->getId ());
  carouselObject->setKey (objectKey);
  carouselObject->setKind (objectKind);

  // size of messageBody
  len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
        | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

  idx = idx + 4;

  // get Number of bindings
  len = (data[idx] & 0xFF) << 8 | (data[idx + 1] & 0xFF);
  idx = idx + 2;

  for (i = 0; i < len; i++)
    {
      binding = processBinding ();
      carouselObject->addBinding (binding);
    }

  processor->pushObject (carouselObject);

  rewind (moduleFd);
  Thread::mutexUnlock (&dataMutex);
  return 0;
}

DsmccBinding *
DsmccBiop::processBinding ()
{
  DsmccBinding *binding = new DsmccBinding ();
  unsigned int numberOfComponents, len;
  string strField;

  numberOfComponents = (data[idx] & 0xFF);
  idx++;

  if (numberOfComponents > 1)
    {
      clog << "Warning: numberOfComponents, Never reach here!!! NOC = ";
      clog << numberOfComponents << endl;
    }

  // binding
  // id_length
  len = (data[idx] & 0xFF);
  idx++;

  // id
  strField = getStringFromData (idx, len);
  binding->setId (strField);
  idx = idx + len;

  // kind_length
  len = (data[idx] & 0xFF);
  idx++;

  // kind
  strField = getStringFromData (idx, len);
  binding->setKind (strField);

  idx = idx + len;

  // bindingType
  len = (data[idx] & 0xFF);
  binding->setType (len);
  idx++;

  processIor (binding);

  // objectInfo
  len = ((data[idx] & 0xFF) << 8) | (data[idx + 1] & 0xFF);
  idx = idx + 2;

  if (len > 0)
    {
      strField = getStringFromData (idx, len);
      binding->setObjectInfo (strField);

      idx = idx + len;
    }

  return binding;
}

void
DsmccBiop::processIor (DsmccBinding *binding)
{
  DsmccIor *ior;
  unsigned int len, n1;
  string strField;

  ior = new DsmccIor ();

  // type_id_length
  n1 = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
       | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

  idx = idx + 4;

  // type_id
  strField = getStringFromData (idx, n1);
  ior->setTypeId (strField);
  idx = idx + n1;

  // CDR alignment rule
  if (n1 % 4 != 0)
    {
      for (unsigned int i = 0; i < (4 - (n1 % 4)); i++)
        {
          if ((data[idx] & 0xFF) != 0xFF)
            {
              clog << "CDR alignment Warning! gap must be 0xFF" << endl;
            }
          idx++;
        }
    }

  // check Number of TaggedProfiles (_count)
  len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
        | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

  idx = idx + 4;
  if (len > 1)
    {
      clog << "Warning: TaggedProfiles, never reach here!!! TP = ";
      clog << len << endl;
    }

  // get ior profile tag and check it
  len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
        | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

  idx = idx + 4;
  if (len != TAG_BIOP)
    {
      clog << "Warning: TAG_BIOP, never reach here!!!";
      clog << " TAG = " << len << endl;
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
        }
      idx++;

      // objectLocation
      // check if tag == "TAG_ObjectLocation"
      len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
            | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

      idx = idx + 4;

      if (len != TAG_BIOP_OBJECT_LOCATION)
        {
          clog << "Warning: TAG_ObjectLocation, never reach here!!!";
          clog << " TAG = " << len << endl;
        }
      else
        {
          // skip size of object data, we don't need it.
          idx++;

          // carousel_id
          len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
                | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

          ior->setCarouselId (len);
          idx = idx + 4;

          // module_id
          len = ((data[idx] & 0xFF) << 8) | (data[idx + 1] & 0xFF);
          ior->setModuleId (len);
          idx = idx + 2;

          // check version: major == 1 && minor == 0
          if (((data[idx] & 0xFF) != 0x01)
              || ((data[idx + 1] & 0xFF) != 0x00))
            {
              clog << "BIOP Warning: Obj version, never reach here!!!";
              clog << endl;

              clog << "current size '" << hex << currentSize;
              clog << "' current ix '";
              clog << hex << idx << "'" << endl;
            }

          idx = idx + 2;

          // objectKey_len
          len = (data[idx] & 0xFF);
          idx++;

          // if len > 4 then the unsigned int is not larger enough
          if (len > 4)
            {
              clog << "Warning! Size of";
              clog << " objectKey > 4, never reaches here!";
              clog << endl;
            }
          else if (len > 0)
            {
              unsigned int key;
              if (len == 4)
                {
                  key = ((data[idx] & 0xFF) << 24)
                        | ((data[idx + 1] & 0xFF) << 16)
                        | ((data[idx + 2] & 0xFF) << 8)
                        | (data[idx + 3] & 0xFF);
                }
              else if (len == 3)
                {
                  key = ((data[idx] & 0xFF) << 16)
                        | ((data[idx + 1] & 0xFF) << 8)
                        | (data[idx + 2] & 0xFF);
                }
              else if (len == 2)
                {
                  key = ((data[idx] & 0xFF) << 8) | (data[idx + 1] & 0xFF);
                }
              else
                {
                  key = (data[idx] & 0xFF);
                }
              ior->setObjectKey (key);
            }
          idx = idx + len;
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

  binding->setIor (ior);
}

void
DsmccBiop::processDirectory ()
{
  unsigned int i, len;
  DsmccBinding *binding;
  DsmccObject *carouselObject;

  carouselObject = new DsmccObject ();
  carouselObject->setCarouselId (module->getCarouselId ());
  carouselObject->setModuleId (module->getId ());
  carouselObject->setKey (objectKey);
  carouselObject->setKind (objectKind);

  // skip size of messageBody
  idx = idx + 4;

  // get Number of bindings
  len = ((data[idx] & 0xFF) << 8) | (data[idx + 1] & 0xFF);
  idx = idx + 2;

  for (i = 0; i < len; i++)
    {
      binding = processBinding ();
      carouselObject->addBinding (binding);
    }

  processor->pushObject (carouselObject);
}

void
DsmccBiop::processFile ()
{
  unsigned int len;
  DsmccObject *carouselObject;

  carouselObject = new DsmccObject ();
  carouselObject->setCarouselId (module->getCarouselId ());
  carouselObject->setModuleId (module->getId ());
  carouselObject->setKey (objectKey);
  carouselObject->setKind (objectKind);

  // skip size of messageBody
  idx = idx + 4;

  // get size of file
  len = ((data[idx] & 0xFF) << 24) | ((data[idx + 1] & 0xFF) << 16)
        | ((data[idx + 2] & 0xFF) << 8) | (data[idx + 3] & 0xFF);

  idx = idx + 4;

  // get file data
  char *fileData;

  fileData = new char[len];
  memcpy ((void *)&(fileData[0]), (void *)&(data[idx]), len);
  carouselObject->setData (fileData);
  carouselObject->setDataSize (len);

  processor->pushObject (carouselObject);

  // skip file data
  idx = idx + len;
}

void
DsmccBiop::print ()
{
  Thread::mutexLock (&dataMutex);

  clog << "BIOP" << endl;
  clog << "objectKind = " << objectKind.c_str () << endl;
  clog << "objectInfo = " << objectInfo.c_str () << endl;

  Thread::mutexUnlock (&dataMutex);
}

void
DsmccBiop::processObject ()
{
  if (objectKind == "srg" || objectKind == "DSM::ServiceGateway")
    {
      skipObject ();
      if (objectKind == "fil" || objectKind == "DSM::File")
        {
          processFile ();
        }
      else if (objectKind == "dir" || objectKind == "DSM::Directory")
        {
          processDirectory ();
        }
    }
  else if (objectKind == "fil" || objectKind == "DSM::File")
    {
      processFile ();
    }
  else if (objectKind == "dir" || objectKind == "DSM::Directory")
    {
      processDirectory ();
    }
}

int
DsmccBiop::process ()
{
  bool processed = false;

  do
    {
      Thread::mutexLock (&dataMutex);
      isValidHdr = processMessageHeader ();

      if (isValidHdr)
        {
          if (processMessageSubHeader () < 0)
            {
              return -1;
            }
          processObject ();
        }
      else
        {
          clog << "DsmccBiop::process Warning! Invalid Header" << endl;
        }

      if (hasMoreBiopMessage)
        {
          releaseData ();

          clog << module->getModuleFileName () << endl;
        }
      else
        {
          processed = true;
        }
      Thread::mutexUnlock (&dataMutex);
    }
  while (!processed);

  Thread::mutexLock (&dataMutex);
  releaseData ();
  closeModule ();
  Thread::mutexUnlock (&dataMutex);

  return 1;
}

GINGA_DATAPROC_END
