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

#ifndef _NCLMetadataParser_H_
#define _NCLMetadataParser_H_

#include "INCLMetadata.h"

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_DATAPROC_BEGIN

class NCLMetadataParser
{
public:
  static INCLMetadata *parse (const string &xmlDocument);
  static INCLMetadata *parse (char *xmlStream, int streamSize);

private:
  static void startElementHandler (void *data, const XML_Char *element,
                                   const XML_Char **attrs);

  static void parseMetadata (void *data, const XML_Char **attrs);
  static void parseBaseData (void *data, const XML_Char **attrs);
  static void parseRoot (void *data, const XML_Char **attrs);
  static void parseData (void *data, const XML_Char **attrs);

  static INCLDataFile *createObject (void *data, const XML_Char **attrs);

  static void stopElementHandler (void *data, const XML_Char *element);
};

GINGA_DATAPROC_END

#endif //_NCLMetadataParser_H_
