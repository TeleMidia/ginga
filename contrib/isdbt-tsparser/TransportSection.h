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

#ifndef TRANSPORTSECTION_H_
#define TRANSPORTSECTION_H_

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "util/functions.h"
using namespace ::ginga::util;

#include "ITransportSection.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#endif

GINGA_TSPARSER_BEGIN

class TransportSection : public ITransportSection
{
private:
  // Original TS packet id.
  unsigned int pid;

protected:
  // ISO/IEC 13818-1 TS SECTION HEADER
  unsigned int tableId;
  bool sectionSyntaxIndicator;

  /*
   * Number of bytes of the section, starting immediately following
   * the sectionLength field, and including CRC32. Thus, 9 bytes
   * of overhead.
   */
  unsigned int sectionLength;

  /*
   * Transport_stream_id for PAT
   * program_number for PMT
   * table_id_extension for DSM_CC Section
   */
  unsigned int idExtention;

  unsigned int versionNumber;
  bool currentNextIndicator;
  unsigned int sectionNumber;
  unsigned int lastSectionNumber;

  // Section data.
  char section[4096];

  unsigned int currentSize;

  string sectionName;

public:
  TransportSection ();
  TransportSection (char *sectionBytes, unsigned int size);

  virtual ~TransportSection ();

private:
  void initialize ();

public:
  void setESId (unsigned int id);
  unsigned int getESId ();
  void addData (char *bytes, unsigned int size);

private:
  void setSectionName ();
  bool create (char *sectionBytes, unsigned int size);
  bool constructionFailed;

public:
  string getSectionName ();
  unsigned int getTableId ();
  bool getSectionSyntaxIndicator ();
  unsigned int getSectionLength ();
  unsigned int getCurrentSize ();
  unsigned int getExtensionId ();
  unsigned int getVersionNumber ();
  bool getCurrentNextIndicator ();
  unsigned int getSectionNumber ();
  unsigned int getLastSectionNumber ();
  void *getPayload ();
  unsigned int getPayload (char **buffer);
  unsigned int getPayloadSize ();
  bool isConsolidated ();
  bool isConstructionFailed ();
  char *getSection ();
  static unsigned int crc32 (char *data, unsigned int len);
  void print ();
};

GINGA_TSPARSER_END

#endif /*TRANSPORTSECTION_H_*/
