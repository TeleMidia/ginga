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
#include "NCLMetadata.h"

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_DATAPROC_BEGIN

NCLMetadata::NCLMetadata (const string &name)
{
  this->name = name;
  this->targetTotalLen = 0;
  this->totalLength = 0;
  this->baseUri = "";
  this->mdUri = "";
  this->rootObject = NULL;
  this->dataFiles = new map<int, INCLDataFile *>;
}

NCLMetadata::~NCLMetadata ()
{
  map<int, INCLDataFile *>::iterator i;

  if (dataFiles != NULL)
    {
      i = dataFiles->begin ();
      while (i != dataFiles->end ())
        {
          delete i->second;
          ++i;
        }

      delete dataFiles;
      dataFiles = NULL;
    }
}

string
NCLMetadata::getRootUri ()
{
  string rootUri = "";

  if (rootObject != NULL)
    {
      rootUri = rootObject->getUri ();
      if (!SystemCompat::isAbsolutePath (rootUri))
        {
          rootUri = SystemCompat::updatePath (
              baseUri + SystemCompat::getIUriD () + rootUri);
        }
    }

  return rootUri;
}

string
NCLMetadata::getName ()
{
  return this->name;
}

double
NCLMetadata::getTotalLength ()
{
  return this->totalLength;
}

void
NCLMetadata::setTargetTotalLength (double targetSize)
{
  this->targetTotalLen = targetSize;
}

bool
NCLMetadata::isConsolidated ()
{
  bool consolidated = false;

  if (xnumeq (targetTotalLen, totalLength))
    {
      consolidated = true;
    }

  return consolidated;
}

void
NCLMetadata::setBaseUri (const string &uri)
{
  this->baseUri = uri;
  this->mdUri = SystemCompat::updatePath (baseUri + "/metadata.xml");
}

string
NCLMetadata::getBaseUri ()
{
  return baseUri;
}

void
NCLMetadata::setRootObject (INCLDataFile *rootObject)
{
  if (updateTotalLength (rootObject))
    {
      this->rootObject = rootObject;
    }
}

INCLDataFile *
NCLMetadata::getRootObject ()
{
  return rootObject;
}

void
NCLMetadata::addDataFile (INCLDataFile *dataFile)
{
  updateTotalLength (dataFile);
  (*dataFiles)[dataFile->getId ()] = dataFile;
}

INCLDataFile *
NCLMetadata::getDataFile (int structureId)
{
  map<int, INCLDataFile *>::iterator i;
  INCLDataFile *dataFile = NULL;

  i = dataFiles->find (structureId);
  if (i != dataFiles->end ())
    {
      dataFile = i->second;
    }

  return dataFile;
}

map<int, INCLDataFile *> *
NCLMetadata::getDataFiles ()
{
  return dataFiles;
}

bool
NCLMetadata::updateTotalLength (INCLDataFile *dataFile)
{
  if (rootObject != dataFile && dataFiles->count (dataFile->getId ()) == 0)
    {
      totalLength = totalLength + dataFile->getSize ();
      return true;
    }
  return false;
}

vector<StreamData *> *
NCLMetadata::createNCLSections ()
{
  map<int, INCLDataFile *>::iterator i;
  vector<StreamData *> *streams;
  string fileUri, cTag;
  int structId, fileSize;
  INCLDataFile *dataFile;
  StreamData *streamData;

  streams = new vector<StreamData *>;

  createMetadataFile ();
  openMetadataElement ();
  openBaseDataElement ();

  structId = rootObject->getId ();
  fileUri = rootObject->getUri ();
  fileSize = (int) rootObject->getSize ();
  cTag = rootObject->getCopmonentTag ();

  writeDataElement (xstrbuild ("%d", structId), fileUri, xstrbuild ("%d", fileSize), cTag);
  streamData = createStreamData (structId, ST_DATAFILE, fileUri, fileSize);

  streams->push_back (streamData);

  i = dataFiles->begin ();
  while (i != dataFiles->end ())
    {
      structId = i->first;
      dataFile = i->second;
      fileUri = dataFile->getUri ();
      fileSize = (int) dataFile->getSize ();
      cTag = dataFile->getCopmonentTag ();

      writeDataElement (xstrbuild ("%d", structId), fileUri, xstrbuild ("%d", fileSize), cTag);

      streamData
          = createStreamData (structId, ST_DATAFILE, fileUri, fileSize);

      streams->push_back (streamData);
      ++i;
    }

  closeBaseDataElement ();
  closeMetadataElement ();
  closeMetadataFile ();

  fileSize = getFileSize (mdUri);
  streamData = createStreamData (0, ST_METADATA, mdUri, fileSize);
  streams->insert (streams->begin (), streamData);

  return streams;
}

bool
NCLMetadata::createMetadataFile ()
{
  remove (mdUri.c_str ());

  mdFile = new ofstream (mdUri.c_str (), ios::out | ios::binary);

  if (!mdFile)
    {
      clog << "NCLMetadata::createMetadataFile warning: can't create '";
      clog << mdUri << "'" << endl;
      return false;
    }
  else
    {
      mdFile->seekp (ofstream::end);
      if (mdFile->tellp () > 2)
        {
          clog << "ComponentDescription::createFile warning: opening ";
          clog << " existent file '" << mdFile << "' pos = '";
          clog << mdFile->tellp () << "'" << endl;
          return false;
        }
      mdFile->seekp (0);
    }

  return true;
}

void
NCLMetadata::closeMetadataFile ()
{
  mdFile->close ();
  delete mdFile;
  mdFile = NULL;
}

void
NCLMetadata::openMetadataElement ()
{
  *mdFile << "<metadata name=\"" << name << "\" size=\"";
  *mdFile << totalLength << "\">" << endl;
}

void
NCLMetadata::closeMetadataElement ()
{
  *mdFile << "</metadata>" << endl;
}

void
NCLMetadata::openBaseDataElement ()
{
  *mdFile << "  <baseData uri=\"" << baseUri << "\">" << endl;
}

void
NCLMetadata::closeBaseDataElement ()
{
  *mdFile << "  </baseData>" << endl;
}

void
NCLMetadata::writeRootElement (const string &sId, const string &uri, const string &size,
                               string componentTag)
{
  *mdFile << "    <pushedRoot structureId=\"" << sId << "\" uri=\"";
  *mdFile << uri << "\" size=\"" << size << "\" component_tag=\"";
  *mdFile << componentTag << "\"/>" << endl;
}

void
NCLMetadata::writeDataElement (const string &sId, const string &uri, const string &size,
                               string componentTag)
{
  *mdFile << "    <pushedData structureId=\"" << sId << "\" uri=\"";
  *mdFile << uri << "\" size=\"" << size << "\" component_tag=\"";
  *mdFile << componentTag << "\"/>" << endl;
}

void
NCLMetadata::copyContent (const string &uri, char *stream, int fileSize)
{
  FILE *fd;
  int bytes;
  string absUri;

  if (SystemCompat::isAbsolutePath (uri))
    {
      absUri = uri;
    }
  else
    {
      absUri = SystemCompat::updatePath (baseUri + SystemCompat::getIUriD ()
                                         + uri);
    }

  fd = fopen (absUri.c_str (), "rb");
  if (fd != NULL)
    {
      bytes = (int) fread (stream, 1, fileSize, fd);
      if (bytes != fileSize)
        {
          clog << "NCLMetadata::copyContent Warning! Can't read '";
          clog << fileSize << "' from file '" << absUri << "' (";
          clog << bytes << " bytes read)" << endl;
        }
      fclose (fd);
    }
  else
    {
      clog << "NCLMetadata::copyContent Warning! Can't open file '";
      clog << absUri << "'" << endl;
    }
}

StreamData *
NCLMetadata::createStreamData (int structId, int structType, const string &uri,
                               int fileSize)
{
  StreamData *streamData;
  char *stream = new char[fileSize + 2];

  stream[0] = (((unsigned int)structType) & 0xFF);
  stream[1] = (((unsigned int)structId) & 0xFF);

  copyContent (uri, stream + 2, fileSize);

  streamData = new StreamData;
  streamData->stream = stream;
  streamData->size = fileSize + 2;

  return streamData;
}

int
NCLMetadata::getFileSize (const string &uri)
{
  FILE *fd;
  int bytes = 0;

  fd = fopen (uri.c_str (), "rb");
  if (fd != NULL)
    {
      fseek (fd, 0L, SEEK_END);
      bytes = ftell (fd);
      fclose (fd);
    }

  return bytes;
}

GINGA_DATAPROC_END
