/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "converter/framework/XMLParsing.h"

XMLParsing * XMLParsing::myInstance = NULL;

XMLParsing::XMLParsing() {
	// initialize the XML library.
	XMLPlatformUtils::Initialize();
}

XMLParsing::~XMLParsing() {
	// terminate the XML library.
	XMLPlatformUtils::Terminate();
}

DOMNode *XMLParsing::parse(string src) {
	bool bFailed = false;

	if (myInstance==NULL) {
		myInstance = new XMLParsing();
	}

	// create new parser instance.
	XercesDOMParser *parser = new XercesDOMParser();
	if (!parser) {
		return NULL;

	} else {
		parser->setValidationScheme(XercesDOMParser::Val_Auto);
		parser->setDoNamespaces(false);
		parser->setDoSchema(false);

		// skip this if you haven't written your own error
		// reporter class.
		DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
		parser->setErrorHandler(errReporter);
		parser->setCreateEntityReferenceNodes(false);

		//parser->setToCreateXMLDeclTypeNode(true);
		try {
			//checking if source is xml or uri
			if (src.find("<") != std::string::npos) {
				MemBufInputSource xmlSource(
						(XMLByte*)(src.c_str()),
						src.length(),
						XMLString::transcode("xmlContent"));

				parser->parse(xmlSource);

			} else {
				LocalFileInputSource source(
						XMLString::transcode(src.c_str()));

				parser->parse(source);
			}

			bFailed = parser->getErrorCount() != 0;
			if (bFailed) {
				std::cerr << "Parsing " << src <<std::endl;
				std::cerr << " error count: ";
				std::cerr << parser->getErrorCount() << std::endl;
			}

		} catch (const DOMException& e) {
			std::cerr << "DOM Exception parsing ";
			std::cerr << src;
			std::cerr << " reports: ";

			// was message provided?
			if (e.msg) {
				// yes: display it as ascii.
				char *strMsg = XMLString::transcode(e.msg);
				std::cerr << strMsg << std::endl;
				XMLString::release(&strMsg);

			} else {
				// no: just display the error code.
				std::cerr << e.code << std::endl;
			}

			bFailed = true;

		} catch (const XMLException& e) {
			std::cerr << "XML Exception parsing ";
			std::cerr << src;
			std::cerr << " reports: ";
			std::cerr << e.getMessage() << std::endl;
			bFailed = true;

		} catch (const SAXException& e) {
			std::cerr << "SAX Exception parsing ";
			std::cerr << src;
			std::cerr << " reports: ";
			std::cerr << e.getMessage() << std::endl;
			bFailed = true;

		} catch (...) {
			std::cerr << "An exception parsing ";
			std::cerr << src << std::endl;
			bFailed = true;
		}

		// did the input document parse okay?
		if (!bFailed) {
			DOMNode *pDoc = parser->getDocument();
			// insert code to do something with the DOM document here.
			return pDoc;
		}
		return NULL;
	}
}
