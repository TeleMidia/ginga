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

#include "config.h"
#include "NclDocumentConverter.h"
#include "NclConnectorsConverter.h"
#include "NclImportConverter.h"
#include "NclPresentationControlConverter.h"
#include "NclComponentsConverter.h"
#include "NclStructureConverter.h"
#include "NclPresentationSpecConverter.h"
#include "NclLayoutConverter.h"
#include "NclInterfacesConverter.h"
#include "NclLinkingConverter.h"
#include "NclTransitionConverter.h"
#include "NclMetainformationConverter.h"

GINGA_NCLCONV_BEGIN

NclDocumentConverter::NclDocumentConverter () : NclDocumentParser ()
{
  this->parentObject = NULL;
  this->privateBaseContext = NULL;
  this->ownManager = false;
  this->parseEntityVar = false;
}

NclDocumentConverter::~NclDocumentConverter ()
{
  if (presentationSpecificationParser != NULL)
    {
      delete presentationSpecificationParser;
      presentationSpecificationParser = NULL;
    }

  if (structureParser != NULL)
    {
      delete structureParser;
      structureParser = NULL;
    }

  if (componentsParser != NULL)
    {
      delete componentsParser;
      componentsParser = NULL;
    }

  if (connectorsParser != NULL)
    {
      delete connectorsParser;
      connectorsParser = NULL;
    }

  if (linkingParser != NULL)
    {
      delete linkingParser;
      linkingParser = NULL;
    }

  if (interfacesParser != NULL)
    {
      delete interfacesParser;
      interfacesParser = NULL;
    }

  if (layoutParser != NULL)
    {
      delete layoutParser;
      layoutParser = NULL;
    }

  if (transitionParser != NULL)
    {
      delete transitionParser;
      transitionParser = NULL;
    }

  if (presentationControlParser != NULL)
    {
      delete presentationControlParser;
      presentationControlParser = NULL;
    }

  if (importParser != NULL)
    {
      delete importParser;
      importParser = NULL;
    }

  if (metainformationParser != NULL)
    {
      delete metainformationParser;
      metainformationParser = NULL;
    }
}

void
NclDocumentConverter::setConverterInfo (IPrivateBaseContext *pbc,
                                        DeviceLayout *deviceLayout)
{

  setDeviceLayout (deviceLayout);
  this->privateBaseContext = pbc;

  setDocumentPath (getUserCurrentPath () + getIUriD ());
  initialize ();
  setDependencies ();
}

void
NclDocumentConverter::initialize ()
{
  presentationSpecificationParser
      = new NclPresentationSpecConverter (this, deviceLayout);

  structureParser = new NclStructureConverter (this);
  componentsParser = new NclComponentsConverter (this);
  connectorsParser = new NclConnectorsConverter (this);
  linkingParser = new NclLinkingConverter (
      this, (NclConnectorsConverter *)connectorsParser);

  interfacesParser = new NclInterfacesConverter (this);
  layoutParser = new NclLayoutConverter (this, deviceLayout);
  transitionParser = new NclTransitionConverter (this);
  presentationControlParser = new NclPresentationControlConverter (this);
  importParser = new NclImportConverter (this);
  metainformationParser = new NclMetainformationConverter (this);
}

string
NclDocumentConverter::getAttribute (void *element, string attribute)
{

  return XMLString::transcode (
      ((DOMElement *)element)
          ->getAttribute (XMLString::transcode (attribute.c_str ())));
}

Node *
NclDocumentConverter::getNode (string nodeId)
{
  NclDocument *document;

  document
      = (NclDocument *)NclDocumentParser::getObject ("return", "document");

  return document->getNode (nodeId);
}

bool
NclDocumentConverter::removeNode (Node *node)
{
  NclDocument *document;
  document
      = (NclDocument *)NclDocumentParser::getObject ("return", "document");

  // return document->removeDocument()
  return true;
}

IPrivateBaseContext *
NclDocumentConverter::getPrivateBaseContext ()
{
  return privateBaseContext;
}

NclDocument *
NclDocumentConverter::importDocument (string *docLocation)
{
  string uri;
  string::size_type pos;

  uri = *docLocation;
  while (true)
    {
      pos = uri.find_first_of (getFUriD ());
      if (pos == string::npos)
        {
          break;
        }
      uri.replace (pos, 1, iUriD);
    }

  if (!isAbsolutePath (uri))
    {
      if (uri.find_first_of (getIUriD ()) == std::string::npos)
        {
          uri = getAbsolutePath (uri) + getIUriD () + uri;
        }
      else
        {
          uri = getAbsolutePath (uri)
                + uri.substr (uri.find_last_of (getIUriD ()),
                              uri.length ()
                                  - uri.find_last_of (getIUriD ()));
        }
    }

  clog << "NclDocumentConverter::importDocument location '";
  clog << *docLocation << "' translated URI = '" << uri << "'";
  clog << endl;

  *docLocation = uri;
  return (NclDocument *)(privateBaseContext->addVisibleDocument (
      uri, deviceLayout));
}

void *
NclDocumentConverter::parseEntity (string entityLocation,
                                   NclDocument *document, void *parent)
{

  void *entity;

  parseEntityVar = true;
  parentObject = parent;
  addObject ("return", "document", document);
  entity = parse (entityLocation, iUriD, fUriD);
  parseEntityVar = false;
  return entity;
}

void *
NclDocumentConverter::parseRootElement (DOMElement *rootElement)
{
  string elementName;
  void *object;

  if (parseEntityVar)
    {
      elementName = XMLString::transcode (rootElement->getTagName ());
      if (elementName == "region")
        {
          return getLayoutParser ()->parseRegion (rootElement,
                                                  parentObject);
        }
      else if (elementName == "regionBase")
        {
          return getLayoutParser ()->parseRegionBase (rootElement,
                                                      parentObject);
        }
      else if (elementName == "transition")
        {
          return getTransitionParser ()->parseTransition (rootElement,
                                                          parentObject);
        }
      else if (elementName == "transitionBase")
        {
          return getTransitionParser ()->parseTransitionBase (rootElement,
                                                              parentObject);
        }
      else if (elementName == "rule")
        {
          return getPresentationControlParser ()->parseRule (rootElement,
                                                             parentObject);
        }
      else if (elementName == "compositeRule")
        {
          return getPresentationControlParser ()->parseRule (rootElement,
                                                             parentObject);
        }
      else if (elementName == "ruleBase")
        {
          return getPresentationControlParser ()->parseRuleBase (
              rootElement, parentObject);
        }
      else if (elementName == "causalConnector")
        {
          return getConnectorsParser ()->parseCausalConnector (
              rootElement, parentObject);
        }
      else if (elementName == "connectorBase")
        {
          return getConnectorsParser ()->parseConnectorBase (rootElement,
                                                             parentObject);
        }
      else if (elementName == "descriptor")
        {
          return getPresentationSpecificationParser ()->parseDescriptor (
              rootElement, parentObject);
        }
      else if (elementName == "descriptorSwitch")
        {
          return getPresentationControlParser ()->parseDescriptorSwitch (
              rootElement, parentObject);
        }
      else if (elementName == "descriptorBase")
        {
          return getPresentationSpecificationParser ()
              ->parseDescriptorBase (rootElement, parentObject);
        }
      else if (elementName == "importBase")
        {
          return getImportParser ()->parseImportBase (rootElement,
                                                      parentObject);
        }
      else if (elementName == "importedDocumentBase")
        {
          return getImportParser ()->parseImportedDocumentBase (
              rootElement, parentObject);
        }
      else if (elementName == "importNCL")
        {
          return getImportParser ()->parseImportNCL (rootElement,
                                                     parentObject);
        }
      else if (elementName == "media")
        {
          return getComponentsParser ()->parseMedia (rootElement,
                                                     parentObject);
        }
      else if (elementName == "context")
        {
          object = getComponentsParser ()->parseContext (rootElement,
                                                         parentObject);

          getComponentsParser ()->posCompileContext (rootElement, object);
          return object;
        }
      else if (elementName == "switch")
        {
          object = getPresentationControlParser ()->parseSwitch (
              rootElement, parentObject);

          getPresentationControlParser ()->posCompileSwitch (rootElement,
                                                             object);

          return object;
        }
      else if (elementName == "link")
        {
          return getLinkingParser ()->parseLink (rootElement, parentObject);
        }
      else if (elementName == "area")
        {
          return getInterfacesParser ()->parseArea (rootElement,
                                                    parentObject);
        }
      else if (elementName == "property")
        {
          return getInterfacesParser ()->parseProperty (rootElement,
                                                        parentObject);
        }
      else if (elementName == "port")
        {
          return getInterfacesParser ()->parsePort (rootElement,
                                                    parentObject);
        }
      else if (elementName == "switchPort")
        {
          return getInterfacesParser ()->parseSwitchPort (rootElement,
                                                          parentObject);
        }
      else
        {
          return NULL;
        }
    }
  else
    {
      return NclDocumentParser::parseRootElement (rootElement);
    }

  return NULL;
}

GINGA_NCLCONV_END
