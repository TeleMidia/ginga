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

#ifndef NCLPRESENTATIONCONTROLPARSER_H_
#define NCLPRESENTATIONCONTROLPARSER_H_

#include "ModuleParser.h"
#include "DocumentParser.h"
using namespace ::ginga::nclconv;

XERCES_CPP_NAMESPACE_USE


#include "NclPresentationSpecificationParser.h"
#include "NclInterfacesParser.h"
#include "NclImportParser.h"

GINGA_NCLCONV_BEGIN

  class NclPresentationControlParser : public ModuleParser {
	private:
		NclPresentationSpecificationParser *presentationSpecificationParser;
		void *componentsParser;
		NclInterfacesParser *interfacesParser;
		NclImportParser *importParser;

	public:
		NclPresentationControlParser(DocumentParser *documentParser);

		void *parseBindRule(DOMElement *parentElement, void *objGrandParent);
		virtual void *createBindRule(
			    DOMElement *parentElement, void *objGrandParent)=0;

		void *parseRuleBase(DOMElement *parentElement, void *objGrandParent);
		virtual void *createRuleBase(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addImportBaseToRuleBase(
			    void *parentObject, void *childObject)=0;

		virtual void addRuleToRuleBase(
			    void *parentObject, void *childObject)=0;

		virtual void addCompositeRuleToRuleBase(
			    void *parentObject, void *childObject)=0;

		void *parseRule(DOMElement *parentElement, void *objGrandParent);
		virtual void *createRule(
			    DOMElement *parentElement, void *objGrandParent)=0;

		void *parseSwitch(DOMElement *parentElement, void *objGrandParent);
		virtual void *posCompileSwitch(
			    DOMElement *parentElement, void *parentObject);

		void *parseDefaultComponent(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createDefaultComponent(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addDefaultComponentToSwitch(
			    void *parentObject, void *childObject)=0;

		void *parseDefaultDescriptor(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createDefaultDescriptor(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addDefaultDescriptorToDescriptorSwitch(
			    void *parentObject, void *childObject)=0;

		virtual void *createSwitch(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addSwitchPortToSwitch(
			    void *parentObject, void *childObject)=0;

		virtual void addMediaToSwitch(void *parentObject, void *childObject)=0;
		virtual void addContextToSwitch(
			    void *parentObject, void *childObject)=0;

		virtual void addSwitchToSwitch(
			    void *parentObject, void *childObject)=0;

		virtual void addBindRuleToSwitch(
			    void *parentObject, void *childObject)=0;

		virtual void addUnmappedNodesToSwitch(void *parentObject)=0;

		void *parseCompositeRule(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createCompositeRule(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addRuleToCompositeRule(
			    void *parentObject, void *childObject)=0;

		virtual void addCompositeRuleToCompositeRule(
			    void *parentObject, void *childObject)=0;

		void *parseDescriptorSwitch(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createDescriptorSwitch(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addBindRuleToDescriptorSwitch(
			    void *parentObject, void *childObject)=0;

		virtual void addDescriptorToDescriptorSwitch(
			    void *parentObject, void *childObject)=0;

		NclPresentationSpecificationParser*
			    getPresentationSpecificationParser();

		void setPresentationSpecificationParser(
			    NclPresentationSpecificationParser*
			    	    presentationSpecificationParser);

		void *getComponentsParser();
		void setComponentsParser(void *componentsParser);
		NclInterfacesParser *getInterfacesParser();
		void setInterfacesParser(NclInterfacesParser *interfacesParser);
		NclImportParser *getImportParser();
		void setImportParser(NclImportParser *importParser);
  };

GINGA_NCLCONV_END
#endif /*NCLPRESENTATIONCONTROLPARSER_H_*/
