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

#ifndef NclPresentationControlConverter_H
#define NclPresentationControlConverter_H

#include "ncl/reuse/ReferNode.h"
#include "ncl/reuse/ReferredNode.h"
using namespace ::br::pucrio::telemidia::ncl::reuse;

#include "ncl/descriptor/Descriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/switches/DescriptorSwitch.h"
#include "ncl/switches/SwitchNode.h"
#include "ncl/switches/CompositeRule.h"
#include "ncl/switches/SimpleRule.h"
#include "ncl/switches/Rule.h"
#include "ncl/switches/RuleBase.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl/interfaces/Port.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/util/Comparator.h"
using namespace ::br::pucrio::telemidia::ncl::util;

#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include <map>
#include <string>
#include <vector>
using namespace std;

#include "ModuleParser.h"
#include "DocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "NclPresentationControlParser.h"
using namespace ::br::pucrio::telemidia::converter::framework::ncl;

#include "NclPresentationSpecConverter.h"
#include "NclComponentsConverter.h"

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

BR_PUCRIO_TELEMIDIA_CONVERTER_NCL_BEGIN

  class NclPresentationControlConverter : public NclPresentationControlParser {
	// tabelas com componentes e regras de cada switch
	// para reusar logica
	// 1 copiar tabelas abaixo e inicializar
	// 2 copiar os metodos:
	// addBindRuleGroupToContentControlGroup,
	// addCompositionContentGroupToContentControlGroup
	// addMediaContentGroupToContentControlGroup
	// createBindRuleGroup
	// addContentControlGroupToContentControlGroup
	// posCreateContentControlGroup
	// addDescriptorGroupToDescriptorSwitchGroup
	// addBindRuleGroupToDescriptorSwitchGroup
	// 3 adicionar posCreateDescriptorSwitchGroup no createDescriptorSwitchGroup
	// 3 adicionar posCreateContentControlGroup no createContentControlGroup
	// 4 copiar adaptando posCompileContentControlGroup
	// 4 copiar adaptando posCompileDescriptorSwitchGroup
	private:
		map<string, map<string, NodeEntity*>*>* switchConstituents;

	public:
		NclPresentationControlConverter(DocumentParser *documentParser);
		virtual ~NclPresentationControlConverter();

		vector<Node*> *getSwitchConstituents(SwitchNode *switchNode);
		void addCompositeRuleToCompositeRule(
			    void *parentObject, void *childObject);

		void addCompositeRuleToRuleBase(void *parentObject, void *childObject);
		void addRuleToCompositeRule(void *parentObject, void *childObject);
		void addRuleToRuleBase(void *parentObject, void *childObject);
		void addSwitchPortToSwitch(void *parentObject, void *childObject);
		void *createCompositeRule(
			    DOMElement *parentElement, void *objGrandParent);

		void *createSwitch(DOMElement *parentElement, void *objGrandParent);
		void *createRuleBase(DOMElement *parentElement, void *objGrandParent);
		void *createRule(DOMElement *parentElement, void *objGrandParent);
		void *createDescriptorSwitch(
			    DOMElement *parentElement, void *objGrandParent);

		void addDescriptorToDescriptorSwitch(
			    void *parentObject, void *childObject);

		void addImportBaseToRuleBase(void *parentObject, void *childObject);
		void addBindRuleToDescriptorSwitch(
			    void *parentObject, void *childObject);

		void addBindRuleToSwitch(void *parentObject, void *childObject);
		void addDefaultComponentToSwitch(void *parentObject, void *childObject);
		void addDefaultDescriptorToDescriptorSwitch(
			    void *parentObject, void *childObject);

		void addContextToSwitch(void *parentObject, void *childObject);
		void addMediaToSwitch(void *parentObject, void *childObject);
		void addSwitchToSwitch(void *parentObject, void *childObject);
		void addUnmappedNodesToSwitch(void *parentObject);

	private:
		void addNodeToSwitch(SwitchNode *switchNode, NodeEntity *node);

	public:
		void *createBindRule(DOMElement *parentElement, void *objGrandParent);
		void *createDefaultComponent(
			    DOMElement *parentElement, void *objGrandParent);

		void *createDefaultDescriptor(
			    DOMElement *parentElement, void *objGrandParent);

		void *posCompileSwitch(DOMElement *parentElement, void *parentObject);

	private:
		short convertComparator(string comparator);
  };

BR_PUCRIO_TELEMIDIA_CONVERTER_NCL_END
#endif
