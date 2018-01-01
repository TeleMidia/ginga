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

#ifndef _Metadata_H_
#define _Metadata_H_

#include "INCLMetadata.h"

GINGA_DATAPROC_BEGIN

class NCLMetadata : public INCLMetadata
{
private:
  ofstream *mdFile;
  string name;
  double totalLength;
  double targetTotalLen;
  string baseUri;
  string mdUri;
  INCLDataFile *rootObject;
  map<int, INCLDataFile *> *dataFiles;

public:
  NCLMetadata (const string &name);
  virtual ~NCLMetadata ();

  string getRootUri ();
  string getName ();
  double getTotalLength ();
  void setTargetTotalLength (double targetSize);
  bool isConsolidated ();

  void setBaseUri (const string &uri);
  string getBaseUri ();

  void setRootObject (INCLDataFile *rootObject);
  INCLDataFile *getRootObject ();

  void addDataFile (INCLDataFile *dataObject);
  INCLDataFile *getDataFile (int structureId);
  map<int, INCLDataFile *> *getDataFiles ();

private:
  bool updateTotalLength (INCLDataFile *dataFile);

public:
  vector<StreamData *> *createNCLSections ();

private:
  bool createMetadataFile ();
  void closeMetadataFile ();

  void openMetadataElement ();
  void closeMetadataElement ();

  void openBaseDataElement ();
  void closeBaseDataElement ();

  void writeRootElement (const string &sId, const string &uri, const string &size,
                         string componentTag);

  void writeDataElement (const string &sId, const string &uri, const string &size,
                         string componentTag);

  void copyContent (const string &uri, char *stream, int fileSize);

  StreamData *createStreamData (int structId, int structType, const string &uri,
                                int fileSize);

public:
  static int getFileSize (const string &uri);
};

GINGA_DATAPROC_END

#endif //_Metadata_H_
