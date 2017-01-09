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

#include "gingancl/model/CompositeExecutionObject.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
	CompositeExecutionObject::CompositeExecutionObject(
		    string id,
		    Node* dataObject,
		    bool handling,
		    ILinkActionListener* seListener) : ExecutionObject(
		    		id, dataObject, handling, seListener) {

		initializeCompositeExecutionObject(id, dataObject, NULL);
	}

	CompositeExecutionObject::CompositeExecutionObject(
		    string id,
		    Node* dataObject,
		    CascadingDescriptor* descriptor,
		    bool handling,
		    ILinkActionListener* seListener) : ExecutionObject(
		    		id, dataObject, descriptor, handling, seListener) {

		initializeCompositeExecutionObject(id, dataObject, descriptor);
	}

	CompositeExecutionObject::~CompositeExecutionObject() {
		ExecutionObject* object;
		FormatterLink* link;
		set<FormatterLink*>::iterator i;
		map<string, ExecutionObject*>::iterator j;

		removeInstance(this);
		deleting = true;

		if (running) {
			Thread::mSleep(mSleepTime);
		}

		destroyEvents();

		pthread_mutex_lock(&stlMutex);
		runningEvents.clear();
		pausedEvents.clear();
		pendingLinks.clear();

		i = links.begin();
		while (i != links.end()) {
			link = *i;
			if (link != NULL) {
				delete link;
				link = NULL;
			}
			++i;
		}
		links.clear();
		uncompiledLinks.clear();

		pthread_mutex_unlock(&stlMutex);
		Thread::mutexDestroy(&stlMutex);

		pthread_mutex_lock(&compositeMutex);
		j = execObjList.begin();
		while (j != execObjList.end()) {
			object = j->second;
			if (object != this && hasInstance(object, false)) {
				object->removeParentObject(dataObject, (void*)this);
			}
			++j;
		}
		execObjList.clear();

		pthread_mutex_unlock(&compositeMutex);
		Thread::mutexDestroy(&compositeMutex);
	}

	void CompositeExecutionObject::initializeCompositeExecutionObject(
		    string id,
		    Node* dataObject,
		    CascadingDescriptor* descriptor) {

		ContextNode* compositeNode;
		set<Link*>* compositionLinks;
		set<Link*>::iterator i;
		Entity* entity;

		typeSet.insert("CompositeExecutionObject");
		Thread::mutexInit(&compositeMutex, true);
		Thread::mutexInit(&stlMutex, false);

		execObjList.clear();
		links.clear();
		uncompiledLinks.clear();
		runningEvents.clear();
		pausedEvents.clear();
		pendingLinks.clear();

		entity = dataObject->getDataEntity();

		if (entity != NULL && entity->instanceOf("ContextNode")) {
			compositeNode = (ContextNode*)entity;
			compositionLinks = compositeNode->getLinks();
			if (compositionLinks != NULL) {
				i = compositionLinks->begin();
				while (i != compositionLinks->end()) {
					uncompiledLinks.insert(*i);
					++i;
				}
			}
		}
	}

	CompositeExecutionObject* CompositeExecutionObject::getParentFromDataObject(
			Node* dataObject) {

		ExecutionObject* object;
		Node* parentDataObject;
		map<string, ExecutionObject*>::iterator i;

		pthread_mutex_lock(&compositeMutex);
		parentDataObject = (Node*)(dataObject->getParentComposition());

		if (parentDataObject != NULL) {
			i = execObjList.begin();
			while (i != execObjList.end()) {
				object = i->second;
				if (object->getDataObject() == parentDataObject) {
					pthread_mutex_unlock(&compositeMutex);
					return (CompositeExecutionObject*)object;
				}
				++i;
			}
		}
		pthread_mutex_unlock(&compositeMutex);

		return NULL;
	}

	void CompositeExecutionObject::suspendLinkEvaluation(bool suspend) {
		set<FormatterLink*>::iterator i;

		lockSTL();
		i = links.begin();
		while (i != links.end()) {
			(*i)->suspendLinkEvaluation(suspend);
			++i;
		}
		unlockSTL();
	}

	bool CompositeExecutionObject::addExecutionObject(ExecutionObject* obj) {
		string objId;

		if (obj == NULL) {
			return false;
		}

		lockComposite();
		objId = obj->getId();
		if (execObjList.count(objId) != 0) {
			clog << "CompositeExecutionObject::addExecutionObject Warning! ";
			clog << "trying to add same obj twice: '" << objId << "'" << endl;
			unlockComposite();
			return false;
		}

		execObjList[objId] = obj;
		unlockComposite();

		obj->addParentObject((void*)this, getDataObject());
		return true;
	}

	bool CompositeExecutionObject::containsExecutionObject(string execObjId) {
		if (getExecutionObject(execObjId) != NULL) {
			return true;

		} else {
			return false;
		}
	}

	ExecutionObject* CompositeExecutionObject::getExecutionObject(string id) {
		map<string, ExecutionObject*>::iterator i;
		ExecutionObject* execObj;

		lockComposite();
		if (execObjList.empty()) {
			unlockComposite();
			return NULL;
		}

		i = execObjList.find(id);
		if (i != execObjList.end()) {
			execObj = i->second;
			unlockComposite();
			return execObj;
		}

		unlockComposite();
		return NULL;
	}

	map<string, ExecutionObject*>*
			CompositeExecutionObject::getExecutionObjects() {

		map<string, ExecutionObject*>* objs;

		lockComposite();
		if (execObjList.empty()) {
			unlockComposite();
			return NULL;
		}
		objs = new map<string, ExecutionObject*>(execObjList);
		unlockComposite();

		return objs;
	}

	int CompositeExecutionObject::getNumExecutionObjects() {
		int size = 0;

		lockComposite();
		size = execObjList.size();
		unlockComposite();

		return size;
	}

	map<string, ExecutionObject*>*
		    CompositeExecutionObject::recursivellyGetExecutionObjects() {

		map<string, ExecutionObject*>* objects;
		ExecutionObject* childObject;
		map<string, ExecutionObject*>* grandChildrenObjects;
		map<string, ExecutionObject*>::iterator i, j;

		objects = new map<string, ExecutionObject*>;

		lockComposite();
		i = execObjList.begin();
		while (i != execObjList.end()) {
			childObject = i->second;
			(*objects)[i->first] = childObject;
			if (childObject->instanceOf("CompositeExecutionObject")) {
				grandChildrenObjects = ((CompositeExecutionObject*)
						childObject)->recursivellyGetExecutionObjects();

				j = grandChildrenObjects->begin();
				while (j != grandChildrenObjects->end()) {
					(*objects)[j->first] = j->second;
					++j;
				}
				delete grandChildrenObjects;
				grandChildrenObjects = NULL;
			}
			++i;
		}

		unlockComposite();
		return objects;
	}

	bool CompositeExecutionObject::removeExecutionObject(ExecutionObject* obj) {
		map<string, ExecutionObject*>::iterator i;

		lockComposite();
		if (!deleting) {
			i = execObjList.find(obj->getId());
			if (i != execObjList.end()) {
				execObjList.erase(i);
				unlockComposite();
				return true;
			}
		}
		unlockComposite();
		return false;
	}

	set<Link*>* CompositeExecutionObject::getUncompiledLinks() {
		clog << "CompositeExecutionObject::getUncompiledLinks '" << getId();
		clog << "' has '" << uncompiledLinks.size() << "' uncompiled links";
		clog << endl;

		lockSTL();
		set<Link*>* uLinks = new set<Link*>(uncompiledLinks);
		unlockSTL();

		return uLinks;
	}

	bool CompositeExecutionObject::containsUncompiledLink(Link* dataLink) {
		lockSTL();
		if (uncompiledLinks.count(dataLink) != 0) {
			unlockSTL();
			return true;
		}

		unlockSTL();
		return false;
	}

	void CompositeExecutionObject::removeLinkUncompiled(Link* ncmLink) {
		set<Link*>::iterator i;

		lockSTL();
		clog << "CompositeExecutionObject::removeLinkUncompiled '";
		clog << ncmLink->getId() << "'" << endl;
		i = uncompiledLinks.find(ncmLink);
		if (i != uncompiledLinks.end()) {
			uncompiledLinks.erase(i);
			unlockSTL();
			return;
		}
		unlockSTL();
	}

	void CompositeExecutionObject::setLinkCompiled(FormatterLink* link) {
		if (link == NULL) {
			clog << "CompositeExecutionObject::setLinkCompiled Warning! ";
			clog << "trying to compile a NULL link" << endl;
			return;
		}

		Link* compiledLink;
		compiledLink = link->getNcmLink();

		if (compiledLink == NULL) {
			clog << "CompositeExecutionObject::setLinkCompiled Warning! ";
			clog << "formatterLink has returned a NULL ncmLink" << endl;
			return;
		}

		lockSTL();
		links.insert(link);
		unlockSTL();
	}

	void CompositeExecutionObject::addNcmLink(Link* ncmLink) {
		lockSTL();
		if (uncompiledLinks.count(ncmLink) != 0) {
			clog << "CompositeExecutionObject::addNcmLink Warning! ";
			clog << "Trying to add same link twice" << endl;
		}
		uncompiledLinks.insert(ncmLink);
		unlockSTL();
	}

	void CompositeExecutionObject::removeNcmLink(Link* ncmLink) {
		bool contains;
  		FormatterLink* link;
  		Link* compiledNcmLink;
	  	set<Link*>::iterator i;
  		set<FormatterLink*>::iterator j;

  		contains = false;

  		clog << "CompositeExecutionObject::removeNcmLink(" << ncmLink->getId();
  		clog << "'" << endl;

	  	lockSTL();
		i = uncompiledLinks.find(ncmLink);
		if (i != uncompiledLinks.end()) {
			contains = true;
			uncompiledLinks.erase(i);
		}

	  	if (!contains) {
			for (j = links.begin(); j != links.end(); ++j) {
	  			link = *j;
	  			compiledNcmLink = link->getNcmLink();
	  			if (ncmLink == compiledNcmLink) {
	  				links.erase(j);
	  				delete link;
	  				link = NULL;
	  				unlockSTL();
	  				return;
	  			}
	  		}
	  	}
	  	unlockSTL();
	}

	void CompositeExecutionObject::setAllLinksAsUncompiled(bool isRecursive) {
		ExecutionObject* childObject;
		map<string, ExecutionObject*>::iterator i;

		FormatterLink* link;
		Link* ncmLink;
		set<FormatterLink*>::iterator j;

		clog << "CompositeExecutionObject::setAllLinksAsUncompiled for '";
		clog << getId() << "'" << endl;

		lockSTL();
		j = links.begin();
		while (j != links.end()) {
			link = *j;
			ncmLink = link->getNcmLink();
			if (uncompiledLinks.count(ncmLink) != 0) {
				clog << "CompositeExecutionObject::setAllLinksAsUncompiled ";
				clog << "Warning! Trying to add same link twice" << endl;
			}
			uncompiledLinks.insert(ncmLink);
			delete link;
			link = NULL;
			++j;
		}
		links.clear();
		unlockSTL();

		if (isRecursive) {
			lockComposite();
			i = execObjList.begin();
			while (i != execObjList.end()) {
				childObject = i->second;
				if (childObject->instanceOf("CompositeExecutionObject")) {
					assert(childObject != this);
					((CompositeExecutionObject*)childObject)->
						    setAllLinksAsUncompiled(isRecursive);
				}
				++i;
			}
			unlockComposite();
		}
	}

	void CompositeExecutionObject::setParentsAsListeners() {
		map<Node*, void*>::iterator i;

		lockSTL();
		i = parentTable.begin();
		while (i != parentTable.end()) {
			if (FormatterEvent::hasInstance(wholeContent, false)) {
				wholeContent->addEventListener(
						(CompositeExecutionObject*)i->second);
			}
			++i;
		}
		unlockSTL();
	}

	void CompositeExecutionObject::unsetParentsAsListeners() {
		map<Node*, void*>::iterator i;

		if (deleting) {
			return;
		}

		lockSTL();
		i = parentTable.begin();
		while (i != parentTable.end()) {
			if (deleting || !FormatterEvent::hasInstance(wholeContent, false)) {
				return;
			}

			// unregister parent as a composite presentation listener
			wholeContent->removeEventListener(
					(CompositeExecutionObject*)i->second);

			++i;
		}
		unlockSTL();
	}

	void CompositeExecutionObject::eventStateChanged(
		    void* changedEvent, short transition, short previousState) {

		FormatterEvent* event;
		set<FormatterEvent*>::iterator i;

		event = (FormatterEvent*)changedEvent;
		if (!(event->instanceOf("PresentationEvent")) ||
				!FormatterEvent::hasInstance(event, false)) {

			return;
		}

		switch (transition) {
			case EventUtil::TR_STARTS:
				if (runningEvents.empty() && pausedEvents.empty()) {
					setParentsAsListeners();
					wholeContent->start();
				}

				runningEvents.insert(event);
				break;

			case EventUtil::TR_ABORTS:
				lastTransition = transition;
				if (previousState == EventUtil::ST_OCCURRING) {
					i = runningEvents.find(event);
					if (i != runningEvents.end()) {
						runningEvents.erase(i);
					}

				} else if (previousState == EventUtil::ST_PAUSED) {
					i = pausedEvents.find(event);
					if (i != pausedEvents.end()) {
						pausedEvents.erase(i);
					}
				}

				if (runningEvents.empty() && pausedEvents.empty() &&
					    pendingLinks.empty()) {

					wholeContent->abort();
					unsetParentsAsListeners();
				}
				break;

			case EventUtil::TR_STOPS:
				if (((PresentationEvent*)event)->getRepetitions() == 0) {
					lockComposite();
					lastTransition = transition;
					if (previousState == EventUtil::ST_OCCURRING) {
						i = runningEvents.find(event);
						if (i != runningEvents.end()) {
							runningEvents.erase(i);
						}

					} else if (previousState == EventUtil::ST_PAUSED) {
						i = pausedEvents.find(event);
						if (i != pausedEvents.end()) {
							pausedEvents.erase(i);
						}
					}

					if (runningEvents.size() < 2) {
						listRunningObjects();
					}

					if (runningEvents.size() < 2 && pausedEvents.empty() &&
							!pendingLinks.empty()) {

						listPendingLinks();
					}

					unlockComposite();
					if (runningEvents.empty() && pausedEvents.empty() &&
						    pendingLinks.empty()) {

						checkLinkConditions();
					}
				}
				break;

			case EventUtil::TR_PAUSES:
				i = runningEvents.find(event);
				if (i != runningEvents.end()) {
					runningEvents.erase(i);
				}

				pausedEvents.insert(event);
				if (runningEvents.empty()) {
					wholeContent->pause();
				}
				break;

			case EventUtil::TR_RESUMES:
				i = pausedEvents.find(event);
				if (i != pausedEvents.end()) {
					pausedEvents.erase(i);
				}

				runningEvents.insert(event);
				if (runningEvents.size() == 1) {
					wholeContent->resume();
				}
				break;

			default:
				break;
		}
	}

	short CompositeExecutionObject::getPriorityType() {
		return IEventListener::PT_OBJECT;
	}

	void CompositeExecutionObject::linkEvaluationStarted(
		    FormatterCausalLink* link) {

		int linkNumber = 0;
		FormatterLink* evalLink;

		lockSTL();

		evalLink = link;
		if (pendingLinks.count(evalLink) != 0) {
			linkNumber = pendingLinks[evalLink];
		}
		pendingLinks[evalLink] = linkNumber + 1;

		unlockSTL();
	}

	void CompositeExecutionObject::linkEvaluationFinished(
		    FormatterCausalLink* link, bool start) {

		int linkNumber;
		FormatterLink* finishedLink;
		map<FormatterLink*, int>::iterator i;

		lockSTL();

		clog << "CompositeExecutionObject::linkEvaluationFinished(" << id;
		clog << ") '";
		clog << link->getNcmLink()->getId() << "'" << endl;

		finishedLink = link;
		i = pendingLinks.find(finishedLink);
		if (i != pendingLinks.end()) {
			linkNumber = i->second;
			if (linkNumber == 1) {
				pendingLinks.erase(i);
				if (runningEvents.empty() && pausedEvents.empty() &&
					    pendingLinks.empty()) {

					if (lastTransition == EventUtil::TR_STOPS) {
						checkLinkConditions();

					} else if (!start) {
						unlockSTL();
						wholeContent->abort();
						if (deleting) {
							return;
						}
						unsetParentsAsListeners();
						lockSTL();

					} else {
						// if nothing starts the composition may
						// stay locked as occurring
						clog << "CompositeExecutionObject::";
						clog << "linkEvaluationFinished ";
						clog << "if nothing starts the composition may ";
						clog << "stay locked as occurring";
						clog << endl;
					}
				}

			} else {
				pendingLinks[finishedLink] = linkNumber - 1;
			}
		}
		unlockSTL();
	}

	bool CompositeExecutionObject::setPropertyValue(
			AttributionEvent* event, string value) {

		if (event->getAnchor()->getPropertyName() == "visible") {
			if (value == "true") {
				visible = true;

			} else if (value == "false") {
				visible = false;
			}
			return false;
		}

		return ExecutionObject::setPropertyValue(event, value);
	}

	void CompositeExecutionObject::checkLinkConditions() {
		if (!running || (running && !Thread::wakeUp())) {
			Thread::startThread();
		}
	}

	void CompositeExecutionObject::run() {
		running = true;
		if (Thread::mSleep(mSleepTime)) {
			if (deleting || (runningEvents.empty() && pausedEvents.empty() &&
					pendingLinks.empty())) {

				clog << "CompositeExecutionObject::run ";
				clog << "I (" << id << ") am ending because of STOP of";
				clog << " the last running event (no pending links nor ";
				clog << "paused events)";
				clog << endl;

				if (FormatterEvent::hasInstance(wholeContent, false)) {
					wholeContent->stop();
					unsetParentsAsListeners();
				}
			}
		}
		running = false;
	}

	void CompositeExecutionObject::listRunningObjects() {
		map<string, ExecutionObject*>::iterator i;
		vector<FormatterEvent*>::iterator j;
		vector<FormatterEvent*>* events;
		ExecutionObject* object;
		FormatterEvent* event;

		clog << "CompositeExecutionObject::listRunningObjects for '";
		clog << id << "': ";
		i = execObjList.begin();
		while (i != execObjList.end()) {
			object = i->second;
			events = object->getEvents();
			if (events != NULL) {
				j = events->begin();
				while (j != events->end()) {
					event = *j;
					if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
						clog << "'" << i->first << "', ";
					}
					++j;
				}
				delete events;
				events = NULL;
			}
			++i;
		}
		clog << " runingEvents->size = '" << runningEvents.size() << "'";
		clog << endl;
	}

	void CompositeExecutionObject::listPendingLinks() {
		map<FormatterLink*, int>::iterator i;
		Link* ncmLink;

		clog << "CompositeExecutionObject::listPendingLinks for '";
		clog << id << "': ";

		i = pendingLinks.begin();
		while (i != pendingLinks.end()) {
			ncmLink = i->first->getNcmLink();

			if (Entity::hasInstance(ncmLink, false)) {
				clog << "'" << ncmLink->getId() << "', ";
			}
			++i;
		}

		clog << " pendingLinks.size = '" << pendingLinks.size() << "'";
		clog << endl;
	}

	void CompositeExecutionObject::lockComposite() {
		if (!deleting) {
			Thread::mutexLock(&compositeMutex);
		}
	}

	void CompositeExecutionObject::unlockComposite() {
		if (!deleting) {
			Thread::mutexUnlock(&compositeMutex);
		}
	}

	void CompositeExecutionObject::lockSTL() {
		if (!deleting) {
			Thread::mutexLock(&stlMutex);
		}
	}

	void CompositeExecutionObject::unlockSTL() {
		if (!deleting) {
			Thread::mutexUnlock(&stlMutex);
		}
	}
}
}
}
}
}
}
}
