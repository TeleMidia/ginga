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

#include "config.h"

#include "player/BerkeliumHandler.h"
#include "player/PlayersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {

	map<int, int> BerkeliumHandler::fromGingaToBklm;
	int BerkeliumHandler::callCount  = 0;

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

		scroll_buffer      = new unsigned char[w*(h+1)*4];
		needs_full_refresh = false;
	}

	BerkeliumHandler::~BerkeliumHandler() {
		clog << "BerkeliumHandler::~BerkeliumHandler " << endl;

		if (isValid) {
			isValid = false;

			setKeyHandler(false);

			bWindow->setDelegate(NULL);
/*
			clog << "BerkeliumHandler::~BerkeliumHandler deleting win" << endl;
			bWindow->del();

			clog << "BerkeliumHandler::~BerkeliumHandler destroying win";
			clog << endl;
			bWindow->destroy();
*/
		}

		if (context != NULL) {
			clog << "BerkeliumHandler::~BerkeliumHandler destroying context";
			clog << endl;
			context->destroy();
			context = NULL;
		}

		//Caution: Surface is deleted by Player

		clog << "BerkeliumHandler::~BerkeliumHandler all done" << endl;
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
		fromGingaToBklm[CodeMap::KEY_ENTER]             = '\r';
		fromGingaToBklm[CodeMap::KEY_OK]                = '\r';


		fromGingaToBklm[CodeMap::KEY_GREATER_THAN_SIGN] = '>';
		fromGingaToBklm[CodeMap::KEY_LESS_THAN_SIGN]    = '<';

		fromGingaToBklm[CodeMap::KEY_TAB]               = 9;
		fromGingaToBklm[CodeMap::KEY_TAP]               = '\n';
	}

	void BerkeliumHandler::stop() {
		if (isValid) {
			bWindow->mouseMoved(-1, -1);
			bWindow->mouseButton(0, true);
			bWindow->mouseButton(0, false);
		}

		clog << "BerkeliumHandler::stop all done" << endl;
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
				mouseMoved = false;
			}

			if (mouseClick) {
				bWindow->mouseMoved(x, y);
				bWindow->mouseButton(0, true);
				bWindow->mouseButton(0, false);
				clog << "BerkeliumHandler::updateEvents ";
				clog << "mouse click on (" << x << ", " << y << ")";
				clog << endl;
				mouseClick = false;
			}

			if (textEvent) {
				bool specialKey = false;
				int mods = 0;

				if (keyCode == CodeMap::KEY_CURSOR_LEFT) {
					keyCode = fromGingaToBklm[CodeMap::KEY_BACKSPACE];

				} else if (keyCode == CodeMap::KEY_CURSOR_DOWN) {
					keyCode = fromGingaToBklm[CodeMap::KEY_TAB];
					specialKey = true;

				} else if (keyCode == CodeMap::KEY_CURSOR_UP) {
					keyCode = fromGingaToBklm[CodeMap::KEY_TAB];
					mods    = Berkelium::SHIFT_MOD;
					specialKey = true;

				} else if (keyCode == CodeMap::KEY_CURSOR_RIGHT) {
					keyCode    = 0;
					specialKey = true;

				}

				string txt = "";
				wchar_t outchars[2];
				outchars[0] = keyCode;
				outchars[1] = 0;

				bWindow->keyEvent(true, mods, keyCode, 0);
				if (!specialKey) {
					bWindow->textEvent(outchars, 1);
				}

				bWindow->keyEvent(false, mods, keyCode, 0);

				clog << "BerkeliumHandler::updateEvents ";
				clog << "text event '" << (char)keyCode;
				clog << "' on (" << x << ", " << y << ")";
				clog << endl;

				textEvent = false;
			}
		}
	}

	bool BerkeliumHandler::userEventReceived(IInputEvent* userEvent) {
		map<int, int>::iterator i;

		clog << "BerkeliumHandler::userEventReceived " << endl;

		//browserReceiveEvent(mBrowser, (void*)(userEvent->getContent()));

		if (userEvent->getKeyCode(myScreen) == CodeMap::KEY_QUIT) {
			im = NULL;

		} else if (userEvent->isButtonPressType()) {
			/*if (isValid) {
				bWindow->mouseButton(1, true);
			}*/
			mouseClick = true;

		} else if (userEvent->isKeyType()) {
			keyCode = userEvent->getKeyCode(myScreen);

			i = fromGingaToBklm.find(keyCode);
			if (i != fromGingaToBklm.end()) {
				clog << "BerkeliumHandler::userEventReceived MB key = '";
				clog << keyCode << "' translated to '" << i->second << "'";
				clog << " (" << (char)i->second << ")";
				clog << endl;
				keyCode = i->second;
			}

			textEvent = true;
		}

		clog << "BerkeliumHandler::userEventReceived all done" << endl;
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
        clog << "BerkeliumHandler::onAddressChanged to " << newURL << endl;
	}

	void BerkeliumHandler::onStartLoading(Window *win, URLString newURL) {
		clog << "BerkeliumHandler::Start loading " << newURL;
		clog << " from " << mURL << endl;

		wstring str_css(L"::-webkit-scrollbar { display: none; }");
		wstring str_js(L"document.body.style.overflow='hidden'");

		win->insertCSS(
				WideString::point_to(str_css.c_str()),
				WideString::empty());

		win->executeJavascript(WideString::point_to(str_js.c_str()));
	}

	void BerkeliumHandler::onLoadingStateChanged(Window *win, bool isLoading) {
		clog << "BerkeliumHandler::Loading state changed ";
		clog << mURL << " to " << (isLoading?"loading":"stopped") << endl;
	}

	void BerkeliumHandler::onLoad(Window *win) {
		wstring str_css(L"::-webkit-scrollbar { display: none; }");
		wstring str_js(L"document.body.style.overflow='hidden'");

		win->insertCSS(
				WideString::point_to(str_css.c_str()),
				WideString::empty());

		win->executeJavascript(WideString::point_to(str_js.c_str()));
	}

	void BerkeliumHandler::onLoadError(Window *win, WideString error) {
        clog << L"*** onLoadError " << mURL << ": ";
        clog << error << endl;
	}

	void BerkeliumHandler::onResponsive(Window *win) {
		clog << "BerkeliumHandler::onResponsive " << mURL << endl;
	}

	void BerkeliumHandler::onUnresponsive(Window *win) {
		clog << "BerkeliumHandler::onUnresponsive " << mURL << endl;
	}

#define BERKELIUM_SCROLL_NEW 1

	bool BerkeliumHandler::mapOnPaintToTexture(
			Berkelium::Window *wini,
			const unsigned char* bitmap_in,
			const Berkelium::Rect& bitmap_rect,
			size_t num_copy_rects,
			const Berkelium::Rect *copy_rects,
			int dx, int dy,
			const Berkelium::Rect& scroll_rect,
			unsigned int dest_texture_width,
			unsigned int dest_texture_height,
			bool ignore_partial,
			unsigned char* scroll_buffer) {

		string strFile;
		ISurface* s;
		const int kBytesPerPixel = 4;

		// If we've reloaded the page and need a full update, ignore updates
		// until a full one comes in. This handles out of date updates due to
		// delays in event processing.

		if (ignore_partial) {
			if (bitmap_rect.left() != 0 ||
					bitmap_rect.top() != 0 ||
					bitmap_rect.right() != dest_texture_width ||
					bitmap_rect.bottom() != dest_texture_height) {

				return false;
			}

			strFile = createFile(
					bitmap_in, dest_texture_width, dest_texture_height);

			s = createRenderedSurface(strFile);
			remove(strFile.c_str());

			surface->blit(0, 0, s);
			delete s;

			if (surface->getParent() != NULL) {
				((IWindow*)(surface->getParent()))->validate();
			}

			ignore_partial = false;
			return true;
		}

		// Now, we first handle scrolling. We need to do this first since it
		// requires shifting existing data, some of which will be overwritten by
		// the regular dirty rect update.
		if (dx != 0 || dy != 0) {
			// scroll_rect contains the Rect we need to move
			// First we figure out where the the data is moved to by translating it
			Berkelium::Rect scrolled_rect = scroll_rect.translate(-dx, -dy);
			// Next we figure out where they intersect, giving the scrolled
			// region
			Berkelium::Rect scrolled_shared_rect = scroll_rect.intersect(scrolled_rect);
			// Only do scrolling if they have non-zero intersection
			if (scrolled_shared_rect.width() > 0 && scrolled_shared_rect.height() > 0) {
				// And the scroll is performed by moving shared_rect by (dx,dy)
				Berkelium::Rect shared_rect = scrolled_shared_rect.translate(dx, dy);

				int wid = scrolled_shared_rect.width();
				int hig = scrolled_shared_rect.height();
				int inc = 1;

#if BERKELIUM_SCROLL_NEW
				int top = scrolled_rect.top();
				int left = scrolled_rect.left();

				wid = scrolled_rect.width();
				hig = scrolled_rect.height();
				
				if(dx > 0) {
					surface->blit((-1)*scrolled_rect.left(), 
						(-1)*scrolled_rect.top(),
						surface,
						0,
						0,
						wid + scrolled_rect.left(),
						hig + scrolled_rect.top());
				}
				else if(dy > 0)
				{
					int surface_w, surface_h;
					surface->getSize(&surface_w, &surface_h);
					s = dm->createSurface(myScreen, w, h);

					/* I don't now why but it only work if I create
					a temporary surface. */
					s->blit(0, dy,
						surface,
						0, 0,
						wid, hig);

					surface->blit(0, dy,
						s,
						0, dy,
						wid, hig);

					delete s;
				}
				else {
					surface->blit(0, 0, surface,
						left, top, wid, hig);
				}

				if (surface->getParent() != NULL) {
					((IWindow*)(surface->getParent()))->validate();
				}
#else
				unsigned char *outputBuffer = scroll_buffer;
				// source data is offset by 1 line to prevent memcpy aliasing
				// In this case, it can happen if dy==0 and dx!=0.
				unsigned char *inputBuffer = scroll_buffer+(dest_texture_width*1*kBytesPerPixel);
				int jj = 0;
				if (dy > 0) {
					// Here, we need to shift the buffer around so that we start in the
					// extra row at the end, and then copy in reverse so that we
					// don't clobber source data before copying it.
					outputBuffer = scroll_buffer+(
							(scrolled_shared_rect.top()+hig+1)*dest_texture_width
							- hig*wid)*kBytesPerPixel;
					inputBuffer = scroll_buffer;
					inc = -1;
					jj = hig-1;
				}

				// Copy the data out of the texture
				strFile = createFile(
						(const unsigned char*)inputBuffer,
						wid, hig);

				s = createRenderedSurface(strFile);
				remove(strFile.c_str());
				surface->blit(0, 0, s);
				delete s;

				if (surface->getParent() != NULL) {
					((IWindow*)(surface->getParent()))->validate();
				}

				// copy out the region to the beginning of the buffer
				for(; jj < hig && jj >= 0; jj+=inc) {
					memcpy(
							outputBuffer + (jj*wid) * kBytesPerPixel,
							//scroll_buffer + (jj*wid * kBytesPerPixel),
							inputBuffer + (
									(scrolled_shared_rect.top()+jj)*dest_texture_width
									+ scrolled_shared_rect.left()) * kBytesPerPixel,
									wid*kBytesPerPixel
					);
				}

				// And finally, we push it back into the texture in the right
				// location
				strFile = createFile(
						(const unsigned char*)outputBuffer,
						wid, hig);

				s = createRenderedSurface(strFile);
				remove(strFile.c_str());

				surface->blit(
						shared_rect.left(), shared_rect.top(),
						s, 0, 0, shared_rect.width(), shared_rect.height());

				delete s;

				if (surface->getParent() != NULL) {
					((IWindow*)(surface->getParent()))->validate();
				}
#endif
			}
		}

		for (size_t i = 0; i < num_copy_rects; i++) {
			int wid = copy_rects[i].width();
			int hig = copy_rects[i].height();
			int top = copy_rects[i].top() - bitmap_rect.top();
			int left = copy_rects[i].left() - bitmap_rect.left();

#if BERKELIUM_SCROLL_NEW
			unsigned char *tmp_buffer = new unsigned char[wid*hig*kBytesPerPixel];
#endif
			for(int jj = 0; jj < hig; jj++) {
				memcpy(
#if BERKELIUM_SCROLL_NEW
						tmp_buffer + jj*wid*kBytesPerPixel,
#else
						scroll_buffer + jj*wid*kBytesPerPixel,
#endif
						bitmap_in + (left + (jj+top)*bitmap_rect.width())*kBytesPerPixel,
						wid*kBytesPerPixel);
			}

			// Finally, we perform the main update, just copying the rect that
			// is marked as dirty but not from scrolled data.
#if BERKELIUM_SCROLL_NEW
			strFile = createFile(
				(const unsigned char*) tmp_buffer, wid, hig);
#else
			strFile = createFile(
				(const unsigned char*)scroll_buffer, wid, hig);
#endif

			s = createRenderedSurface(strFile);
			// remove(strFile.c_str());

#if BERKELIUM_SCROLL_NEW
			delete [] tmp_buffer;
#endif
			left =  copy_rects[i].left();
			top = copy_rects[i].top();

			surface->blit(
					copy_rects[i].left(), copy_rects[i].top(),
					s, 0, 0, wid, hig);

			delete s;

			if (surface->getParent() != NULL) {
				((IWindow*)(surface->getParent()))->validate();
			}
		}

		return true;
	}

	ISurface* BerkeliumHandler::createRenderedSurface(string fileName) {
		ISurface* s;
		IImageProvider* img;

		img = dm->createImageProvider(myScreen, fileName.c_str());
		s   = dm->createSurface(myScreen);

		img->playOver(s);
		delete img;

		return s;
	}

	string BerkeliumHandler::createFile(
			const unsigned char *sourceBuffer,
			int width, int height) {

		string str;
		FILE *outfile;
		{
			std::ostringstream os;
			os << SystemCompat::getTemporaryDir() + "bh_r_" << time(NULL) << "_" << (callCount++) << ".ppm";
			str = os.str();
			outfile = fopen(str.c_str(), "wb");
		}

		if (width > 0 && height > 0) {
			fprintf(outfile, "P6 %d %d 255\n", width, height);
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x) {
					unsigned char r,g,b,a;

					b = *(sourceBuffer++);
					g = *(sourceBuffer++);
					r = *(sourceBuffer++);
					a = *(sourceBuffer++);
					fputc(r, outfile);  // Red
					//fputc(255-a, outfile);  // Alpha
					fputc(g, outfile);  // Green
					fputc(b, outfile);  // Blue
					//(pixel >> 24) & 0xff;  // Alpha
				}
			}
		}
		fclose(outfile);

		return str;
	}

	void BerkeliumHandler::onPaint(
			Window *wini,
			const unsigned char *sourceBuffer,
			const Rect &sourceBufferRect,
			size_t numCopyRects,
			const Rect *copyRects,
			int dx,
			int dy,
			const Rect &scrollRect) {

		bool updated = mapOnPaintToTexture(
				wini, sourceBuffer, sourceBufferRect, numCopyRects, copyRects,
				dx, dy, scrollRect,
				w, h, needs_full_refresh, scroll_buffer);

		if (updated) {
			needs_full_refresh = false;
		}

		clog << "BerkeliumHandler::onPaint all done" << endl;
	}

	void BerkeliumHandler::onCrashed(Window *win) {
		clog << "BerkeliumHandler::onCrashed " << mURL << endl;
	}

	void BerkeliumHandler::onCreatedWindow(
			Window *win, Window *newWindow, const Rect &initialRect) {

		clog << "BerkeliumHandler::onCreatedWindow from source ";
		clog << mURL << endl;
        //newWindow->setDelegate(new BerkeliumHandler);
	}

	void BerkeliumHandler::onExternalHost(
			Window *win,
			WideString message,
			URLString origin,
			URLString target) {

		clog << "BerkeliumHandler::onChromeSend at URL ";
		clog << mURL << " from " << origin;
		clog << " to " << target << ": ";
		clog << message << endl;
	}

	void BerkeliumHandler::onPaintPluginTexture(
			Window *win,
			void* sourceGLTexture,
			const std::vector<Rect> srcRects,
			const Rect &destRect) {

		clog << "BerkeliumHandler::onPaintPluginTexture from source ";
		clog << mURL << endl;

	}

	void BerkeliumHandler::onWidgetCreated(
			Window *win, Widget *newWidget, int zIndex) {

		clog << "BerkeliumHandler::onWidgetCreated from source " << mURL;
		clog << endl;
	}

	void BerkeliumHandler::onWidgetDestroyed(Window *win, Widget *newWidget) {
		clog << "BerkeliumHandler::onWidgetDestroyed from source ";
		clog << mURL << endl;
	}

	void BerkeliumHandler::onWidgetResize(
			Window *win, Widget *wid, int newWidth, int newHeight) {

		clog << "BerkeliumHandler::onWidgetResize from source " << mURL << endl;
	}

	void BerkeliumHandler::onWidgetMove(
			Window *win, Widget *wid, int newX, int newY) {

		clog << "BerkeliumHandler::onWidgetMove from source " << mURL << endl;
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

		clog << "BerkeliumHandler::onWidgetPaint from source " << mURL << endl;
	}
}
}
}
}
}
}
