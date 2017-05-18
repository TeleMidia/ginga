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
#include "NclParser.h"

#include "ginga.h"
#include "NclParserModules.h"

GINGA_NCLCONV_BEGIN

NclParser::NclParser (PrivateBaseContext *pbc, DeviceLayout *deviceLayout) :
  _presentationSpecificationParser(this), _structureParser(this),
  _componentsParser(this), _connectorsParser(this), _linkingParser(this),
  _interfacesParser(this), _layoutParser(this), _transitionParser(this),
  _presentationControlParser(this), _importParser(this),
  _metainformationParser(this)
{
  this->_privateBaseContext = nullptr;
  this->_ownManager = false;

  this->_privateBaseContext = pbc;
  this->_deviceLayout = deviceLayout;
}

NclParser::~NclParser ()
{

}

NclTransitionParser &
NclParser::getTransitionParser ()
{
  return _transitionParser;
}

NclConnectorsParser &
NclParser::getConnectorsParser ()
{
  return _connectorsParser;
}

NclImportParser &
NclParser::getImportParser ()
{
  return _importParser;
}

NclPresentationControlParser &
NclParser::getPresentationControlParser ()
{
  return _presentationControlParser;
}

NclComponentsParser &
NclParser::getComponentsParser ()
{
  return _componentsParser;
}

NclStructureParser &
NclParser::getStructureParser ()
{
  return _structureParser;
}

NclPresentationSpecificationParser &
NclParser::getPresentationSpecificationParser ()
{
  return _presentationSpecificationParser;
}

NclLayoutParser &
NclParser::getLayoutParser ()
{
  return _layoutParser;
}

NclInterfacesParser &
NclParser::getInterfacesParser ()
{
  return _interfacesParser;
}

NclMetainformationParser &
NclParser::getMetainformationParser ()
{
  return _metainformationParser;
}

NclLinkingParser &
NclParser::getLinkingParser ()
{
  return _linkingParser;
}

NclDocument *
NclParser::parseRootElement (DOMElement *rootElement)
{
  string tagName = dom_element_tagname(rootElement);
  if (unlikely (tagName != "ncl"))
    syntax_error ("bad root element '%s'", tagName.c_str ());

  return getStructureParser ().parseNcl (rootElement);
}

string
NclParser::getDirName ()
{
  return this->_dirname;
}

string
NclParser::getPath ()
{
  return this->_path;
}

NclDocument *
NclParser::getNclDocument ()
{
  return this->_ncl;
}

void
NclParser::setNclDocument (NclDocument *ncl)
{
  this->_ncl = ncl;
}

DeviceLayout *
NclParser::getDeviceLayout()
{
  return this->_deviceLayout;
}

void
NclParser::warning (const SAXParseException &e)
{
  char *file = XMLString::transcode (e.getSystemId ());
  char *errMsg = XMLString::transcode (e.getMessage ());
  if (file == NULL || strlen (file) <= 0)
    g_warning ("%s", errMsg);
  else
    g_warning ("%s:%u.%u: %s", file,
               (guint)e.getLineNumber (),
               (guint)e.getColumnNumber (),
               errMsg);
  XMLString::release(&file);
}

void G_GNUC_NORETURN
NclParser::error (const SAXParseException &e)
{
  char *file = XMLString::transcode (e.getSystemId ());
  char *errMsg = XMLString::transcode (e.getMessage ());
  if (file == NULL || strlen (file) <= 0)
    g_error ("%s", errMsg);
  else
    g_error ("%s:%u.%u: %s", file,
             (guint)e.getLineNumber (),
             (guint)e.getColumnNumber (),
             errMsg);

  XMLString::release(&file);
  XMLString::release(&errMsg);

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

  this->_path = xpathmakeabs (path);
  this->_dirname = xpathdirname (path);

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

  this->_ncl = (NclDocument *) parseRootElement (elt);
  g_assert_nonnull (this->_ncl);

  delete parser;

  // FIXME: Should we call this?
  // XMLPlatformUtils::Terminate ();

  return this->_ncl;
}

Node *
NclParser::getNode (const string &nodeId)
{
  NclDocument *doc = getNclDocument ();
  g_assert_nonnull(doc);

  return doc->getNode (nodeId);
}

PrivateBaseContext *
NclParser::getPrivateBaseContext ()
{
  return _privateBaseContext;
}

NclDocument *
NclParser::importDocument (string &path)
{
  if (!xpathisuri (path) && !xpathisabs (path))
    path = xpathbuildabs (this->getDirName (), path);

  return (NclDocument *)(_privateBaseContext
                          ->addVisibleDocument (path, _deviceLayout));
}

GINGA_NCLCONV_END
