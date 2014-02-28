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

#include "gingancl/emconverter/FormatterLinkConverter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace emconverter {
	FormatterLinkConverter::FormatterLinkConverter(
		    FormatterConverter* compiler) {

		this->compiler = compiler;
	}

	FormatterLinkConverter::~FormatterLinkConverter() {
		compiler = NULL;
	}

	FormatterCausalLink* FormatterLinkConverter::createCausalLink(
		     CausalLink* ncmLink,
		     CompositeExecutionObject* parentObject,
		     int depthLevel) {

		CausalConnector* connector;
		ConditionExpression* conditionExpression;
		Action* actionExpression;
		LinkCondition* formatterCondition;
		LinkAction* formatterAction;
		FormatterCausalLink* formatterLink;
		vector<LinkAction*>* acts;
		vector<LinkAction*>::iterator i;
		LinkAssignmentAction* action;
		string value;
		FormatterEvent* event;
		Animation* anim;

		clog << "FormatterLinkConverter::createCausalLink inside '";
		clog << parentObject->getId() << "' from ncmlinkId '";
		clog << ncmLink->getId() << "'" << endl;

		if (ncmLink == NULL) {
			clog << "FormatterLinkConverter::createCausalLink Warning!";
			clog << " cannot create formatter link inside '";
			clog << parentObject->getId() << "' NCM link is NULL" << endl;
			return NULL;
		}

		if (!ncmLink->isConsistent()) {
			clog << "FormatterLinkConverter::createCausalLink Warning!";
			clog << " cannot create formatter link inside '";
			clog << parentObject->getId() << "' from ncmlinkId '";
			clog << ncmLink->getId() << "': inconsistent link (check min and ";
			clog << "max attributes!)" << endl;
			return NULL;
		}

		// compile link condition and verify if it is a trigger condition
		connector = (CausalConnector*)(ncmLink->getConnector());
		conditionExpression = connector->getConditionExpression();
		formatterCondition = createCondition(
			    (TriggerExpression*)conditionExpression,
			    ncmLink,
			    parentObject,
				depthLevel);

		if (formatterCondition == NULL ||
			    !(formatterCondition->instanceOf("LinkTriggerCondition"))) {

			clog << "FormatterLinkConverter::createCausalLink Warning!";
			clog << " cannot create formatter link inside '";
			clog << parentObject->getId() << "' from ncmlinkId '";
			clog << ncmLink->getId() << "' with a unknown condition (";
			clog << formatterCondition << ")" << endl;
			if (formatterCondition != NULL) {
				delete formatterCondition;
			}
			return NULL;
		}

		// compile link action
		actionExpression = connector->getAction();
		formatterAction = createAction(
			    actionExpression, ncmLink, parentObject, depthLevel);

		if (formatterAction == NULL) {
			clog << "FormatterLinkConverter::createCausalLink Warning!";
			clog << " cannot create formatter link inside '";
			clog << parentObject->getId() << "' from ncmlinkId '";
			clog << ncmLink->getId() << "' with a NULL action (";
			clog << "actionExpression '" << actionExpression->toString();
			clog << endl;
			return NULL;
		}

		// create formatter causal link
		formatterLink = new FormatterCausalLink(
			    (LinkTriggerCondition*)formatterCondition,
			    formatterAction,
			    ncmLink,
			    (CompositeExecutionObject*)parentObject);

		//&got
		if (formatterCondition->instanceOf("LinkCompoundTriggerCondition")) {
			acts = formatterAction->getImplicitRefRoleActions();
			if (acts != NULL) {
				i = acts->begin();
				while (i != acts->end()) {
					action = (LinkAssignmentAction*)(*i);
					value = action->getValue();
					if (value != "" && value.substr(0, 1) == "$") {
						event = ((LinkRepeatAction*)(*i))->getEvent();
						setImplicitRefAssessment(
								value.substr(1, value.length()),
								ncmLink,
								event);
					}

					anim = action->getAnimation();
					if (anim != NULL) {
						value = anim->getDuration();
						if (value != "" && value.substr(0, 1) == "$") {
							event = ((LinkRepeatAction*)(*i))->getEvent();
							setImplicitRefAssessment(
									value.substr(1, value.length()),
									ncmLink,
									event);
						}

						value = anim->getBy();
						if (value != "" && value.substr(0, 1) == "$") {
							event = ((LinkRepeatAction*)(*i))->getEvent();
							setImplicitRefAssessment(
									value.substr(1, value.length()),
									ncmLink,
									event);
						}
					}
					++i;
				}

				delete acts;
			}
		}

		return formatterLink;
	}

	void FormatterLinkConverter::setImplicitRefAssessment(
			string roleId, CausalLink* ncmLink, FormatterEvent* event) {

		FormatterEvent* refEvent;
		vector<Node*>* ncmPerspective;
		vector<Bind*>* binds;
		vector<Bind*>::iterator i;
		NodeNesting* refPerspective;
		ExecutionObject* refObject;
		InterfacePoint* refInterface;
		string value;

		if (event->instanceOf("AttributionEvent")) {
			binds = ncmLink->getBinds();
			i = binds->begin();
			while (i != binds->end()) {
				value = (*i)->getRole()->getLabel();
				if (roleId == value) {
					refInterface = (*i)->getInterfacePoint();
					if (refInterface != NULL && refInterface->instanceOf(
							"PropertyAnchor")) {

						ncmPerspective = (*i)->getNode()->getPerspective();
						refPerspective = new NodeNesting(ncmPerspective);

						delete ncmPerspective;

						/*clog << "FormatterLinkConverter::";
						clog << "createCausalLink '";
						clog << " perspective = '";
						clog << refPerspective->getId() << "'";
						if ((*j)->getDescriptor() != NULL) {
							clog << " descriptor = '";
							clog << (*j)->getDescriptor()->getId();
							clog << "'";
						}
						clog << endl;*/

						refObject = compiler->getExecutionObjectFromPerspective(
								refPerspective, (*i)->getDescriptor(), 1);

						delete refPerspective;

						refEvent = compiler->getEvent(
								refObject,
								refInterface,
								EventUtil::EVT_ATTRIBUTION, "");

						((AttributionEvent*)event)->
								setImplicitRefAssessmentEvent(roleId, refEvent);

						break;
					}
				}
				++i;
			}
		}
	}

	LinkAction* FormatterLinkConverter::createAction(
		   Action* actionExpression,
		   CausalLink* ncmLink,
		   CompositeExecutionObject* parentObject,
		   int depthLevel) {

		double delay;
		SimpleAction* sae;
		CompoundAction* cae;
		vector<Bind*>* binds;
		int i, size;
		string delayObject;
		LinkSimpleAction* simpleAction;
		LinkCompoundAction* compoundAction;

		if (actionExpression == NULL) {
			clog << "FormatterLinkConverter::createAction ";
			clog << "Warning! ActionExpression is NULL" << endl;
			return NULL;
		}

		if (actionExpression->instanceOf("SimpleAction")) {
			sae = (SimpleAction*)actionExpression;
			binds = ncmLink->getRoleBinds(sae);
			if (binds != NULL) {
				size = binds->size();
				if (size == 1) {
					return createSimpleAction(
						   sae, (*binds)[0], ncmLink, parentObject, depthLevel);

				} else if (size > 1) {
					compoundAction = new LinkCompoundAction(
						    sae->getQualifier());

					for (i = 0; i < size; i++) {
						simpleAction = createSimpleAction(
							    sae,
							    (*binds)[i],
							    ncmLink,
							    parentObject,
							    depthLevel);

						if (simpleAction == NULL) {
							clog << "FormatterLinkConverter::createAction ";
							clog << "Warning! cannot create compound action: ";
							clog << "found invalid action(s)" << endl;
							delete compoundAction;
							return NULL;
						}
						compoundAction->addAction(simpleAction);
					}

					return compoundAction;

				} else {
					clog << "FormatterLinkConverter::createAction ";
					clog << "Warning! cannot create action of link '";
					clog << ncmLink->getId() << "' because ";
					clog << "number of binds is = " << size << endl;
					return NULL;
				}
			}

		} else { // CompoundAction)
			delayObject = actionExpression->getDelay();
			delay = compileDelay(ncmLink, delayObject, NULL);
			cae = (CompoundAction*)actionExpression;
			return createCompoundAction(
				    cae->getOperator(),
				    delay,
				    cae->getActions(),
				    ncmLink,
				    parentObject,
				    depthLevel);

		}

		clog << "FormatterLinkConverter::createAction ";
		clog << "Warning! cannot create action of link '";
		clog << ncmLink->getId() << "' returning NULL" << endl;
		return NULL;
	}

	LinkCondition* FormatterLinkConverter::createCondition(
		    ConditionExpression* ncmExpression,
		    CausalLink* ncmLink,
		    CompositeExecutionObject* parentObject,
		    int depthLevel) {

		if (ncmExpression->instanceOf("TriggerExpression")) {
			return createCondition(
				    (TriggerExpression*)ncmExpression,
				    ncmLink,
				    parentObject,
				    depthLevel);

		} else { // IStatement
			return createStatement(
				    (Statement*)ncmExpression,
				    ncmLink,
				    parentObject,
				    depthLevel);
		}
	}

	LinkCompoundTriggerCondition*
		FormatterLinkConverter::createCompoundTriggerCondition(
		    short op,
		    double delay,
		    vector<ConditionExpression*>* ncmChildConditions,
		    CausalLink* ncmLink,
		    CompositeExecutionObject* parentObject,
		    int depthLevel) {

		LinkCompoundTriggerCondition* condition;
		ConditionExpression* ncmChildCondition;
		LinkCondition* childCondition;

		if (op == CompoundCondition::OP_AND) {
			condition = new LinkAndCompoundTriggerCondition();

		} else {
			condition = new LinkCompoundTriggerCondition();
		}

		if (delay > 0) {
			condition->setDelay(delay);
		}

		if (ncmChildConditions != NULL) {
			vector<ConditionExpression*>::iterator i;
			i = ncmChildConditions->begin();
			while (i != ncmChildConditions->end()) {
				ncmChildCondition = (*i);
				childCondition = createCondition(
					    ncmChildCondition, ncmLink, parentObject, depthLevel);

				condition->addCondition(childCondition);
				++i;
			}
		}

		return condition;
	}

	LinkCondition* FormatterLinkConverter::createCondition(
		    TriggerExpression* condition,
		    CausalLink* ncmLink,
		    CompositeExecutionObject* parentObject,
		    int depthLevel) {

		double delay;
		SimpleCondition* ste;
		CompoundCondition* cte;
		vector<Bind*>* binds;
		int i, size;
		string delayObject;
		LinkCompoundTriggerCondition* compoundCondition;
		LinkTriggerCondition* simpleCondition;

		if (condition->instanceOf("SimpleCondition")) {
			ste = (SimpleCondition*)condition;
			binds = ncmLink->getRoleBinds(ste);
			if (binds != NULL) {
				size = binds->size();
				if (size == 1) {
					return createSimpleCondition(
						    ste,
						    (*binds)[0], ncmLink, parentObject, depthLevel);

				} else if (size > 1) {
					if (ste->getQualifier() ==
						    CompoundCondition::OP_AND) {

						compoundCondition =
							    new LinkAndCompoundTriggerCondition();

					} else {
						compoundCondition = new LinkCompoundTriggerCondition();
					}

					for (i = 0; i < size; i++) {
						simpleCondition = createSimpleCondition(ste,
							    (*binds)[i], ncmLink, parentObject, depthLevel);

						compoundCondition->addCondition(simpleCondition);
					}
					return compoundCondition;

				} else {
					clog << "FormatterLinkConverter::createCondition ";
					clog << "Warning! cannot create condition of link '";
					clog << ncmLink->getId() << "' because ";
					clog << "number of binds is = " << size << endl;
					return NULL;
				}
			}

		} else { // CompoundCondition
			delayObject = condition->getDelay();
			delay = compileDelay(ncmLink, delayObject, NULL);
			cte = (CompoundCondition*)condition;
			return createCompoundTriggerCondition(
				    cte->getOperator(),
				    delay,
				    cte->getConditions(),
				    ncmLink, parentObject, depthLevel);
		}

		clog << "FormatterLinkConverter::createCondition ";
		clog << "Warning! cannot create condition of link '";
		clog << ncmLink->getId() << "' returning NULL" << endl;
		return NULL;
	}

	LinkAssessmentStatement* FormatterLinkConverter::createAssessmentStatement(
		    AssessmentStatement* assessmentStatement,
		    Bind* bind,
		    Link* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		LinkAttributeAssessment* mainAssessment;
		LinkAssessment* otherAssessment;
		AttributeAssessment* aa;
		LinkAssessmentStatement* statement;
		ValueAssessment* valueAssessment;
		string paramValue;
		Parameter* connParam,* param;
		vector<Bind*>* otherBinds;

		mainAssessment = createAttributeAssessment(
			    assessmentStatement->getMainAssessment(),
			    bind, ncmLink, parentObject, depthLevel);

		if (assessmentStatement->getOtherAssessment()->
			    instanceOf("ValueAssessment")) {

			valueAssessment = (ValueAssessment*)(
				    assessmentStatement->getOtherAssessment());

			paramValue = valueAssessment->getValue();
			if (paramValue[0] == '$') { //instanceOf("Parameter")
				connParam = new Parameter(
					    paramValue.substr(1, paramValue.length() - 1), "");

				param = bind->getParameter(connParam->getName());
				if (param == NULL) {
					param = ncmLink->getParameter(connParam->getName());
				}

				if (param != NULL) {
					paramValue = param->getValue();
				}
			}

			otherAssessment = new LinkValueAssessment(paramValue);

		} else {
			aa = (AttributeAssessment*)(assessmentStatement->
				    getOtherAssessment());

			otherBinds = ncmLink->getRoleBinds(aa);
			if (otherBinds != NULL && !otherBinds->empty()) {
				otherAssessment = createAttributeAssessment(
					    aa,
					    (*otherBinds)[0],
					    ncmLink,
					    parentObject,
					    depthLevel);

			} else {
				otherAssessment = createAttributeAssessment(
					    aa, NULL, ncmLink, parentObject, depthLevel);
			}
		}
		statement = new LinkAssessmentStatement(
			    assessmentStatement->getComparator(),
			    mainAssessment, otherAssessment);

		return statement;
	}

	LinkStatement* FormatterLinkConverter::createStatement(
		    Statement* statementExpression,
		    Link* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		AssessmentStatement* as;
		CompoundStatement* cs;
		vector<Bind*>* binds;
		int size;
		LinkStatement* statement;
		LinkStatement* childStatement;
		vector<Statement*>* statements;
		vector<Statement*>::iterator i;
		Statement* ncmChildStatement;

		if (statementExpression->instanceOf("AssessmentStatement")) {
			as = (AssessmentStatement*)statementExpression;
			binds = ncmLink->getRoleBinds(as->getMainAssessment());
			if (binds != NULL) {
				size = binds->size();
				if (size == 1) {
					statement = createAssessmentStatement(
						    as,
						    (*binds)[0], ncmLink, parentObject, depthLevel);

				} else {
					clog << "FormatterLinkConverter::createStatement ";
					clog << "Warning! cannot create statement of link '";
					clog << ncmLink->getId() << "' because ";
					clog << "binds size = '" << size << "'" << endl;
					return NULL;
				}

			} else {
				clog << "FormatterLinkConverter::createStatement ";
				clog << "Warning! cannot create statement of link '";
				clog << ncmLink->getId() << "' because ";
				clog << "binds == NULL" << endl;
				return NULL;
			}

		} else { // CompoundStatement
			cs = (CompoundStatement*)statementExpression;
			statement = new LinkCompoundStatement(cs->getOperator());
			((LinkCompoundStatement*)statement)->setNegated(cs->isNegated());
			statements = cs->getStatements();
			if (statements != NULL) {
				i = statements->begin();
				while (i != statements->end()) {
					ncmChildStatement = (*i);
					childStatement = createStatement(
						    ncmChildStatement,
						    ncmLink, parentObject, depthLevel);

					((LinkCompoundStatement*)statement)->addStatement(
						    childStatement);

					++i;
				}
			}
		}

		return statement;
	}

	LinkAttributeAssessment* FormatterLinkConverter::createAttributeAssessment(
		    AttributeAssessment* attributeAssessment,
		    Bind* bind, Link* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		FormatterEvent* event;

		event = createEvent(bind, ncmLink, parentObject, depthLevel);
		return new LinkAttributeAssessment(
			    event, attributeAssessment->getAttributeType());
	}

	LinkSimpleAction* FormatterLinkConverter::createSimpleAction(
			SimpleAction* sae,
			Bind* bind, Link* ncmLink,
			CompositeExecutionObject* parentObject, int depthLevel) {

		FormatterEvent* event;
		short actionType;
		short eventType = -1;
		LinkSimpleAction* action;
		Parameter* connParam;
		Parameter* param;
		string paramValue;
		Animation* animation;
		long repeat;
		double delay;
		Animation* newAnimation;
		bool isUsing;

		newAnimation = new Animation();
		isUsing      = false;
		action       = NULL;
		event        = createEvent(bind, ncmLink, parentObject, depthLevel);

		actionType = sae->getActionType();
		if (event != NULL) {
			eventType = bind->getRole()->getEventType();
			event->setEventType(eventType);
			/*
			if (event->instanceOf("PresentationEvent")) {
				eventType = EventUtil::EVT_PRESENTATION;

			} else if (event->instanceOf("AttributionEvent")) {
				eventType = EventUtil::EVT_ATTRIBUTION;

			} else if (event->instanceOf("SwitchEvent")) {
				eventType = bind->getRole()->getEventType();

			} else {
				clog << "FormatterLinkConverter::createSimpleAction Warning! ";
				clog << "Event isn't presentation neither attribution ";
				clog << "bind role event type is '";
				clog << bind->getRole()->getEventType() << "'";
				clog << endl;
			}*/

		} else {
			clog << "FormatterLinkConverter::createSimpleAction Warning! ";
			clog << "Trying to create a simple action with a NULL event";
			clog << endl;
		}

		switch (actionType) {
			case SimpleAction::ACT_START:
			case SimpleAction::ACT_SET:
				if (eventType == EventUtil::EVT_PRESENTATION) {
					action = new LinkRepeatAction(event, actionType);

					//repeat
					paramValue = sae->getRepeat();
					if (paramValue == "") {
						repeat = 0;

					} else if (paramValue[0] == '$') {
						connParam = new Parameter(
								paramValue.substr(
										1, paramValue.length() - 1), "");

						param = bind->getParameter(connParam->getName());
						if (param == NULL) {
							param = ncmLink->getParameter(connParam->getName());
						}

						if (param == NULL) {
							repeat = 0;

						} else {
							repeat = (long)util::stof(param->getValue());
						}

					} else {
						repeat = (long)util::stof(paramValue);
					}

					((LinkRepeatAction*)action)->setRepetitions(repeat);

					//repeatDelay
					paramValue = sae->getRepeatDelay();
					delay = compileDelay(ncmLink, paramValue, bind);
					((LinkRepeatAction*)action)->setRepetitionInterval(delay);

				} else if (eventType == EventUtil::EVT_ATTRIBUTION) {
					paramValue = sae->getValue();
					if (paramValue != "" && paramValue[0] == '$') {
						connParam = new Parameter(
								paramValue.substr(
										1, paramValue.length() - 1), "");

						param = bind->getParameter(connParam->getName());
						if (param == NULL) {
							param = ncmLink->getParameter(connParam->getName());
						}

						delete connParam;
						connParam = NULL;

						if (param != NULL) {
							paramValue = param->getValue();

						} else {
							paramValue = "";
						}
					}

					action = new LinkAssignmentAction(
							event, actionType, paramValue);

					//animation
					animation = sae->getAnimation();

					if (animation != NULL) {
						string durVal = "0";
						string byVal  = "0";

						paramValue = animation->getDuration();
						if (paramValue[0] == '$') {
							connParam = new Parameter(
									paramValue.substr(
											1, paramValue.length() - 1), "");

							param = bind->getParameter(connParam->getName());
							if (param == NULL) {
								param = ncmLink->getParameter(
										connParam->getName());
							}

							delete connParam;
							connParam = NULL;

							if (param != NULL) {
								durVal = param->getValue();
							}

							newAnimation->setDuration(durVal);

						} else {
							durVal = paramValue;
							newAnimation->setDuration(durVal);
						}

						paramValue = animation->getBy();
						if (paramValue[0] == '$') {
							connParam = new Parameter(
									paramValue.substr(
											1, paramValue.length() - 1), "");

							param = bind->getParameter(connParam->getName());
							if (param == NULL) {
								param = ncmLink->getParameter(
										connParam->getName());
							}

							delete connParam;
							connParam = NULL;

							if (param != NULL) {
								byVal = param->getValue();
							}

							newAnimation->setBy(byVal);

						} else {
							byVal = paramValue;
							newAnimation->setBy(byVal);
						}

						if (durVal != "0") {
							isUsing = true;
							((LinkAssignmentAction*)action)->setAnimation(
									newAnimation);
						}
					}

				} else {
					clog << "FormatterLinkConverter::createSimpleAction ";
					clog << "Warning! Unknown event type '" << eventType;
					clog << "' for action type '";
					clog << actionType << "'" << endl;
				}
				break;

			case SimpleAction::ACT_STOP:
			case SimpleAction::ACT_PAUSE:
			case SimpleAction::ACT_RESUME:
			case SimpleAction::ACT_ABORT:
				action = new LinkSimpleAction(event, actionType);
				break;

			default:
				action = NULL;
				clog << "FormatterLinkConverter::createSimpleAction ";
				clog << "Warning! Unknown action type '";
				clog << actionType << "'" << endl;
				break;
		}

		if (action != NULL) {
			paramValue = sae->getDelay();
			delay = compileDelay(ncmLink, paramValue, bind);
			action->setWaitDelay(delay);
		}

		if (!isUsing) {
			delete newAnimation;
		}

		return action;
	}

	LinkCompoundAction* FormatterLinkConverter::createCompoundAction(
		    short op,
		    double delay,
		    vector<Action*>* ncmChildActions,
		    CausalLink* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		LinkCompoundAction* action;
		Action* ncmChildAction;
		LinkAction* childAction;

		action = new LinkCompoundAction(op);
		if (delay > 0) {
			action->setWaitDelay(delay);
		}

		if (ncmChildActions != NULL) {
			vector<Action*>::iterator i;
			i = ncmChildActions->begin();
			while (i != ncmChildActions->end()) {
				ncmChildAction = (*i);
				childAction = createAction(
					    ncmChildAction, ncmLink, parentObject, depthLevel);

				if (childAction != NULL) {
					action->addAction(childAction);

				} else {
					clog << "FormatterLinkConverter::createCompoundAction ";
					clog << "creating link '" << ncmLink->getId() << "' ";
					clog << "Warning! Can't create ";
					if (ncmChildAction->instanceOf("SimpleAction")) {
						clog << "simple action type '";
						clog << ((SimpleAction*)ncmChildAction)->getActionType();
						clog << "'" << endl;

					} else if (ncmChildAction->instanceOf("CompoundAction")) {
						clog << "inner compound action " << endl;
					}
				}
				++i;
			}
		}

		return action;
	}

	LinkTriggerCondition* FormatterLinkConverter::createSimpleCondition(
		    SimpleCondition* simpleCondition,
		    Bind* bind,
		    Link* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		FormatterEvent* event;
		double delay;
		string delayObject;
		LinkTriggerCondition* condition;

		event = createEvent(bind, ncmLink, parentObject, depthLevel);
		condition = new LinkTransitionTriggerCondition(
			    event, simpleCondition->getTransition(), bind);

		delayObject = simpleCondition->getDelay();
		delay = compileDelay(ncmLink, delayObject, bind);
		if (delay > 0) {
			condition->setDelay(delay);
		}
		return condition;
	}

	FormatterEvent* FormatterLinkConverter::createEvent(
		    Bind* bind,
		    Link* ncmLink,
		    CompositeExecutionObject* parentObject, int depthLevel) {

		NodeNesting* endPointNodeSequence;
		NodeNesting* endPointPerspective;
		Node* parentNode;
		ExecutionObject* executionObject;
		InterfacePoint* interfacePoint;
		string key;
		FormatterEvent* event = NULL;
		vector<Node*>* seq;

		endPointPerspective = parentObject->getNodePerspective();

		// parent object may be a refer
		parentNode = endPointPerspective->getAnchorNode();

		// teste para verificar se ponto terminal eh o proprio contexto ou
		// refer para o proprio contexto
		seq = bind->getNodeNesting();
		endPointNodeSequence = new NodeNesting(seq);
		if (endPointNodeSequence->getAnchorNode() !=
			    endPointPerspective->getAnchorNode() &&
			    endPointNodeSequence->getAnchorNode() !=
			    parentNode->getDataEntity()) {

			endPointPerspective->append(endPointNodeSequence);
		}

		delete seq;
		delete endPointNodeSequence;

		try {
			/*clog << "FormatterLinkConverter::createEvent '";
			clog << " perspective = '" << endPointPerspective->getId() << "'";
			if (bind->getDescriptor() != NULL) {
				clog << " descriptor = '" << bind->getDescriptor()->getId();
				clog << "'";
			}
			clog << endl;*/

			executionObject = compiler->getExecutionObjectFromPerspective(
				    endPointPerspective, bind->getDescriptor(), depthLevel);

			if (executionObject == NULL) {
				clog << "FormatterLinkConverter::createEvent Warning! ";
				clog << "can't find execution object for perspective '";
				clog << endPointPerspective->getId() << "'" << endl;

				delete endPointPerspective;
				return NULL;
			}

		} catch (ObjectCreationForbiddenException* exc) {
			clog << "FormatterLinkConverter::createEvent Warning! ";
			clog << "can't execution object exception for perspective '";
			clog << endPointPerspective->getId() << "'" << endl;

			delete endPointPerspective;
			return NULL;
		}

		interfacePoint = bind->getEndPointInterface();
		if (interfacePoint == NULL) {
			//TODO: This is an error, the formatter then return the main event
			clog << "FormatterLinkConverter::createEvent Warning! ";
			clog << "can't find an interface point for '";
			clog << endPointPerspective->getId() << "' bind '";
			clog << bind->getRole()->getLabel() << "'" << endl;
			delete endPointPerspective;
			return executionObject->getWholeContentPresentationEvent();
		}

		key = getBindKey(ncmLink, bind);
		event = compiler->getEvent(
			    executionObject,
			    interfacePoint, bind->getRole()->getEventType(), key);

		delete endPointPerspective;
		return event;
	}

	double FormatterLinkConverter::getDelayParameter(
		    Link* ncmLink, Parameter* connParam, Bind* ncmBind) {

		Parameter* parameter;
		string param;

		parameter = NULL;
		if (ncmBind != NULL) {
			parameter = ncmBind->getParameter(connParam->getName());
		}

		if (parameter == NULL) {
			parameter = ncmLink->getParameter(connParam->getName());
		}

		if (parameter == NULL) {
			return 0.0;

		} else {
			try {
				param = parameter->getValue();
				if (param == "") {
					return 0.0;
				} else {
					return util::stof(param) * 1000;
				}

			} catch (exception* exc) {
				return 0.0;
			}
		}
	}

	string FormatterLinkConverter::getBindKey(Link* ncmLink, Bind* ncmBind) {
		Role* role;
		string keyValue;
		Parameter* param,* auxParam;
		string key;

		role = ncmBind->getRole();
		if (role == NULL) {
			return "";
		}

		if (role->instanceOf("SimpleCondition")) {
			keyValue = ((SimpleCondition*)role)->getKey();

		} else if (role->instanceOf("AttributeAssessment")) {
			keyValue = ((AttributeAssessment*)role)->getKey();

		} else {
			return "";
		}

		if (keyValue == "") {
			key = "";

		} else if (keyValue[0] == '$') { // instanceof Parameter
			param = new Parameter(
				    keyValue.substr(1, keyValue.length() - 1), "");

			auxParam = ncmBind->getParameter(param->getName());
			if (auxParam == NULL) {
				auxParam = ncmLink->getParameter(param->getName());
			}

			if (auxParam != NULL) {
				key = auxParam->getValue();

			} else {
				key = "";
			}

			delete param;

		} else {
			key = keyValue;
		}

		return key;
	}

	double FormatterLinkConverter::compileDelay(
		    Link* ncmLink,
		    string delayObject, Bind* bind) {

		double delay;
		string::size_type pos;
		Parameter* param;
		string delayValue;

		if (delayObject == "") {
			delay = 0;

		} else {
			pos = delayObject.find("$");
			if (pos != std::string::npos && pos == 0) { // instanceof Parameter
				delayValue = delayObject.substr(1, delayObject.length() - 1);
				param = new Parameter(delayValue, "");
				delay = getDelayParameter(ncmLink, param, bind);
				delete param;

			} else {
				delay = (double)(util::stof(delayObject));
			}
		}

		return delay;
	}
}
}
}
}
}
}
