/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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
