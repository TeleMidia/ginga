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

#include "NclConnectorsConverter.h"
#include "NclDocumentConverter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
	NclConnectorsConverter::NclConnectorsConverter(
		    DocumentParser* documentParser) : NclConnectorsParser(
		    	    documentParser) {


	}

	void NclConnectorsConverter::addCausalConnectorToConnectorBase(
		    void *parentObject, void *childObject) {

		((ConnectorBase*)parentObject)->addConnector((Connector*)childObject);
	}

	void NclConnectorsConverter::addConnectorParamToCausalConnector(
		    void *parentObject, void *childObject) {

		((Connector*)parentObject)->addParameter((Parameter*)childObject);
	}

	void NclConnectorsConverter::addImportBaseToConnectorBase(
		    void *parentObject, void *childObject) {

		string baseAlias, baseLocation;
		NclDocumentConverter *compiler;
		NclDocument *importedDocument;
		ConnectorBase *connectorBase;

		// get the external base alias and location
		baseAlias = XMLString::transcode(((DOMElement*)childObject)->
			    getAttribute(XMLString::transcode("alias")));

		baseLocation = XMLString::transcode(((DOMElement*)childObject)->
			    getAttribute(XMLString::transcode("documentURI")));

		compiler = (NclDocumentConverter*)getDocumentParser();

		importedDocument = compiler->importDocument(&baseLocation);
		if (importedDocument == NULL) {
			clog << "NclConnectorsConverter::addImportBaseToConnectorBase ";
			clog << "Warning! Can't import document location '";
			clog << baseLocation << "' with alias '" << baseAlias << "'";
			clog << endl;
			return;
		}

		connectorBase = importedDocument->getConnectorBase();
		if (connectorBase == NULL) {
			clog << "NclConnectorsConverter::addImportBaseToConnectorBase ";
			clog << "Warning! Can't get connector base from imported ";
			clog << baseLocation << "' with alias '" << baseAlias << "'";
			clog << endl;
			return;
		}

		// insert the imported base into the document connector base
		((ConnectorBase*)parentObject)->addBase(
			    connectorBase, baseAlias, baseLocation);

		/*
		 * Hashtable connBaseimports = new Hashtable();
		 * connBaseimports->put(baseAlias,baseLocation);
		 * getDocumentParser()->addObject("return","ConnectorImports",
		 * connBaseimports);
		 */
	}

	void *NclConnectorsConverter::createCausalConnector(
		    DOMElement *parentElement, void *objGrandParent) {

		string connectorId = "";
		/*
		 * if (connectorUri->equalsIgnoreCase("")) { //se nao tiver uma uri do
		 * arquivo do conector, atribuir somente o id
		 * do elemento conector como id do conector
		 * connectorId = parentElement->getAttribute("id"); connectorId =
		 * "#" + parentElement->getAttribute("id"); } else { //atribuir a id do
		 * conector como sendo a uri do seu arquivo
		 * connectorId = connectorUri;
		 */
		connectorId = XMLString::transcode(parentElement->getAttribute(
			    XMLString::transcode("id")));

		connector = new CausalConnector(connectorId);
		return connector;
	}

	void *NclConnectorsConverter::createConnectorBase(
		    DOMElement *parentElement, void *objGrandParent) {

		ConnectorBase *connBase;
		// criar nova base de conectores com id gerado a partir do nome de seu
		// elemento
		connBase = new ConnectorBase(XMLString::transcode(
			    parentElement->getAttribute(XMLString::transcode("id"))));

		return connBase;
	}

	void *NclConnectorsConverter::createConnectorParam(
		    DOMElement *parentElement, void *objGrandParent) {

		Parameter *parameter;
		parameter = new Parameter(
			    XMLString::transcode(parentElement->getAttribute(
			     	     XMLString::transcode("name"))),

			    XMLString::transcode(parentElement->getAttribute(
			    	     XMLString::transcode("type"))));

		return parameter;
	}

	void NclConnectorsConverter::compileRoleInformation(
		    Role *role, DOMElement *parentElement) {

		string attValue;
		// event type
		if (parentElement->hasAttribute(XMLString::transcode("eventType"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("eventType")));

			role->setEventType(EventUtil::getTypeCode(attValue));
		}

		//  cardinality
		if (parentElement->hasAttribute(XMLString::transcode("min"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("min")));

			((Role*)role)->setMinCon((atoi(attValue.c_str())));
		}

		if (parentElement->hasAttribute(XMLString::transcode("max"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("max")));

			if (XMLString::compareIString(attValue.c_str(), "unbounded")==0) {
				((Role*)role)->setMaxCon(Role::UNBOUNDED);

			} else {
				((Role*)role)->setMaxCon(atoi(attValue.c_str()));
			}
		}
	}

	void *NclConnectorsConverter::createSimpleCondition(
		    DOMElement *parentElement, void *objGrandParent) {

		SimpleCondition* conditionExpression;
		string attValue;

		string roleLabel;
		roleLabel = XMLString::transcode(parentElement->getAttribute(
			    XMLString::transcode("role")));

		conditionExpression = new SimpleCondition(roleLabel);

		compileRoleInformation(conditionExpression, parentElement);

		// transition
		if (parentElement->hasAttribute(XMLString::transcode("transition"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("transition")));

			conditionExpression->setTransition(
				    EventUtil::getTransitionCode(attValue));
		}

		// parametro
		if (conditionExpression->getEventType() == EventUtil::EVT_SELECTION) {
			if (parentElement->hasAttribute(XMLString::transcode("key"))) {
				attValue = XMLString::transcode(parentElement->getAttribute(
					    XMLString::transcode("key")));

				conditionExpression->setKey(attValue);
			}
		}

		// qualifier
		if (parentElement->hasAttribute(XMLString::transcode(
			    "qualifier"))) {

			attValue = XMLString::transcode(parentElement->
				    getAttribute(XMLString::transcode("qualifier")));

			if (attValue == "or") {
				conditionExpression->setQualifier(
					    CompoundCondition::OP_OR);

			} else {
				conditionExpression->setQualifier(
					    CompoundCondition::OP_AND);
			}
		}

		//testar delay
		if (parentElement->hasAttribute(XMLString::transcode("delay"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("delay")));

			if (attValue[0] == '$') {
				conditionExpression->setDelay( attValue );

			} else {
				double delayValue;
				delayValue = util::stof(
						attValue.substr(0, (attValue.length() - 1))) * 1000;

				conditionExpression->setDelay(itos(delayValue));
			}
		}

		// retornar expressao de condicao
		return conditionExpression;
	}

	void *NclConnectorsConverter::createCompoundCondition(
		    DOMElement *parentElement, void *objGrandParent) {

		CompoundCondition *conditionExpression;
		string attValue;

		conditionExpression = new CompoundCondition();

		if (XMLString::compareIString(
			    XMLString::transcode(parentElement->getAttribute(
			    	    XMLString::transcode("operator"))),

			    "and") == 0) {

			conditionExpression->setOperator(CompoundCondition::OP_AND);

		} else {
			conditionExpression->setOperator(CompoundCondition::OP_OR);
		}

		//  testar delay
		if (parentElement->hasAttribute(XMLString::transcode("delay"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("delay")));

			if (attValue[0] == '$') {
				conditionExpression->setDelay( attValue );

			} else {
				double delayValue = util::stof(attValue.substr(
					    0, (attValue.length() - 1))) * 1000;

				conditionExpression->setDelay( itos(delayValue) );
			}
		}

		// retornar expressao de condicao
		return conditionExpression;
	}

	void* NclConnectorsConverter::createAttributeAssessment(
		    DOMElement *parentElement, void *objGrandParent) {

		AttributeAssessment *attributeAssessment;
		string attValue;

		string roleLabel;
		roleLabel = XMLString::transcode(parentElement->getAttribute(
			   XMLString::transcode("role")));

		attributeAssessment = new AttributeAssessment(roleLabel);

		// event type
		if (parentElement->hasAttribute(XMLString::transcode("eventType"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("eventType")));

			attributeAssessment->setEventType(
				    EventUtil::getTypeCode(attValue));
		}

		// event type
		if (parentElement->hasAttribute(XMLString::transcode(
			     "attributeType"))) {

			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("attributeType")));

			attributeAssessment->setAttributeType(
				    EventUtil::getAttributeTypeCode(attValue));
		}

		// parameter
		if (attributeAssessment->getEventType() == EventUtil::EVT_SELECTION) {
			if (parentElement->hasAttribute(XMLString::transcode("key"))) {
				attValue = XMLString::transcode(parentElement->getAttribute(
					    XMLString::transcode("key")));

				attributeAssessment->setKey(attValue);
			}
		}

		//testing offset
		if (parentElement->hasAttribute(XMLString::transcode("offset"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				     XMLString::transcode("offset")));

			attributeAssessment->setOffset(attValue);
		}

		return attributeAssessment;
	}

	void *NclConnectorsConverter::createValueAssessment(
		    DOMElement *parentElement, void *objGrandParent) {

		string attValue;

		attValue = XMLString::transcode(parentElement->getAttribute(
			    XMLString::transcode("value")));

		return new ValueAssessment(attValue);
	}

	void *NclConnectorsConverter::createAssessmentStatement(
		     DOMElement *parentElement, void *objGrandParent) {

		AssessmentStatement *assessmentStatement;
		string attValue;

		if (parentElement->hasAttribute(XMLString::transcode("comparator"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("comparator")));

			assessmentStatement = new AssessmentStatement(
				    Comparator::fromString(attValue));

		} else {
			assessmentStatement = new AssessmentStatement(Comparator::CMP_EQ);
		}

		return assessmentStatement;
	}

	void *NclConnectorsConverter::createCompoundStatement(
		    DOMElement *parentElement, void *objGrandParent) {

		CompoundStatement *compoundStatement;
		string attValue;

		compoundStatement = new CompoundStatement();

		if (XMLString::compareIString(XMLString::transcode(
			    parentElement->getAttribute(
			    	    XMLString::transcode("operator"))),"and") == 0) {

			compoundStatement->setOperator(CompoundStatement::OP_AND);

		} else {
			compoundStatement->setOperator(CompoundStatement::OP_OR);
		}

		// testing isNegated
		if (parentElement->hasAttribute(XMLString::transcode("isNegated"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("isNegated")));

			compoundStatement->setNegated(XMLString::compareIString(
				     attValue.c_str(), "true") == 0);
		}

		return compoundStatement;
	}

	void *NclConnectorsConverter::createSimpleAction(
		     DOMElement *parentElement, void *objGrandParent) {

	    SimpleAction *actionExpression;
	    string attValue;

	    attValue = XMLString::transcode(
	    		parentElement->getAttribute(XMLString::transcode("role")));

		actionExpression = new SimpleAction(attValue);

		// transition
		if (parentElement->hasAttribute(XMLString::transcode("actionType"))) {
			    attValue = XMLString::transcode(parentElement->getAttribute(
						        XMLString::transcode("actionType")));

			    actionExpression->setActionType(convertActionType(attValue));
		}

		if (parentElement->hasAttribute(XMLString::transcode("eventType"))) {
			    attValue = XMLString::transcode(parentElement->getAttribute(
			    		        XMLString::transcode("eventType")));

				actionExpression->setEventType(EventUtil::getTypeCode(attValue));
		}

		//animation
		if (actionExpression->getEventType() == EventUtil::EVT_ATTRIBUTION &&
				actionExpression->getActionType() == SimpleAction::ACT_START) {

			Animation* animation = NULL;
			string durVal = "";
			string byVal  = "";

			if (parentElement->hasAttribute(
						XMLString::transcode("duration"))) {

				durVal = XMLString::transcode(parentElement->
						getAttribute(XMLString::transcode("duration")));
			}

			if (parentElement->hasAttribute(
						XMLString::transcode("by"))) {

				byVal = XMLString::transcode(parentElement->
						getAttribute(XMLString::transcode("by")));
			}

			if (durVal != "" || byVal != "") {

				animation = new Animation();

				if (durVal[0] == '$') {
					animation->setDuration(durVal);

				} else {
					if (durVal.find("s") != std::string::npos) {
						animation->setDuration(itos(util::stof(durVal.substr(
								0, durVal.length() - 1))));
					} else {
						animation->setDuration(itos(util::stof(durVal)));
					}
				}

				if (byVal.find("s") != std::string::npos) {
					animation->setBy(itos(util::stof(byVal.substr(
							0, byVal.length() - 1))));
				} else {
					animation->setBy(itos(util::stof(byVal)));
				}
			}

			actionExpression->setAnimation(animation);
		}

		compileRoleInformation(actionExpression, parentElement);

		if (parentElement->hasAttribute(XMLString::transcode("qualifier"))) {
			if (XMLString::compareIString(XMLString::transcode(
				     parentElement->getAttribute(
				     	    XMLString::transcode("qualifier"))),"seq") == 0) {

				actionExpression->setQualifier(CompoundAction::OP_SEQ);

			} else { // any
				actionExpression->setQualifier(CompoundAction::OP_PAR);
			}
		}

		//testing delay
		if (parentElement->hasAttribute(XMLString::transcode("delay"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("delay")));

			if (attValue[0] == '$') {
				actionExpression->setDelay(attValue);

			} else {
				actionExpression->setDelay(itos(util::stof(
					    attValue.substr(0, attValue.length() - 1)) * 1000));
			}
		}

		//  testing repeatDelay
		if (parentElement->hasAttribute(XMLString::transcode("repeatDelay"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("repeatDelay")));

			actionExpression->setDelay(attValue);
			if (attValue[0] == '$') {
				actionExpression->setDelay(attValue);

			} else {
				actionExpression->setDelay(itos(util::stof(attValue.substr(
					    0, attValue.length() - 1)) * 1000));
			}
		}

		// repeat
		if (parentElement->hasAttribute(XMLString::transcode("repeat"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("repeat")));

			if (XMLString::compareIString(attValue.c_str(), "indefinite") == 0) {
				actionExpression->setRepeat(itos(2^30));

			} else {
				actionExpression->setRepeat(attValue);
			}
		}

		// testing value
		if (parentElement->hasAttribute(XMLString::transcode("value"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("value")));

			actionExpression->setValue(attValue);
		}

		// returning action expression
		return actionExpression;
	}

	void *NclConnectorsConverter::createCompoundAction(
		    DOMElement *parentElement, void *objGrandParent) {

		CompoundAction *actionExpression;
		string attValue;

		actionExpression = new CompoundAction();

		if (XMLString::compareIString(XMLString::transcode(
			    parentElement->getAttribute(XMLString::transcode("operator"))),
			    	    "seq") == 0) {

			actionExpression->setOperator(CompoundAction::OP_SEQ);

		} else {
			actionExpression->setOperator(CompoundAction::OP_PAR);
		}

		//  testar delay
		if (parentElement->hasAttribute(XMLString::transcode("delay"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				     XMLString::transcode("delay")));

			if (attValue[0]=='$') {
				actionExpression->setDelay(attValue);

			} else {
				actionExpression->setDelay(itos(util::stof(attValue.substr(
					    0, attValue.length() - 1)) * 1000));
			}
		}

		// retornar expressao de condicao
		return actionExpression;
	}

	Parameter *NclConnectorsConverter::getParameter(string paramName) {
		return (Parameter*) (connector->getParameter(paramName));
	}

	short NclConnectorsConverter::convertActionType(string actionType) {
		if (actionType=="start")
			return SimpleAction::ACT_START;

		else if (actionType=="stop")
			return SimpleAction::ACT_STOP;

		else if (actionType=="set")
			return SimpleAction::ACT_SET;

		else if (actionType=="pause")
			return SimpleAction::ACT_PAUSE;

		else if (actionType=="resume")
			return SimpleAction::ACT_RESUME;

		else if (actionType=="abort")
			return SimpleAction::ACT_ABORT;

		return -1;
	}

	short NclConnectorsConverter::convertEventState(string eventState) {
		if (eventState=="occurring") {
			return EventUtil::ST_OCCURRING;

		} else if (eventState=="paused") {
			return EventUtil::ST_PAUSED;

		} else if (eventState=="sleeping") {
			return EventUtil::ST_SLEEPING;

		}

		return -1;
	}

	void NclConnectorsConverter::addSimpleConditionToCompoundCondition(
		    void *parentObject, void *childObject) {

		((CompoundCondition*)parentObject)->addConditionExpression(
			    (ConditionExpression*)childObject);
	}

	void NclConnectorsConverter::addCompoundConditionToCompoundCondition(
		    void *parentObject, void *childObject) {

		((CompoundCondition*)parentObject)->addConditionExpression(
			    (ConditionExpression*)childObject);
	}

	void NclConnectorsConverter::addAssessmentStatementToCompoundCondition(
		    void *parentObject, void *childObject) {

		((CompoundCondition*)parentObject)->addConditionExpression(
			    (ConditionExpression*)childObject);
	}

	void NclConnectorsConverter::addCompoundStatementToCompoundCondition(
		    void *parentObject, void *childObject) {

		((CompoundCondition*)parentObject)->addConditionExpression(
			    (ConditionExpression*)childObject);
	}

	void NclConnectorsConverter::addAttributeAssessmentToAssessmentStatement(
		    void *parentObject, void *childObject) {

		AssessmentStatement *statement;

		statement = (AssessmentStatement*)parentObject;
		if (statement->getMainAssessment() == NULL) {
			statement->setMainAssessment((AttributeAssessment*)childObject);

		} else {
			statement->setOtherAssessment((AttributeAssessment*)childObject);
		}
	}

	void NclConnectorsConverter::addValueAssessmentToAssessmentStatement(
		    void *parentObject, void *childObject) {

		((AssessmentStatement*)parentObject)->setOtherAssessment(
			   (ValueAssessment*)childObject);
	}

	void NclConnectorsConverter::addAssessmentStatementToCompoundStatement(
		    void *parentObject, void *childObject) {

		((CompoundStatement*)parentObject)->addStatement(
			   (Statement*)childObject);
	}

	void NclConnectorsConverter::addCompoundStatementToCompoundStatement(
		    void *parentObject, void *childObject) {

		((CompoundStatement*)parentObject)->addStatement(
			    (Statement*)childObject);
	}

	void NclConnectorsConverter::addSimpleActionToCompoundAction(
		    void *parentObject, void *childObject) {

		((CompoundAction*)parentObject)->addAction((Action*)childObject);
	}

	void NclConnectorsConverter::addCompoundActionToCompoundAction(
		    void *parentObject, void *childObject) {

		((CompoundAction*)parentObject)->addAction((Action*)childObject);
	}

	void NclConnectorsConverter::addSimpleConditionToCausalConnector(
		    void *parentObject, void *childObject) {

		((CausalConnector*)parentObject)->setConditionExpression(
			    (ConditionExpression*)childObject);
	}

	void NclConnectorsConverter::addCompoundConditionToCausalConnector(
		    void *parentObject, void *childObject) {

		((CausalConnector*)parentObject)->setConditionExpression(
			    (ConditionExpression*)childObject);
	}

	void NclConnectorsConverter::addSimpleActionToCausalConnector(
		    void *parentObject, void *childObject) {

		((CausalConnector*)parentObject)->setAction((Action*)childObject);
	}

	void NclConnectorsConverter::addCompoundActionToCausalConnector(
		    void *parentObject, void *childObject) {

		((CausalConnector*)parentObject)->setAction((Action*)childObject);
	}

	void NclConnectorsConverter::addAssessmentStatementToConstraintConnector(
		    void *parentObject, void *childObject) {

		// TODO Auto-generated method stub
	}

	void NclConnectorsConverter::addCompoundStatementToConstraintConnector(
		    void *parentObject, void *childObject) {

		// TODO Auto-generated method stub
	}

	void NclConnectorsConverter::addConstraintConnectorToConnectorBase(
		    void *parentObject, void *childObject) {

		// TODO Auto-generated method stub
	}

	void *NclConnectorsConverter::createConstraintConnector(
		    DOMElement *parentElement, void *objGrandParent) {

		// TODO Auto-generated method stub
		return NULL;
	}
}
}
}
}
}
