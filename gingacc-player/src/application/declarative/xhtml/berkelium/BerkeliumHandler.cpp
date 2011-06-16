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

#ifdef __cplusplus
extern "C" {
#endif
#include <directfb.h>
#ifdef __cplusplus
}
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	IInputManager* BerkeliumHandler::im = NULL;

	BerkeliumHandler::BerkeliumHandler(string mrl) {
		mURL = mrl;

#if HAVE_COMPSUPPORT
		dm = ((LocalDeviceManagerCreator*)(cm->getObject(
				"LocalDeviceManager")))();

		if (im == NULL) {
			im = ((InputManagerCreator*)(cm->getObject("InputManager")))();
		}
#else
		dm = LocalDeviceManager::getInstance();
		im = InputManager::getInstance();
#endif
	}

	BerkeliumHandler::~BerkeliumHandler() {
		cout << "BerkeliumHandler::~BerkeliumHandler " << endl;
		im->removeInputEventListener(this);
	}

	bool BerkeliumHandler::userEventReceived(IInputEvent* userEvent) {
		cout << "BerkeliumHandler::userEventReceived " << endl;
		//browserReceiveEvent(mBrowser, (void*)(userEvent->getContent()));
		return true;
	}

	void BerkeliumHandler::onAddressBarChanged(Window *win, URLString newURL) {
        std::string x = "hi";
        x+= newURL;
        mURL = newURL.get<std::string>();
        std::cout << "*** onAddressChanged to "<<newURL<<std::endl;
	}

	void BerkeliumHandler::onStartLoading(Window *win, URLString newURL) {
		std::cout << "*** Start loading "<<newURL<<" from "<<mURL<<std::endl;
	}

	void BerkeliumHandler::onLoadingStateChanged(Window *win, bool isLoading) {
		std::cout << "*** Loading state changed "<<mURL<<" to "<<(isLoading?"loading":"stopped")<<std::endl;
	}

	void BerkeliumHandler::onLoad(Window *win) {
        sleep(1);
        win->resize(1280,1024);
        win->resize(500,400);
        win->resize(600,500);
        win->resize(1024,768);
	}

	void BerkeliumHandler::onLoadError(Window *win, WideString error) {
        std::cout << L"*** onLoadError "<<mURL<<": ";
        std::wcout << error<<std::endl;
	}

	void BerkeliumHandler::onResponsive(Window *win) {
		std::cout << "*** onResponsive "<<mURL<<std::endl;
	}

	void BerkeliumHandler::onUnresponsive(Window *win) {
		std::cout << "*** onUnresponsive "<<mURL<<std::endl;
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

		cout << "*** onPaint "<< mURL << endl;

		FILE *outfile;
		{
			std::ostringstream os;
			os << "/tmp/chromium_render_" << time(NULL) << "_" << (call_count++) << ".ppm";
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

		IDirectFB* dfb = NULL;
		IDirectFBImageProvider* provider;
		IDirectFBSurface* destination;
		DFBSurfaceDescription sDesc;
		DFBDataBufferDescription desc;

		IDirectFBWindow* win;
		IDirectFBSurface* winSur;
		DFBWindowDescription dsc;

		dsc.flags  = (DFBWindowDescriptionFlags)(
				DWDESC_POSX |
				DWDESC_POSY |
				DWDESC_WIDTH |
				DWDESC_HEIGHT);

		dsc.posx   = 0;
		dsc.posy   = 0;
		dsc.width  = width;
		dsc.height = height;

		win = (IDirectFBWindow*)(dm->createWindow(&dsc));

		win->SetOpacity(win, 0xFF);
		win->GetSurface(win, &winSur);

		dfb = (IDirectFB*)(dm->getGfxRoot());

		dfb->CreateImageProvider(dfb, str.c_str(), &provider);
		provider->GetSurfaceDescription(provider, &sDesc);
		destination = (IDirectFBSurface*)(dm->createSurface(&sDesc));

		provider->RenderTo(provider, destination, NULL);

		winSur->Blit(winSur, destination, NULL, 0, 0);
		winSur->Flip(winSur, NULL, (DFBSurfaceFlipFlags)0);
	}

	void BerkeliumHandler::onCrashed(Window *win) {
		std::cout << "*** onCrashed "<<mURL<<std::endl;
	}

	void BerkeliumHandler::onCreatedWindow(
			Window *win, Window *newWindow, const Rect &initialRect) {

        std::cout << "*** onCreatedWindow from source "<<mURL<<std::endl;
        //newWindow->setDelegate(new BerkeliumHandler);
	}

	void BerkeliumHandler::onExternalHost(
			Window *win,
			WideString message,
			URLString origin,
			URLString target) {

        std::cout << "*** onChromeSend at URL "<<mURL<<" from "<<origin<<" to "<<target<<":"<<std::endl;
        std::wcout << message<<std::endl;
	}

	void BerkeliumHandler::onPaintPluginTexture(
			Window *win,
			void* sourceGLTexture,
			const std::vector<Rect> srcRects,
			const Rect &destRect) {

	}

	void BerkeliumHandler::onWidgetCreated(
			Window *win, Widget *newWidget, int zIndex) {

	}

	void BerkeliumHandler::onWidgetDestroyed(Window *win, Widget *newWidget) {

	}

	void BerkeliumHandler::onWidgetResize(
			Window *win, Widget *wid, int newWidth, int newHeight) {

	}

	void BerkeliumHandler::onWidgetMove(
			Window *win, Widget *wid, int newX, int newY) {

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

	}
}
}
}
}
}
}
