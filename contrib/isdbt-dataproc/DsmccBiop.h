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

#ifndef BIOP_H_
#define BIOP_H_

#include "util/functions.h"
using namespace ::ginga::util;

#include "system/Thread.h"
using namespace ::ginga::system;

#include "DsmccObjectProcessor.h"
#include "DsmccModule.h"

GINGA_DATAPROC_BEGIN

class DsmccBiop
{
private:
  // DsmccBiop Attributes
  DsmccModule *module;
  unsigned int idx;
  unsigned int currentSize;
  FILE *moduleFd;
  bool isValidHdr;
  bool hasMoreBiopMessage;
  char *data;

  // MessageHeader
  unsigned int messageSize;

  // MessageSubHeader
  unsigned int objectKey;
  string objectKind;
  string objectInfo;

  // MessageBody
  map<string, DsmccObject *> objects;

  DsmccObjectProcessor *processor;

  pthread_mutex_t dataMutex;

public:
  DsmccBiop (DsmccModule *module, DsmccObjectProcessor *processor);
  virtual ~DsmccBiop ();

private:
  void closeModule ();
  void createData (unsigned int dataSize);
  void releaseData ();

  string getStringFromData (unsigned int offset, unsigned int len);

  string getObjectKind ();
  string getObjectInfo ();

  void abortProcess (const string &warningText);
  bool processServiceContext ();
  bool processMessageHeader ();
  int processMessageSubHeader ();
  int skipObject ();

public:
  int processServiceGateway (unsigned int srgObjectKey);

private:
  DsmccBinding *processBinding ();
  void processIor (DsmccBinding *binding);

  void processDirectory ();
  void processFile ();

public:
  void print ();

private:
  void processObject ();

public:
  int process ();
};

GINGA_DATAPROC_END

#endif /*BIOP_H_*/
