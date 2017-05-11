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
#include "DocumentParser.h"

GINGA_NCLCONV_BEGIN

DocumentParser::DocumentParser ()
{
  this->ncl = NULL;
}

DocumentParser::~DocumentParser ()
{
}

string
DocumentParser::getDirName ()
{
  return this->dirname;
}

string
DocumentParser::getPath ()
{
  return this->path;
}

NclDocument *
DocumentParser::getNclDocument ()
{
  return this->ncl;
}

void
DocumentParser::setNclDocument (NclDocument *ncl)
{
  this->ncl = ncl;
}

void
DocumentParser::warning (const SAXParseException &e)
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
DocumentParser::error (const SAXParseException &e)
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
DocumentParser::fatalError (const SAXParseException &e)
{
  this->error (e);
}

NclDocument *
DocumentParser::parse (const string &path)
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

  LocalFileInputSource src (XMLString::transcode (path.c_str ()));
  try
    {
      parser->parse (src);
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
  //XMLPlatformUtils::Terminate ();

  return this->ncl;
}

GINGA_NCLCONV_END
