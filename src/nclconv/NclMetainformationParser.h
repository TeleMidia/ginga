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

#ifndef NCLMETAINFORMATIONPARSER_H_
#define NCLMETAINFORMATIONPARSER_H_

#include "ModuleParser.h"
#include "DocumentParser.h"
using namespace ::ginga::nclconv;

XERCES_CPP_NAMESPACE_USE


GINGA_NCLCONV_BEGIN

	class NclMetainformationParser : public ModuleParser {
		public:
			NclMetainformationParser(DocumentParser *documentParser);
			void *parseMeta(DOMElement *parentElement, void *objGrandParent);

			virtual void* createMeta(
				    DOMElement *parentElement, void *objGrandParent)=0;

			void *parseMetadata(
				    DOMElement *parentElement, void *objGrandParent);

			virtual void* createMetadata(
				    DOMElement *parentElement, void *objGrandParent)=0;
	};

GINGA_NCLCONV_END
#endif //NCLMETAINFORMATIONPARSER_H_
