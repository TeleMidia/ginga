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

#include "ginga.h"
#include "NclMetainformationParser.h"

GINGA_NCLCONV_BEGIN

NclMetainformationParser::NclMetainformationParser (
    NclDocumentParser *documentParser)
    : ModuleParser (documentParser)
{
}

Meta *
NclMetainformationParser::parseMeta (DOMElement *parentElement,
                                     arg_unused (void *objGrandParent))
{
  string name, content;

  if (parentElement->hasAttribute (XMLString::transcode ("name")))
    {
      name = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("name")));
    }
  else
    {
      syntax_error ("meta: missing name");
    }

  if (parentElement->hasAttribute (XMLString::transcode ("content")))
    {
      content = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("content")));
    }
  else
    {
      syntax_error ("meta: missing content");
    }

  return new Meta (name, deconst (void *, content.c_str ()));
}

Metadata *
NclMetainformationParser::parseMetadata (arg_unused (DOMElement *parentElement),
                                         arg_unused (void *objGrandParent) )
{
  return new Metadata ();
}

GINGA_NCLCONV_END
