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

	ILocalScreenManager* AwesomiumHandler::dm = NULL;
	AwesomiumHDR AwesomiumHandler::s_id       = 0;

	/* static since we have to respect browser isolation */
	bool AwesomiumHandler::initialized        = false;
	awe_webview* AwesomiumHandler::webView    = NULL;

	map<int, int> AwesomiumHandler::fromGingaToAwesomium;
	map<AwesomiumHDR, AwesomiumInfo*> AwesomiumHandler::s_infos;
	map<AwesomiumHDR, IInputManager*> AwesomiumHandler::s_ims;
	pthread_mutex_t AwesomiumHandler::s_lMutex;

	AwesomiumInfo::AwesomiumInfo(GingaScreenID screenId, AwesomiumHDR id) {
		LocalScreenManager::addListenerInstance(this);
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
		Thread::mutexInit(&_eM, NULL);
	}

	AwesomiumInfo::~AwesomiumInfo() {
		LocalScreenManager::removeListenerInstance(this);
		pthread_cond_signal(&_eMVar);
		pthread_cond_destroy(&_eMVar);
		pthread_mutex_destroy(&_eM);
	}

	void AwesomiumInfo::waitEvent() {
		_eMVarW = true;
		Thread::mutexLock(&_eM);
		pthread_cond_wait(&_eMVar, &_eM);

		_eMVarW = false;
		Thread::mutexUnlock(&_eM);
	}

	bool AwesomiumInfo::eventArrived() {
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

		eventArrived();
		return true;
	}

	bool AwesomiumInfo::motionEventReceived(int x, int y, int z) {
		int tmpX, tmpY;

		tmpX = x - this->x;
		tmpY = y - this->y;

		/*clog << "AwesomiumInfo::motionEventReceived (" << x << "," << y;
		clog << ") current mouse position (" << mouseX << "," << mouseY;
		clog << ") window position (";
		clog << this->x << "," << this->y << "," << this->w << "," << this->h;
		clog << ")" << endl;*/

		if (tmpX <= this->w && tmpY <= this->h &&
				(tmpX != mouseX || tmpY != mouseY)) {

			eventArrived();
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
			Thread::mutexInit(&s_lMutex, NULL);

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
				im->removeMotionEventListener(aInfo);
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

	void AwesomiumHandler::onDOMReady(awe_webview* caller) {
		clog << "AwesomiumHandler::onDOMReady" << endl;
	}

	void AwesomiumHandler::loadUrl(AwesomiumHDR id, string url) {
		AwesomiumInfo* aInfo;

		awe_string* awe_custom_css;
		awe_string* awe_base;
		awe_string* awe_file;

		string str_custom_css;

		Thread::mutexLock(&s_lMutex);
		if (webView != NULL) {
			Thread::mutexUnlock(&s_lMutex);
			return;
		}

		if (getAwesomeInfo(id, &aInfo)) {
			aInfo->mURL   = url;
			aInfo->update = true;

			if (!initialized) {
				initialized = true;

				if (fromGingaToAwesomium.empty()) {
					initCodeMap();
				}
				str_custom_css = "::-webkit-scrollbar { display: none; }";
				str_custom_css = str_custom_css +
						"body { font-size: 12px !important; }";

				awe_custom_css = awe_string_create_from_utf8(
						str_custom_css.c_str(), str_custom_css.length());

				awe_webcore_initialize(
						true,               //enable_plugins
						true,               //enable_javascript
						true,               //enable_databases
						awe_string_empty(), //package_path
						awe_string_empty(), //locale_path
						awe_string_empty(), //user_data_path
						awe_string_empty(), //plugin_path
						awe_string_empty(), //log_path
						AWE_LL_VERBOSE,     //log_level
						false,              //force_single_process
						awe_string_empty(), //child_process_path
						true,               //enable_auto_detect_encoding
						awe_string_empty(), //accept_language_override
						awe_string_empty(), //default_charset_override
						awe_string_empty(), //user_agent_override
						awe_string_empty(), //proxy_server
						awe_string_empty(), //proxy_config_script
						awe_string_empty(), //auth_server_whitelist
						true,               //save_cache_and_cookies
						0,                  //max_cache_size
						false,              //disable_same_origin_policy
						false,              //disable_win_message_pump
						awe_custom_css);

				awe_string_destroy(awe_custom_css);
			}

			if (webView == NULL) {
				if (aInfo->w <= 0 || aInfo->h <= 0) {
					if (aInfo->surface->getParentWindow() != NULL) {
						aInfo->x = ((IWindow*)(
								aInfo->surface->getParentWindow()))->getX();

						aInfo->y = ((IWindow*)(
								aInfo->surface->getParentWindow()))->getY();

						aInfo->w = ((IWindow*)(
								aInfo->surface->getParentWindow()))->getW();

						aInfo->h = ((IWindow*)(
								aInfo->surface->getParentWindow()))->getH();

						clog << "Window coords: '";
						clog << aInfo->x << ", ";
						clog << aInfo->y << ", ";
						clog << aInfo->w << ", ";
						clog << aInfo->h << "' ";
						clog << endl;
					}

					if (aInfo->w <= 0 || aInfo->h <= 0) {
						Thread::mutexUnlock(&s_lMutex);
						return;
					}
				}

				clog << "Creating WebView w = '" << aInfo->w << "' and h = '";
				clog << aInfo->h << "'";
				clog << endl;

				webView = awe_webcore_create_webview(aInfo->w, aInfo->h, false);
			}

			if (aInfo->rFile == "") {
				aInfo->rFile = SystemCompat::getGingaBinPath() + itos(
						(long)(void*)aInfo) + ".png";
			}

			if (webView != NULL) {
				/*awe_webview_set_callback_dom_ready(
						webView, &AwesomiumHandler::onDOMReady);

				awe_webview_activate_ime(webView, false);*/

				string exportAweData = ultostr(
						(unsigned long)dm->getScreenUnderlyingWindow(
								aInfo->myScreen));

				exportAweData = "AWE_EMB=" + exportAweData +
						"," + itos(aInfo->x) +
						"," + itos(aInfo->y) +
						"," + itos(aInfo->w) +
						"," + itos(aInfo->h);

				cout << "AwesomiumHandler::loadUrl putenv '";
				cout << exportAweData << "'" << endl;

				putenv((char*)exportAweData.c_str());

				clog << "AwesomiumHandler::loadUrl call loadUrl '";
				clog << aInfo->mURL << "'";
				clog << endl;

				if (fileExists(aInfo->mURL)) {
					string base, file;

					if (SystemCompat::isAbsolutePath(aInfo->mURL)) {
						base = SystemCompat::getPath(aInfo->mURL);

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

					awe_base = awe_string_create_from_utf8(
							base.c_str(), base.length());

					awe_webcore_set_base_directory(awe_base);

					awe_file = awe_string_create_from_utf8(
							file.c_str(), file.length());

					awe_webview_load_file(
							webView, awe_file, awe_string_empty());

					awe_string_destroy(awe_base);
					awe_string_destroy(awe_file);

				} else {
					if (aInfo->mURL.find("://") == std::string::npos) {
						aInfo->mURL = "http://" + aInfo->mURL;
					}

					awe_file = awe_string_create_from_utf8(
							aInfo->mURL.c_str(), aInfo->mURL.length());

					awe_webview_load_url(
							webView,
							awe_file,
							awe_string_empty(),
							awe_string_empty(),
							awe_string_empty());

					awe_string_destroy(awe_file);
				}

				clog << "AwesomiumHandler::loadUrl call while isloading '";
				clog << aInfo->mURL << "'" << endl;

				while (awe_webview_is_loading_page(webView)) {
					update(aInfo, 50);
				}

				clog << "AwesomiumHandler::loadUrl '";
				clog << aInfo->mURL << "' is loaded" << endl;
				update(aInfo, 300);

				clog << "AwesomiumHandler::loadUrl call refresh" << endl;
				refresh(id);

				clog << "AwesomiumHandler::loadUrl refresh OK" << endl;

				unfocus(aInfo);

				while (aInfo->update) {
					do {
						update(aInfo, 50);

					} while (awe_webview_is_loading_page(webView) &&
							aInfo->update);

					setFocus(aInfo);
					if (awe_webview_is_dirty(webView)) {
						refresh(id);
					}

					AwesomiumHandler::eventHandler(aInfo);
					//refresh(id);
					//aInfo->waitEvent();
				}

				destroyAwesomium(aInfo->id);

				/*
				 *
				 //XXX: TO CHECK
				 void awe_webview_set_callback_plugin_crashed(
                            awe_webview* webview,
                            void (*callback)(awe_webview* caller,
							                 const awe_string* plugin_name));
				 */

				clog << "AwesomiumHandler::loadUrl call destroy" << endl;
				if (webView != NULL) {
					awe_webview_destroy(webView);
					webView = NULL;
				}
			}
		}
		Thread::mutexUnlock(&s_lMutex);

		clog << "AwesomiumHandler::loadUrl all done";
	}

	void AwesomiumHandler::eventHandler(AwesomiumInfo* aInfo) {
		map<int, int>::iterator i;
		awe_webkeyboardevent keyEvent;
		unsigned char key;
		IInputManager* im;

		short eventType  = aInfo->eventType;
		int keyCode      = aInfo->eventCode;

		aInfo->eventType = AwesomiumInfo::ET_NONE;
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

						awe_webview_inject_mouse_move(
								webView, aInfo->mouseX, aInfo->mouseY);
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
			        }

					injectKey(aInfo, key);
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


					awe_webview_inject_mouse_move(
							webView, aInfo->mouseX, aInfo->mouseY);

					update(aInfo, 25);
					awe_webview_inject_mouse_down(webView, AWE_MB_LEFT);
					update(aInfo, 25);
					awe_webview_inject_mouse_up(webView, AWE_MB_LEFT);
				}
				break;
		}
	}

	void AwesomiumHandler::injectKey(AwesomiumInfo* aInfo, int keyCode) {
		char* buf;
		/*awe_webkeyboardevent keyEv;

		buf = new char[20];

		Awesomium::getKeyIdentifierFromVirtualKeyCode(KeyCode, &buf);
		strcpy(keyEv.keyIdentifier, buf);
		delete[] buf;

		keyEv.virtual_key_code = keyCode;
		keyEv.modifiers        = 0;
		keyEv.native_key_code  = keyCode;
		keyEv.type             = AWE_WKT_KEYDOWN;

		awe_webview_inject_keyboard_event(webView, keyEv);
		update(aInfo, 25);
		keyEv.type = AWE_WKT_KEYUP;
		awe_webview_inject_keyboard_event(webView, keyEv);*/

		awe_webkeyboardevent e;
		e.is_system_key      = false;
		e.modifiers          = 0;
		e.text[0]            = keyCode;
		e.text[1]            = 0;
		e.text[2]            = 0;
		e.text[3]            = 0;
		e.unmodified_text[0] = keyCode;
		e.unmodified_text[1] = 0;
		e.unmodified_text[2] = 0;
		e.unmodified_text[3] = 0;
		e.virtual_key_code   = keyCode;
		e.native_key_code    = keyCode;
		e.type               = AWE_WKT_KEYDOWN;
		awe_webview_inject_keyboard_event(webView, e);

		// Key Char
		e.unmodified_text[0] = 0;
		e.virtual_key_code   = 0;
		e.native_key_code    = 0;
		e.type               = AWE_WKT_CHAR;
		awe_webview_inject_keyboard_event(webView, e);

		// Key Up
		e.text[0]            = keyCode;
		e.unmodified_text[0] = keyCode;
		e.virtual_key_code   = keyCode;
		e.native_key_code    = keyCode;
		e.type               = AWE_WKT_KEYUP;
		awe_webview_inject_keyboard_event(webView, e);
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
			aInfo->eventArrived();
		}
	}

	void AwesomiumHandler::setFocus(AwesomiumInfo* aInfo) {
		if (aInfo->setFocus && !aInfo->hasFocus) {
			focus(aInfo);

		} else if (!aInfo->setFocus && aInfo->hasFocus) {
			unfocus(aInfo);
		}
	}

	void AwesomiumHandler::focus(AwesomiumInfo* aInfo) {
		IInputManager* im;

		aInfo->hasFocus = true;
		if (getAwesomeIM(aInfo->id, &im, false)) {
			im->addInputEventListener(aInfo, NULL);
			//im->addMotionEventListener(aInfo);
		}
		if (webView != NULL) {
			clog << "AwesomiumHandler::focus" << endl;
			awe_webview_focus(webView);
			awe_webview_resume_rendering(webView);
		}
	}

	void AwesomiumHandler::unfocus(AwesomiumInfo* aInfo) {
		IInputManager* im;

		aInfo->hasFocus = false;
		if (getAwesomeIM(aInfo->id, &im, false)) {
			im->removeInputEventListener(aInfo);
			im->removeMotionEventListener(aInfo);
		}

		if (webView != NULL) {
			clog << "AwesomiumHandler::unfocus" << endl;
			awe_webview_unfocus(webView);
			awe_webview_pause_rendering(webView);
		}
	}

	void AwesomiumHandler::refresh(AwesomiumHDR id) {
		IWindow* win;
		AwesomiumInfo* aInfo;
		awe_string* awe_file;

		if (getAwesomeInfo(id, &aInfo)) {
			const awe_renderbuffer* renderBuffer = awe_webview_render(webView);

			if (renderBuffer != NULL) {
				awe_file = awe_string_create_from_utf8(
						aInfo->rFile.c_str(), aInfo->rFile.length());

				awe_renderbuffer_save_to_png(renderBuffer, awe_file, true);

				win = (IWindow*)aInfo->surface->getParentWindow();
				if (win != NULL) {
					win->renderImgFile(aInfo->rFile);
					//win->validate();
				}

				awe_string_destroy(awe_file);
				remove(aInfo->rFile.c_str());
			}
		}
	}

	void AwesomiumHandler::update(AwesomiumInfo* aInfo, double value) {
		SystemCompat::uSleep(value * 1000);
		awe_webcore_update();
	}

	bool AwesomiumHandler::stopUpdate(AwesomiumHDR id) {
		AwesomiumInfo* aInfo;

		if (getAwesomeInfo(id, &aInfo)) {
			if (aInfo->update) {
				aInfo->update = false;
				aInfo->eventArrived();

				if (webView != NULL) {
					return true;
				}
			}
		}

		return false;
	}
}
}
}
}
}
}
