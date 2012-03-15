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

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "mb/interface/CodeMap.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "player/AwesomiumHandler.h"
#include "player/PlayersComponentSupport.h"

extern "C" {
#include <pthread.h>
}

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

using namespace Awesomium;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {

	ILocalScreenManager* AwesomiumHandler::dm     = NULL;
	AwesomiumHDR AwesomiumHandler::s_id           = 0;

	/* static since we have to respect browser isolation */
	Awesomium::WebCore* AwesomiumHandler::webCore = NULL;
	WebView* AwesomiumHandler::webView            = NULL;

	map<int, int> AwesomiumHandler::fromGingaToAwesomium;
	map<AwesomiumHDR, AwesomiumInfo*> AwesomiumHandler::s_infos;
	map<AwesomiumHDR, IInputManager*> AwesomiumHandler::s_ims;
	pthread_mutex_t AwesomiumHandler::s_lMutex;

	AwesomiumInfo::AwesomiumInfo(GingaScreenID screenId, AwesomiumHDR id) {
		this->myScreen  = screenId;
		this->id        = id;
		this->mURL      = "";
		this->surface   = NULL;
		this->mouseX    = -1;
		this->mouseY    = -1;
		this->x         = 0;
		this->y         = 0;
		this->w         = -1;
		this->h         = -1;
		this->hasFocus  = false;
		this->setFocus  = false;
		this->update    = false;
		this->rFile     = "";
		this->eventCode = -1;
		this->eventType = ET_NONE;

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
		int x, y, z;
		clog << "AwesomiumInfo::userEventReceived " << endl;
		this->eventCode = ev->getKeyCode(myScreen);

		if (ev->isKeyType()) {
			this->eventType = ET_KEY;

		} else if (ev->isButtonPressType()) {
			this->eventType = ET_BUTTON;

		} else {
			this->eventType = ET_NONE;
		}

		return true;
	}

	void AwesomiumHandler::initCodeMap() {
		fromGingaToAwesomium[CodeMap::KEY_0]                 = KeyCodes::AK_0;
		fromGingaToAwesomium[CodeMap::KEY_1]                 = KeyCodes::AK_1;
		fromGingaToAwesomium[CodeMap::KEY_2]                 = KeyCodes::AK_2;
		fromGingaToAwesomium[CodeMap::KEY_3]                 = KeyCodes::AK_3;
		fromGingaToAwesomium[CodeMap::KEY_4]                 = KeyCodes::AK_4;
		fromGingaToAwesomium[CodeMap::KEY_5]                 = KeyCodes::AK_5;
		fromGingaToAwesomium[CodeMap::KEY_6]                 = KeyCodes::AK_6;
		fromGingaToAwesomium[CodeMap::KEY_7]                 = KeyCodes::AK_7;
		fromGingaToAwesomium[CodeMap::KEY_8]                 = KeyCodes::AK_8;
		fromGingaToAwesomium[CodeMap::KEY_9]                 = KeyCodes::AK_9;

		fromGingaToAwesomium[CodeMap::KEY_SMALL_A]           = KeyCodes::AK_A;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_B]           = KeyCodes::AK_B;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_C]           = KeyCodes::AK_C;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_D]           = KeyCodes::AK_D;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_E]           = KeyCodes::AK_E;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_F]           = KeyCodes::AK_F;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_G]           = KeyCodes::AK_G;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_H]           = KeyCodes::AK_H;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_I]           = KeyCodes::AK_I;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_J]           = KeyCodes::AK_J;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_K]           = KeyCodes::AK_K;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_L]           = KeyCodes::AK_L;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_M]           = KeyCodes::AK_M;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_N]           = KeyCodes::AK_N;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_O]           = KeyCodes::AK_O;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_P]           = KeyCodes::AK_P;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_Q]           = KeyCodes::AK_Q;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_R]           = KeyCodes::AK_R;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_S]           = KeyCodes::AK_S;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_T]           = KeyCodes::AK_T;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_U]           = KeyCodes::AK_U;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_V]           = KeyCodes::AK_V;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_W]           = KeyCodes::AK_W;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_X]           = KeyCodes::AK_X;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_Y]           = KeyCodes::AK_Y;
		fromGingaToAwesomium[CodeMap::KEY_SMALL_Z]           = KeyCodes::AK_Z;

		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_A]         = KeyCodes::AK_A;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_B]         = KeyCodes::AK_B;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_C]         = KeyCodes::AK_C;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_D]         = KeyCodes::AK_D;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_E]         = KeyCodes::AK_E;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_F]         = KeyCodes::AK_F;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_G]         = KeyCodes::AK_G;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_H]         = KeyCodes::AK_H;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_I]         = KeyCodes::AK_I;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_J]         = KeyCodes::AK_J;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_K]         = KeyCodes::AK_K;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_L]         = KeyCodes::AK_L;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_M]         = KeyCodes::AK_M;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_N]         = KeyCodes::AK_N;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_O]         = KeyCodes::AK_O;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_P]         = KeyCodes::AK_P;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_Q]         = KeyCodes::AK_Q;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_R]         = KeyCodes::AK_R;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_S]         = KeyCodes::AK_S;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_T]         = KeyCodes::AK_T;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_U]         = KeyCodes::AK_U;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_V]         = KeyCodes::AK_V;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_W]         = KeyCodes::AK_W;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_X]         = KeyCodes::AK_X;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_Y]         = KeyCodes::AK_Y;
		fromGingaToAwesomium[CodeMap::KEY_CAPITAL_Z]         = KeyCodes::AK_Z;

		fromGingaToAwesomium[CodeMap::KEY_PAGE_DOWN]         = KeyCodes::AK_DOWN;
		fromGingaToAwesomium[CodeMap::KEY_PAGE_UP]           = KeyCodes::AK_UP;

		fromGingaToAwesomium[CodeMap::KEY_F1]                = KeyCodes::AK_F1;
		fromGingaToAwesomium[CodeMap::KEY_F2]                = KeyCodes::AK_F2;
		fromGingaToAwesomium[CodeMap::KEY_F3]                = KeyCodes::AK_F3;
		fromGingaToAwesomium[CodeMap::KEY_F4]                = KeyCodes::AK_F4;
		fromGingaToAwesomium[CodeMap::KEY_F5]                = KeyCodes::AK_F5;
		fromGingaToAwesomium[CodeMap::KEY_F6]                = KeyCodes::AK_F6;
		fromGingaToAwesomium[CodeMap::KEY_F7]                = KeyCodes::AK_F7;
		fromGingaToAwesomium[CodeMap::KEY_F8]                = KeyCodes::AK_F8;
		fromGingaToAwesomium[CodeMap::KEY_F9]                = KeyCodes::AK_F9;
		fromGingaToAwesomium[CodeMap::KEY_F10]               = KeyCodes::AK_F10;
		fromGingaToAwesomium[CodeMap::KEY_F11]               = KeyCodes::AK_F11;
		fromGingaToAwesomium[CodeMap::KEY_F12]               = KeyCodes::AK_F12;

		fromGingaToAwesomium[CodeMap::KEY_PLUS_SIGN]         = KeyCodes::AK_OEM_PLUS;
		fromGingaToAwesomium[CodeMap::KEY_MINUS_SIGN]        = KeyCodes::AK_OEM_MINUS;

		fromGingaToAwesomium[CodeMap::KEY_ASTERISK]          = KeyCodes::AK_MULTIPLY;
		fromGingaToAwesomium[CodeMap::KEY_NUMBER_SIGN]       = KeyCodes::AK_HANJA;

		fromGingaToAwesomium[CodeMap::KEY_PERIOD]            = KeyCodes::AK_OEM_PERIOD;

		fromGingaToAwesomium[CodeMap::KEY_SUPER]             = KeyCodes::AK_CAPITAL;
		fromGingaToAwesomium[CodeMap::KEY_PRINTSCREEN]       = KeyCodes::AK_PRINT;
		fromGingaToAwesomium[CodeMap::KEY_MENU]              = KeyCodes::AK_MENU;
		fromGingaToAwesomium[CodeMap::KEY_INFO]              = KeyCodes::AK_MENU;
		fromGingaToAwesomium[CodeMap::KEY_EPG]               = KeyCodes::AK_MENU;

		fromGingaToAwesomium[CodeMap::KEY_CURSOR_DOWN]       = KeyCodes::AK_DOWN;
		fromGingaToAwesomium[CodeMap::KEY_CURSOR_LEFT]       = KeyCodes::AK_LEFT;
		fromGingaToAwesomium[CodeMap::KEY_CURSOR_RIGHT]      = KeyCodes::AK_RIGHT;
		fromGingaToAwesomium[CodeMap::KEY_CURSOR_UP]         = KeyCodes::AK_UP;

		fromGingaToAwesomium[CodeMap::KEY_CHANNEL_DOWN]      = KeyCodes::AK_DOWN;
		fromGingaToAwesomium[CodeMap::KEY_CHANNEL_UP]        = KeyCodes::AK_UP;

		fromGingaToAwesomium[CodeMap::KEY_VOLUME_DOWN]       = KeyCodes::AK_VOLUME_DOWN;
		fromGingaToAwesomium[CodeMap::KEY_VOLUME_UP]         = KeyCodes::AK_VOLUME_UP;

		fromGingaToAwesomium[CodeMap::KEY_ENTER]             = KeyCodes::AK_RETURN;
		fromGingaToAwesomium[CodeMap::KEY_OK]                = KeyCodes::AK_RETURN;

		fromGingaToAwesomium[CodeMap::KEY_RED]               = KeyCodes::AK_UNKNOWN;
		fromGingaToAwesomium[CodeMap::KEY_GREEN]             = KeyCodes::AK_UNKNOWN;
		fromGingaToAwesomium[CodeMap::KEY_YELLOW]            = KeyCodes::AK_UNKNOWN;
		fromGingaToAwesomium[CodeMap::KEY_BLUE]              = KeyCodes::AK_UNKNOWN;

		fromGingaToAwesomium[CodeMap::KEY_SPACE]             = KeyCodes::AK_SPACE;
		fromGingaToAwesomium[CodeMap::KEY_BACKSPACE]         = KeyCodes::AK_BACK;
		fromGingaToAwesomium[CodeMap::KEY_BACK]              = KeyCodes::AK_BACK;
		fromGingaToAwesomium[CodeMap::KEY_ESCAPE]            = KeyCodes::AK_ESCAPE;
		fromGingaToAwesomium[CodeMap::KEY_EXIT]              = KeyCodes::AK_ESCAPE;

		fromGingaToAwesomium[CodeMap::KEY_POWER]             = KeyCodes::AK_UNKNOWN;
		fromGingaToAwesomium[CodeMap::KEY_REWIND]            = KeyCodes::AK_MEDIA_PREV_TRACK;
		fromGingaToAwesomium[CodeMap::KEY_STOP]              = KeyCodes::AK_MEDIA_STOP;
		fromGingaToAwesomium[CodeMap::KEY_EJECT]             = KeyCodes::AK_UNKNOWN;
		fromGingaToAwesomium[CodeMap::KEY_PLAY]              = KeyCodes::AK_MEDIA_PLAY_PAUSE;
		fromGingaToAwesomium[CodeMap::KEY_RECORD]            = KeyCodes::AK_UNKNOWN;
		fromGingaToAwesomium[CodeMap::KEY_PAUSE]             = KeyCodes::AK_MEDIA_PLAY_PAUSE;

		fromGingaToAwesomium[CodeMap::KEY_GREATER_THAN_SIGN] = KeyCodes::AK_UNKNOWN;
		fromGingaToAwesomium[CodeMap::KEY_LESS_THAN_SIGN]    = KeyCodes::AK_UNKNOWN;

		fromGingaToAwesomium[CodeMap::KEY_TAB]               = KeyCodes::AK_TAB;
		fromGingaToAwesomium[CodeMap::KEY_TAP]               = KeyCodes::AK_RETURN;
	}

	bool AwesomiumHandler::getAwesomeInfo(
			AwesomiumHDR id, AwesomiumInfo** aInfo, bool removeInfo) {

		map<AwesomiumHDR, AwesomiumInfo*>::iterator i;
		bool hasInfo = false;

		i = s_infos.find(id);
		if (i != s_infos.end()) {
			*aInfo = i->second;
			hasInfo = true;

			if (removeInfo) {
				s_infos.erase(i);

				if (s_infos.empty()) {
					s_id = 0;
				}
			}
		}

		return hasInfo;
	}

	bool AwesomiumHandler::getAwesomeIM(
			AwesomiumHDR id, IInputManager** im, bool removeInfo) {

		map<AwesomiumHDR, IInputManager*>::iterator i;
		bool hasInfo = false;

		i = s_ims.find(id);
		if (i != s_ims.end()) {
			*im = i->second;
			hasInfo = true;

			if (removeInfo) {
				s_ims.erase(i);
			}
		}

		return hasInfo;
	}

	AwesomiumHDR AwesomiumHandler::createAwesomium(GingaScreenID screenId) {
		AwesomiumInfo* aInfo;
		IInputManager* im;

		if (dm == NULL) {
			pthread_mutex_init(&s_lMutex, NULL);

#if HAVE_COMPSUPPORT
			dm = ((LocalScreenManagerCreator*)(
				cm->getObject("LocalScreenManager")))();

#else
			dm = LocalScreenManager::getInstance();
#endif
		}

		im = dm->getInputManager(screenId);

		s_id++;
		aInfo = new AwesomiumInfo(screenId, s_id);

		aInfo->surface = dm->createSurface(screenId);

		s_infos[s_id]  = aInfo;
		s_ims[s_id]    = im;

		return s_id;
	}

	void AwesomiumHandler::destroyAwesomium(AwesomiumHDR id) {
		AwesomiumInfo* aInfo;
		IInputManager* im;

		if (getAwesomeInfo(id, &aInfo, true)) {
			if (getAwesomeIM(id, &im, true)) {
				im->removeInputEventListener(aInfo);
			}
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

	void AwesomiumHandler::setAwesomiumBounds(
			AwesomiumHDR id, int x, int y, int w, int h) {

		AwesomiumInfo* aInfo;

		if (getAwesomeInfo(id, &aInfo)) {
			aInfo->x = x;
			aInfo->y = y;
			aInfo->w = w;
			aInfo->h = h;
		}
	}

	void AwesomiumHandler::loadUrl(AwesomiumHDR id, string url) {
		AwesomiumInfo* aInfo;

		pthread_mutex_lock(&s_lMutex);
		if (webView != NULL) {
			pthread_mutex_unlock(&s_lMutex);
			return;
		}

		if (getAwesomeInfo(id, &aInfo)) {
			aInfo->mURL   = url;
			aInfo->update = true;

			if (webCore == NULL) {
				if (fromGingaToAwesomium.empty()) {
					initCodeMap();
				}
				Awesomium::WebCoreConfig config;
		        config.setEnablePlugins(true);
		        config.setSaveCacheAndCookies(true);
		        config.setEnableJavascript(true);
		        config.setForceSingleProcess(false);

				config.setCustomCSS("::-webkit-scrollbar { display: none; }");

				webCore = new WebCore(config);
			}

			if (webView == NULL) {
				if (aInfo->w <= 0 || aInfo->h <= 0) {
					if (aInfo->surface->getParent() != NULL) {
						aInfo->x = ((IWindow*)(
								aInfo->surface->getParent()))->getX();

						aInfo->y = ((IWindow*)(
								aInfo->surface->getParent()))->getY();

						aInfo->w = ((IWindow*)(
								aInfo->surface->getParent()))->getW();

						aInfo->h = ((IWindow*)(
								aInfo->surface->getParent()))->getH();

						clog << "Window coords: '";
						clog << aInfo->x << ", ";
						clog << aInfo->y << ", ";
						clog << aInfo->w << ", ";
						clog << aInfo->h << "' ";
						clog << endl;
					}

					if (aInfo->w <= 0 || aInfo->h <= 0) {
						pthread_mutex_unlock(&s_lMutex);
						return;
					}
				}

				clog << "Creating WebView w = '" << aInfo->w << "' and h = '";
				clog << aInfo->h << "'";
				clog << endl;

				webView = webCore->createWebView(aInfo->w, aInfo->h);
			}

			if (aInfo->rFile == "") {
				aInfo->rFile = "/tmp/awesomium" + itos(
						(long)(void*)aInfo) + ".png";
			}

			if (webView != NULL) {
				clog << "AwesomiumHandler::loadUrl call loadUrl" << endl;

				if (fileExists(aInfo->mURL)) {
					string base, file;

					if (aInfo->mURL.substr(0, 1) == SystemCompat::getIUriD()) {
						base = aInfo->mURL.substr(
								0, aInfo->mURL.find_last_of(
										SystemCompat::getIUriD()));

						file = aInfo->mURL.substr(
								aInfo->mURL.find_last_of(
										SystemCompat::getIUriD()) + 1,

								aInfo->mURL.length() - (
										aInfo->mURL.find_last_of(
												SystemCompat::getIUriD()) + 1));

					} else {
						base = SystemCompat::getUserCurrentPath();
						file = aInfo->mURL;
					}

					webCore->setBaseDirectory(base);
					webView->loadFile(file);

				} else {
					if (aInfo->mURL.find("://") == std::string::npos) {
						aInfo->mURL = "http://" + aInfo->mURL;
					}
					webView->loadURL(aInfo->mURL);
				}

				setFocus(aInfo);

				clog << "AwesomiumHandler::loadUrl call while isloading '";
				clog << aInfo->mURL << "'" << endl;

				while (webView->isLoadingPage()) {
					update(aInfo, 50);
				}

				clog << "AwesomiumHandler::loadUrl '";
				clog << aInfo->mURL << "' is loaded" << endl;
				update(aInfo, 300);

				clog << "AwesomiumHandler::loadUrl call refresh" << endl;
				refresh(id);

				clog << "AwesomiumHandler::loadUrl refresh OK" << endl;

				while (aInfo->update) {
					update(aInfo, 25);
					setFocus(aInfo);
					if (webView->isDirty()) {
						refresh(id);
					}

					AwesomiumHandler::eventHandler(aInfo);
				}

				destroyAwesomium(aInfo->id);

				clog << "AwesomiumHandler::loadUrl call destroy" << endl;
				if (webView != NULL) {
					webView->destroy();
					webView = NULL;
				}
			}
		}
		pthread_mutex_unlock(&s_lMutex);
	}

	void AwesomiumHandler::eventHandler(AwesomiumInfo* aInfo) {
		map<int, int>::iterator i;
		Awesomium::WebKeyboardEvent keyEvent;
		unsigned char key;
		IInputManager* im;

		short eventType  = aInfo->eventType;
		int keyCode      = aInfo->eventCode;

		aInfo->eventType = AwesomiumInfo::ET_NONE;
		aInfo->eventUsed();

		if (!aInfo->hasFocus) {
			return;
		}

		switch (eventType) {
			case AwesomiumInfo::ET_NONE:
				int tmpX, tmpY;
				if (getAwesomeIM(aInfo->id, &im, false)) {
					tmpX = im->getCurrentXAxisValue() - aInfo->x;
					tmpY = im->getCurrentYAxisValue() - aInfo->y;

					if (tmpX != aInfo->mouseX || tmpY != aInfo->mouseY) {
						aInfo->mouseX = tmpX;
						aInfo->mouseY = tmpY;

						webView->injectMouseMove(aInfo->mouseX, aInfo->mouseY);
					}
				}
				break;

			case AwesomiumInfo::ET_KEY:
				i = fromGingaToAwesomium.find(keyCode);
				if (i != fromGingaToAwesomium.end()) {
					key = i->second;

					if ((keyCode >= CodeMap::KEY_0 &&
							keyCode <= CodeMap::KEY_CAPITAL_Z) ||
							keyCode == CodeMap::KEY_SPACE) {

						if (keyCode >= CodeMap::KEY_SMALL_A &&
								keyCode <= CodeMap::KEY_SMALL_Z) {

							key  = key + ('a' - 'A');
						}
						keyEvent.type = Awesomium::WebKeyboardEvent::TYPE_CHAR;
						keyEvent.text[0]           = key;
						keyEvent.unmodifiedText[0] = key;
						keyEvent.virtualKeyCode    = key;
						keyEvent.nativeKeyCode     = key;
						webView->injectKeyboardEvent(keyEvent);

					} else {
						injectKey(aInfo, i->second);
			        }
				}
				break;

			case AwesomiumInfo::ET_BUTTON:
				if (getAwesomeIM(aInfo->id, &im, false)) {
					aInfo->mouseX = im->getCurrentXAxisValue() - aInfo->x;
					aInfo->mouseY = im->getCurrentYAxisValue() - aInfo->y;

					clog << "AwesomiumHandler::eventHandler MOUSE CLICK on '";
					clog << im->getCurrentXAxisValue() << ", ";
					clog << im->getCurrentYAxisValue() << "' window = '";
					clog << aInfo->x << ", ";
					clog << aInfo->y << ", ";
					clog << aInfo->w << ", ";
					clog << aInfo->h << "' Calculated click on '";
					clog << aInfo->mouseX;
					clog << "," << aInfo->mouseY << "'";
					clog << endl;

					webView->injectMouseMove(aInfo->mouseX, aInfo->mouseY);
					update(aInfo, 25);
					webView->injectMouseDown(Awesomium::LEFT_MOUSE_BTN);
					update(aInfo, 25);
					webView->injectMouseUp(Awesomium::LEFT_MOUSE_BTN);
				}
				break;
		}
	}

	void AwesomiumHandler::injectKey(AwesomiumInfo* aInfo, int keyCode) {
		char* buf;
		Awesomium::WebKeyboardEvent keyEv;

		buf = new char[20];
		keyEv.virtualKeyCode = keyCode;
		Awesomium::getKeyIdentifierFromVirtualKeyCode(
				keyEv.virtualKeyCode, &buf);

		strcpy(keyEv.keyIdentifier, buf);
		delete[] buf;

		keyEv.modifiers     = 0;
		keyEv.nativeKeyCode = 0;
		keyEv.type          = Awesomium::WebKeyboardEvent::TYPE_KEY_DOWN;

		webView->injectKeyboardEvent(keyEv);
		update(aInfo, 25);
		keyEv.type = Awesomium::WebKeyboardEvent::TYPE_KEY_UP;

		webView->injectKeyboardEvent(keyEv);
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
		IInputManager* im;

		if (aInfo->setFocus && !aInfo->hasFocus) {
			aInfo->hasFocus = true;
			if (getAwesomeIM(aInfo->id, &im, false)) {
				im->addInputEventListener(aInfo, NULL);
			}
			if (webView != NULL) {
				clog << "AwesomiumHandler::setFocus focus" << endl;
				webView->focus();
			}

		} else if (!aInfo->setFocus && aInfo->hasFocus) {
			aInfo->hasFocus = false;
			if (getAwesomeIM(aInfo->id, &im, false)) {
				im->removeInputEventListener(aInfo);
			}
			if (webView != NULL) {
				clog << "AwesomiumHandler::setFocus unfocus" << endl;
				webView->unfocus();
			}
		}
	}

	void AwesomiumHandler::refresh(AwesomiumHDR id) {
		IWindow* win;
		AwesomiumInfo* aInfo;

		if (getAwesomeInfo(id, &aInfo)) {
			const RenderBuffer* renderBuffer = webView->render();

			if (renderBuffer != NULL) {
				wstring pngFile(aInfo->rFile.length(), L' ');

				std::copy(
						aInfo->rFile.begin(),
						aInfo->rFile.end(),
						pngFile.begin());

				renderBuffer->saveToPNG(pngFile, true);

				win = (IWindow*)aInfo->surface->getParent();
				if (win != NULL) {
					win->renderImgFile(aInfo->rFile);
					//win->validate();
				}

				remove(aInfo->rFile.c_str());
			}
		}
	}

	void AwesomiumHandler::update(AwesomiumInfo* aInfo, double value) {
		SystemCompat::uSleep(value * 1000);
		webCore->update();
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
