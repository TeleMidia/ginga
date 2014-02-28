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

#include "converter/ncl/NclTransitionConverter.h"
#include "converter/ncl/NclDocumentConverter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
	NclTransitionConverter::NclTransitionConverter(
		    DocumentParser *documentParser) : NclTransitionParser(
		    	   documentParser) {
	}

	void NclTransitionConverter::addTransitionToTransitionBase(
		    void* parentObject, void* childObject) {

		((TransitionBase*)parentObject)->
			    addTransition((Transition*)childObject);
	}

	void* NclTransitionConverter::createTransitionBase(
		    DOMElement* parentElement, void* objGrandParent) {

		return new TransitionBase(XMLString::transcode(
			    parentElement->getAttribute(XMLString::transcode("id"))));
	}

	void* NclTransitionConverter::createTransition(
		    DOMElement *parentElement, void *objGrandParent) {

		Transition* transition;
		string id, attValue;
		int type, subtype;
		short direction;
		double dur;
		Color* color;

		if (!parentElement->hasAttribute(
			     XMLString::transcode("id"))) {

			return NULL;
		}

		id = XMLString::transcode(parentElement->getAttribute(
			     XMLString::transcode("id")));

		if (parentElement->hasAttribute(XMLString::transcode("type"))) {
			attValue = XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("type")));

			type = TransitionUtil::getTypeCode(attValue);
			if (type < 0) {
				return NULL;
			}

		} else {
			// type must be defined
			clog << "NclTransitionConverter::createTransition warning!";
			clog << " transition type must be defined" << endl;
			return NULL;
		}

		transition = new Transition(id, type);

		if (parentElement->hasAttribute(XMLString::transcode("subtype"))) {
			attValue = XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("subtype")));

			subtype = TransitionUtil::getSubtypeCode(type, attValue);
			if (subtype >= 0) {
				transition->setSubtype(subtype);
			}
		}

		if (parentElement->hasAttribute(XMLString::transcode("dur"))) {
			attValue = XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("dur")));

			dur = util::stof(attValue.substr(0, attValue.length() - 1));
			transition->setDur(dur * 1000);
		}

		if (parentElement->hasAttribute(
			    XMLString::transcode("startProgress"))) {

			attValue = XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("startProgress")));

			transition->setStartProgress(util::stof(attValue));
		}

		if (parentElement->hasAttribute(XMLString::transcode("endProgress"))) {
			attValue = XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("endProgress")));

			transition->setEndProgress(util::stof(attValue));
		}

		if (parentElement->hasAttribute(XMLString::transcode("direction"))) {
			attValue = XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("direction")));

			direction = TransitionUtil::getDirectionCode(attValue);
			if (direction >= 0) {
				transition->setDirection(direction);
			}
		}

		if (parentElement->hasAttribute(XMLString::transcode("fadeColor"))) {
			color = new Color(XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("fadeColor"))));

			transition->setFadeColor(color);
		}

		if (parentElement->hasAttribute(XMLString::transcode("horzRepeat"))) {
			attValue = XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("horzRepeat")));

			transition->setHorzRepeat((int)util::stof(attValue));
		}

		if (parentElement->hasAttribute(XMLString::transcode("vertRepeat"))) {
			attValue = XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("vertRepeat")));

			transition->setVertRepeat((int)util::stof(attValue));
		}

		if (parentElement->hasAttribute(XMLString::transcode("borderWidth"))) {
			attValue = XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("borderWidth")));

			transition->setBorderWidth((int)util::stof(attValue));
		}

		if (parentElement->hasAttribute(XMLString::transcode("borderColor"))) {
			color = new Color(XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("borderColor"))));

			transition->setBorderColor(color);
		}

		return transition;
	}

	void NclTransitionConverter::addImportBaseToTransitionBase(
		    void* parentObject, void* childObject) {

		string baseAlias, baseLocation;
		NclDocumentConverter* compiler;
		NclDocument* importedDocument;
		TransitionBase* createdBase;

		// get the external base alias and location
		baseAlias = XMLString::transcode(((DOMElement*)childObject)->
			    getAttribute(XMLString::transcode("alias")));

		baseLocation = XMLString::transcode(((DOMElement*)childObject)->
			    getAttribute(XMLString::transcode("documentURI")));

		compiler = (NclDocumentConverter*)getDocumentParser();
		importedDocument = compiler->importDocument(&baseLocation);
		if (importedDocument == NULL) {
			return;
		}

		createdBase = importedDocument->getTransitionBase();
		if (createdBase == NULL) {
			return;
		}

		// insert the imported base into the document region base
		try {
			((TransitionBase*)parentObject)->addBase(
				    createdBase, baseAlias, baseLocation);

		} catch (std::exception* exc) {
			clog << "NclTransitionConverter::addImportBaseToTransitionBase";
			clog << "Warning! exception '" << exc->what() << "'" << endl;
		}
	}
}
}
}
}
}
