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

#ifndef NclConnectorsConverter_H
#define NclConnectorsConverter_H

#include "ncl/Parameter.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl/animation/Animation.h"
using namespace ::br::pucrio::telemidia::ncl::animation;

#include "ncl/connectors/EventUtil.h"
#include "ncl/connectors/ValueAssessment.h"
#include "ncl/connectors/CompoundAction.h"
#include "ncl/connectors/CompoundCondition.h"
#include "ncl/connectors/CompoundStatement.h"
#include "ncl/connectors/CausalConnector.h"
#include "ncl/connectors/AttributeAssessment.h"
#include "ncl/connectors/AssessmentStatement.h"
#include "ncl/connectors/SimpleAction.h"
#include "ncl/connectors/SimpleCondition.h"
#include "ncl/connectors/ConnectorBase.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/util/Comparator.h"
using namespace ::br::pucrio::telemidia::ncl::util;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "../framework/DocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "../framework/ncl/NclConnectorsParser.h"
using namespace ::br::pucrio::telemidia::converter::framework::ncl;

//#include <stdio.h>
#include <map>
#include <vector>
using namespace std;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
  class NclConnectorsConverter : public NclConnectorsParser {
	private:
		Connector *connector;

	public:
		NclConnectorsConverter(DocumentParser *documentParser);
		void addCausalConnectorToConnectorBase(
			    void *parentObject, void *childObject);

		void addConnectorParamToCausalConnector(
			    void *parentObject, void *childObject);

		void addImportBaseToConnectorBase(
			    void *parentObject, void *childObject);

		void *createCausalConnector(
			    DOMElement *parentElement, void *objGrandParent);

		void *createConnectorBase(
			    DOMElement *parentElement, void *objGrandParent);

		void *createConnectorParam(
			    DOMElement *parentElement, void *objGrandParent);

	private:
		void compileRoleInformation(Role *role, DOMElement *parentElement);

	public:
		void *createSimpleCondition(
			    DOMElement *parentElement, void *objGrandParent);

		void *createCompoundCondition(
			    DOMElement *parentElement, void *objGrandParent);

		void *createAttributeAssessment(
			    DOMElement *parentElement, void *objGrandParent);

		void *createValueAssessment(
			    DOMElement *parentElement, void *objGrandParent);

		void *createAssessmentStatement(
			    DOMElement *parentElement, void *objGrandParent);

		void *createCompoundStatement(
			    DOMElement *parentElement, void *objGrandParent);

		void *createSimpleAction(
			    DOMElement *parentElement, void *objGrandParent);

		void *createCompoundAction(
			    DOMElement *parentElement, void *objGrandParent);

	private:
		Parameter *getParameter(string paramName);
		short convertActionType(string actionType);

	public:
		static short convertEventState(string eventState);
		void addSimpleConditionToCompoundCondition(
			    void *parentObject, void *childObject);

		void addCompoundConditionToCompoundCondition(
			    void *parentObject, void *childObject);

		void addAssessmentStatementToCompoundCondition(
			    void *parentObject, void *childObject);

		void addCompoundStatementToCompoundCondition(
			    void *parentObject, void *childObject);

		void addAttributeAssessmentToAssessmentStatement(
			    void *parentObject, void *childObject);

		void addValueAssessmentToAssessmentStatement(
			    void *parentObject, void *childObject);

		void addAssessmentStatementToCompoundStatement(
			    void *parentObject, void *childObject);

		void addCompoundStatementToCompoundStatement(
			    void *parentObject, void *childObject);

		void addSimpleActionToCompoundAction(
			    void *parentObject, void *childObject);

		void addCompoundActionToCompoundAction(
			    void *parentObject, void *childObject);

		void addSimpleConditionToCausalConnector(
			    void *parentObject, void *childObject);

		void addCompoundConditionToCausalConnector(
			    void *parentObject, void *childObject);

		void addSimpleActionToCausalConnector(
			    void *parentObject, void *childObject);

		void addCompoundActionToCausalConnector(
			    void *parentObject, void *childObject);

		void addAssessmentStatementToConstraintConnector(
			    void *parentObject, void *childObject);

		void addCompoundStatementToConstraintConnector(
			    void *parentObject, void *childObject);

		void addConstraintConnectorToConnectorBase(
			    void *parentObject, void *childObject);

		void *createConstraintConnector(
			    DOMElement *parentElement, void *objGrandParent);
  };
}
}
}
}
}

#endif
