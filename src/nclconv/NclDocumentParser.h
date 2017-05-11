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

#ifndef NCLDOCUMENTPARSER_H_
#define NCLDOCUMENTPARSER_H_

#include "DocumentParser.h"

#include "NclConnectorsParser.h"
#include "NclImportParser.h"
#include "NclTransitionParser.h"
#include "NclPresentationControlParser.h"
#include "NclComponentsParser.h"
#include "NclStructureParser.h"
#include "NclPresentationSpecificationParser.h"
#include "NclLayoutParser.h"
#include "NclInterfacesParser.h"
#include "NclLinkingParser.h"
#include "NclMetainformationParser.h"

#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

GINGA_NCLCONV_BEGIN

class NclDocumentParser : public DocumentParser
{
protected:
  NclConnectorsParser *connectorsParser;
  NclImportParser *importParser;
  NclTransitionParser *transitionParser;
  NclPresentationControlParser *presentationControlParser;
  NclComponentsParser *componentsParser;
  NclStructureParser *structureParser;
  NclPresentationSpecificationParser *presentationSpecificationParser;
  NclLayoutParser *layoutParser;
  NclInterfacesParser *interfacesParser;
  NclLinkingParser *linkingParser;
  NclMetainformationParser *metainformationParser;
  DeviceLayout *deviceLayout;

public:
  NclDocumentParser ();
  virtual ~NclDocumentParser ();

  void setDeviceLayout (DeviceLayout *deviceLayout);

protected:
  void setDependencies ();

public:
  NclTransitionParser *getTransitionParser ();
  NclConnectorsParser *getConnectorsParser ();
  void setConnectorsParser (NclConnectorsParser *connectorsParser);
  NclImportParser *getImportParser ();
  void setBaseReuseParser (NclImportParser *importParser);
  NclPresentationControlParser *getPresentationControlParser ();
  void setPresentationControlParser (
      NclPresentationControlParser *presentationControlParser);

  NclComponentsParser *getComponentsParser ();
  void setComponentsParser (NclComponentsParser *componentsParser);
  NclStructureParser *getStructureParser ();
  void setStructureParser (NclStructureParser *structureParser);
  NclPresentationSpecificationParser *getPresentationSpecificationParser ();

  void setPresentationSpecificationParser (
      NclPresentationSpecificationParser *presentationSpecificationParser);

  NclLayoutParser *getLayoutParser ();
  void setLayoutParser (NclLayoutParser *layoutParser);
  NclInterfacesParser *getInterfacesParser ();
  void setInterfacesParser (NclInterfacesParser *interfacesParser);
  NclMetainformationParser *getMetainformationParser ();
  void setMetainformationParser (
      NclMetainformationParser *metainformationParser);

  NclLinkingParser *getLinkingParser ();
  void setLinkingParser (NclLinkingParser *linkingParser);

protected:
  virtual void *parseRootElement (DOMElement *rootElement);
  virtual void initialize () = 0;
};

GINGA_NCLCONV_END

#endif /*NCLDOCUMENTPARSER_H_*/
