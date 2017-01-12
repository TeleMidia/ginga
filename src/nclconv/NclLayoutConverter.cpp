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

#include "NclLayoutConverter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
	NclLayoutConverter::NclLayoutConverter(
		     DocumentParser *documentParser, IDeviceLayout* deviceLayout) :
		    	 NclLayoutParser(documentParser) {

		this->deviceLayout = deviceLayout;
	}

	void NclLayoutConverter::addImportBaseToRegionBase(
		     void *parentObject, void *childObject) {

		map<int, RegionBase*>* bases;
		map<int, RegionBase*>::iterator i;
		string baseAlias, baseLocation;
		NclDocumentConverter *compiler;
		NclDocument *importedDocument;

		// get the external base alias and location
		baseAlias = XMLString::transcode(((DOMElement*)childObject)->
			    getAttribute(XMLString::transcode("alias")));

		baseLocation = XMLString::transcode(((DOMElement*)childObject)->
			    getAttribute(XMLString::transcode("documentURI")) );

		compiler = (NclDocumentConverter*)getDocumentParser();
		importedDocument = compiler->importDocument(&baseLocation);
		if (importedDocument == NULL) {
			return;
		}

		bases = importedDocument->getRegionBases();
		if (bases == NULL || bases->empty()) {
			return;
		}

		// insert the imported base into the document region base
		i = bases->begin();
		while (i != bases->end()) {
			((RegionBase*) parentObject)->addBase(
				    i->second, baseAlias, baseLocation);
			++i;
		}
	}

	void NclLayoutConverter::addRegionToRegion(
		    void *parentObject, void *childObject) {

		((LayoutRegion*) parentObject)->addRegion((LayoutRegion*) childObject);
	}

	void NclLayoutConverter::addRegionToRegionBase(
		    void *parentObject, void *childObject) {

		RegionBase *layout = (RegionBase*) parentObject;
		layout->addRegion((LayoutRegion*) childObject);
	}

	void *NclLayoutConverter::createRegionBase(
		    DOMElement* parentElement, void *objGrandParent) {

		RegionBase *layout;
		string mapId = "";

		layout = new RegionBase(
				XMLString::transcode(parentElement->getAttribute(
						XMLString::transcode("id"))),

				deviceLayout);

		// device attribute
		if (parentElement->hasAttribute(XMLString::transcode("device"))) {
			// region for output bit map attribute
			if (parentElement->hasAttribute(XMLString::transcode("region"))) {
				mapId = XMLString::transcode(parentElement->
					    getAttribute(XMLString::transcode("region")));
			}

			layout->setDevice(XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("device"))), mapId);

		} else {
			layout->setDevice(deviceLayout->getLayoutName(), "");
		}

		return layout;
	}

	void *NclLayoutConverter::createRegion(
		    DOMElement* parentElement, void *objGrandParent) {

		LayoutRegion *ncmRegion;
		string attribute;
		float percentValue;

		attribute = XMLString::transcode(parentElement->getAttribute(
			     XMLString::transcode("id")));

		// cria nova regiao
		ncmRegion = new LayoutRegion(attribute);

		// atributo title
		if (parentElement->hasAttribute(XMLString::transcode("title"))) {
			ncmRegion->setTitle(XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("title"))));
		}

		// atributo: left
		if (parentElement->hasAttribute(XMLString::transcode("left"))) {
			attribute = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("left")));

			if (attribute != "") {
				if (isPercentualValue(attribute)) {
					percentValue = getPercentualValue(attribute);
					ncmRegion->setLeft(percentValue, true);
				} else {
					double pixelValue;
					pixelValue = getPixelValue(attribute);
					ncmRegion->setLeft(pixelValue, false);
				}
			}
		}

		// atributo: right
		if (parentElement->hasAttribute(XMLString::transcode("right"))) {
			attribute = XMLString::transcode(parentElement->getAttribute(
				     XMLString::transcode("right")));

			if (isPercentualValue(attribute)) {
				percentValue = getPercentualValue(attribute);
				ncmRegion->setRight(percentValue, true);
			} else {
				ncmRegion->setRight(getPixelValue(attribute), false);
			}
		}

		// atributo: top
		if (parentElement->hasAttribute(XMLString::transcode("top"))) {
			attribute = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("top")));

			if (isPercentualValue(attribute)) {
				percentValue = getPercentualValue(attribute);
				ncmRegion->setTop(percentValue, true);
			} else {
				ncmRegion->setTop(getPixelValue(attribute), false);
			}
		}

		// atributo: bottom
		if (parentElement->hasAttribute(XMLString::transcode("bottom"))) {
			attribute = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("bottom")));

			if (isPercentualValue(attribute)) {
				percentValue = getPercentualValue(attribute);
				ncmRegion->setBottom(percentValue, true);
			} else {
				ncmRegion->setBottom(getPixelValue(attribute), false);
			}
		}

		// atributo: width
		if (parentElement->hasAttribute(XMLString::transcode("width"))) {
			attribute = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("width")));

			if (isPercentualValue(attribute)) {
				percentValue = getPercentualValue(attribute);
				ncmRegion->setWidth(percentValue, true);
			} else {
				ncmRegion->setWidth(getPixelValue(attribute), false);
			}
		}

		// atributo: height
		if (parentElement->hasAttribute(XMLString::transcode("height"))) {
			attribute = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("height")));

			if (isPercentualValue(attribute)) {
				percentValue = getPercentualValue(attribute);
				ncmRegion->setHeight(percentValue, true);
			} else {
				ncmRegion->setHeight(getPixelValue(attribute), false);
			}
		}

		// atributo: zIndex
		if (parentElement->hasAttribute(XMLString::transcode("zIndex"))) {
			attribute = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("zIndex")));

			ncmRegion->setZIndex(atoi(attribute.c_str()));
		}

		// atributo movable
		if (parentElement->hasAttribute(XMLString::transcode("movable"))) {
			attribute = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("movable")));

			if (XMLString::compareIString(attribute.c_str(), "false")==0) {
				ncmRegion->setMovable(false);
			} else {
				ncmRegion->setMovable(true);
			}
		}

		// atributo resizable
		if (parentElement->hasAttribute(XMLString::transcode("resizable"))) {
			attribute = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("resizable")));

			if (XMLString::compareIString(attribute.c_str(), "false")==0) {
				ncmRegion->setResizable(false);
			} else {
				ncmRegion->setResizable(true);
			}
		}

		// atributo decorated
		if (parentElement->hasAttribute(XMLString::transcode("decorated"))) {
			attribute = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("decorated")));

			if (XMLString::compareIString(attribute.c_str(), "false")==0) {
				ncmRegion->setDecorated(false);
			} else {
				ncmRegion->setDecorated(true);
			}
		}

		// retorna regiao
		return ncmRegion;
	}

	float NclLayoutConverter::getPercentualValue(string value) {
		string actualValue;
		float floatValue;

		// retirar o caracter percentual da string
		actualValue = value.substr(0, value.length() - 1);
		// converter para float
		floatValue = util::stof(actualValue);

		// se menor que zero, retornar zero
		if (floatValue < 0)
			floatValue = 0;
		//else if (floatValue > 100)
			// se maior que 100, retornar 100
			//floatValue = 100;

		// retornar valor percentual
		return floatValue;
	}

	int NclLayoutConverter::getPixelValue(string value) {
		string actualValue;

		if (value.length() > 2 && value.substr(value.length()-2, 2) =="px") {
			// string indica que o valor e' em pixel
			actualValue = value.substr(0, value.length() - 2);
		} else {
			// string simplesmente indicao o valor
			actualValue = value;
		}

		// converter para inteiro e retornar
		return atoi(actualValue.c_str());
	}

	bool NclLayoutConverter::isPercentualValue(string value) {
		if (value[ value.length()-1 ] == '%')
			return true;
		else
			return false;
	}
}
}
}
}
}
