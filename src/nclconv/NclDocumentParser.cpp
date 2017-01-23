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
#include "NclDocumentParser.h"

GINGA_NCLCONV_BEGIN

NclDocumentParser::NclDocumentParser () : DocumentParser ()
{
  this->deviceLayout = NULL;
  this->presentationSpecificationParser = NULL;
  this->structureParser = NULL;
  this->componentsParser = NULL;
  this->connectorsParser = NULL;
  this->linkingParser = NULL;
  this->interfacesParser = NULL;
  this->layoutParser = NULL;
  this->presentationControlParser = NULL;
  this->importParser = NULL;
  this->metainformationParser = NULL;
  this->transitionParser = NULL;
}

NclDocumentParser::~NclDocumentParser () {}

void
NclDocumentParser::setDeviceLayout (DeviceLayout *deviceLayout)
{
  this->deviceLayout = deviceLayout;
}

void
NclDocumentParser::setDependencies ()
{
  getConnectorsParser ()->setImportParser (importParser);
  getPresentationControlParser ()->setPresentationSpecificationParser (
      presentationSpecificationParser);

  getPresentationControlParser ()->setComponentsParser (componentsParser);
  getPresentationControlParser ()->setInterfacesParser (interfacesParser);
  getPresentationControlParser ()->setImportParser (importParser);
  getComponentsParser ()->setPresentationSpecificationParser (
      presentationSpecificationParser);

  getComponentsParser ()->setLinkingParser (linkingParser);
  getComponentsParser ()->setInterfacesParser (interfacesParser);
  getComponentsParser ()->setPresentationControlParser (
      presentationControlParser);

  getStructureParser ()->setPresentationSpecificationParser (
      presentationSpecificationParser);

  getStructureParser ()->setComponentsParser (componentsParser);
  getStructureParser ()->setLinkingParser (linkingParser);
  getStructureParser ()->setLayoutParser (layoutParser);
  getStructureParser ()->setInterfacesParser (interfacesParser);
  getStructureParser ()->setPresentationControlParser (
      presentationControlParser);

  getStructureParser ()->setConnectorsParser (connectorsParser);
  getStructureParser ()->setImportParser (importParser);
  getStructureParser ()->setTransitionParser (transitionParser);
  getStructureParser ()->setMetainformationParser (metainformationParser);
  getPresentationSpecificationParser ()->setImportParser (importParser);
  getPresentationSpecificationParser ()->setPresentationControlParser (
      presentationControlParser);

  getLayoutParser ()->setImportParser (importParser);
  getTransitionParser ()->setImportParser (importParser);
}

NclTransitionParser *
NclDocumentParser::getTransitionParser ()
{
  return transitionParser;
}

NclConnectorsParser *
NclDocumentParser::getConnectorsParser ()
{
  return connectorsParser;
}

void
NclDocumentParser::setConnectorsParser (
    NclConnectorsParser *connectorsParser)
{
  this->connectorsParser = connectorsParser;
}

NclImportParser *
NclDocumentParser::getImportParser ()
{
  return importParser;
}

void
NclDocumentParser::setBaseReuseParser (NclImportParser *importParser)
{
  this->importParser = importParser;
}

NclPresentationControlParser *
NclDocumentParser::getPresentationControlParser ()
{
  return presentationControlParser;
}

void
NclDocumentParser::setPresentationControlParser (
    NclPresentationControlParser *presentationControlParser)
{
  this->presentationControlParser = presentationControlParser;
}

NclComponentsParser *
NclDocumentParser::getComponentsParser ()
{
  return componentsParser;
}

void
NclDocumentParser::setComponentsParser (
    NclComponentsParser *componentsParser)
{
  this->componentsParser = componentsParser;
}

NclStructureParser *
NclDocumentParser::getStructureParser ()
{
  return structureParser;
}

void
NclDocumentParser::setStructureParser (NclStructureParser *structureParser)
{
  this->structureParser = structureParser;
}

NclPresentationSpecificationParser *
NclDocumentParser::getPresentationSpecificationParser ()
{
  return presentationSpecificationParser;
}

void
NclDocumentParser::setPresentationSpecificationParser (
    NclPresentationSpecificationParser *presentationSpecificationParser)
{
  this->presentationSpecificationParser = presentationSpecificationParser;
}

NclLayoutParser *
NclDocumentParser::getLayoutParser ()
{
  return layoutParser;
}

void
NclDocumentParser::setLayoutParser (NclLayoutParser *layoutParser)
{
  this->layoutParser = layoutParser;
}

NclInterfacesParser *
NclDocumentParser::getInterfacesParser ()
{
  return interfacesParser;
}

void
NclDocumentParser::setInterfacesParser (
    NclInterfacesParser *interfacesParser)
{
  this->interfacesParser = interfacesParser;
}

NclMetainformationParser *
NclDocumentParser::getMetainformationParser ()
{
  return metainformationParser;
}

void
NclDocumentParser::setMetainformationParser (
    NclMetainformationParser *metainformationParser)
{
  this->metainformationParser = metainformationParser;
}

NclLinkingParser *
NclDocumentParser::getLinkingParser ()
{
  return linkingParser;
}

void
NclDocumentParser::setLinkingParser (NclLinkingParser *linkingParser)
{
  this->linkingParser = linkingParser;
}

void *
NclDocumentParser::parseRootElement (DOMElement *rootElement)
{
  string tagName;
  tagName = XMLString::transcode (rootElement->getTagName ());
  if (tagName == "ncl")
    {
      return getStructureParser ()->parseNcl (rootElement, NULL);
    }
  else
    {
      clog << "NclDocumentParser::parseRootElement Warning!";
      clog << " Trying to parse a non NCL document. rootElement";
      clog << "->getTagName = '" << tagName.c_str () << "'" << endl;
      return NULL;
    }
}

GINGA_NCLCONV_END
