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

#include "FormatterConverter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::emconverter;

#include "model/LinkTransitionTriggerCondition.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "FormatterMultiDevice.h"
#include "gingancl/FormatterScheduler.h"

#include "ncl/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
	FormatterScheduler::FormatterScheduler(
		    PlayerAdapterManager* playerManager,
		    RuleAdapter* ruleAdapter,
		    IFormatterMultiDevice* multiDevice,
		    void* compiler) {

		this->playerManager = playerManager;
		this->ruleAdapter   = ruleAdapter;
		this->presContext   = ruleAdapter->getPresentationContext();
		this->multiDevPres  = multiDevice;
		this->compiler      = compiler;
		this->running       = false;
		this->focusManager  = new FormatterFocusManager(
				this->playerManager,
				presContext,
				multiDevPres,
				this,
				(FormatterConverter*)compiler);

		this->presContext->setGlobalVarListener(this);

		this->multiDevPres->setFocusManager(this->focusManager);

		Thread::mutexInit(&mutexD, true);
		Thread::mutexInit(&mutexActions, true);
		Thread::mutexInit(&lMutex, true);
	}

	FormatterScheduler::~FormatterScheduler() {
		set<void*>::iterator i;
		set<FormatterEvent*>::iterator j;
		LinkSimpleAction* action;

		clog << "FormatterScheduler::~FormatterScheduler(" << this << ")";
		clog << endl;

		running = false;

		Thread::mutexLock(&lMutex);
		Thread::mutexLock(&mutexD);
		Thread::mutexLock(&mutexActions);

		j = listening.begin();
		while (j != listening.end()) {
			if (FormatterEvent::hasInstance((*j), false)) {
				(*j)->removeEventListener(this);
			}
			++j;
		}
		Thread::mutexUnlock(&lMutex);
		Thread::mutexDestroy(&lMutex);

		i = actions.begin();
		while (i != actions.end()) {
			action = (LinkSimpleAction*)(*i);
			action->setSimpleActionListener(NULL);
			++i;
		}
		actions.clear();
		Thread::mutexUnlock(&mutexActions);
		Thread::mutexDestroy(&mutexActions);

		if (presContext != NULL) {
			this->presContext->setGlobalVarListener(NULL);
		}

		ruleAdapter   = NULL;
		presContext   = NULL;

		if (focusManager != NULL) {
			delete focusManager;
			focusManager = NULL;
		}

		schedulerListeners.clear();
		compiler = NULL;
		documentEvents.clear();
		documentStatus.clear();

		Thread::mutexUnlock(&mutexD);
		Thread::mutexDestroy(&mutexD);
	}

	void FormatterScheduler::addAction(void* action) {
		Thread::mutexLock(&mutexActions);
		actions.insert(action);
		Thread::mutexUnlock(&mutexActions);
	}

	void FormatterScheduler::removeAction(void* action) {
		set<void*>::iterator i;
		vector<LinkSimpleAction*>::iterator j;

		Thread::mutexLock(&mutexActions);
		i = actions.find(action);
		if (i != actions.end()) {
			actions.erase(i);
		}
		Thread::mutexUnlock(&mutexActions);
	}

	FormatterFocusManager* FormatterScheduler::getFocusManager() {
		return focusManager;
	}

	void* FormatterScheduler::getFormatterLayout(
			void* descriptor, void* object) {

		return ((FormatterMultiDevice*)multiDevPres)->getFormatterLayout(
				(CascadingDescriptor*)descriptor,
				(ExecutionObject*)object);
	}

	bool FormatterScheduler::setKeyHandler(bool isHandler) {
		return focusManager->setKeyHandler(isHandler);
	}

	/*void FormatterScheduler::setStandByState(bool standBy) {
		focusManager->setStandByState(standBy);
	}*/

	bool FormatterScheduler::isDocumentRunning(FormatterEvent* event) {
		ExecutionObject* executionObject;
		CompositeExecutionObject* parentObject;
		FormatterEvent* documentEvent;

		/*NodeEntity* dataObject;
		NodeNesting* referPerspective;
		set<ReferNode*>* sameInstances = NULL;
		set<ReferNode*>::iterator i;*/

		executionObject = (ExecutionObject*)(event->getExecutionObject());
		/*dataObject = (NodeEntity*)(executionObject->getDataObject());

		if (dataObject != NULL && dataObject->instanceOf("NodeEntity")) {
			sameInstances = dataObject->getSameInstances();
			if (sameInstances != NULL && !sameInstances->empty()) {
				i = sameInstances->begin();
				while (i != sameInstances->end()) {
					referPerspective = new NodeNesting((*i)->getPerspective());
					++i;
				}
			}
		}*/

		parentObject = (CompositeExecutionObject*)(executionObject->
			    getParentObject());

		if (parentObject != NULL) {
			while (parentObject->getParentObject() != NULL) {
				executionObject = (ExecutionObject*)(parentObject);
				parentObject = (CompositeExecutionObject*)(parentObject->
					   getParentObject());
			}

			documentEvent = executionObject->getWholeContentPresentationEvent();

		} else {
			documentEvent = event;
		}

		Thread::mutexLock(&mutexD);
		if (documentStatus.count(documentEvent) != 0) {
			bool eventStatus = documentStatus[documentEvent];
			Thread::mutexUnlock(&mutexD);
			return eventStatus;
		}

		Thread::mutexUnlock(&mutexD);
		return false;
	}

	void FormatterScheduler::setTimeBaseObject(
		    ExecutionObject* object,
		    FormatterPlayerAdapter* objectPlayer, string nodeId) {

		ExecutionObject* documentObject;
		ExecutionObject* parentObject;
		ExecutionObject* timeBaseObject;

		Node* documentNode;
		Node* compositeNode;
		Node* timeBaseNode;
		NodeNesting* perspective;
		NodeNesting* compositePerspective;
		FormatterPlayerAdapter* timeBasePlayer;

		if (nodeId.find_last_of('#') != std::string::npos) {
		  	return;
		}

		documentObject = object;
		parentObject = (ExecutionObject*)(documentObject->getParentObject());
		if (parentObject != NULL) {
			while (parentObject->getParentObject() != NULL) {
				documentObject = parentObject;
				if (documentObject->getDataObject()->instanceOf("ReferNode")) {
					break;
				}
				parentObject = (ExecutionObject*)(documentObject->
					    getParentObject());
			}
		}

		if (documentObject == NULL || documentObject->getDataObject() == NULL){
			return;
		}

		documentNode = documentObject->getDataObject();
		if (documentNode->instanceOf("ReferNode")) {
			compositeNode = (NodeEntity*)((ReferNode*)documentNode)->
				    getReferredEntity();
		} else {
			compositeNode = documentNode;
		}

		if (compositeNode == NULL ||
			    !(compositeNode->instanceOf("CompositeNode"))) {

			return;
		}

		timeBaseNode = ((CompositeNode*)compositeNode)->
			    recursivelyGetNode(nodeId);

		if (timeBaseNode == NULL ||
			    !(timeBaseNode->instanceOf("ContentNode"))) {

			return;
		}

		perspective = new NodeNesting(timeBaseNode->getPerspective());
		if (documentNode->instanceOf("ReferNode")) {
			perspective->removeHeadNode();
			compositePerspective = new NodeNesting(
				    documentNode->getPerspective());

			compositePerspective->append(perspective);
			perspective = compositePerspective;
		}

		try {
			timeBaseObject = ((FormatterConverter*)compiler)->
				    getExecutionObjectFromPerspective(
				    	    perspective,
				    	    NULL,
				    	    ((FormatterConverter*)compiler)->getDepthLevel());

			if (timeBaseObject != NULL) {
				timeBasePlayer = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(timeBaseObject);
				if (timeBasePlayer != NULL) {
					objectPlayer->setTimeBasePlayer(timeBasePlayer);
				}
			}

		} catch (ObjectCreationForbiddenException* exc) {
			return;
		}
	}

	void FormatterScheduler::printAction(
			string action,
			LinkCondition* condition,
			LinkSimpleAction* linkAction) {

		
		ExecutionObject* cndObject;
		ExecutionObject* object;
		FormatterEvent* cndEvent;
		FormatterEvent* event;
		double delay;
		Node* node;
		Bind* bind;
		string role;
		string nodeId;
		double specTime;

		event  = linkAction->getEvent();
		object = (ExecutionObject*)event->getExecutionObject();
		node   = object->getDataObject();

		if (node->instanceOf("CompositeNode")) {
			return;
		}

		action = action + "::" + node->getId();
		if (condition != NULL &&
				condition->instanceOf("LinkTransitionTriggerCondition")) {

			cndEvent  = ((LinkTransitionTriggerCondition*)condition)->getEvent();
			bind      = ((LinkTransitionTriggerCondition*)condition)->getBind();
			role      = bind->getRole()->getLabel();
			cndObject = (ExecutionObject*)cndEvent->getExecutionObject();
			nodeId    = cndObject->getDataObject()->getId();
			//nodeId    = bind->getNode()->getId();

			delay = linkAction->getWaitDelay();
			specTime = 0;
			if (role == "onBegin") {
				if (cndEvent->instanceOf("PresentationEvent")) {
					specTime = ((PresentationEvent*)cndEvent)->getBegin();
				}

			} else if (role == "onEnd") {
				if (cndEvent->instanceOf("PresentationEvent")) {
					specTime = ((PresentationEvent*)cndEvent)->getEnd();
				}
			}

			specTime = specTime + delay;
			if (isInfinity(specTime)) {
				specTime = -1;
			}

			action = action + "::" + role + "::" + nodeId;
			action = action + "::" + itos(specTime);
		}

		FormatterPlayerAdapter::printAction(action);
	}

	void FormatterScheduler::scheduleAction(void* condition, void* someAction) {
		pthread_mutex_lock(&mutexActions);
		assert(someAction != NULL);
		runAction(
				(LinkCondition*)condition,
				(LinkSimpleAction*)someAction);

		pthread_mutex_unlock(&mutexActions);

		return;
	}

	void FormatterScheduler::runAction(
			LinkCondition* condition, LinkSimpleAction* action) {

		FormatterEvent* event = action->getEvent();

		assert(event != NULL);

		if (event->instanceOf("SelectionEvent")) {
			event->start();
			delete action;
			return;
		}

		runAction(event, condition, action);
	}

	void FormatterScheduler::runAction(
			FormatterEvent* event,
			LinkCondition* condition,
			LinkSimpleAction* action) {

		ExecutionObject* executionObject;
		CascadingDescriptor* descriptor;
		FormatterPlayerAdapter* player;
		IPlayer* playerContent;
		NodeEntity* dataObject;
		short actionType;
		string attName;
		string attValue;
		double time;
		GingaWindowID winId = 0;

		time            = getCurrentTimeMillis();
		executionObject = (ExecutionObject*)(event->getExecutionObject());

		if (isDocumentRunning(event) && !executionObject->isCompiled()) {
			((FormatterConverter*)compiler)->compileExecutionObjectLinks(
				    executionObject,
				    ((FormatterConverter*)compiler)->getDepthLevel());
		}

		dataObject = (NodeEntity*)(executionObject->getDataObject()->
			    getDataEntity());

		if (dataObject->instanceOf("NodeEntity")) {
			set<ReferNode*>* gradSame;
			set<ReferNode*>::iterator i;
			gradSame = ((NodeEntity*)dataObject)->getGradSameInstances();
			if (gradSame != NULL) {
				((FormatterConverter*)compiler)->checkGradSameInstance(
						gradSame, executionObject);

				/*clog << "FormatterScheduler::runAction refer = '";
				clog << dataObject->getId() << "' perspective = '";
				clog << executionObject->getNodePerspective()->
						getHeadNode()->getId() << "'";

				clog << endl;*/
			}
		}

		if (executionObject->instanceOf("ExecutionObjectSwitch") &&
			    event->instanceOf("SwitchEvent")) {

			runActionOverSwitch(
				    (ExecutionObjectSwitch*)executionObject,
				    (SwitchEvent*)event, action);

		} else if (executionObject->instanceOf("CompositeExecutionObject") &&
				(executionObject->getDescriptor() == NULL ||
				executionObject->getDescriptor()->getPlayerName() == "")) {

			clog << "FormatterScheduler::runAction event '";
			clog << event->getId() << "' for '";
			clog << executionObject->getId() << "' OVER COMPOSITION with ";
			clog << "action event = '" << action->getEvent()->getId() << "'";
			clog << endl;
			runActionOverComposition(
				    (CompositeExecutionObject*)executionObject, action);

		} else if (event->instanceOf("AttributionEvent")) {
			clog << "FormatterScheduler::runAction event '";
			clog << event->getId() << "' for '";
			clog << executionObject->getId() << "' OVER PROPERTY";
			clog << endl;
			runActionOverProperty(event, action);

		} else {
			clog << "FormatterScheduler::runAction ";
			clog << "acquiring player for '" << executionObject->getId();
			clog << "' ";
			player = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(executionObject);
			if (player == NULL) {
				clog << "Player = NULL for ";
				clog << executionObject->getId() << endl;
				return;
			}

			if (executionObject->instanceOf("ApplicationExecutionObject") &&
					!event->instanceOf("AttributionEvent")) {

				clog << "FormatterScheduler::runAction event '";
				clog << event->getId() << "' for '";
				clog << executionObject->getId();
				clog << "' action '" << action->getType() << "'" << endl;

				runActionOverApplicationObject(
						(ApplicationExecutionObject*)executionObject,
						event,
						player,
						action);

				return;
			}

			actionType = action->getType();
			switch (actionType) {
				case SimpleAction::ACT_START:
					if (!player->hasPrepared()) {
						if (ruleAdapter->adaptDescriptor(executionObject)) {
							descriptor = executionObject->getDescriptor();
							if (descriptor != NULL) {
								descriptor->setFormatterLayout(
										getFormatterLayout(
												descriptor, executionObject));
							}
						}

						if (!player->prepare(
								executionObject, (PresentationEvent*)event)) {

							return;
						}

						playerContent = player->getPlayer();
						if (playerContent != NULL) {
							multiDevPres->listenPlayer(playerContent);
						}

						if (executionObject != NULL && executionObject->
								getDescriptor() != NULL) {

							// look for a reference time base player
							attValue = executionObject->getDescriptor()->
									getParameterValue("x-timeBaseObject");

							if (attValue != "") {
								setTimeBaseObject(
										executionObject,
										player,
										attValue);
							}
						}

						if (playerContent != NULL) {
							winId = ((FormatterMultiDevice*)multiDevPres)->
									prepareFormatterRegion(
											executionObject,
											playerContent->getSurface());

							player->setOutputWindow(winId);
							player->flip();
						}

						event->addEventListener(this);
						Thread::mutexLock(&lMutex);
						listening.insert(event);
						Thread::mutexUnlock(&lMutex);
					}

					if (!player->start()) {
						set<FormatterEvent*>::iterator it;

						clog << "FormatterScheduler::runAction can't start '";
						clog << executionObject->getId() << "'";
						clog << endl;

						if (event->getCurrentState() == EventUtil::ST_SLEEPING) {
							event->removeEventListener(this);
							Thread::mutexLock(&lMutex);
							it = listening.find(event);
							if (it != listening.end()) {
								listening.erase(it);
							}
							Thread::mutexUnlock(&lMutex);
						}

					} else {
						time = getCurrentTimeMillis() - time;
						/*clog << "FormatterScheduler::runAction takes '";
						clog << time << "' ms to start '";
						clog << executionObject->getId() << "'";
						clog << endl;*/

						printAction("start", condition, action);

					}
					break;

				case SimpleAction::ACT_PAUSE:
					/*clog << "FormatterScheduler::runAction event '";
					clog << event->getId() << "' for '";
					clog << executionObject->getId() << "' PAUSE" << endl;*/
					if (!player->pause()) {
						/*clog << "FormatterScheduler::runAction event '";
						clog << event->getId() << "' for '";
						clog << executionObject->getId() << "' PLAYER ";
						clog << "is not able to pause event anymore. ";
						clog << endl;*/

					} else {
						printAction("pause", condition, action);
					}
					break;

				case SimpleAction::ACT_RESUME:
					/*clog << "FormatterScheduler::runAction event '";
					clog << event->getId() << "' for '";
					clog << executionObject->getId() << "' RESUME" << endl;*/
					if (!player->resume()) {
						/*clog << "FormatterScheduler::runAction event '";
						clog << event->getId() << "' for '";
						clog << executionObject->getId() << "' PLAYER ";
						clog << "is not able to resume event anymore. ";
						clog << endl;*/

					} else {
						printAction("resume", condition, action);
					}
					break;

				case SimpleAction::ACT_ABORT:
					/*clog << "FormatterScheduler::runAction event '";
					clog << event->getId() << "' for '";
					clog << executionObject->getId() << "' ABORT" << endl;*/
					if (!player->abort()) {
						/*clog << "FormatterScheduler::runAction event '";
						clog << event->getId() << "' for '";
						clog << executionObject->getId() << "' PLAYER ";
						clog << "is not able to abort event anymore. ";
						clog << endl;*/

					} else {
						printAction("abort", condition, action);
					}
					break;

				case SimpleAction::ACT_STOP:
					/*clog << "FormatterScheduler::runAction event '";
					clog << event->getId() << "' for '";
					clog << executionObject->getId() << "' STOP" << endl;*/
					if (!player->stop()) {
						/*clog << "FormatterScheduler::runAction event '";
						clog << event->getId() << "' for '";
						clog << executionObject->getId() << "' PLAYER ";
						clog << "is not able to STOP event anymore. ";
						clog << endl;*/

					} else {
						printAction("stop", condition, action);
					}
					break;
			}
		}
	}

	void FormatterScheduler::runActionOverProperty(
			FormatterEvent* event, LinkSimpleAction* action) {

		short actionType;
		string propName, propValue;

		NodeEntity* dataObject;
		ExecutionObject* executionObject;
		FormatterPlayerAdapter* player;
		Animation* anim;

		executionObject = (ExecutionObject*)(event->getExecutionObject());
		dataObject      = (NodeEntity*)(
				executionObject->getDataObject()->getDataEntity());

		if (dataObject->instanceOf("ContentNode") &&
				((ContentNode*)dataObject)->isSettingNode() &&
				action->instanceOf("LinkAssignmentAction")) {

			propName = ((AttributionEvent*)event)->getAnchor()->
					getPropertyName();

			propValue = ((LinkAssignmentAction*)action)->getValue();
			if (propValue != "" && propValue.substr(0, 1) == "$") {
				propValue = solveImplicitRefAssessment(
						propValue,
						(AttributionEvent*)event);
			}

			event->start();
			((AttributionEvent*)event)->setValue(propValue);

			/*clog << "FormatterScheduler::runActionOverProperty settingnode";
			clog << " evId '" << event->getId() << "' for '";
			clog << executionObject->getId() << "' propName '";
			clog << porpName << "', propValue '" << propValue << "'" << endl;*/

			if (propName == "service.currentFocus") {
				focusManager->setFocus(propValue);

			} else if (propName == "service.currentKeyMaster") {
				focusManager->setKeyMaster(propValue);

			} else {
				presContext->setPropertyValue(propName, propValue);
			}
			event->stop();

		} else {

			if (action->instanceOf("LinkAssignmentAction")) {
				anim = ((LinkAssignmentAction*)action)->getAnimation();

			} else {
				anim = NULL;
			}

			player = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(executionObject);
			actionType = action->getType();

			switch (actionType) {
				case SimpleAction::ACT_START:
				case SimpleAction::ACT_SET:
					clog << "FormatterScheduler::runActionOverProperty";
					clog << " over '" << event->getId() << "' for '";
					clog << executionObject->getId() << "' player '";
					clog << player << "'" << endl;
					if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
						clog << "FormatterScheduler::runActionOverProperty";
						clog << " trying to set an event that is not ";
						clog << "sleeping: '" << event->getId() << "'" << endl;
						return;
					}

					if (action->instanceOf("LinkAssignmentAction")) {
						propValue = ((LinkAssignmentAction*)action)->getValue();
						if (propValue != "" && propValue.substr(0, 1) == "$") {
							propValue = solveImplicitRefAssessment(
									propValue,
									(AttributionEvent*)event);

							clog << "FormatterScheduler::runActionOverProperty";
							clog << " IMPLICIT found '" << propValue;
							clog << "' for event '" << event->getId() << "'";
							clog << endl;

						}

						event->start();
						((AttributionEvent*)event)->setValue(propValue);

					} else {
						event->start();
					}

					if (anim != NULL) {
						string durVal, byVal;

						durVal = anim->getDuration();
						if (durVal.substr(0, 1) == "$") {
							anim->setDuration(solveImplicitRefAssessment(
									durVal,
									(AttributionEvent*)event));
						}

						byVal = anim->getBy();
						if (byVal.substr(0, 1) == "$") {
							anim->setDuration(solveImplicitRefAssessment(
									byVal,
									(AttributionEvent*)event));
						}

						AnimationController::startAnimation(
								executionObject,
								playerManager,
								player,
								(AttributionEvent*)event,
								propValue,
								anim);

						anim->setDuration(durVal);
						anim->setBy(byVal);

					} else if (player != NULL && player->hasPrepared()) {
						player->setPropertyValue(
								(AttributionEvent*)event, propValue);

						event->stop();

					} else {
						executionObject->setPropertyValue(
								(AttributionEvent*)event,
								propValue);

						event->stop();
					}

					break;

				default:
					clog << "FormatterScheduler::runActionOverProperty";
					clog << "unknown actionType = '" << actionType << "'";
					clog << endl;
					break;
			}
		}
	}

	void FormatterScheduler::runActionOverApplicationObject(
			ApplicationExecutionObject* executionObject,
			FormatterEvent* event,
			FormatterPlayerAdapter* player,
			LinkSimpleAction* action) {

		CascadingDescriptor* descriptor;
		IPlayer* playerContent;

		string attValue, attName;

		double time         = getCurrentTimeMillis();
		int actionType      = action->getType();
		GingaWindowID winId = 0;

		clog << "FormatterScheduler::";
		clog << "runActionOverApplicationObject ACTION = '";
		clog << actionType << "' event = '" << event->getId();
		clog << "'" << endl;

		switch (actionType) {
			case SimpleAction::ACT_START:
				if (!player->hasPrepared()) {
					if (ruleAdapter->adaptDescriptor(
							executionObject)) {

						descriptor = executionObject->getDescriptor();
						if (descriptor != NULL) {
							descriptor->setFormatterLayout(
									getFormatterLayout(
											descriptor, executionObject));
						}
					}

					clog << "FormatterScheduler::";
					clog << "runActionOverApplicationObject ";
					clog << "START '" << event->getId();
					clog << "' call player->prepare1";
					clog << endl;

					player->prepare(executionObject, event);

					playerContent = player->getPlayer();
					if (playerContent != NULL) {
						multiDevPres->listenPlayer(playerContent);
					}

					if (executionObject->getDescriptor() != NULL) {
						// look for a reference time base player
						attValue = executionObject->getDescriptor()->
								getParameterValue("x-timeBaseObject");

						if (attValue != "") {
							setTimeBaseObject(
									executionObject,
									player,
									attValue);
						}
					}

					if (playerContent != NULL) {
						winId = ((FormatterMultiDevice*)multiDevPres)->
								prepareFormatterRegion(
										executionObject,
										playerContent->getSurface());

						player->setOutputWindow(winId);
						if (player->getObjectDevice() == 0) {
							player->flip();
						}
					}

				} else {
					clog << "FormatterScheduler::";
					clog << "runActionOverApplicationObject ";
					clog << "START '" << event->getId();
					clog << "' call player->prepare2";
					clog << endl;

					player->prepare(executionObject, event);
				}

				event->addEventListener(this);
				Thread::mutexLock(&lMutex);
				listening.insert(event);
				Thread::mutexUnlock(&lMutex);
				if (((ApplicationPlayerAdapter*)player)->setAndLockCurrentEvent(
						event)) {

					if (!player->start()) {
						clog << "FormatterScheduler::";
						clog << "runActionOverApplicationObject can't start '";
						clog << executionObject->getId() << "'";
						clog << endl;

						set<FormatterEvent*>::iterator it;

						// checking if player failed to start
						if (event->getCurrentState() == EventUtil::ST_SLEEPING) {
							event->removeEventListener(this);
							Thread::mutexLock(&lMutex);
							it = listening.find(event);
							if (it != listening.end()) {
								listening.erase(it);
							}
							Thread::mutexUnlock(&lMutex);
						}
					}

					((ApplicationPlayerAdapter*)player)->unlockCurrentEvent(
							event);
				}

				time = getCurrentTimeMillis() - time;

				clog << "FormatterScheduler::runActionOverApp takes '";
				clog << time << "' ms to start '";
				clog << executionObject->getId() << "'";
				clog << endl;

				break;

			case SimpleAction::ACT_PAUSE:
				if (((ApplicationPlayerAdapter*)player)->setAndLockCurrentEvent(
						event)) {

					player->pause();
					((ApplicationPlayerAdapter*)player)->unlockCurrentEvent(
							event);
				}

				break;

			case SimpleAction::ACT_RESUME:
				if (((ApplicationPlayerAdapter*)player)->setAndLockCurrentEvent(
						event)) {

					player->resume();
					((ApplicationPlayerAdapter*)player)->unlockCurrentEvent(
							event);
				}

				break;

			case SimpleAction::ACT_ABORT:
				if (((ApplicationPlayerAdapter*)player)->setAndLockCurrentEvent(
						event)) {

					player->abort();
					((ApplicationPlayerAdapter*)player)->unlockCurrentEvent(
							event);
				}

				break;

			case SimpleAction::ACT_STOP:
				if (((ApplicationPlayerAdapter*)player)->setAndLockCurrentEvent(
						event)) {

					player->stop();
					((ApplicationPlayerAdapter*)player)->unlockCurrentEvent(
							event);
				}

				break;
		}
	}

	void FormatterScheduler::runActionOverComposition(
		    CompositeExecutionObject* compositeObject,
		    LinkSimpleAction* action) {

		CompositeNode* compositeNode;
		Port* port;
		NodeNesting* compositionPerspective;
		NodeNesting* perspective;

		map<string, ExecutionObject*>* objects;
		map<string, ExecutionObject*>::iterator j;
		ExecutionObject* childObject;

		FormatterPlayerAdapter* pAdapter;
		AttributionEvent* attrEvent;
		FormatterEvent* event;
		string propName;
		string propValue;

		vector<Node*>* nestedSeq;

		FormatterEvent* childEvent;
		int i, size = 0;
		vector<FormatterEvent*>* events;
		short eventType = -1;

		clog << "FormatterScheduler::runActionOverComposition ";
		clog << "action '" << action->getType() << "' over COMPOSITION '";
		clog << compositeObject->getId() << "'" << endl;

		if (action->getType() == SimpleAction::ACT_START ||
				action->getType() == SimpleAction::ACT_SET) {

			event = action->getEvent();
			if (event != NULL) {
				eventType = event->getEventType();
				if (eventType < 0) {
					if (event->instanceOf("PresentationEvent")) {
						eventType = EventUtil::EVT_PRESENTATION;

					} else if (event->instanceOf("AttributionEvent")) {
						eventType = EventUtil::EVT_ATTRIBUTION;

					} else if (event->instanceOf("SwitchEvent")) {
						eventType = EventUtil::EVT_PRESENTATION;

					} else {
						clog << "FormatterScheduler::runActionOverComposition ";
						clog << "Warning! action '" << action->getType() << "'";
						clog << " over COMPOSITION '";
						clog << compositeObject->getId();
						clog << "' has an unknown eventType" << endl;
					}
				}
			}

			if (eventType == EventUtil::EVT_ATTRIBUTION) {
				event = action->getEvent();
				if (!event->instanceOf("AttributionEvent")) {
					clog << "FormatterScheduler::runActionOverComposition SET ";
					clog << "Warning! event ins't of attribution type";
					clog << endl;
					return;
				}

				attrEvent = (AttributionEvent*)event;
				propName  = attrEvent->getAnchor()->getPropertyName();
				propValue = ((LinkAssignmentAction*)action)->getValue();
				event     = compositeObject->getEventFromAnchorId(propName);

				/*clog << "FormatterScheduler::runActionOverComposition ";
				clog << "Run SET action over COMPOSITION '";
				clog << compositeObject->getId() << "' event '";
				clog << propName << "' value '";
				clog << propValue << "'";
				clog << endl;*/

				if (event != NULL) {
					event->start();
					compositeObject->setPropertyValue(
							(AttributionEvent*)event, propValue);

					((AttributionEvent*)event)->setValue(propValue);
					event->stop();

				} else {
					compositeObject->setPropertyValue(attrEvent, propValue);
					attrEvent->stop();
				}

				objects = compositeObject->getExecutionObjects();
				if (objects == NULL) {
					/*
					clog << "FormatterScheduler::runActionOverComposition SET ";
					clog << "no childs found!";
					clog << endl;*/
					return;
				}

				j = objects->begin();
				while (j != objects->end()) {
					childObject = j->second;
					if (childObject->instanceOf("CompositeExecutionObject")) {
						clog << "FormatterScheduler::runActionOverComposition ";
						clog << "'" << compositeObject->getId() << "' has '";
						clog << childObject->getId() << "' as its child ";
						clog << "using recursive call";
						clog << endl;

						runActionOverComposition(
								(CompositeExecutionObject*)childObject, action);

					} else {
						childEvent = childObject->getEventFromAnchorId(propName);
						if (childEvent != NULL) { //attribution with transition
							runAction(childEvent, NULL, action);

						} else { //force attribution
							pAdapter = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(
									childObject);

							if (pAdapter != NULL) {
								pAdapter->setPropertyValue(attrEvent, propValue);
							}
						}
					}
					++j;
				}

			} else if (eventType == EventUtil::EVT_PRESENTATION) {
				compositeObject->suspendLinkEvaluation(false);

				compositeNode = (CompositeNode*)(
						compositeObject->getDataObject()->getDataEntity());

				size = compositeNode->getNumPorts();

				if (compositeNode->getParentComposition() == NULL) {
					compositionPerspective = new NodeNesting(
							compositeNode->getPerspective());

				} else {
					compositionPerspective =
							compositeObject->getNodePerspective();
				}

				events = new vector<FormatterEvent*>;
				for (i = 0; i < size; i++) {
				    port = compositeNode->getPort(i);
					perspective = compositionPerspective->copy();
					nestedSeq = port->getMapNodeNesting();
					perspective->append(nestedSeq);
					try {
						childObject = ((FormatterConverter*)compiler)->
								getExecutionObjectFromPerspective(
										perspective,
										NULL,
										((FormatterConverter*)compiler)->
											getDepthLevel());

						if (childObject != NULL &&
								port->getEndInterfacePoint() != NULL &&
								port->getEndInterfacePoint()->instanceOf(
										"ContentAnchor")) {

							childEvent = (PresentationEvent*)(
									((FormatterConverter*)compiler)->getEvent(
											childObject,
											port->getEndInterfacePoint(),
											EventUtil::EVT_PRESENTATION, ""));

							if (childEvent != NULL) {
								events->push_back(childEvent);

								clog << "FormatterScheduler::";
								clog << "runActionOverComposition '";
								clog << compositeObject->getId() << "'";
								clog << " dataCompositeObject = '";
								clog << compositeNode->getId() << "' ";
								clog << " dataCompositeObjectParent = '";
								clog << compositeNode->getParentComposition();
								clog << "' ";
								clog << "perspective = '";
								clog << perspective->getId();
								clog << "' adding event '";
								clog << childEvent->getId();
								clog << "' (child object = '";
								clog << childObject->getId();
								clog << "', port = '";
								clog << port->getId();
								clog << "')";
								clog << endl;
							}
						}

					} catch (ObjectCreationForbiddenException* exc) {
						// keep on starting child objects
					}

					delete nestedSeq;
					delete perspective;
				}

				delete compositionPerspective;

				size = events->size();

				clog << "FormatterScheduler::runActionOverComposition ";
				clog << "action '" << action->getType() << "' over ";
				clog << "COMPOSITION '" << compositeObject->getId();
				clog << "': '" << size << "' EVENTS FOUND" << endl;

				for (i = 0; i < size; i++) {
					runAction((*events)[i], NULL, action);
				}
				delete events;
				events = NULL;
			}

		} else {
			event = action->getEvent();
			if (event != NULL) {
				eventType = event->getEventType();
			}

			if ((eventType == EventUtil::EVT_PRESENTATION) && (
					action->getType() == SimpleAction::ACT_STOP ||
					action->getType() == SimpleAction::ACT_ABORT)) {

				if (compositeObject->getWholeContentPresentationEvent() ==
						event) {

					compositeObject->suspendLinkEvaluation(true);
				}
			}

			events = new vector<FormatterEvent*>;

			compositeNode = (CompositeNode*)(
					compositeObject->getDataObject()->getDataEntity());

			objects = compositeObject->getExecutionObjects();
			if (objects != NULL) {
				j = objects->begin();
				while (j != objects->end()) {
					childObject = j->second;

					clog << "FormatterScheduler::runActionOverComposition";
					clog << " getting main event of '";
					clog << childObject->getId() << "'";
					clog << endl;

					childEvent = childObject->getMainEvent();
					if (childEvent == NULL) {
						childEvent = childObject->
							    getWholeContentPresentationEvent();
					}

					if (childEvent != NULL) {
						events->push_back(childEvent);
					}
					++j;
				}
				delete objects;
				objects = NULL;
			}

			if (compositeNode->getParentComposition() == NULL) {
				compositionPerspective = new NodeNesting(
						compositeNode->getPerspective());

				compositeObject = (CompositeExecutionObject*)(
						(FormatterConverter*)compiler)->
						getExecutionObjectFromPerspective(
								compositionPerspective,
								NULL,
								((FormatterConverter*)compiler)->
									getDepthLevel());

				delete compositionPerspective;

				objects = compositeObject->getExecutionObjects();
				if (objects != NULL) {
					j = objects->begin();
					while (j != objects->end()) {
						childObject = j->second;

						clog << "FormatterScheduler::runActionOverComposition";
						clog << " getting main event of '";
						clog << childObject->getId() << "'";
						clog << endl;

						childEvent = childObject->getMainEvent();
						if (childEvent == NULL) {
							childEvent = childObject->
								    getWholeContentPresentationEvent();
						}

						if (childEvent != NULL) {
							events->push_back(childEvent);
						}
						++j;
					}
					delete objects;
					objects = NULL;
				}
			}

			clog << "FormatterScheduler::runActionOverComposition (else) ";
			clog << "action '" << action->getType() << "' over ";
			clog << "COMPOSITION '" << compositeObject->getId();
			clog << "' (objects = '" << objects;
			clog << "'): '" << size << "' EVENTS FOUND" << endl;

			size = events->size();
			for (i = 0; i < size; i++) {
				runAction((*events)[i], NULL, action);
			}

			delete events;
			events = NULL;
		}
	}

	void FormatterScheduler::runActionOverSwitch(
		    ExecutionObjectSwitch* switchObject,
		    SwitchEvent* event, LinkSimpleAction* action) {

		ExecutionObject* selectedObject;
		FormatterEvent* selectedEvent;

		selectedObject = switchObject->getSelectedObject();
		if (selectedObject == NULL) {
			selectedObject = ((FormatterConverter*)compiler)->
				    processExecutionObjectSwitch(switchObject);

			if (selectedObject == NULL) {
				clog << "FormatterScheduler::runActionOverSwitch Warning!";
				clog << " Can't solve switch" << endl;
				return;
			}
		}

		selectedEvent = event->getMappedEvent();
		if (selectedEvent != NULL) {
			runAction(selectedEvent, NULL, action);

		} else {
			runSwitchEvent(switchObject, event, selectedObject, action);
		}

		if (action->getType() == SimpleAction::ACT_STOP ||
			    action->getType() == SimpleAction::ACT_ABORT) {

			switchObject->select(NULL);
		}
	}

	void FormatterScheduler::runSwitchEvent(
		    ExecutionObjectSwitch* switchObject,
		    SwitchEvent* switchEvent,
		    ExecutionObject* selectedObject,
		    LinkSimpleAction* action) {

		FormatterEvent* selectedEvent;
		SwitchPort* switchPort;
		vector<Port*>* mappings;
		vector<Port*>::iterator i;
		Port* mapping;
		NodeNesting* nodePerspective;
		vector<Node*>* nestedSeq;
		ExecutionObject* endPointObject;

		selectedEvent = NULL;
		switchPort = (SwitchPort*)(switchEvent->getInterfacePoint());
		mappings = switchPort->getPorts();
		if (mappings != NULL) {
			i = mappings->begin();
			while (i != mappings->end()) {
				mapping = *i;
				if (mapping->getNode() == selectedObject->getDataObject()) {
					nodePerspective = switchObject->getNodePerspective();
					nestedSeq = mapping->getMapNodeNesting();
					nodePerspective->append(nestedSeq);
					delete nestedSeq;
					try {
						endPointObject = ((FormatterConverter*)compiler)->
							    getExecutionObjectFromPerspective(
							    nodePerspective,
							    NULL,
							    ((FormatterConverter*)compiler)->
							    	    getDepthLevel());

						if (endPointObject != NULL) {
							selectedEvent = ((FormatterConverter*)compiler)->
								    getEvent(
								    endPointObject,
								    mapping->getEndInterfacePoint(),
								    switchEvent->getEventType(),
								    switchEvent->getKey());
						}

					} catch (ObjectCreationForbiddenException* exc) {
						// continue
					}

					break;
				}
				++i;
			}
		}

		if (selectedEvent != NULL) {
			switchEvent->setMappedEvent(selectedEvent);

			runAction(selectedEvent, NULL, action);
		}
	}

	string FormatterScheduler::solveImplicitRefAssessment(
			string propValue, AttributionEvent* event) {

		FormatterEvent* refEvent;
		ExecutionObject* refObject;
		string auxVal = "", roleId = "";

		if (propValue != "") {
			roleId = propValue.substr(1, propValue.length());
		}

		refEvent = ((AttributionEvent*)event)->getImplicitRefAssessmentEvent(
				roleId);

		if (refEvent != NULL) {
			auxVal    = ((AttributionEvent*)refEvent)->getCurrentValue();
			refObject = ((ExecutionObject*)(refEvent->getExecutionObject()));

			clog << "FormatterScheduler::solveImplicitRefAssessment refEvent";
			clog << " for '" << refObject->getId() << "' is '";
			clog << refEvent->getId() << "', got '" << propValue << "'";
			clog << endl;

			return auxVal;

		} else {
			clog << "FormatterScheduler::solveImplicitRefAssessment warning!";
			clog << " refEvent not found for '" << event->getId() << "', ";
			clog << " LinkAssignmentAction value is '" << propValue;
			clog << "'" << endl;
			clog << endl;
		}

		return "";
	}

	void FormatterScheduler::startEvent(FormatterEvent* event) {
		LinkSimpleAction* fakeAction;

		fakeAction = new LinkSimpleAction(event, SimpleAction::ACT_START);
		runAction(NULL, fakeAction);
		delete fakeAction;
	}

	void FormatterScheduler::stopEvent(FormatterEvent* event) {
		LinkSimpleAction* fakeAction;

		fakeAction = new LinkSimpleAction(event, SimpleAction::ACT_STOP);
		runAction(NULL, fakeAction);
		delete fakeAction;
	}

	void FormatterScheduler::pauseEvent(FormatterEvent* event) {
		LinkSimpleAction* fakeAction;

		fakeAction = new LinkSimpleAction(event, SimpleAction::ACT_PAUSE);
		runAction(NULL, fakeAction);
		delete fakeAction;
	}

	void FormatterScheduler::resumeEvent(FormatterEvent* event) {
		LinkSimpleAction* fakeAction;

		fakeAction = new LinkSimpleAction(event, SimpleAction::ACT_RESUME);
		runAction(NULL, fakeAction);
		delete fakeAction;
	}

	void FormatterScheduler::initializeDefaultSettings() {
		string value;
		double alfa;

		value = presContext->getPropertyValue(
				DEFAULT_FOCUS_BORDER_TRANSPARENCY);

		if (value != "") {
			alfa = util::stof(value);

		} else {
			alfa = 1;
		}
		int alpha;
		alpha = (int)(255 * alfa);

		value = presContext->getPropertyValue(DEFAULT_FOCUS_BORDER_COLOR);
		if (value != "") {
			focusManager->setDefaultFocusBorderColor(new Color(value, alpha));
		}

		value = presContext->getPropertyValue(DEFAULT_FOCUS_BORDER_WIDTH);
		if (value != "") {
			focusManager->setDefaultFocusBorderWidth((int)util::stof(value));
		}

		value = presContext->getPropertyValue(DEFAULT_SEL_BORDER_COLOR);
		if (value != "") {
			focusManager->setDefaultSelBorderColor(new Color(value, alpha));
		}
	}

	void FormatterScheduler::initializeDocumentSettings(Node* node) {
		string value;
		vector<Anchor*>* anchors;
		vector<Anchor*>::iterator i;
		vector<Node*>* nodes;
		vector<Node*>::iterator j;
		Anchor* anchor;
		PropertyAnchor* attributeAnchor;

		if (node->instanceOf("ContentNode")) {
			if (((ContentNode*)node)->isSettingNode()) {
				anchors = ((ContentNode*)node)->getAnchors();
				if (anchors != NULL) {
					i = anchors->begin();
					while (i != anchors->end()) {
						anchor = (*i);
						if (anchor->instanceOf("PropertyAnchor")) {
							attributeAnchor = (PropertyAnchor*)anchor;
							value = attributeAnchor->getPropertyValue();
							if (value != "") {
								presContext->setPropertyValue(
										attributeAnchor->getPropertyName(),
									    value);
							}
						}
						++i;
					}
				}
			}

		} else if (node->instanceOf("CompositeNode")) {
			nodes = ((CompositeNode*)node)->getNodes();
			if (nodes != NULL) {
				j = nodes->begin();
				while (j != nodes->end()) {
					initializeDocumentSettings(*j);
					++j;
				}
			}

		} else if (node->instanceOf("ReferNode")) {
			initializeDocumentSettings(
				    (NodeEntity*)((ReferNode*)node)->getDataEntity());
		}
	}

	void FormatterScheduler::startDocument(
		    FormatterEvent* documentEvent,
		    vector<FormatterEvent*>* entryEvents) {

		ExecutionObject* object;
		vector<FormatterEvent*>::iterator it;
		int docEvents = 0;
		int i, size;
		FormatterEvent* event;

		if (documentEvent == NULL || entryEvents == NULL) {
			clog << "FormatterScheduler::startDocument Warning! ";
			clog << "documentEvent == NULL || entryEvents == NULL" << endl;
			return;
		}

		if (entryEvents->empty()) {
			clog << "FormatterScheduler::startDocument Warning! ";
			clog << "entryEvents is empty" << endl;
			return;
		}

		if (isDocumentRunning(documentEvent)) {
			size = entryEvents->size();
			for (i = 0; i < size; i++) {
				event = (*entryEvents)[i];

				if (event->getCurrentState() == EventUtil::ST_SLEEPING) {
					startEvent(event);
				}
			}
			return;
		}

		Thread::mutexLock(&mutexD);
		for (it = documentEvents.begin(); it != documentEvents.end(); ++it) {
			if (*it == documentEvent) {
				clog << "FormatterScheduler::startDocument Warning! ";
				clog << "Can't start document through event '";
				clog << documentEvent->getId() << "'" << endl;

				Thread::mutexUnlock(&mutexD);
				return;
			}
		}

		clog << "FormatterScheduler::startDocument Through event '";
		clog << documentEvent->getId() << "'" << endl;
		documentEvent->addEventListener(this);
		documentEvents.push_back(documentEvent);

		Thread::mutexLock(&lMutex);
		listening.insert(documentEvent);
		Thread::mutexUnlock(&lMutex);

		documentStatus[documentEvent] = true;
		Thread::mutexUnlock(&mutexD);

		object = (ExecutionObject*)(documentEvent->getExecutionObject());
		initializeDocumentSettings(object->getDataObject());
		initializeDefaultSettings();

		size = entryEvents->size();
		for (i = 0; i < size; i++) {
			event = (*entryEvents)[i];

			startEvent(event);
			if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
				docEvents++;
			}
		}

		/*if (docEvents == 0) {
			clog << "FormatterScheduler::startDocument 0 events running";
			clog << " stopping document" << endl;
			stopDocument(documentEvent);
		}*/

		clog << "FormatterScheduler::startDocument Through event '";
		clog << documentEvent->getId() << "' started '" << docEvents << "'";
		clog << " events" << endl;
	}

	void FormatterScheduler::removeDocument(FormatterEvent* documentEvent) {
		ExecutionObject* obj;
		vector<FormatterEvent*>::iterator i;
		map<FormatterEvent*, bool>::iterator j;

		//TODO: do a better way to remove documents (see lockComposite)
		obj = (ExecutionObject*)(documentEvent->getExecutionObject());

		clog << "FormatterScheduler::removeDocument through '";
		clog << obj->getId() << "'" << endl;

		if (compiler != NULL && obj != NULL) {
			((FormatterConverter*)compiler)->removeExecutionObject(obj);
		}

		Thread::mutexLock(&mutexD);
		for (i = documentEvents.begin(); i != documentEvents.end(); ++i) {
			if (*i == documentEvent) {
				documentEvents.erase(i);
				break;
			}
		}

		j = documentStatus.find(documentEvent);
		if (j != documentStatus.end()) {
			documentStatus.erase(j);
		}
		Thread::mutexUnlock(&mutexD);
	}

	void FormatterScheduler::stopDocument(FormatterEvent* documentEvent) {
		ExecutionObject* executionObject;
		vector<IFormatterSchedulerListener*>::iterator i;
		IFormatterSchedulerListener* listener;

		clog << "FormatterScheduler::stopDocument through '";
		clog << documentEvent->getId() << "'" << endl;

		Thread::mutexLock(&mutexD);
		if (documentStatus.count(documentEvent) != 0) {
			documentEvent->removeEventListener(this);
			documentStatus[documentEvent] = false;

			documentEvents.clear();
			Thread::mutexUnlock(&mutexD);

			executionObject = (ExecutionObject*)(documentEvent->
				    getExecutionObject());

			if (executionObject->instanceOf("CompositeExecutionObject")) {
				((CompositeExecutionObject*)executionObject)->
					    setAllLinksAsUncompiled(true);
			}

			//we can't remove the document,
			//since it can be started again
			//removeDocument(documentEvent);
			stopEvent(documentEvent);

			i = schedulerListeners.begin();
			while (i != schedulerListeners.end()) {
				listener = *i;
				listener->presentationCompleted(documentEvent);

				schedulerListeners.erase(i);
				i = schedulerListeners.begin();
			}

		} else {
			documentEvents.clear();
			Thread::mutexUnlock(&mutexD);
		}
	}

	void FormatterScheduler::pauseDocument(FormatterEvent* documentEvent) {
		vector<FormatterEvent*>::iterator i;
		for (i = documentEvents.begin(); i != documentEvents.end(); ++i) {
			if (*i == documentEvent) {
				Thread::mutexLock(&mutexD);
				documentStatus[documentEvent] = false;
				Thread::mutexUnlock(&mutexD);
				pauseEvent(documentEvent);
				break;
			}
		}
	}

	void FormatterScheduler::resumeDocument(FormatterEvent* documentEvent) {
		bool contains;
		contains = false;
		vector<FormatterEvent*>::iterator i;
		for (i = documentEvents.begin(); i != documentEvents.end(); ++i) {
			if (*i == documentEvent) {
				contains = true;
				break;
			}
		}

		if (contains) {
			resumeEvent(documentEvent);
			Thread::mutexLock(&mutexD);
			documentStatus[documentEvent] = true;
			Thread::mutexUnlock(&mutexD);
		}
	}

	void FormatterScheduler::stopAllDocuments() {
		int i, size;
		vector<FormatterEvent*>* auxDocEventList;
		FormatterEvent* documentEvent;

		if (!documentEvents.empty()) {
			auxDocEventList = new vector<FormatterEvent*>(documentEvents);

			size = auxDocEventList->size();
			for (i = 0; i < size; i++) {
				documentEvent = (*auxDocEventList)[i];
				stopDocument(documentEvent);
			}

			auxDocEventList->clear();
			delete auxDocEventList;
			auxDocEventList = NULL;
		}
	}

	void FormatterScheduler::pauseAllDocuments() {
		int i, size;
		FormatterEvent* documentEvent;

		if (!documentEvents.empty()) {
			size = documentEvents.size();
			for (i = 0; i < size; i++) {
				documentEvent = documentEvents[i];
				pauseDocument(documentEvent);
			}
		}
	}

	void FormatterScheduler::resumeAllDocuments() {
		int i, size;
		FormatterEvent* documentEvent;

		if (!documentEvents.empty()) {
			size = documentEvents.size();
			for (i = 0; i < size; i++) {
				documentEvent = documentEvents[i];
				resumeDocument(documentEvent);
			}
		}
	}

	void FormatterScheduler::eventStateChanged(
		    void* someEvent, short transition, short previousState) {

		ExecutionObject* object;
		FormatterPlayerAdapter* player;
		vector<IFormatterSchedulerListener*>::iterator i;
		vector<FormatterEvent*>::iterator it;
		IFormatterSchedulerListener* listener;
		FormatterEvent* event;
		bool contains;
		bool hasOther;

		event = (FormatterEvent*)someEvent;

		clog << "FormatterScheduler::eventStateChanged '";
		clog << event->getId() << "' transition '" << transition;
		clog << "'" << endl;

		hasOther = false;
		contains = false;

		it = documentEvents.begin();
		while (it != documentEvents.end()) {
			if (*it == event) {
				contains = true;

			} else if ((*it)->getCurrentState() != EventUtil::ST_SLEEPING) {
				hasOther = true;
			}
			++it;
		}

		if (contains) {
			switch (transition) {
				case EventUtil::TR_STOPS:
				case EventUtil::TR_ABORTS:
					if (!hasOther) {
						documentEvents.clear();

						i = schedulerListeners.begin();
						while (i != schedulerListeners.end()) {
							listener = *i;
							listener->presentationCompleted(event);

							++i;
						}

						//we can't remove the document,
						//since it can be started again
						//removeDocument(event);
					}
					break;
			}

		} else {
			switch (transition) {
				case EventUtil::TR_STARTS:
					//TODO: if (isDocumentRunning(event)) {
					object = (ExecutionObject*)(event->getExecutionObject());

					player = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(object);
					if (player != NULL) {
						((FormatterMultiDevice*)multiDevPres)->showObject(
								object);

						focusManager->showObject(object);
					}
					//}
					break;

				case EventUtil::TR_STOPS:
					if (((PresentationEvent*)event)->getRepetitions() == 0) {
						bool hideObj = true;

						event->removeEventListener(this);
						object = (ExecutionObject*)(event->
								getExecutionObject());

						if (object->instanceOf("ApplicationExecutionObject")) {
							if (!((ApplicationExecutionObject*)object)->
									isSleeping()) {

								hideObj = false;
							}
						}

						if (hideObj) {
							clog << "FormatterScheduler::eventStateChanged '";
							clog << event->getId();
							clog << "' STOPS: hideObject '" << object->getId();
							clog << endl;

							focusManager->hideObject(object);
							((FormatterMultiDevice*)multiDevPres)->hideObject(
									object);

							player = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(object);
							if (player != NULL && player->getPlayer() != NULL) {
								player->flip();

								multiDevPres->stopListenPlayer(
										player->getPlayer());
							}
						}

					} else {
						/*clog << "FormatterScheduler::";
						clog << "eventStateChanged(" << object->getId();
						clog << ") REPEATING!" << endl;*/
					}
					break;

				case EventUtil::TR_ABORTS:
					bool hideObj = true;

					event->removeEventListener(this);
					object = (ExecutionObject*)(event->getExecutionObject());

					if (object->instanceOf("ApplicationExecutionObject")) {
						if (!((ApplicationExecutionObject*)object)->
								isSleeping()) {

							hideObj = false;
						}
					}

					if (hideObj) {
						clog << "FormatterScheduler::eventStateChanged '";
						clog << event->getId();
						clog << "' ABORTS: hideObject '" << object->getId();
						clog << endl;

						focusManager->hideObject(object);
						((FormatterMultiDevice*)multiDevPres)->hideObject(
								object);

						player = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(object);
						if (player != NULL && player->getPlayer() != NULL &&
								player->getObjectDevice() == 0) {

							player->flip();

							multiDevPres->stopListenPlayer(player->getPlayer());
						}
					}

					break;
			}
		}
	}

	short FormatterScheduler::getPriorityType() {
		return IEventListener::PT_CORE;
	}

	void FormatterScheduler::addSchedulerListener(
		    IFormatterSchedulerListener* listener) {

		bool contains;
		contains = false;
		vector<IFormatterSchedulerListener*>::iterator i;
		for (i = schedulerListeners.begin();
				i != schedulerListeners.end(); ++i) {

			if (*i == listener) {
				contains = true;
				break;
			}
		}

		if (!contains) {
			schedulerListeners.push_back(listener);
		}
	}

	void FormatterScheduler::removeSchedulerListener(
		    IFormatterSchedulerListener* listener) {

		vector<IFormatterSchedulerListener*>::iterator i;
		for (i = schedulerListeners.begin();
			    i != schedulerListeners.end(); ++i) {

			if (*i == listener) {
				schedulerListeners.erase(i);
				return;
			}
		}
	}

	void FormatterScheduler::receiveGlobalAttribution(
			string propertyName, string value) {

		set<ExecutionObject*>* objs;
		set<ExecutionObject*>::iterator i;

		set<string>* names;
		set<string>::iterator j;

		ExecutionObject* object;
		FormatterEvent* event;
		LinkSimpleAction* fakeAction;

		clog << "FormatterScheduler::receiveGlobalAttribution ";
		clog << "prop = '" << propertyName << "', value = '" << value;
		clog << "'" << endl;

		objs = ((FormatterConverter*)compiler)->getSettingNodeObjects();
		if (objs == NULL || objs->empty()) {
			clog << "FormatterScheduler::receiveGlobalAttribution ";
			clog << "there is 0 setting objects" << endl;
			return;
		}

		//call runAction
		i = objs->begin();
		while (i != objs->end()) {
			object = *i;
			event  = object->getEventFromAnchorId(propertyName);
			if (event != NULL && event->instanceOf("AttributionEvent")) {
				fakeAction = new LinkAssignmentAction(
						event, SimpleAction::ACT_START, value);

				runAction(NULL, fakeAction);
				delete fakeAction;
				return;
			}

			clog << "FormatterScheduler::receiveGlobalAttribution ";
			clog << "can't find anchor with name = '" << propertyName;
			clog << "' inside object '" << object->getId() << "'";
			clog << endl;

			++i;
		}

		names = presContext->getPropertyNames();
		j = names->find(propertyName);
		if (j != names->end()) {
			presContext->setPropertyValue(propertyName, value);
		}

		delete names;
	}
}
}
}
}
}
