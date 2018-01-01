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

#ifndef PARSER_LIBXML_H
#define PARSER_LIBXML_H

#include "NclDocument.h"

GINGA_NAMESPACE_BEGIN

class ParserLibXML
{
public:
  static NclDocument *parseBuffer (const void *, size_t, int, int,
                                   string *);
  static NclDocument *parseFile (const string &, int, int, string *);
};

GINGA_NAMESPACE_END

#endif
