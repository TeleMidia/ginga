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

#ifndef NCLCONNECTORSPARSER_H_
#define NCLCONNECTORSPARSER_H_

#include "ModuleParser.h"
#include "DocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "NclImportParser.h"

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {
   class NclConnectorsParser : public ModuleParser {
	//Ref Classes
	private:
		NclImportParser *importParser;

	public:
		NclConnectorsParser(DocumentParser *documentParser);

		void *parseSimpleCondition(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createSimpleCondition(
			    DOMElement *parentElement, void *objGrandParent)=0;

		void *parseCompoundCondition(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createCompoundCondition(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addSimpleConditionToCompoundCondition(
			    void *parentObject, void *childObject)=0;

		virtual void addCompoundConditionToCompoundCondition(
			    void *parentObject, void *childObject)=0;

		virtual void addAssessmentStatementToCompoundCondition(
			    void *parentObject, void *childObject)=0;

		virtual void addCompoundStatementToCompoundCondition(
			    void *parentObject, void *childObject)=0;

		void *parseAssessmentStatement(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createAssessmentStatement(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addAttributeAssessmentToAssessmentStatement(
			    void *parentObject, void *childObject)=0;

		virtual void addValueAssessmentToAssessmentStatement(
			    void *parentObject, void *childObject)=0;

		void *parseAttributeAssessment(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createAttributeAssessment(
			    DOMElement *parentElement, void *objGrandParent)=0;

		void *parseValueAssessment(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createValueAssessment(
			    DOMElement *parentElement, void *objGrandParent)=0;

		void *parseCompoundStatement(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createCompoundStatement(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addAssessmentStatementToCompoundStatement(
			    void *parentObject, void *childObject)=0;

		virtual void addCompoundStatementToCompoundStatement(
			    void *parentObject, void *childObject)=0;

		void *parseSimpleAction(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createSimpleAction(
			    DOMElement *parentElement, void *objGrandParent)=0;

		void *parseCompoundAction(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createCompoundAction(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addSimpleActionToCompoundAction(
			    void *parentObject, void *childObject)=0;

		virtual void addCompoundActionToCompoundAction(
			    void *parentObject, void *childObject)=0;

		void *parseConnectorParam(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createConnectorParam(
			    DOMElement *parentElement, void *objGrandParent)=0;

		void *parseCausalConnector(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createCausalConnector(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addSimpleConditionToCausalConnector(
			    void *parentObject, void *childObject)=0;

		virtual void addCompoundConditionToCausalConnector(
			    void *parentObject, void *childObject)=0;

		virtual void addSimpleActionToCausalConnector(
			    void *parentObject, void *childObject)=0;

		virtual void addCompoundActionToCausalConnector(
			    void *parentObject, void *childObject)=0;

		virtual void addConnectorParamToCausalConnector(
			    void *parentObject, void *childObject)=0;

		void *parseConnectorBase(
			    DOMElement *parentElement, void *objGrandParent);

		virtual void *createConnectorBase(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addImportBaseToConnectorBase(
			    void *parentObject, void *childObject)=0;

		virtual void addCausalConnectorToConnectorBase(
			    void *parentObject, void *childObject)=0;

		NclImportParser *getImportParser();
		void setImportParser(NclImportParser *importParser);
   };
}
}
}
}
}
}

#endif /*NCLCONNECTORSPARSER_H_*/
