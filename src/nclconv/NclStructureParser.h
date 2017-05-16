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

#ifndef NCLSTRUCTUREPARSER_H_
#define NCLSTRUCTUREPARSER_H_

#include "ModuleParser.h"
#include "NclDocumentParser.h"

#include "NclTransitionParser.h"
#include "NclPresentationSpecificationParser.h"
#include "NclComponentsParser.h"
#include "NclLinkingParser.h"
#include "NclLayoutParser.h"
#include "NclInterfacesParser.h"
#include "NclPresentationControlParser.h"
#include "NclConnectorsParser.h"
#include "NclImportParser.h"
#include "NclMetainformationParser.h"

GINGA_NCLCONV_BEGIN

class NclStructureParser : public ModuleParser
{
protected:
  NclTransitionParser *transitionParser;
  NclPresentationSpecificationParser *presentationSpecificationParser;

  NclComponentsParser *componentsParser;
  NclLinkingParser *linkingParser;
  NclLayoutParser *layoutParser;
  NclInterfacesParser *interfacesParser;
  NclPresentationControlParser *presentationControlParser;
  NclConnectorsParser *connectorsParser;
  NclImportParser *importParser;
  NclMetainformationParser *metainformationParser;

public:
  NclStructureParser (NclDocumentParser *documentParser);

  void *parseBody (DOMElement *parentElement, void *objGrandParent);

  void *posCompileBody (DOMElement *parentElement, void *parentObject);

  void *createBody (DOMElement *parentElement, void *objGrandParent);

  void *parseHead (DOMElement *parentElement, void *objGrandParent);

  void addRegionBaseToHead (void *parentObject, void *childObject);

  void addDescriptorBaseToHead (void *parentObject, void *childObject);

  void addTransitionBaseToHead (void *parentObject, void *childObject);

  void addRuleBaseToHead (void *parentObject, void *childObject);

  void addConnectorBaseToHead (void *parentObject, void *childObject);

  void addImportedDocumentBaseToHead (void *parentObject, void *childObject);

  void addMetaToHead (void *parentObject, void *childObject);

  void addMetadataToHead (void *parentObject, void *childObject);

  void *parseNcl (DOMElement *parentElement, void *objGrandParent);

  void *createNcl (DOMElement *parentElement, void *objGrandParent);

  void addBodyToNcl (void *parentObject, void *childObject);

  NclTransitionParser *getTransitionParser ();
  void setTransitionParser (NclTransitionParser *transitionParser);

  NclPresentationSpecificationParser *getPresentationSpecificationParser ();

  void setPresentationSpecificationParser (
      NclPresentationSpecificationParser *presentationSpecificationParser);

  NclComponentsParser *getComponentsParser ();
  void setComponentsParser (NclComponentsParser *componentsParser);
  NclLinkingParser *getLinkingParser ();
  void setLinkingParser (NclLinkingParser *linkingParser);
  NclLayoutParser *getLayoutParser ();
  void setLayoutParser (NclLayoutParser *layoutParser);
  NclMetainformationParser *getMetainformationParser ();
  NclInterfacesParser *getInterfacesParser ();
  void setInterfacesParser (NclInterfacesParser *interfacesParser);
  NclPresentationControlParser *getPresentationControlParser ();

  void setPresentationControlParser (
      NclPresentationControlParser *presentationControlParser);

  NclConnectorsParser *getConnectorsParser ();
  void setConnectorsParser (NclConnectorsParser *connectorsParser);
  NclImportParser *getImportParser ();
  void setImportParser (NclImportParser *importParser);
  void setMetainformationParser (
      NclMetainformationParser *metainformationParser);

private:
  void solveNodeReferences (CompositeNode *composition);
};

GINGA_NCLCONV_END

#endif /*NCLSTRUCTUREPARSER_H_*/
