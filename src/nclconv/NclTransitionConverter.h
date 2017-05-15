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

#ifndef NclTransitionConverter_H
#define NclTransitionConverter_H

#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

#include "ncl/Transition.h"
#include "ncl/TransitionBase.h"
#include "ncl/TransitionUtil.h"
using namespace ::ginga::ncl;

#include "NclDocumentParser.h"

#include "NclTransitionParser.h"

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_NCLCONV_BEGIN

class NclTransitionConverter : public NclTransitionParser
{
public:
  NclTransitionConverter (NclDocumentParser *documentParser);
  void addTransitionToTransitionBase (void *parentObject,
                                      void *childObject);

  void *createTransitionBase (DOMElement *parentElement,
                              void *objGrandParent);

  void *createTransition (DOMElement *parentElement, void *objGrandParent);

  void addImportBaseToTransitionBase (void *parentObject,
                                      void *childObject);
};

GINGA_NCLCONV_END

#endif // NclTransitionConverter_H
