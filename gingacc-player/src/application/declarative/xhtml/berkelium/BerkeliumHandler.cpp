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
		mURL    = mrl;

#if HAVE_COMPSUPPORT
		dm = ((LocalDeviceManagerCreator*)(cm->getObject(
				"LocalDeviceManager")))();

		if (im == NULL) {
			im = ((InputManagerCreator*)(cm->getObject("InputManager")))();
		}

		surface = ((SurfaceCreator*)(cm->getObject("Surface")))(NULL, 0, 0);
#else
		dm = LocalDeviceManager::getInstance();
		im = InputManager::getInstance();
		surface = new DFBSurface(NULL);
#endif
	}

	BerkeliumHandler::~BerkeliumHandler() {
		cout << "BerkeliumHandler::~BerkeliumHandler " << endl;
		im->removeInputEventListener(this);
	}

	ISurface* BerkeliumHandler::getSurface() {
		return surface;
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
		cout << "*** Loading state changed ";
		cout << mURL << " to " << (isLoading?"loading":"stopped") << endl;
	}

	void BerkeliumHandler::onLoad(Window *win) {
        sleep(1);
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
		IWindow* win;

		IDirectFB* dfb = NULL;
		IDirectFBImageProvider* provider;
		IDirectFBSurface* destination;
		DFBSurfaceDescription sDesc;

		cout << "*** onPaint "<< mURL << endl;

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

		win = (IWindow*)(surface->getParent());
		if (win != NULL) {
			surface->setParent(NULL);
		}

		dfb = (IDirectFB*)(dm->getGfxRoot());
		dfb->CreateImageProvider(dfb, str.c_str(), &provider);

		provider->GetSurfaceDescription(provider, &sDesc);
		destination = (IDirectFBSurface*)(dm->createSurface(&sDesc));

		provider->RenderTo(provider, destination, NULL);
		provider->Release(provider);

		surface->setContent(destination);

		if (win != NULL) {
			surface->setParent(win);
			win->renderFrom(surface);
			cout << "BerkeliumHandler::onPaint rendered" << endl;
		}
	}

	void BerkeliumHandler::onCrashed(Window *win) {
		cout << "*** onCrashed " << mURL << endl;
	}

	void BerkeliumHandler::onCreatedWindow(
			Window *win, Window *newWindow, const Rect &initialRect) {

        cout << "*** onCreatedWindow from source " << mURL << endl;
        //newWindow->setDelegate(new BerkeliumHandler);
	}

	void BerkeliumHandler::onExternalHost(
			Window *win,
			WideString message,
			URLString origin,
			URLString target) {

        cout << "*** onChromeSend at URL " << mURL << " from " << origin;
        cout << " to " << target << ":" << endl;
        cout << message << endl;
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
