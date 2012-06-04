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

#include "player/BerkeliumHandler.h"
#include "player/PlayersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {

	map<int, int> BerkeliumHandler::fromGingaToBklm;

	BerkeliumHandler::BerkeliumHandler(
			GingaScreenID myScreen, int x, int y, int w, int h) {

#if HAVE_COMPSUPPORT
		dm = ((LocalScreenManagerCreator*)(
				cm->getObject("LocalScreenManager")))();

#else
		dm = LocalScreenManager::getInstance();
#endif

		im = dm->getInputManager(myScreen);

		this->myScreen = myScreen;

		surface    = dm->createSurface(myScreen, w, h);
		xOffset    = x;
		yOffset    = y;
		this->w    = w;
		this->h    = h;
		mouseClick = false;
		mouseMoved = false;
		textEvent  = false;
		keyCode    = -1;
		isValid    = false;

		if (fromGingaToBklm.empty()) {
			initInputMap();
		}
	}

	BerkeliumHandler::~BerkeliumHandler() {
		cout << "BerkeliumHandler::~BerkeliumHandler " << endl;
		if (isValid) {
			isValid = false;
			bWindow->stop();
			bWindow->setDelegate(NULL);
			bWindow->del();
		}

		if (context != NULL) {
			context->destroy();
			context = NULL;
		}

		if (im != NULL) {
			im->removeInputEventListener(this);
			im = NULL;
		}
		//Caution: Surface is deleted by Player
	}

	void BerkeliumHandler::initInputMap() {
		fromGingaToBklm[CodeMap::KEY_0]                 = '0';
		fromGingaToBklm[CodeMap::KEY_1]                 = '1';
		fromGingaToBklm[CodeMap::KEY_2]                 = '2';
		fromGingaToBklm[CodeMap::KEY_3]                 = '3';
		fromGingaToBklm[CodeMap::KEY_4]                 = '4';
		fromGingaToBklm[CodeMap::KEY_5]                 = '5';
		fromGingaToBklm[CodeMap::KEY_6]                 = '6';
		fromGingaToBklm[CodeMap::KEY_7]                 = '7';
		fromGingaToBklm[CodeMap::KEY_8]                 = '8';
		fromGingaToBklm[CodeMap::KEY_9]                 = '9';

		fromGingaToBklm[CodeMap::KEY_SMALL_A]           = 'a';
		fromGingaToBklm[CodeMap::KEY_SMALL_B]           = 'b';
		fromGingaToBklm[CodeMap::KEY_SMALL_C]           = 'c';
		fromGingaToBklm[CodeMap::KEY_SMALL_D]           = 'd';
		fromGingaToBklm[CodeMap::KEY_SMALL_E]           = 'e';
		fromGingaToBklm[CodeMap::KEY_SMALL_F]           = 'f';
		fromGingaToBklm[CodeMap::KEY_SMALL_G]           = 'g';
		fromGingaToBklm[CodeMap::KEY_SMALL_H]           = 'h';
		fromGingaToBklm[CodeMap::KEY_SMALL_I]           = 'i';
		fromGingaToBklm[CodeMap::KEY_SMALL_J]           = 'j';
		fromGingaToBklm[CodeMap::KEY_SMALL_K]           = 'k';
		fromGingaToBklm[CodeMap::KEY_SMALL_L]           = 'l';
		fromGingaToBklm[CodeMap::KEY_SMALL_M]           = 'm';
		fromGingaToBklm[CodeMap::KEY_SMALL_N]           = 'n';
		fromGingaToBklm[CodeMap::KEY_SMALL_O]           = 'o';
		fromGingaToBklm[CodeMap::KEY_SMALL_P]           = 'p';
		fromGingaToBklm[CodeMap::KEY_SMALL_Q]           = 'q';
		fromGingaToBklm[CodeMap::KEY_SMALL_R]           = 'r';
		fromGingaToBklm[CodeMap::KEY_SMALL_S]           = 's';
		fromGingaToBklm[CodeMap::KEY_SMALL_T]           = 't';
		fromGingaToBklm[CodeMap::KEY_SMALL_U]           = 'u';
		fromGingaToBklm[CodeMap::KEY_SMALL_V]           = 'v';
		fromGingaToBklm[CodeMap::KEY_SMALL_W]           = 'w';
		fromGingaToBklm[CodeMap::KEY_SMALL_X]           = 'x';
		fromGingaToBklm[CodeMap::KEY_SMALL_Y]           = 'y';
		fromGingaToBklm[CodeMap::KEY_SMALL_Z]           = 'z';

		fromGingaToBklm[CodeMap::KEY_CAPITAL_A]         = 'A';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_B]         = 'B';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_C]         = 'C';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_D]         = 'D';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_E]         = 'E';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_F]         = 'F';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_G]         = 'G';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_H]         = 'H';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_I]         = 'I';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_J]         = 'J';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_K]         = 'K';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_L]         = 'L';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_M]         = 'M';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_N]         = 'N';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_O]         = 'O';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_P]         = 'P';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_Q]         = 'Q';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_R]         = 'R';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_S]         = 'S';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_T]         = 'T';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_U]         = 'U';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_V]         = 'V';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_W]         = 'W';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_X]         = 'X';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_Y]         = 'Y';
		fromGingaToBklm[CodeMap::KEY_CAPITAL_Z]         = 'Z';

		fromGingaToBklm[CodeMap::KEY_SPACE]             = ' ';
		fromGingaToBklm[CodeMap::KEY_BACKSPACE]         = '\b';
		fromGingaToBklm[CodeMap::KEY_BACK]              = '\b';
		fromGingaToBklm[CodeMap::KEY_ESCAPE]            = 27;
		fromGingaToBklm[CodeMap::KEY_EXIT]              = 27;
		fromGingaToBklm[CodeMap::KEY_ENTER]             = '\n';


		fromGingaToBklm[CodeMap::KEY_GREATER_THAN_SIGN] = 'UNKNOWN';
		fromGingaToBklm[CodeMap::KEY_LESS_THAN_SIGN]    = 'UNKNOWN';

		fromGingaToBklm[CodeMap::KEY_TAB]               = 9;
		fromGingaToBklm[CodeMap::KEY_TAP]               = '\n';
	}

	void BerkeliumHandler::setKeyHandler(bool handler) {
		if (im != NULL && isValid) {
			if (handler) {
				bWindow->focus();

				im->addInputEventListener(this, NULL);
				im->addMotionEventListener(this);

			} else {
				bWindow->unfocus();
				im->removeInputEventListener(this);
				im->removeMotionEventListener(this);
			}
		}
	}

	void BerkeliumHandler::setContext(Context* context) {
		this->context = context;
	}

	void BerkeliumHandler::setWindow(std::auto_ptr<Window> window) {
		bWindow = window;
		isValid = true;
	}

	void BerkeliumHandler::getSize(int* w, int* h) {
		*w = this->w;
		*h = this->h;
	}

	void BerkeliumHandler::setBounds(int x, int y, int w, int h) {
		xOffset = x;
		yOffset = y;
		this->w = w;
		this->h = h;

		if (isValid) {
			bWindow->resize(w, h);
		}
	}

	void BerkeliumHandler::setUrl(string url) {
		mURL = url;
	}

	string BerkeliumHandler::getUrl() {
		return mURL;
	}

	ISurface* BerkeliumHandler::getSurface() {
		return surface;
	}

	void BerkeliumHandler::updateEvents() {
		if (isValid) {
			if (mouseMoved) {
				bWindow->mouseMoved(x, y);
				cout << "BerkeliumHandler::updateEvents ";
				cout << "mouse moved to (" << x << ", " << y << ")";
				cout << endl;
				mouseMoved = false;
			}

			if (mouseClick) {
				bWindow->mouseMoved(x, y);
				bWindow->mouseButton(0, true);
				bWindow->mouseButton(0, false);
				cout << "BerkeliumHandler::updateEvents ";
				cout << "mouse click on (" << x << ", " << y << ")";
				cout << endl;
				mouseClick = false;
			}

			if (textEvent) {
				bool specialKey = false;

				if (keyCode == CodeMap::KEY_CURSOR_LEFT) {
					bWindow->mouseWheel(20, 0);
					keyCode = fromGingaToBklm[CodeMap::KEY_BACKSPACE];

				} else if (keyCode == CodeMap::KEY_CURSOR_RIGHT) {
					bWindow->mouseWheel(-20, 0);
					specialKey = true;

				} else if (keyCode == CodeMap::KEY_CURSOR_UP) {
					bWindow->mouseWheel(0, 20);
					specialKey = true;

				} else if (keyCode == CodeMap::KEY_CURSOR_DOWN) {
					bWindow->mouseWheel(0, -20);
					specialKey = true;
				}

				string txt = "";
				wchar_t outchars[2];
				outchars[0] = keyCode;
				outchars[1] = 0;

				bWindow->keyEvent(true, 0, keyCode, 0);
				if (!specialKey) {
					bWindow->textEvent(outchars, 1);
				}
				bWindow->keyEvent(false, 0, keyCode, 0);

				cout << "BerkeliumHandler::updateEvents ";
				cout << "text event '" << (char)keyCode;
				cout << "' on (" << x << ", " << y << ")";
				cout << endl;

				textEvent = false;
			}
		}
	}

	bool BerkeliumHandler::userEventReceived(IInputEvent* userEvent) {
		map<int, int>::iterator i;

		cout << "BerkeliumHandler::userEventReceived " << endl;

		//browserReceiveEvent(mBrowser, (void*)(userEvent->getContent()));

		if (userEvent->getKeyCode(myScreen) == CodeMap::KEY_QUIT) {
			im = NULL;

		} else if (userEvent->isButtonPressType()) {
			/*if (isValid) {
				bWindow->mouseButton(1, true);
			}*/
			mouseClick = true;

		} else if (userEvent->isKeyType()) {
			keyCode   = userEvent->getKeyCode(myScreen);
			i = fromGingaToBklm.find(keyCode);
			if (i != fromGingaToBklm.end()) {
				keyCode = i->second;
			}

			textEvent = true;
		}

		return true;
	}

	bool BerkeliumHandler::motionEventReceived(int x, int y, int z) {
		clog << "BerkeliumHandler::motionEventReceived " << endl;

		this->x = x - xOffset;
		this->y = y - yOffset;

		if (this->x < 0) {
			this->x = 0;

		} else if (this->x + 20 > this->w) {
			this->x = this->w - 20;
		}

		if (this->y < 0) {
			this->y = 0;

		} else if (this->y + 20 > this->h) {
			this->y = this->h - 20;
		}

		mouseMoved = true;

		return true;
	}

	void BerkeliumHandler::onAddressBarChanged(Window *win, URLString newURL) {
        std::string x = "hi";
        x+= newURL;
        mURL = newURL.get<std::string>();
        cout << "BerkeliumHandler::onAddressChanged to " << newURL << endl;
	}

	void BerkeliumHandler::onStartLoading(Window *win, URLString newURL) {
		cout << "BerkeliumHandler::Start loading " << newURL;
		cout << " from " << mURL << endl;

		wstring str_css(L"::-webkit-scrollbar { display: none; }");

		win->insertCSS(
				WideString::point_to(str_css.c_str()),
				WideString::empty());
	}

	void BerkeliumHandler::onLoadingStateChanged(Window *win, bool isLoading) {
		cout << "BerkeliumHandler::Loading state changed ";
		cout << mURL << " to " << (isLoading?"loading":"stopped") << endl;
	}

	void BerkeliumHandler::onLoad(Window *win) {
		wstring str_css(L"::-webkit-scrollbar { display: none; }");

		win->insertCSS(
				WideString::point_to(str_css.c_str()),
				WideString::empty());
	}

	void BerkeliumHandler::onLoadError(Window *win, WideString error) {
        cout << L"*** onLoadError " << mURL << ": ";
        cout << error << endl;
	}

	void BerkeliumHandler::onResponsive(Window *win) {
		cout << "BerkeliumHandler::onResponsive " << mURL << endl;
	}

	void BerkeliumHandler::onUnresponsive(Window *win) {
		cout << "BerkeliumHandler::onUnresponsive " << mURL << endl;
	}

	void BerkeliumHandler::onPaint(
			Window *wini,
			const unsigned char *bitmap_in,
			const Rect &bitmap_rect,
			size_t num_copy_rects,
			const Rect *copy_rects,
			int dx,
			int dy,
			const Rect &scroll_rect) {

		string str;
		static int call_count = 0;
		IWindow* win;
		IImageProvider* img;
		ISurface* s;

		int left, top, right, bottom;

		clog << "BerkeliumHandler::onPaint " << mURL << endl;

		FILE *outfile;
		{
			std::ostringstream os;
			os << "/tmp/bh_r_" << time(NULL) << "_" << (call_count++) << ".ppm";
			str = os.str();
			outfile = fopen(str.c_str(), "wb");
		}

		const int width = bitmap_rect.width();
		const int height = bitmap_rect.height();

		fprintf(outfile, "P6 %d %d 255\n", width, height);
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				unsigned char r,g,b,a;

				b = *(bitmap_in++);
				g = *(bitmap_in++);
				r = *(bitmap_in++);
				a = *(bitmap_in++);
				fputc(r, outfile);  // Red
				//fputc(255-a, outfile);  // Alpha
				fputc(g, outfile);  // Green
				fputc(b, outfile);  // Blue
				//(pixel >> 24) & 0xff;  // Alpha
			}
		}
		fclose(outfile);

		left   = bitmap_rect.left();
		top    = bitmap_rect.top();
		right  = bitmap_rect.right();
		bottom = bitmap_rect.bottom();

		clog << "BerkeliumHandler::onPaint '" << mURL << "'" << endl;
		clog << " left   = '" << left << "'" << endl;
		clog << " top    = '" << top << "'" << endl;
		clog << " right  = '" << right << "'" << endl;
		clog << " bottom = '" << bottom << "'" << endl;
		clog << " dx     = '" << dx << "'" << endl;
		clog << " dy     = '" << dy << "'" << endl;
		clog << endl;

		img = dm->createImageProvider(myScreen, str.c_str());
		s   = dm->createSurface(myScreen);

		img->playOver(s);
		surface->blit(left, top, s, 0, 0, right - left, bottom - top);

		delete s;
		delete img;

		clog << "BerkeliumHandler::onPaint all done" << endl;
	}

	void BerkeliumHandler::onCrashed(Window *win) {
		cout << "BerkeliumHandler::onCrashed " << mURL << endl;
	}

	void BerkeliumHandler::onCreatedWindow(
			Window *win, Window *newWindow, const Rect &initialRect) {

		cout << "BerkeliumHandler::onCreatedWindow from source ";
		cout << mURL << endl;
        //newWindow->setDelegate(new BerkeliumHandler);
	}

	void BerkeliumHandler::onExternalHost(
			Window *win,
			WideString message,
			URLString origin,
			URLString target) {

		cout << "BerkeliumHandler::onChromeSend at URL ";
		cout << mURL << " from " << origin;
		cout << " to " << target << ": ";
		cout << message << endl;
	}

	void BerkeliumHandler::onPaintPluginTexture(
			Window *win,
			void* sourceGLTexture,
			const std::vector<Rect> srcRects,
			const Rect &destRect) {

		cout << "BerkeliumHandler::onPaintPluginTexture from source ";
		cout << mURL << endl;

	}

	void BerkeliumHandler::onWidgetCreated(
			Window *win, Widget *newWidget, int zIndex) {

		cout << "BerkeliumHandler::onWidgetCreated from source " << mURL;
		cout << endl;
	}

	void BerkeliumHandler::onWidgetDestroyed(Window *win, Widget *newWidget) {
		cout << "BerkeliumHandler::onWidgetDestroyed from source ";
		cout << mURL << endl;
	}

	void BerkeliumHandler::onWidgetResize(
			Window *win, Widget *wid, int newWidth, int newHeight) {

		cout << "BerkeliumHandler::onWidgetResize from source " << mURL << endl;
	}

	void BerkeliumHandler::onWidgetMove(
			Window *win, Widget *wid, int newX, int newY) {

		cout << "BerkeliumHandler::onWidgetMove from source " << mURL << endl;
	}

	void BerkeliumHandler::onWidgetPaint(
			Window *win,
			Widget *wid,
			const unsigned char *sourceBuffer,
			const Rect &rect,
			size_t num_copy_rects,
			const Rect *copy_rects,
			int dx,
			int dy,
			const Rect &scrollRect) {

		cout << "BerkeliumHandler::onWidgetPaint from source " << mURL << endl;
	}
}
}
}
}
}
}
