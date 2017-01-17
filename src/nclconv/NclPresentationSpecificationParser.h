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

#ifndef NCLPRESENTATIONSPECIFICATIONPARSER_H_
#define NCLPRESENTATIONSPECIFICATIONPARSER_H_

#include "ModuleParser.h"
#include "DocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

#include <iostream>
#include <string>
using namespace std;

#include "NclImportParser.h"

BR_PUCRIO_TELEMIDIA_CONVERTER_FRAMEWORK_NCL_BEGIN

  class NclPresentationSpecificationParser : public ModuleParser {
	private:
		NclImportParser *importParser;
		void *presentationControlParser;

	public:
		NclPresentationSpecificationParser(DocumentParser *documentParser);
		void *parseDescriptor(DOMElement *parentElement, void *objGrandParent);
		virtual void *createDescriptor(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addDescriptorParamToDescriptor(
			    void *parentObject, void *childObject)=0;

		void *parseDescriptorBase(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createDescriptorBase(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addImportBaseToDescriptorBase(
			    void *parentObject, void *childObject)=0;

		virtual void addDescriptorSwitchToDescriptorBase(
			    void *parentObject, void *childObject)=0;

		virtual void addDescriptorToDescriptorBase(
			    void *parentObject, void *childObject)=0;

		void *parseDescriptorBind(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createDescriptorBind(
			    DOMElement *parentElement, void *objGrandParent)=0;

		void *parseDescriptorParam(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createDescriptorParam(
			    DOMElement *parentElement, void *objGrandParent)=0;

		NclImportParser *getImportParser();
		void setImportParser(NclImportParser *importParser);
		void *getPresentationControlParser();
		void setPresentationControlParser(void *presentationControlParser);
  };

BR_PUCRIO_TELEMIDIA_CONVERTER_FRAMEWORK_NCL_END
#endif /*NCLPRESENTATIONSPECIFICATIONPARSER_H_*/
