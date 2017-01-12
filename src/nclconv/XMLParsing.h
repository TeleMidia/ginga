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

#ifndef XMLPARSING_H
#define XMLPARSING_H

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax/SAXException.hpp>

XERCES_CPP_NAMESPACE_USE

#include "DOMTreeErrorReporter.h"
#include "stdlib.h"

#include <string>
using namespace std;

// boilerplate DOM loading example.

//singleton pattern
class XMLParsing {
      private:
         XMLParsing();
         static XMLParsing *myInstance;
      public:
         static DOMNode *parse(string src);
         ~XMLParsing();
        
};

#endif
