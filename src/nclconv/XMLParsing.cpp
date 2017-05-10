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
#include "XMLParsing.h"

XMLParsing *XMLParsing::myInstance = NULL;

void
XMLParsing::warning (const SAXParseException &e)
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
XMLParsing::error (const SAXParseException &e)
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
XMLParsing::fatalError (const SAXParseException &e)
{
  this->error (e);
}

XMLParsing::XMLParsing ()
{
  XMLPlatformUtils::Initialize ();
}

XMLParsing::~XMLParsing ()
{
  XMLPlatformUtils::Terminate ();
}

DOMNode *
XMLParsing::parse (const string &src)
{
  if (myInstance == NULL)
    myInstance = new XMLParsing ();

  XercesDOMParser *parser = new XercesDOMParser ();
  g_assert_nonnull (parser);

  parser->setValidationScheme (XercesDOMParser::Val_Auto);
  parser->setDoNamespaces (false);
  parser->setDoSchema (false);
  parser->setErrorHandler (myInstance);
  parser->setCreateEntityReferenceNodes (false);
  LocalFileInputSource source (XMLString::transcode (src.c_str ()));
  try
    {
      parser->parse (source);
    }
  catch (...)
    {
      g_assert_not_reached ();
    }
  return parser->getDocument ();
}
