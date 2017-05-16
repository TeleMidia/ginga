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
#include "NclParser.h"

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

GINGA_NCLCONV_BEGIN

NclParser::NclParser (PrivateBaseContext *pbc, DeviceLayout *deviceLayout)
{
  this->parentObject = nullptr;
  this->privateBaseContext = nullptr;
  this->ownManager = false;

  this->privateBaseContext = pbc;
  this->deviceLayout = deviceLayout;

  init ();
}

NclParser::~NclParser ()
{
  delete presentationSpecificationParser;
  delete structureParser;
  delete componentsParser;
  delete connectorsParser;
  delete linkingParser;
  delete interfacesParser;
  delete layoutParser;
  delete transitionParser;
  delete presentationControlParser;
  delete importParser;
  delete metainformationParser;
}

NclTransitionParser *
NclParser::getTransitionParser ()
{
  return transitionParser;
}

NclConnectorsParser *
NclParser::getConnectorsParser ()
{
  return connectorsParser;
}

NclImportParser *
NclParser::getImportParser ()
{
  return importParser;
}

NclPresentationControlParser *
NclParser::getPresentationControlParser ()
{
  return presentationControlParser;
}

NclComponentsParser *
NclParser::getComponentsParser ()
{
  return componentsParser;
}

NclStructureParser *
NclParser::getStructureParser ()
{
  return structureParser;
}

NclPresentationSpecificationParser *
NclParser::getPresentationSpecificationParser ()
{
  return presentationSpecificationParser;
}

NclLayoutParser *
NclParser::getLayoutParser ()
{
  return layoutParser;
}

NclInterfacesParser *
NclParser::getInterfacesParser ()
{
  return interfacesParser;
}

NclMetainformationParser *
NclParser::getMetainformationParser ()
{
  return metainformationParser;
}

NclLinkingParser *
NclParser::getLinkingParser ()
{
  return linkingParser;
}

NclDocument *
NclParser::parseRootElement (DOMElement *rootElement)
{
  string tagName = getTagname(rootElement);
  if (unlikely (tagName != "ncl"))
    syntax_error ("bad root element '%s'", tagName.c_str ());

  return getStructureParser ()->parseNcl (rootElement);
}


string
NclParser::getDirName ()
{
  return this->dirname;
}

string
NclParser::getPath ()
{
  return this->path;
}

NclDocument *
NclParser::getNclDocument ()
{
  return this->ncl;
}

void
NclParser::setNclDocument (NclDocument *ncl)
{
  this->ncl = ncl;
}

void
NclParser::warning (const SAXParseException &e)
{
  const char *file = XMLString::transcode (e.getSystemId ());
  if (file == NULL || strlen (file) <= 0)
    g_warning ("%s", XMLString::transcode (e.getMessage ()));
  else
    g_warning ("%s:%u.%u: %s", file,
               (guint)e.getLineNumber (),
               (guint)e.getColumnNumber (),
               XMLString::transcode (e.getMessage ()));
}

void G_GNUC_NORETURN
NclParser::error (const SAXParseException &e)
{
  const char *file = XMLString::transcode (e.getSystemId ());
  if (file == NULL || strlen (file) <= 0)
    g_error ("%s", XMLString::transcode (e.getMessage ()));
  else
    g_error ("%s:%u.%u: %s", file,
             (guint)e.getLineNumber (),
             (guint)e.getColumnNumber (),
             XMLString::transcode (e.getMessage ()));
  exit (EXIT_FAILURE);
}

void
NclParser::fatalError (const SAXParseException &e)
{
  this->error (e);
}

NclDocument *
NclParser::parse (const string &path)
{
  DOMDocument *dom;
  DOMElement *elt;
  XercesDOMParser *parser;

  this->path = xpathmakeabs (path);
  this->dirname = xpathdirname (path);

  XMLPlatformUtils::Initialize ();
  parser = new XercesDOMParser ();
  g_assert_nonnull (parser);

  parser->setValidationScheme (XercesDOMParser::Val_Auto);
  parser->setDoNamespaces (false);
  parser->setDoSchema (false);
  parser->setErrorHandler (this);
  parser->setCreateEntityReferenceNodes (false);

  XMLCh *path_xmlch = XMLString::transcode (path.c_str ());
  LocalFileInputSource src (path_xmlch);
  try
    {
      parser->parse (src);
      XMLString::release(&path_xmlch);
    }
  catch (...)
    {
      g_assert_not_reached ();
    }

  dom = parser->getDocument ();
  g_assert_nonnull (dom);

  elt = (DOMElement *) dom->getDocumentElement ();
  g_assert_nonnull (elt);

  this->ncl = (NclDocument *) parseRootElement (elt);
  g_assert_nonnull (this->ncl);

  delete parser;

  // FIXME: Should we call this?
  // XMLPlatformUtils::Terminate ();

  return this->ncl;
}

void
NclParser::init ()
{
  presentationSpecificationParser
      = new NclPresentationSpecificationParser(this, deviceLayout);

  structureParser = new NclStructureParser (this);
  componentsParser = new NclComponentsParser (this);
  connectorsParser = new NclConnectorsParser (this);
  linkingParser = new NclLinkingParser (this, connectorsParser);

  interfacesParser = new NclInterfacesParser (this);
  layoutParser = new NclLayoutParser (this, deviceLayout);
  transitionParser = new NclTransitionParser (this);
  presentationControlParser = new NclPresentationControlParser (this);
  importParser = new NclImportParser (this);
  metainformationParser = new NclMetainformationParser (this);
}

string
NclParser::getTagname(const DOMElement *element)
{
  char *tagname = XMLString::transcode (element->getTagName ());
  string tagname_str (tagname);
  XMLString::release(&tagname);

  return tagname_str;
}

string
NclParser::getAttribute (const DOMElement *element, const string &attr)
{
  XMLCh *attr_xmlch = XMLString::transcode(attr.c_str());
  char *attr_value_ch =  XMLString::transcode(element->getAttribute (attr_xmlch));
  string attr_value_str(attr_value_ch);

  XMLString::release(&attr_xmlch);
  XMLString::release(&attr_value_ch);

  return attr_value_str;
}

bool
NclParser::hasAttribute (const DOMElement *element, const string &attr)
{
  XMLCh *attr_xmlch = XMLString::transcode(attr.c_str());
  bool result = element->hasAttribute(attr_xmlch);
  XMLString::release(&attr_xmlch);

  return result;
}

Node *
NclParser::getNode (const string &nodeId)
{
  NclDocument *document;

  document = NclParser::getNclDocument ();

  return document->getNode (nodeId);
}

PrivateBaseContext *
NclParser::getPrivateBaseContext ()
{
  return privateBaseContext;
}

NclDocument *
NclParser::importDocument (string &path)
{
  if (!xpathisuri (path) && !xpathisabs (path))
    path = xpathbuildabs (this->getDirName (), path);

  return (NclDocument *)(privateBaseContext
                          ->addVisibleDocument (path, deviceLayout));
}

GINGA_NCLCONV_END
