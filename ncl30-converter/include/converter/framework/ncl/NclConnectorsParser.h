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

#ifndef NCLCONNECTORSPARSER_H_
#define NCLCONNECTORSPARSER_H_

#include "../ModuleParser.h"
#include "../DocumentParser.h"
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
