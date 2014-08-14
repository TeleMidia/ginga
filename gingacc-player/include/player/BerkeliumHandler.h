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

#ifndef BerkeliumHandler_h_
#define BerkeliumHandler_h_

#include "mb/IInputManager.h"
#include "mb/interface/CodeMap.h"
#include "mb/interface/IWindow.h"
#include "mb/interface/IInputEventListener.h"

#include "mb/ILocalScreenManager.h"
#include "mb/interface/IWindow.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "berkelium/Berkelium.hpp"
#include "berkelium/Window.hpp"
#include "berkelium/WindowDelegate.hpp"
#include "berkelium/Context.hpp"

#include <stdio.h>
#include <sys/types.h>
#include <sstream>
#include <iostream>
#include <memory>

using namespace Berkelium;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
  class BerkeliumHandler :
	  public WindowDelegate,
	  public IInputEventListener,
	  public IMotionEventListener {

	private:
		static map<int, int> fromGingaToBklm;
		std::string mURL;
		ILocalScreenManager* dm;
		GingaScreenID myScreen;
		IInputManager* im;
		GingaSurfaceID surface;
		int xOffset, yOffset;
		int x, y;
		int w, h;
		Context* context;
		std::auto_ptr<Window> bWindow;
		bool isValid;
		bool mouseClick;
		bool mouseMoved;
		bool textEvent;
		int keyCode;
		static int callCount;
		bool needs_full_refresh;
		unsigned char* scroll_buffer;

		pthread_mutex_t sMutex; //surface mutex

	public:
		BerkeliumHandler(
				GingaScreenID myScreen, int x, int y, int w, int h);

		virtual ~BerkeliumHandler();

	private:
		void initInputMap();

	public:
		void stop();
		void setKeyHandler(bool handler);

		void setContext(Context* context);
		void setWindow(std::auto_ptr<Window> window);
		void getSize(int* w, int* h);
		void setBounds(int x, int y, int w, int h);
		void setUrl(string url);
		string getUrl();
		GingaSurfaceID getSurface();

		void updateEvents();

		bool userEventReceived(IInputEvent* ev);
		bool motionEventReceived(int x, int y, int z);

		virtual void onAddressBarChanged(Window *win, URLString newURL);
		virtual void onStartLoading(Window *win, URLString newURL);
		void onLoadingStateChanged(Window *win, bool isLoading);
		virtual void onLoad(Window *win);
		virtual void onLoadError(Window *win, WideString error);
		virtual void onResponsive(Window *win);
		virtual void onUnresponsive(Window *win);

	private:
		bool mapOnPaintToTexture(
				Berkelium::Window *wini,
				const unsigned char* bitmap_in, const Berkelium::Rect& bitmap_rect,
				size_t num_copy_rects, const Berkelium::Rect *copy_rects,
				int dx, int dy,
				const Berkelium::Rect& scroll_rect,
				unsigned int dest_texture_width,
				unsigned int dest_texture_height,
				bool ignore_partial,
				unsigned char* scroll_buffer);

		GingaSurfaceID createRenderedSurface(string fileName);

		string createFile(
				const unsigned char *sourceBuffer,
				int width, int height);

	public:
		virtual void onPaint(
				Window *wini,
				const unsigned char *bitmap_in,
				const Rect &bitmap_rect,
				size_t num_copy_rects,
				const Rect *copy_rects,
				int dx,
				int dy,
				const Rect &scroll_rect);

		virtual void onCrashed(Window *win);
		virtual void onCreatedWindow(
				Window *win, Window *newWindow, const Rect &initialRect);
		virtual void onExternalHost(
				Window *win,
				WideString message,
				URLString origin,
				URLString target);

		virtual void onPaintPluginTexture(
				Window *win,
				void* sourceGLTexture,
				const std::vector<Rect> srcRects,
				const Rect &destRect);

		virtual void onWidgetCreated(
				Window *win, Widget *newWidget, int zIndex);

		virtual void onWidgetDestroyed(Window *win, Widget *newWidget);

		virtual void onWidgetResize(
				Window *win, Widget *wid, int newWidth, int newHeight);

		virtual void onWidgetMove(Window *win, Widget *wid, int newX, int newY);

		virtual void onWidgetPaint(
				Window *win,
				Widget *wid,
				const unsigned char *sourceBuffer,
				const Rect &rect,
				size_t num_copy_rects,
				const Rect *copy_rects,
				int dx,
				int dy,
				const Rect &scrollRect);
  };
}
}
}
}
}
}

#endif /*BerkeliumHandler_h_*/
