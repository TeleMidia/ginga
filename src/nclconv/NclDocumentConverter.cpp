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
NclDocumentConverter::setConverterInfo (PrivateBaseContext *pbc,
                                        DeviceLayout *deviceLayout)
{
  setDeviceLayout (deviceLayout);
  this->privateBaseContext = pbc;

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
NclDocumentConverter::getAttribute (void *element, const string &attribute)
{
  return XMLString::transcode (
      ((DOMElement *)element)
          ->getAttribute (XMLString::transcode (attribute.c_str ())));
}

Node *
NclDocumentConverter::getNode (const string &nodeId)
{
  NclDocument *document;

  document
      = (NclDocument *)NclDocumentParser::getObject ("return", "document");

  return document->getNode (nodeId);
}

bool
NclDocumentConverter::removeNode (arg_unused (Node *node))
{
  return true;
}

PrivateBaseContext *
NclDocumentConverter::getPrivateBaseContext ()
{
  return privateBaseContext;
}

NclDocument *
NclDocumentConverter::importDocument (string &path)
{
  if (!xpathisabs (path))
    path = xpathbuildabs (this->getDirName (), path);

  return (NclDocument *)(privateBaseContext
                         ->addVisibleDocument (path, deviceLayout));
}

void *
NclDocumentConverter::parseRootElement (DOMElement *rootElement)
{
  return NclDocumentParser::parseRootElement (rootElement);
}

GINGA_NCLCONV_END
