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

#include "gingancl/focus/FormatterFocusManager.h"

#include "config.h"

#include "mb/LocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "multidevice/services/IDeviceDomain.h"
using namespace ::br::pucrio::telemidia::ginga::core::multidevice;

#include "gingancl/emconverter/FormatterConverter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::emconverter;

#include "gingancl/model/FormatterLayout.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "gingancl/FormatterScheduler.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace focus {

	bool FormatterFocusManager::init = false;
	set<FormatterFocusManager*> FormatterFocusManager::instances;
	pthread_mutex_t FormatterFocusManager::iMutex;

	static IScreenManager* dm = ScreenManagerFactory::getInstance();

	FormatterFocusManager::FormatterFocusManager(
		    PlayerAdapterManager* playerManager,
		    IPresentationContext* presContext,
		    IFormatterMultiDevice* multiDevice,
		    ILinkActionListener* settingActions,
		    void* converter) {

		string strValue;

		LocalScreenManager::addIEListenerInstance(this);
		LocalScreenManager::addMEListenerInstance(this);

		myScreen       = playerManager->getNclPlayerData()->screenId;
		im             = dm->getInputManager(myScreen);
		focusTable     = new map<string, set<ExecutionObject*>*>;
		currentFocus   = "";
		objectToSelect = "";
		isHandler      = false;
		parentManager  = NULL;
		xOffset        = 0;
		yOffset        = 0;
		width          = 0;
		height         = 0;
		focusHandlerTS = 0;

		strValue = presContext->getPropertyValue(DEFAULT_FOCUS_BORDER_COLOR);
		if (strValue == "") {
			defaultFocusBorderColor = new Color("blue");

		} else {
			defaultFocusBorderColor = new Color(strValue);
		}

		strValue = presContext->getPropertyValue(DEFAULT_FOCUS_BORDER_WIDTH);
		if (strValue == "") {
			defaultFocusBorderWidth = -3;

		} else {
			defaultFocusBorderWidth = util::stof(strValue);
		}

		strValue = presContext->getPropertyValue(DEFAULT_SEL_BORDER_COLOR);
		if (strValue == "") {
			defaultSelBorderColor = new Color("green");

		} else {
			defaultSelBorderColor = new Color(strValue);
		}

		this->presContext    = presContext;
		this->selectedObject = NULL;
		this->playerManager  = playerManager;
		this->multiDevice    = multiDevice;
		this->converter      = converter;
		this->settingActions = settingActions;

		Thread::mutexInit(&mutexFocus);
		Thread::mutexInit(&mutexTable);

		checkInit();

		Thread::mutexLock(&iMutex);
		instances.insert(this);
		Thread::mutexUnlock(&iMutex);
	}

	FormatterFocusManager::~FormatterFocusManager() {
		LocalScreenManager::removeIEListenerInstance(this);
		LocalScreenManager::removeMEListenerInstance(this);

		Thread::mutexLock(&mutexFocus);
		Thread::mutexLock(&mutexTable);

		hasInstance(this, true);

		if (hasInstance(parentManager, false)) {
			if (isHandler && !parentManager->setKeyHandler(true)) {
				parentManager->registerNavigationKeys();
			}
		}
		selectedObject = NULL;

		if (defaultFocusBorderColor != NULL) {
			delete defaultFocusBorderColor;
			defaultFocusBorderColor = NULL;
		}

		if (defaultSelBorderColor != NULL) {
			delete defaultSelBorderColor;
			defaultSelBorderColor = NULL;
		}

		if (focusTable != NULL) {
			delete focusTable;
			focusTable = NULL;
		}

		playerManager = NULL;
		presContext   = NULL;
		Thread::mutexUnlock(&mutexFocus);
		Thread::mutexDestroy(&mutexFocus);
		Thread::mutexUnlock(&mutexTable);
		Thread::mutexDestroy(&mutexTable);
	}

	void FormatterFocusManager::checkInit() {
		if (!init) {
			init = true;
			Thread::mutexInit(&iMutex, NULL);
		}
	}

	bool FormatterFocusManager::hasInstance(
			FormatterFocusManager* instance, bool remove) {

		set<FormatterFocusManager*>::iterator i;
		bool find = false;

		if (!init) {
			return false;
		}

		Thread::mutexLock(&iMutex);
		i = instances.find(instance);
		if (i != instances.end()) {
			find = true;
			if (remove) {
				instances.erase(i);
			}
		}
		Thread::mutexUnlock(&iMutex);

		return find;
	}

	void FormatterFocusManager::setParent(FormatterFocusManager* parent) {
		parentManager = parent;

		if (parentManager != NULL) {
			multiDevice->setParent(parentManager->multiDevice);
		}
	}

	bool FormatterFocusManager::isKeyHandler() {
		return isHandler;
	}

	bool FormatterFocusManager::setKeyHandler(bool isHandler) {
		ExecutionObject* focusedObj;
		CascadingDescriptor* dc;
		FormatterRegion* fr;
		string ix;

		/*clog << "FormatterFocusManager::setKeyHandler(" << this << ")";
		clog << " isHandler '" << isHandler << "'";
		clog << " this->isHandler '" << this->isHandler << "'";
		clog << " parentManager '" << parentManager << "'";
		clog << endl;*/

		if (this->isHandler == isHandler) {
			return false;
		}

		if (isHandler && parentManager != NULL &&
				!parentManager->isKeyHandler()) {

			clog << "FormatterFocusManager::setKeyHandler(" << this << ")";
			clog << " can't set handler because parent manager is not ";
			clog << "handling";
			clog << endl;
			return false;
		}

		focusedObj = getObjectFromFocusIndex(currentFocus);

		this->isHandler = isHandler;
		setHandlingObjects(isHandler);
		if (isHandler) {
			Thread::mutexLock(&mutexTable);
			if (focusedObj == NULL) {
				if (!focusTable->empty()) {
					ix = focusTable->begin()->first;
					Thread::mutexUnlock(&mutexTable);
					currentFocus = "";
					setFocus(ix);

				} else {
					Thread::mutexUnlock(&mutexTable);
				}

			} else {
				Thread::mutexUnlock(&mutexTable);
				ix = currentFocus;
				currentFocus = "";
				setFocus(ix);
			}
			registerNavigationKeys();

		} else {
			if (focusedObj != NULL) {
				dc = focusedObj->getDescriptor();
				if (dc != NULL) {
					fr = dc->getFormatterRegion();
					if (fr != NULL) {
						fr->setFocus(false);
					}
				}
				recoveryDefaultState(focusedObj);
			}
			unregister();
		}

		return isHandler;
	}

	/*void FormatterFocusManager::setStandByState(bool standBy) {
		if (!isHandler) {
			return;
		}

		if (standBy) {
			if (parentManager != NULL) {
				parentManager->unregister();
			}
			unregister();

		} else {
			if (parentManager != NULL) {
				parentManager->registerBackKeys();
			}
			registerNavigationKeys();
		}
	}*/

	void FormatterFocusManager::setHandlingObjects(bool isHandling) {
		((FormatterConverter*)converter)->setHandlingStatus(isHandling);
	}

	ExecutionObject* FormatterFocusManager::getObjectFromFocusIndex(
			string focusIndex) {

		map<string, set<ExecutionObject*>*>::iterator i;
		set<ExecutionObject*>::iterator j;
		CascadingDescriptor* desc;
		bool visible;

		i = focusTable->find(focusIndex);
		if (i == focusTable->end()) {
			return NULL;
		}

		j = i->second->begin();
		while (j != i->second->end()) {
			desc = (*j)->getDescriptor();
			if (desc != NULL && desc->getFormatterRegion() != NULL) {
				visible = desc->getFormatterRegion()->isVisible();
				if (visible) {
					return *j;
				}
			}
			++j;
		}

		delete i->second;
		focusTable->erase(i);

		return NULL;
	}

	void FormatterFocusManager::insertObject(
			ExecutionObject* obj, string focusIndex) {

		string auxIndex;
		map<string, set<ExecutionObject*>*>::iterator i;
		vector<string>::iterator j;
		set<ExecutionObject*>* objs;

		i = focusTable->find(focusIndex);
		if (i != focusTable->end()) {
			objs = i->second;

		} else {
			objs = new set<ExecutionObject*>;
			(*focusTable)[focusIndex] = objs;
		}

		objs->insert(obj);
	}

	void FormatterFocusManager::removeObject(
			ExecutionObject* obj, string focusIndex) {

		map<string, set<ExecutionObject*>*>::iterator i;
		set<ExecutionObject*>::iterator j;
		vector<string>::iterator k;
		set<ExecutionObject*>* objs;

		i = focusTable->find(focusIndex);
		if (i != focusTable->end()) {
			objs = i->second;
			j = objs->find(obj);
			if (j != objs->end()) {
				objs->erase(j);
			}
			if (objs->empty()) {
				delete objs;
				focusTable->erase(i);
			}
		}
	}

	void FormatterFocusManager::resetKeyMaster() {
		CascadingDescriptor* desc;
		FormatterPlayerAdapter* player = NULL;

		if (selectedObject != NULL) {
			objectToSelect = "";
			selectedObject->setHandler(false);
			desc = selectedObject->getDescriptor();
			if (desc != NULL) {
				desc->getFormatterRegion()->setSelection(false);
			}

			recoveryDefaultState(selectedObject);

			player = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(selectedObject);
			exitSelection(player);
		}
	}

	void FormatterFocusManager::tapObject(void* executionObject) {
		ExecutionObject* object = (ExecutionObject*)executionObject;
		CascadingDescriptor* ds = NULL;
		FormatterRegion* fr     = NULL;

		if (object->isSleeping()) {
			object = this->getObjectFromFocusIndex(currentFocus);
			if (object == NULL) {
				clog << "FormatterFocusManager::tapObject invalid object";
				clog << endl;
				return;
			}
		}

		ds = object->getDescriptor();
		if (ds != NULL) {
			fr = ds->getFormatterRegion();
		}

		clog << "FormatterFocusManager::tapObject " << endl;

		if (isHandler) {
			if (fr != NULL && fr->isVisible()) {
				if (fr->getFocusIndex() != "") {
					setFocus(fr->getFocusIndex());
				}

				if (fr->setSelection(true)) {
					if (!keyCodeOk(object)) {
						if (im != NULL) {
							im->postInputEvent(CodeMap::KEY_BACK);
						}
					}
				}

			} else {
				clog << "FormatterFocusManager::tapObject can't tap '";
				clog << object->getId();
				clog << "': !visible (FR = " << fr << ")" << endl;
			}

		} else {
			clog << "FormatterFocusManager::tapObject can't ";
			clog << " select '";
			clog << object->getId();
			clog << "' focus manager is handling = '" << isHandler;
			clog << "'" << endl;
		}
	}

	void FormatterFocusManager::setKeyMaster(string mediaId) {
		ExecutionObject* nextObject            = NULL;
		CascadingDescriptor* nextDescriptor    = NULL;
		FormatterRegion* fr                    = NULL;
		FormatterPlayerAdapter* player         = NULL;
		LayoutRegion* ncmRegion                = NULL;
		bool isFRVisible                       = false;
		bool abortKeyMaster                    = false;
		string lastFocus                       = "";

		Thread::mutexLock(&mutexTable);
		Thread::mutexLock(&mutexFocus);

		if (mediaId == "" && selectedObject != NULL) {
			resetKeyMaster();
			Thread::mutexUnlock(&mutexTable);
			Thread::mutexUnlock(&mutexFocus);
			return;
		}

		nextObject = ((FormatterConverter*)converter)->getObjectFromNodeId(
					mediaId);

		if (nextObject == NULL) {
			clog << "FormatterFocusManager::setKeyMaster can't set '";
			clog << mediaId << "' as master: object is not available.";
			clog << endl;

			objectToSelect = mediaId;
			Thread::mutexUnlock(&mutexTable);
			Thread::mutexUnlock(&mutexFocus);
			return;
		}

		if (selectedObject != NULL && selectedObject != nextObject) {
			resetKeyMaster();
		}

		Thread::mutexUnlock(&mutexTable);

		nextDescriptor = nextObject->getDescriptor();
		if (nextDescriptor != NULL) {
			fr = nextDescriptor->getFormatterRegion();
		}

		if (fr != NULL && fr->getFocusIndex() != "") {
			lastFocus = currentFocus;
			currentFocus = fr->getFocusIndex();
		}

		if (fr != NULL) {
			isFRVisible = fr->isVisible();
			if (isFRVisible) {
				fr->setSelection(true);

			} else {
				abortKeyMaster = true;
			}

		} else {
			abortKeyMaster = true;
		}

		if (abortKeyMaster && fr != NULL && fr->getFocusIndex() != "") {
			currentFocus = lastFocus;
			Thread::mutexUnlock(&mutexFocus);
			return;
		}

		//selecting new object
		selectedObject = nextObject;
		selectedObject->setHandler(true);
		player = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(selectedObject);

		enterSelection(player);
		nextObject->selectionEvent(
				CodeMap::KEY_NULL, player->getMediaTime() * 1000);

		Thread::mutexUnlock(&mutexFocus);
		multiDevice->updatePassiveDevices();
	}

	void FormatterFocusManager::setFocus(string focusIndex) {
		ExecutionObject* nextObject = NULL;
		ExecutionObject* currentObject = NULL;
		CascadingDescriptor* currentDescriptor = NULL;
		CascadingDescriptor* nextDescriptor = NULL;

		if ((focusIndex == currentFocus && currentFocus != "") || !isHandler) {
			if (!isHandler) {
				clog << "FormatterFocusManager::setFocus(" << this << ")";
				clog << " can't set focus index because I'm not ";
				clog << "handling";
				clog << endl;
			}
			return;
		}

		Thread::mutexLock(&mutexTable);
		Thread::mutexLock(&mutexFocus);

		nextObject = getObjectFromFocusIndex(focusIndex);
		if (nextObject == NULL) {
			Thread::mutexUnlock(&mutexFocus);
			Thread::mutexUnlock(&mutexTable);
			multiDevice->updatePassiveDevices();
			return;
		}

		currentObject = getObjectFromFocusIndex(currentFocus);
		if (currentObject != NULL) {
			Thread::mutexUnlock(&mutexTable);
			currentDescriptor = currentObject->getDescriptor();

		} else {
			currentDescriptor = NULL;
			clog << "FormatterFocusManager::setFocus index '";
			clog << focusIndex << "' is not in focus Table." << endl;
			Thread::mutexUnlock(&mutexTable);
		}

		currentFocus = focusIndex;
		presContext->setPropertyValue("service.currentFocus", currentFocus);

		if (currentDescriptor != NULL) {
			recoveryDefaultState(currentObject);
			currentDescriptor->getFormatterRegion()->setFocus(false);

		} else if (currentObject != NULL) {
			clog << "FormatterFocusManager::setFocus Warning can't recover '";
			clog << currentObject->getId() << "'" << endl;
		}

		nextDescriptor = nextObject->getDescriptor();
		if (nextDescriptor != NULL) {
			setFocus(nextDescriptor);
		}

		Thread::mutexUnlock(&mutexFocus);
		multiDevice->updatePassiveDevices();
	}

	void FormatterFocusManager::setFocus(CascadingDescriptor* descriptor) {
		double borderAlpha;
		bool canDelFocusColor = false;
		bool canDelSelColor   = false;
		Color* focusColor = NULL;
		Color* selColor = NULL;
		int borderWidth = -3;
		int width;
		FormatterRegion* fr = NULL;

		if (!isHandler) {
			clog << "FormatterFocusManager::setFocus(" << this << ")";
			clog << " can't set focus because I'm not ";
			clog << "handling";
			clog << endl;
			return;
		}

		fr = descriptor->getFormatterRegion();
		if (fr == NULL) {
			clog << "FormatterFocusManager::setFocus(" << this << ")";
			clog << " can't set focus because FR is NULL";
			clog << endl;
			return;
		}

		focusColor  = fr->getFocusBorderColor();
		borderWidth = fr->getFocusBorderWidth();
		selColor    = fr->getSelBorderColor();

		if (focusColor == NULL) {
			focusColor = defaultFocusBorderColor;
		}

		borderAlpha = descriptor->getFocusBorderTransparency();
		if (!isNaN(borderAlpha)) {
			canDelFocusColor = true;
			focusColor       = new Color(
					focusColor->getR(), focusColor->getG(), focusColor->getB(),
					(int)(borderAlpha * 255));
		}

		width = borderWidth;

		if (selColor == NULL) {
			selColor = defaultSelBorderColor;
		}

		if (!isNaN(borderAlpha)) {
			canDelSelColor = true;
			selColor       = new Color(
					selColor->getR(), selColor->getG(), selColor->getB(),
					(int)(borderAlpha * 255));
		}

		if (fr != NULL) {
			fr->setFocusInfo(focusColor, width,
					descriptor->getFocusSrc(), selColor, width,
					descriptor->getSelectionSrc());

			fr->setFocus(true);
		}

		if (canDelFocusColor) {
			delete focusColor;
			focusColor = NULL;
		}

		if (canDelSelColor) {
			delete selColor;
			selColor = NULL;
		}
	}

	void FormatterFocusManager::recoveryDefaultState(ExecutionObject* object) {
		GingaWindowID wId;
		FormatterPlayerAdapter* player;
		FormatterRegion* fRegion;

		if (object == NULL || object->getDescriptor() == NULL ||
				object->getDescriptor()->getFormatterRegion() == NULL) {

			multiDevice->updatePassiveDevices();
			return;
		}

		player = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(object);
		if (player != NULL) {
			fRegion = object->getDescriptor()->getFormatterRegion();
			fRegion->setRenderedSurface(player->getPlayer()->getSurface());
			fRegion->setFocus(false);
			wId = fRegion->getOutputId();
			player->setOutputWindow(wId);
			player->flip();
		}
		multiDevice->updatePassiveDevices();
	}

	void FormatterFocusManager::showObject(ExecutionObject* object) {
		CascadingDescriptor* descriptor;
		FormatterRegion* fr = NULL;
		string focusIndex, auxIndex;
		string paramValue, mediaId;
		FormatterPlayerAdapter* player;
		Node* ncmNode;
		vector<string>::iterator i;

		if (object == NULL) {
			clog << "FormatterFocusManager::showObject Warning! object ";
			clog << "is null." << endl;
			multiDevice->updatePassiveDevices();
			return;
		}

		descriptor = object->getDescriptor();
		if (descriptor == NULL) {
			clog << "FormatterFocusManager::showObject Warning! ";
			clog << " descriptor is null." << endl;
			multiDevice->updatePassiveDevices();
			return;
		}

		fr = descriptor->getFormatterRegion();
		if (fr == NULL) {
			clog << "FormatterFocusManager::showObject Warning! ";
			clog << " FR is null." << endl;
			multiDevice->updatePassiveDevices();
			return;
		}

		focusIndex = fr->getFocusIndex();

		ncmNode = object->getDataObject();
		mediaId = ncmNode->getDataEntity()->getId();

		if (ncmNode->instanceOf("ReferNode")) {
			if (((ReferNode*)ncmNode)->getInstanceType() == "new") {
				mediaId = ncmNode->getId();
			}
		}

		clog << "FormatterFocusManager::showObject '" << mediaId << "'";
		clog << " with focus index = '" << focusIndex << "'";
		clog << " current focus = '" << currentFocus << "'";
		clog << endl;

		if (focusIndex != "") {
			Thread::mutexLock(&mutexTable);
			insertObject(object, focusIndex);
			Thread::mutexUnlock(&mutexTable);
		}

		if (currentFocus == "") {
			registerNavigationKeys();

			paramValue = presContext->getPropertyValue(
					"service.currentKeyMaster");

			if (paramValue == mediaId || objectToSelect == mediaId) {
				objectToSelect = "";
				setKeyMaster(mediaId);

			} else if (focusIndex != "") {
				setFocus(focusIndex);
			}

		} else {
			paramValue = presContext->getPropertyValue("service.currentFocus");
			if (paramValue != "" &&
				    paramValue == focusIndex &&
				    fr->isVisible()) {

				/*if (focusTable->count(currentFocus) != 0) {
					currentObject = (*focusTable)[currentFocus];
					currentObject->getDescriptor()->
						    getFormatterRegion()->setFocus(false);

					recoveryDefaultState(currentObject);
				}

			 	currentFocus = focusIndex;*/
			 	setFocus(focusIndex);
			}

			paramValue = presContext->getPropertyValue(
					"service.currentKeyMaster");

			if ((paramValue == mediaId || objectToSelect == mediaId) &&
					fr->isVisible()) {

				objectToSelect = "";

				if (focusIndex != "") {
					setFocus(focusIndex);
				}

				// then set as selected
				if (fr->setSelection(true)) {
					// unselect the previous selected object, if exists
					if (selectedObject != NULL) {
						selectedObject->setHandler(false);
						selectedObject->getDescriptor()->
							    getFormatterRegion()->setSelection(false);

						recoveryDefaultState(selectedObject);
					}

					selectedObject = object;
					selectedObject->setHandler(true);
					player = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(selectedObject);
					enterSelection(player);
				}
			}
    	}
		multiDevice->updatePassiveDevices();
	}

	void FormatterFocusManager::hideObject(ExecutionObject* object) {
		string focusIndex = "", ix;
		FormatterRegion* fr;
		FormatterPlayerAdapter* player;
		map<string, set<ExecutionObject*>*>::iterator i;

		if (object == NULL || object->getDescriptor() == NULL) {
			return;
		}

		fr = object->getDescriptor()->getFormatterRegion();
		if (fr != NULL) {
			focusIndex = fr->getFocusIndex();
		}

		if (focusIndex != "") {
			Thread::mutexLock(&mutexTable);
			removeObject(object, focusIndex);
			Thread::mutexUnlock(&mutexTable);

			if (fr != NULL &&
					fr->getFocusState() == FormatterRegion::SELECTED &&
					selectedObject == object) {

				player = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(selectedObject);
				exitSelection(player);
				//region->setSelection(false);
				//recoveryDefaultState(selectedObject);
				selectedObject = NULL;
			}

			if (currentFocus == focusIndex) {
				//region->setFocus(false);
				//recoveryDefaultState(object);

				Thread::mutexLock(&mutexTable);
				if (focusTable->empty()) {
					Thread::mutexUnlock(&mutexTable);
					currentFocus = "";
					unregister();

				} else {
					ix = focusTable->begin()->first;
					Thread::mutexUnlock(&mutexTable);
					setFocus(ix);
				}
			}
		}
	}

	bool FormatterFocusManager::keyCodeOk(ExecutionObject* currentObject) {
		FormatterPlayerAdapter* player;
		bool isHandling = false;
/*
		clog << "FormatterFocusManager::keyCodeOk(" << this << ")";
		clog << " this->isHandler '" << this->isHandler << "'";
		clog << " parentManager '" << parentManager << "'";
		clog << " lastHandler '" << lastHandler << "'";
		clog << endl;
*/
		if (parentManager != NULL) {
			parentManager->unregister();
		}

		if (currentObject != selectedObject) {
			if (selectedObject != NULL) {
				clog << "FormatterFocusManager::keyCodeOk Warning! ";
				clog << "selecting an object with another selected." << endl;
				selectedObject->setHandler(false);
			}
		}

		selectedObject = currentObject;
		selectedObject->setHandler(true);
		player = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(selectedObject);

		changeSettingState("service.currentKeyMaster", "start");
		isHandling = enterSelection(player);

		if (selectedObject != NULL) {
			clog << "FormatterFocusManager::keyCodeOk ";
			clog << "selecting '" << selectedObject->getId() << "'" << endl;
			selectedObject->selectionEvent(
					CodeMap::KEY_NULL, player->getMediaTime() * 1000);
		}

		changeSettingState("service.currentKeyMaster", "stop");

		multiDevice->updatePassiveDevices();
		return isHandling;
	}

	bool FormatterFocusManager::keyCodeBack() {
		CascadingDescriptor* selectedDescriptor;
		FormatterRegion* fr = NULL;
		FormatterPlayerAdapter* player;
		string ix;
/*
		clog << "FormatterFocusManager::keyCodeBack(" << this << ")";
		clog << " this->isHandler '" << this->isHandler << "'";
		clog << " parentManager '" << parentManager << "'";
		clog << " lastHandler '" << lastHandler << "'";
		clog << endl;
*/

		if (parentManager != NULL) {
			parentManager->registerBackKeys();
		}

		if (selectedObject == NULL) {
			multiDevice->updatePassiveDevices();
			clog << "FormatterFocusManager::keyCodeBack NULL selObject";
			clog << endl;
			return false;
		}

		selectedObject->setHandler(false);
		selectedDescriptor = selectedObject->getDescriptor();
		if (selectedDescriptor == NULL) {
			multiDevice->updatePassiveDevices();
			clog << "FormatterFocusManager::keyCodeBack NULL selDescriptor";
			clog << endl;
			return false;
		}

		fr = selectedDescriptor->getFormatterRegion();
		if (fr == NULL) {
			multiDevice->updatePassiveDevices();
			clog << "FormatterFocusManager::keyCodeBack NULL formatterRegion";
			clog << endl;
			return false;
		}

		ix = fr->getFocusIndex();
		fr->setSelection(false);

		Thread::mutexLock(&mutexFocus);
		if (ix != "" && ix == currentFocus) {
			fr->setFocus(true);
		}

		if (selectedObject != NULL) {
			player = (FormatterPlayerAdapter*)playerManager->getObjectPlayer(selectedObject);
			changeSettingState("service.currentKeyMaster", "start");
			exitSelection(player);
			changeSettingState("service.currentKeyMaster", "stop");
			selectedObject = NULL;
		}
		Thread::mutexUnlock(&mutexFocus);

		multiDevice->updatePassiveDevices();
		return false;
	}

	bool FormatterFocusManager::enterSelection(FormatterPlayerAdapter* player) {
		bool newHandler = false;
		string keyMaster;

		registerBackKeys();

		if (player != NULL && selectedObject != NULL) {
			keyMaster = (selectedObject->getDataObject()->getDataEntity()->
					getId());

			presContext->setPropertyValue(
					"service.currentKeyMaster", keyMaster);

			newHandler = player->setKeyHandler(true);
			multiDevice->updatePassiveDevices();

			clog << "FormatterFocusManager::enterSelection(" << this << "): '";
			clog << keyMaster << "'" << endl;

		} else {
			clog << "FormatterFocusManager::enterSelection(";
			clog << this << ") can't enter selection" << endl;
		}

		return newHandler;
	}

	void FormatterFocusManager::exitSelection(FormatterPlayerAdapter* player) {
		clog << "FormatterFocusManager::exitSelection(" << this << ")" << endl;

		unregister();

		if (player != NULL) {
			player->setKeyHandler(false);

			presContext->setPropertyValue("service.currentKeyMaster", "");
		}

		registerNavigationKeys();

		if (player != NULL) {
			multiDevice->updatePassiveDevices();
		}
	}

	void FormatterFocusManager::registerNavigationKeys() {
		set<int>* evs;

		if (im != NULL) {
			evs = new set<int>;
			evs->insert(CodeMap::KEY_CURSOR_DOWN);
			evs->insert(CodeMap::KEY_CURSOR_LEFT);
			evs->insert(CodeMap::KEY_CURSOR_RIGHT);
			evs->insert(CodeMap::KEY_CURSOR_UP);

			evs->insert(CodeMap::KEY_ENTER);

			im->addInputEventListener(this, evs);
			im->addMotionEventListener(this);
		}
	}

	void FormatterFocusManager::registerBackKeys() {
		set<int>* evs;

		if (im != NULL) {
			evs = new set<int>;
			evs->insert(CodeMap::KEY_BACKSPACE);
			evs->insert(CodeMap::KEY_BACK);

			im->addInputEventListener(this, evs);
			im->removeMotionEventListener(this);
		}
	}

	void FormatterFocusManager::unregister() {
		if (im != NULL) {
			im->removeInputEventListener(this);
			im->removeMotionEventListener(this);
		}
	}

	void FormatterFocusManager::setDefaultFocusBorderColor(Color* color) {
		if (defaultFocusBorderColor != NULL) {
			delete defaultFocusBorderColor;
		}
		defaultFocusBorderColor = color;
	}

	void FormatterFocusManager::setDefaultFocusBorderWidth(int width) {
		defaultFocusBorderWidth = width;
	}

	void FormatterFocusManager::setDefaultSelBorderColor(Color* color) {
		if (defaultSelBorderColor != NULL) {
			delete defaultSelBorderColor;
		}
		defaultSelBorderColor = color;
	}

	void FormatterFocusManager::setMotionBoundaries(
			int x, int y, int w, int h) {

		xOffset = x;
		yOffset = y;
		width   = w;
		height  = h;
	}

	void FormatterFocusManager::changeSettingState(string name, string act) {
		set<ExecutionObject*>* settingObjects;
		set<ExecutionObject*>::iterator i;
		FormatterEvent* event;
		string keyM;

		settingObjects = ((FormatterConverter*)
				converter)->getSettingNodeObjects();

		/*clog << "FormatterFocusManager::changeSettingState number of ";
		clog << "settings objects: '" << settingObjects->size() << "'";
		clog << endl;*/

		i = settingObjects->begin();
		while (i != settingObjects->end()) {
			event = (*i)->getEventFromAnchorId(name);
			if (event != NULL && event->instanceOf("AttributionEvent")) {
				if (act == "start") {
					event->start();

				} else if (act == "stop") {
					if (name == "service.currentFocus") {
						((AttributionEvent*)(event))->setValue(currentFocus);

					} else if (name == "service.currentKeyMaster") {
						if (selectedObject != NULL) {
							keyM = (selectedObject->getDataObject()
									->getDataEntity()->getId());

							((AttributionEvent*)event)->setValue(keyM);
						}
					}

					event->stop();
				}
			}
			++i;
		}

		delete settingObjects;
		multiDevice->updatePassiveDevices();
	}

	bool FormatterFocusManager::userEventReceived(IInputEvent* userEvent) {
		ExecutionObject* currentObject;
		CascadingDescriptor* currentDescriptor;
		FormatterRegion* fr;
		string nextIndex;
		map<string, set<ExecutionObject*>*>::iterator i;

		const int code = userEvent->getKeyCode(myScreen);

		if (code == CodeMap::KEY_QUIT) {
			this->im = NULL;
			return true;
		}

		if (!isHandler) {
			return true;
		}

		Thread::mutexLock(&mutexTable);

		if (getCurrentTimeMillis() - focusHandlerTS < 300 &&
				code != CodeMap::KEY_BACKSPACE && code != CodeMap::KEY_BACK) {

			Thread::mutexUnlock(&mutexTable);
			return true;
		}

		focusHandlerTS = getCurrentTimeMillis();

		i = focusTable->find(currentFocus);
		if (i == focusTable->end()) {
			if (currentFocus != "") {
				clog << "FormatterFocusManager::userEventReceived ";
				clog << "currentFocus not found which is '" << currentFocus;
				clog << "'" << endl;
			}

			if (selectedObject != NULL && (code == CodeMap::KEY_BACKSPACE ||
					code == CodeMap::KEY_BACK)) {

				bool canBack = keyCodeBack();
				Thread::mutexUnlock(&mutexTable);

				return canBack;
			}

			if (!focusTable->empty()) {
				nextIndex = focusTable->begin()->first;
				Thread::mutexUnlock(&mutexTable);
				setFocus(nextIndex);

			} else {
				Thread::mutexUnlock(&mutexTable);
			}
			multiDevice->updatePassiveDevices();

			return true;
		}

		currentObject = getObjectFromFocusIndex(currentFocus);
		if (currentObject == NULL) {
			clog << "FormatterFocusManager::userEventReceived ";
			clog << "Warning! object == NULL" << endl;
			clog << "'" << endl;

			Thread::mutexUnlock(&mutexTable);
			multiDevice->updatePassiveDevices();

			return true;
		}
		Thread::mutexUnlock(&mutexTable);

		currentDescriptor = currentObject->getDescriptor();
		if (currentDescriptor == NULL) {
			clog << "FormatterFocusManager::userEventReceived ";
			clog << "Warning! descriptor == NULL" << endl;
			clog << "'" << endl;
			multiDevice->updatePassiveDevices();

			return true;
		}

		fr = currentDescriptor->getFormatterRegion();
		nextIndex = "";
		if (selectedObject != NULL) {
			if (code == CodeMap::KEY_BACKSPACE || code == CodeMap::KEY_BACK) {
				bool canItBack = keyCodeBack();

				return canItBack;
			}

		} else if (code == CodeMap::KEY_CURSOR_UP) {
			if (fr != NULL) {
				nextIndex = fr->getMoveUp();
			}

		} else if (code == CodeMap::KEY_CURSOR_DOWN) {
			if (fr != NULL) {
				nextIndex = fr->getMoveDown();
			}

		} else if (code == CodeMap::KEY_CURSOR_LEFT) {
			if (fr != NULL) {
				nextIndex = fr->getMoveLeft();
			}

		} else if (code == CodeMap::KEY_CURSOR_RIGHT) {
			if (fr != NULL) {
				nextIndex = fr->getMoveRight();
			}

		} else if (code == CodeMap::KEY_ENTER ||
				code == CodeMap::KEY_TAP) {

			userEvent->setKeyCode(myScreen, CodeMap::KEY_NULL);
			tapObject(currentObject);
			multiDevice->updatePassiveDevices();

			return false;
		}

		if (nextIndex != "") {
			changeSettingState("service.currentFocus", "start");
			setFocus(nextIndex);
			changeSettingState("service.currentFocus", "stop");
		}

		multiDevice->updatePassiveDevices();
		return true;
	}

	bool FormatterFocusManager::motionEventReceived(int x, int y, int z) {
		FormatterLayout* formatterLayout;
		ExecutionObject* object;
		string objectFocusIndex;

		/*clog << "FormatterFocusManager::motionEventReceived (x, y, z) = '";
		clog << x << ", " << y << ", " << z << "' is handler: ";
		clog << isHandler << "'" << endl;*/

		if (isHandler) {
			formatterLayout = (FormatterLayout*)(multiDevice->getMainLayout());
			if (formatterLayout != NULL) {
				if ((x < xOffset || x > xOffset + width) ||
						y < yOffset || y > yOffset + height) {

					return true;
				}

				object = formatterLayout->getObject(x, y);
				if (object != NULL && object->getDescriptor() != NULL) {
					FormatterRegion* fr;
					fr = object->getDescriptor()->getFormatterRegion();

					if (fr != NULL) {
						objectFocusIndex = fr->getFocusIndex();
						if (objectFocusIndex != "" &&
								objectFocusIndex != currentFocus) {

							setFocus(objectFocusIndex);
						}
					}
				}
			}
		}

		return true;
	}
}
}
}
}
}
}
