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

#ifndef FORMATTERLINKCONVERTER_H_
#define FORMATTERLINKCONVERTER_H_

#include "ncl/connectors/Action.h"
#include "ncl/connectors/AssessmentStatement.h"
#include "ncl/connectors/CausalConnector.h"
#include "ncl/connectors/ValueAssessment.h"
#include "ncl/connectors/CompoundAction.h"
#include "ncl/connectors/CompoundStatement.h"
#include "ncl/connectors/CompoundCondition.h"
#include "ncl/connectors/ConditionExpression.h"
#include "ncl/connectors/AttributeAssessment.h"
#include "ncl/connectors/Role.h"
#include "ncl/connectors/SimpleAction.h"
#include "ncl/connectors/SimpleCondition.h"
#include "ncl/connectors/Statement.h"
#include "ncl/connectors/TriggerExpression.h"
#include "ncl/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/link/Bind.h"
#include "ncl/link/CausalLink.h"
#include "ncl/link/Link.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "ncl/components/Node.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/interfaces/InterfacePoint.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/Parameter.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "../model/FormatterCausalLink.h"
#include "../model/LinkAction.h"
#include "../model/LinkAssessment.h"
#include "../model/LinkAndCompoundTriggerCondition.h"
#include "../model/LinkAssessmentStatement.h"
#include "../model/LinkValueAssessment.h"
#include "../model/LinkAssignmentAction.h"
#include "../model/LinkCompoundAction.h"
#include "../model/LinkAttributeAssessment.h"
#include "../model/LinkCompoundStatement.h"
#include "../model/LinkCompoundTriggerCondition.h"
#include "../model/LinkRepeatAction.h"
#include "../model/LinkSimpleAction.h"
#include "../model/LinkCondition.h"
#include "../model/LinkStatement.h"
#include "../model/LinkTriggerCondition.h"
#include "../model/LinkTransitionTriggerCondition.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "../model/FormatterEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "../model/CompositeExecutionObject.h"
#include "../model/ExecutionObject.h"
#include "../model/NodeNesting.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "FormatterConverter.h"
#include "ObjectCreationForbiddenException.h"

#include <string>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace emconverter {
  class FormatterLinkConverter {
	private:
		FormatterConverter* compiler;

	public:
		FormatterLinkConverter(FormatterConverter* compiler);
		~FormatterLinkConverter();

		FormatterCausalLink* createCausalLink(
			    CausalLink* ncmLink,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

	private:
		void setImplicitRefAssessment(
				string roleId, CausalLink* ncmLink, FormatterEvent* event);

		LinkAction* createAction(
			    Action* actionExpression,
			    CausalLink* ncmLink,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

		LinkCondition* createCondition(
			    ConditionExpression* ncmExpression,
			    CausalLink* ncmLink,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

		LinkCompoundTriggerCondition* createCompoundTriggerCondition(
			    short op,
			    double delay,
			    vector<ConditionExpression*>* ncmChildConditions,
			    CausalLink* ncmLink,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

		LinkCondition* createCondition(
			    TriggerExpression* triggerExpression,
			    CausalLink* ncmLink,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

		LinkAssessmentStatement* createAssessmentStatement(
			    AssessmentStatement* assessmentStatement,
			    Bind* bind,
			    Link* ncmLink,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

		LinkStatement* createStatement(
			    Statement* statementExpression,
			    Link* ncmLink,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

		LinkAttributeAssessment* createAttributeAssessment(
			    AttributeAssessment* attributeAssessment,
			    Bind* bind,
			    Link* ncmLink,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

		LinkSimpleAction* createSimpleAction(
			    SimpleAction* sae,
			    Bind* bind,
			    Link* ncmLink,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

		LinkCompoundAction* createCompoundAction(
			    short op,
			    double delay,
			    vector<Action*>* ncmChildActions,
			    CausalLink* ncmLink,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

		LinkTriggerCondition* createSimpleCondition(
			    SimpleCondition* condition,
			    Bind* bind,
			    Link* ncmLink,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

		FormatterEvent* createEvent(
			    Bind* bind,
			    Link* ncmLink,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

		double getDelayParameter(
			    Link* ncmLink, Parameter* connParam, Bind* ncmBind);

		string getBindKey(Link* ncmLink, Bind* ncmBind);
		double compileDelay(Link* ncmLink, string delayObject, Bind* bind);
  };
}
}
}
}
}
}

#endif /*FORMATTERLINKCONVERTER_H_*/
