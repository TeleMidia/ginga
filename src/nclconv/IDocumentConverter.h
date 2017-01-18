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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef IDocumentConverter_H
#define IDocumentConverter_H

#include "ncl/NclDocument.h"
#include "ncl/IPrivateBaseContext.h"
using namespace ::ginga::ncl;

#include <string>
using namespace std;

GINGA_NCLCONV_BEGIN

  class IDocumentConverter {
	public:
		virtual ~IDocumentConverter(){};

		virtual void setConverterInfo(
				IPrivateBaseContext* pbc, IDeviceLayout* deviceLayout)=0;
		virtual string getAttribute(void* element, string attribute)=0;
		virtual void* getObject(string tableName, string key)=0;
		virtual void* parse(string uri, string iUriD, string fUriD)=0;
		virtual void* parseEntity(
				string entityLocation, NclDocument* document, void* parent)=0;
  };

GINGA_NCLCONV_END
#endif //IDocumentConverter_H
