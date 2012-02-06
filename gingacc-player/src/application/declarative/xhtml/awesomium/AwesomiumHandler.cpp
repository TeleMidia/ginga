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

#include "../../../../../config.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "player/AwesomiumHandler.h"
#include "player/PlayersComponentSupport.h"

#include <unistd.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	AwesomiumInfo::AwesomiumInfo(GingaScreenID screenId, AwesomiumHDR id) {
		this->myScreen = screenId;
		this->id       = id;
		this->webCore  = NULL;
		this->webView  = NULL;
		this->mURL     = "";
		this->surface  = NULL;
		this->mouseX   = -1;
		this->mouseY   = -1;
		this->w        = -1;
		this->h        = -1;
		this->hasFocus = false;
		this->setFocus = false;
		this->update   = false;
		this->rFile    = "";
		this->ev       = NULL;

		this->_eMVarW  = false;
		pthread_cond_init(&_eMVar, NULL);
		pthread_mutex_init(&_eM, NULL);
	}

	AwesomiumInfo::~AwesomiumInfo() {
		pthread_cond_signal(&_eMVar);
		pthread_cond_destroy(&_eMVar);
		pthread_mutex_destroy(&_eM);
	}

	void AwesomiumInfo::useEvent() {
		_eMVarW = true;
		pthread_mutex_lock(&_eM);
		pthread_cond_wait(&_eMVar, &_eM);

		_eMVarW = false;
		pthread_mutex_unlock(&_eM);
	}

	bool AwesomiumInfo::eventUsed() {
		if (_eMVarW) {
			pthread_cond_signal(&_eMVar);
			return true;
		}
		return false;
	}

	bool AwesomiumInfo::userEventReceived(IInputEvent* ev) {
		this->ev = ev;
		useEvent();

		return true;
	}

	map<AwesomiumHDR, AwesomiumInfo*> AwesomiumHandler::_infos;
	AwesomiumHDR AwesomiumHandler::_id = 0;

	bool AwesomiumHandler::getAwesomeInfo(
			AwesomiumHDR id, AwesomiumInfo** aInfo, bool removeInfo) {

		map<AwesomiumHDR, AwesomiumInfo*>::iterator i;
		bool hasInfo = false;

		i = _infos.find(id);
		if (i != _infos.end()) {
			*aInfo = i->second;
			hasInfo = true;

			if (removeInfo) {
				_infos.erase(i);

				if (_infos.empty()) {
					_id = 0;
				}
			}
		}

		return hasInfo;
	}

	AwesomiumHDR AwesomiumHandler::createAwesomium(GingaScreenID screenId) {
		AwesomiumInfo* aInfo;

#if HAVE_COMPSUPPORT
		if (dm == NULL) {
			dm = ((LocalScreenManagerCreator*)(
				cm->getObject("LocalScreenManager")))();
		}

#else
		if (dm == NULL) {
			dm = LocalScreenManager::getInstance();
		}
#endif

		im = dm->getInputManager(screenId);

		_id++;
		aInfo = new AwesomiumInfo(screenId, _id);

		aInfo->surface = dm->createSurface(screenId);

		_infos[_id] = aInfo;

		return _id;
	}

	void AwesomiumHandler::destroyAwesomium(AwesomiumHDR id) {
		AwesomiumInfo* aInfo;

		if (getAwesomeInfo(id, &aInfo, true)) {
			im->removeInputEventListener(aInfo);
			delete aInfo;
		}
	}

	void AwesomiumHandler::getSize(AwesomiumHDR id, int* w, int* h) {
		AwesomiumInfo* aInfo;

		if (getAwesomeInfo(id, &aInfo)) {
			*w = aInfo->w;
			*h = aInfo->h;

		} else {
			*w = 0;
			*h = 0;
		}
	}

	void AwesomiumHandler::setSize(AwesomiumHDR id, int w, int h) {
		AwesomiumInfo* aInfo;

		if (getAwesomeInfo(id, &aInfo)) {
			aInfo->w = w;
			aInfo->h = h;
		}
	}

	void AwesomiumHandler::loadUrl(AwesomiumHDR id, string url) {
		AwesomiumInfo* aInfo;

		if (getAwesomeInfo(id, &aInfo)) {
			aInfo->mURL   = url;
			aInfo->update = true;

			if (aInfo->webCore == NULL) {
				Awesomium::WebCoreConfig config;
		        config.setEnablePlugins(true);
		        config.setSaveCacheAndCookies(false);

				config.setCustomCSS("::-webkit-scrollbar { display: none; }");

				aInfo->webCore = new Awesomium::WebCore(config);
			}

			if (aInfo->webView == NULL) {
				aInfo->webView = aInfo->webCore->createWebView(
						aInfo->w, aInfo->h);
			}

			if (aInfo->rFile == "") {
				aInfo->rFile = "/tmp/awesomium" + itos(
						(long)(void*)aInfo) + ".png";
			}

			if (aInfo->webView != NULL) {
				cout << "AwesomiumHandler::loadUrl call loadUrl" << endl;

				if (fileExists(aInfo->mURL)) {
					string base, file;

					if (aInfo->mURL.substr(0, 1) == "/") {
						base = aInfo->mURL.substr(
								0, aInfo->mURL.find_last_of("/"));

						file = aInfo->mURL.substr(
								aInfo->mURL.find_last_of("/") + 1,
								aInfo->mURL.length() - (
										aInfo->mURL.find_last_of("/") + 1));

					} else {
						base = getCurrentPath();
						file = aInfo->mURL;
					}

					aInfo->webCore->setBaseDirectory(base);
					aInfo->webView->loadFile(file);

				} else {
					aInfo->webView->loadURL(aInfo->mURL);
				}

				setFocus(aInfo);

				cout << "AwesomiumHandler::loadUrl call while isloading '";
				cout << aInfo->mURL << "'" << endl;

				while (aInfo->webView->isLoadingPage()) {
					update(aInfo, 50);
				}

				cout << "AwesomiumHandler::loadUrl '";
				cout << aInfo->mURL << "' is loaded" << endl;
				update(aInfo, 300);

				cout << "AwesomiumHandler::loadUrl call refresh" << endl;
				refresh(id);

				while (aInfo->update) {
					update(aInfo, 25);
					setFocus(aInfo);
					if (aInfo->webView->isDirty()) {
						refresh(id);
					}
					eventHandler(aInfo);
				}

				cout << "AwesomiumHandler::loadUrl call destroy" << endl;
				if (aInfo->webView != NULL) {
					aInfo->webView->destroy();
				}

				cout << "AwesomiumHandler::loadUrl call delete core" << endl;
				if (aInfo->webCore != NULL) {
					delete aInfo->webCore;
				}
			}
		}
	}

	void AwesomiumHandler::eventHandler(AwesomiumInfo* aInfo) {
		unsigned char key;
		int keyCode;
		int x = 0, y = 0, z = 0;
		IInputEvent* ev;

		ev = aInfo->ev;
		if (ev == NULL) {
			return;
		}

		if (ev->isKeyType()) {
			keyCode = ev->getKeyCode(aInfo->myScreen);
			key = keyCode;

	        Awesomium::WebKeyboardEvent keyEvent;

	        keyEvent.text[0] = key;
	        keyEvent.unmodifiedText[0] = key;
	        keyEvent.type = Awesomium::WebKeyboardEvent::TYPE_CHAR;
	        keyEvent.virtualKeyCode = key;
	        keyEvent.nativeKeyCode = key;

	        cout << "AwesomiumHandler::eventHandler CHAR" << endl;
	        aInfo->webView->injectKeyboardEvent(keyEvent);

		} else if (ev->isButtonPressType()) {
			aInfo->mouseX = im->getCurrentXAxisValue();
			aInfo->mouseY = im->getCurrentYAxisValue();

			cout << "AwesomiumHandler::eventHandler MOUSE CLICK on ";
			cout << aInfo->mouseX << "," << aInfo->mouseY << "'" << endl;

			aInfo->webView->injectMouseMove(aInfo->mouseX, aInfo->mouseY);
			update(aInfo, 25);
			aInfo->webView->injectMouseDown(Awesomium::LEFT_MOUSE_BTN);
			update(aInfo, 25);
			aInfo->webView->injectMouseUp(Awesomium::LEFT_MOUSE_BTN);
		}

		aInfo->ev = NULL;
		aInfo->eventUsed();
	}

	string AwesomiumHandler::getUrl(AwesomiumHDR id) {
		AwesomiumInfo* aInfo;
		string url = "";

		if (getAwesomeInfo(id, &aInfo)) {
			url = aInfo->mURL;
		}

		return (url);
	}

	ISurface* AwesomiumHandler::getSurface(AwesomiumHDR id) {
		AwesomiumInfo* aInfo;
		ISurface* s = NULL;

		if (getAwesomeInfo(id, &aInfo)) {
			s = aInfo->surface;
		}
		return (s);
	}

	void AwesomiumHandler::setFocus(AwesomiumHDR id, bool focus) {
		AwesomiumInfo* aInfo;

		if (getAwesomeInfo(id, &aInfo)) {
			aInfo->setFocus = focus;
		}
	}

	void AwesomiumHandler::setFocus(AwesomiumInfo* aInfo) {
		if (aInfo->setFocus && !aInfo->hasFocus) {
			aInfo->hasFocus = true;
			im->addInputEventListener(aInfo, NULL);
			if (aInfo->webView != NULL) {
				cout << "AwesomiumHandler::setFocus focus" << endl;
				aInfo->webView->focus();
			}

		} else if (!aInfo->setFocus && aInfo->hasFocus) {
			aInfo->hasFocus = false;
			im->removeInputEventListener(aInfo);
			if (aInfo->webView != NULL) {
				cout << "AwesomiumHandler::setFocus unfocus" << endl;
				aInfo->webView->unfocus();
			}
		}
	}

	void AwesomiumHandler::refresh(AwesomiumHDR id) {
		IWindow* win;
		AwesomiumInfo* aInfo;

		if (getAwesomeInfo(id, &aInfo)) {
			const RenderBuffer* renderBuffer = aInfo->webView->render();

			if (renderBuffer != NULL) {
				wstring pngFile(aInfo->rFile.length(), L' ');

				std::copy(
						aInfo->rFile.begin(),
						aInfo->rFile.end(),
						pngFile.begin());

				renderBuffer->saveToPNG(pngFile, true);

				win = (IWindow*)aInfo->surface->getParent();
				if (win != NULL) {
					win->renderFrom(aInfo->rFile);
					//win->validate();
				}

				remove(aInfo->rFile.c_str());
			}
		}
	}

	void AwesomiumHandler::update(AwesomiumInfo* aInfo, double value) {
		::usleep(value * 1000);
		aInfo->webCore->update();
	}

	void AwesomiumHandler::stopUpdate(AwesomiumHDR id) {
		AwesomiumInfo* aInfo;

		if (getAwesomeInfo(id, &aInfo)) {
			aInfo->update = false;
		}
	}
}
}
}
}
}
}
