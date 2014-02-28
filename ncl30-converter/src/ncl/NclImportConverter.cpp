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

#include "converter/ncl/NclImportConverter.h"


namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
    NclImportConverter::NclImportConverter(
    	    DocumentParser *documentParser) : NclImportParser(
    	    	    documentParser) {


	}

	void NclImportConverter::addImportNCLToImportedDocumentBase(
		    void *parentObject, void *childObject) {

		string docAlias, docLocation;
		NclDocumentConverter *compiler;
		NclDocument *thisDocument, *importedDocument;

		// apanha o alias e a localizacao do documento
		docAlias = XMLString::transcode(((DOMElement*)childObject)->
			    getAttribute(XMLString::transcode("alias")));

		docLocation = XMLString::transcode(((DOMElement*)childObject)->
			    getAttribute(XMLString::transcode("documentURI")));

		compiler = (NclDocumentConverter*)getDocumentParser();
		importedDocument = compiler->importDocument(&docLocation);
		if (importedDocument != NULL) {
			thisDocument = (NclDocument*)getDocumentParser()->
				    getObject("return", "document");

			thisDocument->addDocument(
				    importedDocument, docAlias, docLocation);
		}
	}

	void *NclImportConverter::createImportBase(
		    DOMElement *parentElement, void *objGrandParent) {

		return parentElement;
	}

	void *NclImportConverter::createImportNCL(
		    DOMElement *parentElement, void *objGrandParent) {

		return parentElement;
	}
	void *NclImportConverter::createImportedDocumentBase(
		    DOMElement *parentElement, void *objGrandParent) {

		return parentElement;
	}
}
}
}
}
}
