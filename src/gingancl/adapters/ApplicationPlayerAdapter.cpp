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

#include "ApplicationPlayerAdapter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace application {
	ApplicationPlayerAdapter::ApplicationPlayerAdapter() :
			FormatterPlayerAdapter() {

		typeSet.insert("ApplicationPlayerAdapter");
		Thread::mutexInit(&eventMutex, NULL);
		Thread::mutexInit(&eventsMutex, NULL);

		currentEvent           = NULL;
		editingCommandListener = NULL;
		running                = false;
		isDeleting             = false;

		clog << "ApplicationPlayerAdapter::ApplicationPlayerAdapter(" << this;
		clog << ")" << endl;
	}

	ApplicationPlayerAdapter::~ApplicationPlayerAdapter() {
		vector<ApplicationStatus*>::iterator i;

		clog << "ApplicationPlayerAdapter::ApplicationPlayerAdapter(" << this;
		clog << ")" << endl;

		isDeleting = true;
		running    = false;
		unlockConditionSatisfied();

		lock();
		i = notes.begin();
		while (i != notes.end()) {
			delete (*i);
			++i;
		}
		notes.clear();
		unlock();

		editingCommandListener = NULL;

		lockPreparedEvents();
		preparedEvents.clear();
		unlockPreparedEvents();
		currentEvent = NULL;
		object = NULL;
		Thread::mutexDestroy(&eventMutex);
		Thread::mutexDestroy(&eventsMutex);
	}

	void ApplicationPlayerAdapter::setNclEditListener(
			IPlayerListener* listener) {

		this->editingCommandListener = listener;
	}

	void ApplicationPlayerAdapter::checkPlayerSurface(ExecutionObject* obj) {
		CascadingDescriptor* descriptor;
		FormatterRegion* fRegion;
		LayoutRegion* ncmRegion;
		GingaSurfaceID wrapper;
		int w, h;

		if (player != NULL) {
			wrapper = player->getSurface();
			if (wrapper == 0) {
				descriptor = obj->getDescriptor();
				if (descriptor != NULL) {
					fRegion = descriptor->getFormatterRegion();
					if (fRegion != NULL) {
						ncmRegion = fRegion->getLayoutRegion();
						if (ncmRegion != NULL) {
							w = ncmRegion->getWidthInPixels();
							h = ncmRegion->getHeightInPixels();
							if (w > 0 && h > 0) {
								wrapper = dm->createSurface(myScreen, w, h);
								player->setSurface(wrapper);
							}
						}
					}
				}
			}
		}
	}

	void ApplicationPlayerAdapter::createPlayer() {
		clog << "ApplicationPlayerAdapter::createPlayer(" << this << ")";
		clog << endl;

		FormatterPlayerAdapter::createPlayer();
	}

	bool ApplicationPlayerAdapter::hasPrepared() {
		bool presented;

		if (object == NULL || player == NULL) {
			clog << "ApplicationPlayerAdapter::hasPrepared(" << mrl;
			clog << ") return false because";
			clog << " object = '" << object << "' and player = '";
			clog << player << "'" << endl;
			return false;
		}

		presented = player->hasPresented();
		if (presented) {
			clog << "ApplicationPlayerAdapter::hasPrepared(" << mrl;
			clog << ") return false because";
			clog << " presented is true" << endl;
			return false;
		}

		return true;
	}

	bool ApplicationPlayerAdapter::prepare(
			ExecutionObject* object, FormatterEvent* event) {

		Content* content;
		CascadingDescriptor* descriptor;
		double explicitDur;

		if (object == NULL) {
			clog << "ApplicationPlayerAdapter::prepare(";
			clog << this << ") Warning! Can't prepare NULL object" << endl;

			return false;
		}

		if (this->object != object) {
			lockPreparedEvents();
			preparedEvents.clear();
			unlockPreparedEvents();

			lockObject();
			this->object = object;
			unlockObject();

			if (this->object->getDataObject() != NULL &&
					this->object->getDataObject()->getDataEntity() != NULL) {

				content = ((NodeEntity*)(object->getDataObject()->
						getDataEntity()))->getContent();

				if (content != NULL && content->instanceOf(
						"ReferenceContent")) {

					this->mrl = ((ReferenceContent*)content)->
						    getCompleteReferenceUrl();

					// Update the url with the iUriD separation path
					// FIXME: This code is here and in FomatterPlayerAdapter.cpp
					str_replace_all( this->mrl,
						SystemCompat::getFUriD(),
						SystemCompat::getIUriD());
				} else {
					this->mrl = "";
				}
			}

			if (anchorMonitor != NULL) {
				anchorMonitor->stopMonitor();
				delete anchorMonitor;
				anchorMonitor = NULL;
			}

			if (player != NULL) {
				delete player;
				player = NULL;
			}

			explicitDur = prepareProperties(object);
			createPlayer();
			checkPlayerSurface(object);

		} else {
			explicitDur = prepareProperties(object);
		}

		if (event->instanceOf("PresentationEvent")) {
			double duration = ((PresentationEvent*)event)->getDuration();
			bool infDur     = (isNaN(duration) || isInfinity(duration));

			if (explicitDur < 0) {
				descriptor = object->getDescriptor();
				if (descriptor != NULL) {
					explicitDur = descriptor->getExplicitDuration();
				}
			}

			if (!infDur && duration <= 0 && explicitDur <= 0) {
				clog << "FormatterPlayerAdapter::prepare '";
				clog << object->getId() << "' Warning! Can't prepare an ";
				clog << "object with an event duration <= 0" << endl;

				return false;
			}

			//explicit duration overwrites implicit duration
			if (!isNaN(explicitDur) && explicitDur > 0) {
				object->removeEvent(event);

				//the explicit duration is a property of
				//the object. Which means: start an interface with
				//begin = 4s an explicit duration = 5s => new duration
				// will be 1s
				((PresentationEvent*)event)->setEnd(explicitDur);

				/*
				 * Adding event in object even though the it is added inside
				 * application execution object prepare (we have to consider
				 * that the event could be already prepared
				 */
				object->addEvent(event);

				clog << "ApplicationPlayerAdapter::prepare '";
				clog << object->getId() << "' ";
				clog << "with explicitDur = '";
				clog << explicitDur << "' object duration was '";
				clog << duration << "'. Updated info: event begin = '";
				clog << ((PresentationEvent*)event)->getBegin() << "'";
				clog << " event end = '";
				clog << ((PresentationEvent*)event)->getEnd() << "'";
				clog << endl;
			}
		}

		if (event->getCurrentState() == EventUtil::ST_SLEEPING) {
			if (!this->object->prepare(event, 0)) {
				return false;
			}

			prepare(event);
			return true;

		} else {
			clog << "ApplicationPlayerAdapter::prepare event '";
			clog << event->getId() << "' is not sleeping!" << endl;

			return false;
		}
	}

	void ApplicationPlayerAdapter::prepare(FormatterEvent* event) {
		double duration;
		IntervalAnchor* intervalAnchor;
		CascadingDescriptor* descriptor;
		LayoutRegion* region;

		descriptor = object->getDescriptor();
		if (descriptor != NULL) {
			region = descriptor->getRegion();
			//the player has NULL address if it is a remote one
			if (region != NULL && player != NULL) {
				player->setNotifyContentUpdate(
						region->getDeviceClass() == 1);
			}
		}

		if (player != NULL && event->instanceOf("AnchorEvent")) {
			if ((((AnchorEvent*)event)->getAnchor())->instanceOf(
					"LambdaAnchor")) {

				duration = ((PresentationEvent*)event)->getDuration();

				if (duration < IntervalAnchor::OBJECT_DURATION) {
					player->setScope(
							"",
							IPlayer::TYPE_PRESENTATION,
							0.0, duration / 1000);
				}

			} else if (((((AnchorEvent*)event)->getAnchor()))->
					instanceOf("IntervalAnchor")) {

				intervalAnchor = (IntervalAnchor*)(
						((AnchorEvent*)event)->getAnchor());

				player->setScope(
						((AnchorEvent*)event)->getAnchor()->getId(),
						IPlayer::TYPE_PRESENTATION,
						(intervalAnchor->getBegin() / 1000),
						(intervalAnchor->getEnd() / 1000));

			} else if (((((AnchorEvent*)event)->getAnchor()))->
					instanceOf("LabeledAnchor")) {

				duration = ((PresentationEvent*)event)->getDuration();

				clog << "ApplicationPlayerAdapter::prepare '" << object->getId();
				clog << "' with dur = '" << duration << "'" << endl;

				if (isNaN(duration)) {
					player->setScope(((LabeledAnchor*)(
							(AnchorEvent*)event)->getAnchor())->getLabel(),
							IPlayer::TYPE_PRESENTATION);

				} else {
					player->setScope(((LabeledAnchor*)(
							(AnchorEvent*)event)->getAnchor())->getLabel(),
							IPlayer::TYPE_PRESENTATION,
							0.0, duration / 1000);
				}
			}
		}

		lockPreparedEvents();
		preparedEvents[event->getId()] = event;
		unlockPreparedEvents();
	}

	bool ApplicationPlayerAdapter::start() {
		bool startSuccess = false;

		clog << "ApplicationPlayerAdapter::start ";
		clog << endl;

		if (im != NULL && object != NULL) {
			im->addInputEventListener(this, object->getInputEvents());
		}

		if (player != NULL) {
			startSuccess = player->play();
		}
		if ((startSuccess)||(objectDevice == 2)){ //DeviceDomain::CT_ACTIVE
			//clog << "ApplicationPlayerAdapter::play objectDevice" << objectDevice << endl;
			if (object != NULL && !object->start()) {
				if (player != NULL) {
					player->stop();
				}
				startSuccess = false;

			} else {
				checkAnchorMonitor();

				FormatterPlayerAdapter::printAction("start", object);
			}

			return startSuccess;

		} else {
			if (im != NULL) {
				im->removeInputEventListener(this);
			}
		}
		return false;
	}

	bool ApplicationPlayerAdapter::stop() {
		map<string, FormatterEvent*>::iterator i;
		FormatterEvent* event;
		bool stopLambda = false;

		if (currentEvent != NULL && currentEvent->instanceOf("AnchorEvent") &&
				((AnchorEvent*)currentEvent)->getAnchor() != NULL &&
				((AnchorEvent*)currentEvent)->getAnchor()->instanceOf(
						"LambdaAnchor")) {

			stopLambda = true;
		}

		if (stopLambda) {
			clog << "ApplicationPlayerAdapter::stop ALL" << endl;

			lockPreparedEvents();
			if (currentEvent->getCurrentState() != EventUtil::ST_SLEEPING &&
					player != NULL) {

				player->stop();
				player->notifyReferPlayers(EventUtil::TR_STOPS);
				FormatterPlayerAdapter::printAction("stop", object);
			}

			i = preparedEvents.begin();
			while (i != preparedEvents.end()) {
				event = i->second;
				if (event != currentEvent &&
						event->getCurrentState() != EventUtil::ST_SLEEPING) {

					preparedEvents.erase(i);
					i = preparedEvents.begin();

					clog << "ApplicationPlayerAdapter::stop ALL forcing '";
					clog << event->getId() << "' to stop" << endl;
					event->stop();

				} else {
					++i;
				}
			}

			unlockPreparedEvents();

		} else if (player != NULL && !player->isForcedNaturalEnd()) {
			clog << "ApplicationPlayerAdapter::stop calling stop player";
			clog << endl;

			player->stop();
			player->notifyReferPlayers(EventUtil::TR_STOPS);
		}

		if (object != NULL && object->stop()) {
			clog << "ApplicationPlayerAdapter::stop calling unprepare";
			clog << endl;

			unprepare();
			return true;
		}

		if (stopLambda && !currentEvent->stop()) {
			clog << "ApplicationPlayerAdapter::stop '";
			clog << currentEvent->getId() << "' is already sleeping";
			clog << endl;

		} else {
			clog << "ApplicationPlayerAdapter::stop(" << this;
			clog << ") Can't stop an already stopped object = '";
			clog << object << "'. mrl = '" << mrl << "' device class = '";
			clog << getObjectDevice() << "'" << endl;
		}
		return false;
	}

	bool ApplicationPlayerAdapter::pause() {
		if (object != NULL && object->pause()) {
			if (player != NULL) {
				player->pause();
				player->notifyReferPlayers(EventUtil::TR_PAUSES);

				FormatterPlayerAdapter::printAction("pause", object);
			}
			return true;

		} else {
			return false;
		}
	}

	bool ApplicationPlayerAdapter::resume() {
		if (object != NULL && object->resume()) {
			if (player != NULL) {
				player->resume();
				player->notifyReferPlayers(EventUtil::TR_RESUMES);

				FormatterPlayerAdapter::printAction("resume", object);
			}
			return true;
		}
		return false;
	}

	bool ApplicationPlayerAdapter::abort() {
		map<string, FormatterEvent*>::iterator i;
		FormatterEvent* event;
		bool abortLambda = false;

		if (currentEvent != NULL && currentEvent->instanceOf("AnchorEvent") &&
				((AnchorEvent*)currentEvent)->getAnchor() != NULL &&
				((AnchorEvent*)currentEvent)->getAnchor()->instanceOf(
						"LambdaAnchor")) {

			abortLambda = true;
		}

		if (abortLambda) {
			clog << "ApplicationPlayerAdapter::abort ALL" << endl;

			player->stop();
			player->notifyReferPlayers(EventUtil::TR_ABORTS);

			FormatterPlayerAdapter::printAction("abort", object);

			lockPreparedEvents();
			i = preparedEvents.begin();
			while (i != preparedEvents.end()) {
				event = i->second;
				if (event != currentEvent &&
						event->getCurrentState() != EventUtil::ST_SLEEPING) {

					preparedEvents.erase(i);
					i = preparedEvents.begin();

					clog << "ApplicationPlayerAdapter::abort ALL forcing '";
					clog << event->getId() << "' to abort" << endl;
					event->abort();

				} else {
					++i;
				}
			}

			unlockPreparedEvents();

		} else if (player != NULL && !player->isForcedNaturalEnd()) {
			clog << "ApplicationPlayerAdapter::abort calling stop player";
			clog << endl;

			player->stop();
			player->notifyReferPlayers(EventUtil::TR_ABORTS);
		}

		if (object != NULL && object->abort()) {
			clog << "ApplicationPlayerAdapter::abort calling unprepare";
			clog << endl;

			unprepare();
			return true;
		}

		if (abortLambda && !currentEvent->abort()) {
			clog << "ApplicationPlayerAdapter::abort '";
			clog << currentEvent->getId() << "' is already sleeping";
			clog << endl;

		} else {
			clog << "ApplicationPlayerAdapter::abort(" << this;
			clog << ") Can't abort an already sleeping object = '";
			clog << object << "'. mrl = '" << mrl << "' device class = '";
			clog << getObjectDevice() << "'" << endl;
		}
		return false;
	}

	bool ApplicationPlayerAdapter::unprepare() {
		map<string, FormatterEvent*>::iterator i;

		clog << "ApplicationPlayerAdapter::unprepare ";
		clog << endl;

		if (currentEvent == NULL) {
			if (im != NULL) {
				im->removeInputEventListener(this);
				im = NULL;
			}

			if (anchorMonitor != NULL) {
				anchorMonitor->stopMonitor();
			}

			if (object != NULL) {
				manager->removePlayer(object);
				object->unprepare();
			}

			return true;
		}

		if (currentEvent->getCurrentState() == EventUtil::ST_OCCURRING ||
				currentEvent->getCurrentState() == EventUtil::ST_PAUSED) {

			clog << "ApplicationPlayerAdapter::unprepare stopping ";
			clog << "current event '" << currentEvent->getId() << "'";
			clog << endl;

			currentEvent->stop();
		}

		lockPreparedEvents();
		if (preparedEvents.count(currentEvent->getId()) != 0 &&
				preparedEvents.size() == 1) {

			if (object != NULL) {
				object->unprepare();
				manager->removePlayer(object);
			}

			preparedEvents.clear();

			object = NULL;

		} else {
			if (object != NULL) {
				object->unprepare();
			}

			i = preparedEvents.find(currentEvent->getId());
			if (i != preparedEvents.end()) {
				preparedEvents.erase(i);
			}

			clog << "ApplicationPlayerAdapter::unprepare I still have '";
			clog << preparedEvents.size() << "' prepared events" << endl;
		}

		unlockPreparedEvents();

		return true;
	}

	void ApplicationPlayerAdapter::naturalEnd() {
		map<string, FormatterEvent*>::iterator i;
		FormatterEvent* event;

		clog << "ApplicationPlayerAdapter::naturalEnd ";
		clog << endl;

		if (player != NULL) {
			player->notifyReferPlayers(EventUtil::TR_STOPS);
		}

		lockPreparedEvents();
		i = preparedEvents.begin();
		while (i != preparedEvents.end()) {
			event = i->second;
			if (event != NULL && event->instanceOf("AnchorEvent") &&
					((AnchorEvent*)event)->getAnchor() != NULL &&
					((AnchorEvent*)event)->getAnchor()->instanceOf(
							"LambdaAnchor")) {

				unlockPreparedEvents();
				event->stop();
				FormatterPlayerAdapter::printAction("stop", object);
				unprepare();
				return;
			}
			++i;
		}

		unlockPreparedEvents();

		if (object != NULL && object->stop()) {
			clog << "ApplicationPlayerAdapter::naturalEnd call unprepare";
			clog << endl;
			FormatterPlayerAdapter::printAction("stop", object);
			unprepare();
		}
	}

	void ApplicationPlayerAdapter::updateStatus(
			short code, string param, short type, string value) {

		ApplicationStatus* data;

		lock();
		data         = new ApplicationStatus;
		data->code   = code;
		data->param  = param;
		data->type   = type;
		data->value  = value;

		if (!running) {
			running = true;
			Thread::startThread();
		}

		notes.push_back(data);

		unlock();

		unlockConditionSatisfied();
	}

	void ApplicationPlayerAdapter::notificationHandler(
			short code, string param, short type, string value) {

		clog << "ApplicationPlayerAdapter::notificationHandler";
		clog << " with code = '" << code << "' param = '";
		clog << param << "'";
		clog << " type = '" << type << "' and value = '";
		clog << value << "'";
		clog << endl;

		switch(code) {
			case IPlayer::PL_NOTIFY_START:
				clog << "ApplicationPlayerAdapter::notificationHandler";
				clog << " call startEvent '" << param << "'";
				clog << " type '" << type << "'";
				clog << endl;

				startEvent(param, type, value);
				break;

			case IPlayer::PL_NOTIFY_PAUSE:
				pauseEvent(param, type);
				break;

			case IPlayer::PL_NOTIFY_RESUME:
				resumeEvent(param, type);
				break;

			case IPlayer::PL_NOTIFY_STOP:
				if (param == "") {
					clog << "ApplicationPlayerAdapter::notificationHandler";
					clog << " call naturalEnd '" << param << "'";
					clog << " type '" << type << "'";
					clog << endl;
					naturalEnd();

				} else {
					clog << "ApplicationPlayerAdapter::notificationHandler";
					clog << " call stopEvent '" << param << "'";
					clog << " type '" << type << "'";
					clog << endl;
					stopEvent(param, type, value);
				}
				break;

			case IPlayer::PL_NOTIFY_ABORT:
				abortEvent(param, type);
				break;

			case IPlayer::PL_NOTIFY_NCLEDIT:
				if (editingCommandListener != NULL) {
					string cTag, bId, dId;
					string::size_type pos;
					NclPlayerData* pData = manager->getNclPlayerData();

					pos   = param.find_first_of(",");
					cTag  = param.substr(0, pos);
					param = param.substr(pos + 1, param.length() - pos + 1);

					pos   = param.find_first_of(",");
					bId   = trim(param.substr(0, pos));
					param = param.substr(pos + 1, param.length() - pos + 1);

					if (bId == "" || bId == "''") {
						bId = pData->baseId;
					}

					pos   = param.find_first_of(",");
					dId   = trim(param.substr(0, pos));
					param = param.substr(pos + 1, param.length() - pos + 1);

					if (dId == "" || dId == "''") {
						dId = pData->docId;
					}

					param = cTag + "," + bId + "," + dId + "," + param;

					clog << "ApplicationPlayerAdapter::notificationHandler";
					clog << " NCLEDIT newParam = '" << param << "'";
					clog << endl;

					editingCommandListener->updateStatus(
							code, param, type, value);

				} else {
					clog << "ApplicationPlayerAdapter::notificationHandler";
					clog << " NCLEDIT Warning! editing command listener is ";
					clog << "NULL";
					clog << endl;
				}
				break;

			default:
				clog << "ApplicationPlayerAdapter::notificationHandler";
				clog << " Warning! Received an unknown notification";
				clog << endl;
				break;
		}
	}

	void ApplicationPlayerAdapter::run() {
		short code = 0;
		string param;
		short type;
		string value;
		ApplicationStatus* data;

		while (running) {
			lock();
			if (!notes.empty()) {
				data  = *notes.begin();

				code  = data->code;
				param = data->param;
				type  = data->type;
				value = data->value;

				delete data;
				data = NULL;
				notes.erase(notes.begin());

			} else {
				code = -1;
			}
			unlock();

			if (object == NULL) {
				break;
			}

			if (code >= 0) {
				notificationHandler(code, param, type, value);
			}

			code = -1;

			lock();
			if (notes.empty() && running && !isDeleting) {
				unlock();
				waitForUnlockCondition();

			} else {
				unlock();
			}
		}
	}

	bool ApplicationPlayerAdapter::checkEvent(
			FormatterEvent* event, short type) {

		bool isPresentation;
		bool isAttribution;

		if (event != NULL) {
			isPresentation = event->instanceOf("PresentationEvent") &&
					type == IPlayer::TYPE_PRESENTATION;

			isAttribution = event->instanceOf("AttributionEvent") &&
					type == IPlayer::TYPE_ATTRIBUTION;

			if (isPresentation || isAttribution) {
				return true;
			}
		}

		return false;
	}

	bool ApplicationPlayerAdapter::startEvent(
			string anchorId, short type, string value) {

		FormatterEvent* event;
		bool fakeStart = false;

		event = object->getEventFromAnchorId(anchorId);
		if (checkEvent(event, type)) {
			if (prepare(object, event)) {
				/*clog << "ApplicationPlayerAdapter::startEvent '";
				clog << event->getId() << "' with anchorId = '";
				clog << anchorId << "'" << endl;*/

				if (setAndLockCurrentEvent(event)) {
					if (type == IPlayer::TYPE_PRESENTATION) {
						fakeStart = object->start();
						unlockCurrentEvent(event);

					} else {
						fakeStart = event->start();
						((AttributionEvent*)event)->setValue(value);
						unlockCurrentEvent(event);

						/*if (hasPrepared()) {
							setPropertyValue(
									(AttributionEvent*)event, value);

							player->setPropertyValue(anchorId, value);

						} else {
							object->setPropertyValue(
									(AttributionEvent*)event, value);
						}*/
					}
				}

			} else {
				clog << "ApplicationPlayerAdapter::startEvent can't ";
				clog << "prepare '" << event->getId() << "' from ";
				clog << "anchor ID = '" << anchorId << "'" << endl;
			}

		} else {
			clog << "ApplicationPlayerAdapter::startEvent event not found '";
			clog << anchorId;
			clog << "' in object '" << object->getId();
			clog << endl;
		}

		return fakeStart;
	}

	bool ApplicationPlayerAdapter::stopEvent(
			string anchorId, short type, string value) {

		FormatterEvent* event;

		if (object->getId() == anchorId) {
			clog << "ApplicationPlayerAdapter::stopEvent ";
			clog << " considering anchor '";
			clog << anchorId << "' will call naturalEnd" << endl;
			naturalEnd();
			return false;
		}

		event = object->getEventFromAnchorId(anchorId);
		if (checkEvent(event, type)) {
			clog << "ApplicationPlayerAdapter::stopEvent '";
			clog << event->getId();
			clog << "' from anchorId = '";
			clog << anchorId << "' calling setAndLockCurrentEvent";
			clog << endl;

			if (setAndLockCurrentEvent(event)) {
				if (type == IPlayer::TYPE_PRESENTATION) {
					if (object->stop()) {
						unprepare();
						unlockCurrentEvent(event);
						return true;
					}

				} else {
					((AttributionEvent*)event)->setValue(value);
					if (event->stop()) {
						unprepare();
						unlockCurrentEvent(event);

						/*if (hasPrepared()) {
							setPropertyValue(
									(AttributionEvent*)event, value);

							player->setPropertyValue(anchorId, value);

						} else {
							object->setPropertyValue(
									(AttributionEvent*)event, value);
						}*/
						return true;
					}
				}
				unlockCurrentEvent(event);
			}

		} else {
			clog << "ApplicationPlayerAdapter::stopEvent can't stop event";
			clog << "'" << anchorId << "'";
			clog << endl;
		}

		return false;
	}

	bool ApplicationPlayerAdapter::abortEvent(string anchorId, short type) {
		FormatterEvent* event;
		string cvt_id = anchorId;

		if (object->getId() == anchorId) {
			cvt_id = "";
		}

		event = object->getEventFromAnchorId(cvt_id);
		if (checkEvent(event, type)) {
			clog << "ApplicationPlayerAdapter::abortEvent '";
			clog << event->getId();
			clog << "' from anchorId = '";
			clog << anchorId << "' calling setAndLockCurrentEvent";
			clog << endl;

			if (setAndLockCurrentEvent(event)) {
				if (type == IPlayer::TYPE_PRESENTATION) {
					if (object->abort()) {
						unprepare();
						unlockCurrentEvent(event);
						return true;
					}

				} else {
					if (event->abort()) {
						unprepare();
						unlockCurrentEvent(event);

						/*if (hasPrepared()) {
							setPropertyValue(
									(AttributionEvent*)event, value);

							player->setPropertyValue(anchorId, value);

						} else {
							object->setPropertyValue(
									(AttributionEvent*)event, value);
						}*/
						return true;
					}
				}
				unlockCurrentEvent(event);
			}

		} else {
			clog << "ApplicationPlayerAdapter::abortEvent can't abort event";
			clog << "'" << anchorId << "'";
			clog << endl;
		}

		return false;
	}

	bool ApplicationPlayerAdapter::pauseEvent(string anchorId, short type) {
		FormatterEvent* event;
		string cvt_id = anchorId;

		if (object->getId() == anchorId) {
			cvt_id = "";
		}

		event = object->getEventFromAnchorId(cvt_id);
		if (checkEvent(event, type)) {
			clog << "ApplicationPlayerAdapter::pauseEvent '";
			clog << event->getId();
			clog << "' from anchorId = '";
			clog << anchorId << "' calling setAndLockCurrentEvent";
			clog << endl;

			if (setAndLockCurrentEvent(event)) {
				if (type == IPlayer::TYPE_PRESENTATION) {
					if (object->pause()) {
						unlockCurrentEvent(event);
						return true;
					}

				} else {
					if (event->pause()) {
						unlockCurrentEvent(event);
						return true;
					}
				}
				unlockCurrentEvent(event);
			}

		} else {
			clog << "ApplicationPlayerAdapter::pauseEvent can't pause event";
			clog << "'" << anchorId << "'";
			clog << endl;
		}

		return false;
	}

	bool ApplicationPlayerAdapter::resumeEvent(string anchorId, short type) {
		FormatterEvent* event;
		string cvt_id = anchorId;

		if (object->getId() == anchorId) {
			cvt_id = "";
		}

		event = object->getEventFromAnchorId(cvt_id);
		if (checkEvent(event, type)) {
			clog << "ApplicationPlayerAdapter::resumeEvent '";
			clog << event->getId();
			clog << "' from anchorId = '";
			clog << anchorId << "' calling setAndLockCurrentEvent";
			clog << endl;

			if (setAndLockCurrentEvent(event)) {
				if (type == IPlayer::TYPE_PRESENTATION) {
					if (object->resume()) {
						unlockCurrentEvent(event);
						return true;
					}

				} else {
					if (event->resume()) {
						unlockCurrentEvent(event);
						return true;
					}
				}
				unlockCurrentEvent(event);
			}

		} else {
			clog << "ApplicationPlayerAdapter::resumeEvent can't resume event";
			clog << "'" << anchorId << "'";
			clog << endl;
		}

		return false;
	}

	void ApplicationPlayerAdapter::lockEvent() {
		Thread::mutexLock(&eventMutex);
	}

	void ApplicationPlayerAdapter::unlockEvent() {
		Thread::mutexUnlock(&eventMutex);
	}

	void ApplicationPlayerAdapter::lockPreparedEvents() {
		Thread::mutexLock(&eventsMutex);
	}

	void ApplicationPlayerAdapter::unlockPreparedEvents() {
		Thread::mutexUnlock(&eventsMutex);
	}
}
}
}
}
}
}
}
