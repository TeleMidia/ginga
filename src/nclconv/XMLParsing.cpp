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

XMLParsing::XMLParsing ()
{
  // initialize the XML library.
  XMLPlatformUtils::Initialize ();
}

XMLParsing::~XMLParsing ()
{
  // terminate the XML library.
  XMLPlatformUtils::Terminate ();
}

DOMNode *
XMLParsing::parse (const string &src)
{
  bool bFailed = false;

  if (myInstance == NULL)
    {
      myInstance = new XMLParsing ();
    }

  // create new parser instance.
  XercesDOMParser *parser = new XercesDOMParser ();
  if (!parser)
    {
      return NULL;
    }
  else
    {
      parser->setValidationScheme (XercesDOMParser::Val_Auto);
      parser->setDoNamespaces (false);
      parser->setDoSchema (false);

      // skip this if you haven't written your own error
      // reporter class.
      DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter ();
      parser->setErrorHandler (errReporter);
      parser->setCreateEntityReferenceNodes (false);
      try
        {
          if (src.find ("<") != std::string::npos)
            {
              MemBufInputSource xmlSource (
                  deconst (XMLByte *, src.c_str ()), src.length (),
                  XMLString::transcode ("xmlContent"));

              parser->parse (xmlSource);
            }
          else
            {
              LocalFileInputSource source (
                  XMLString::transcode (src.c_str ()));

              parser->parse (source);
            }

          bFailed = parser->getErrorCount () != 0;
          if (bFailed)
            {
              std::cerr << "Parsing " << src << std::endl;
              std::cerr << " error count: ";
              std::cerr << parser->getErrorCount () << std::endl;
            }
        }
      catch (const DOMException &e)
        {
          std::cerr << "DOM Exception parsing ";
          std::cerr << src;
          std::cerr << " reports: ";

          // was message provided?
          if (e.msg)
            {
              // yes: display it as ascii.
              char *strMsg = XMLString::transcode (e.msg);
              std::cerr << strMsg << std::endl;
              XMLString::release (&strMsg);
            }
          else
            {
              // no: just display the error code.
              std::cerr << e.code << std::endl;
            }

          bFailed = true;
        }
      catch (const XMLException &e)
        {
          std::cerr << "XML Exception parsing ";
          std::cerr << src;
          std::cerr << " reports: ";
          std::cerr << e.getMessage () << std::endl;
          bFailed = true;
        }
      catch (const SAXException &e)
        {
          std::cerr << "SAX Exception parsing ";
          std::cerr << src;
          std::cerr << " reports: ";
          std::cerr << e.getMessage () << std::endl;
          bFailed = true;
        }
      catch (...)
        {
          std::cerr << "An exception parsing ";
          std::cerr << src << std::endl;
          bFailed = true;
        }

      // did the input document parse okay?
      if (!bFailed)
        {
          DOMNode *pDoc = parser->getDocument ();
          // insert code to do something with the DOM document here.
          return pDoc;
        }
      return NULL;
    }
}
