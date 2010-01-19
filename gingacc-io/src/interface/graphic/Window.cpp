/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licen�a Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribu�do na expectativa de que seja util, porem, SEM
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

#include "../../../include/IOHandler.h"
#include "../../../include/Graphics.h"

#if HAVE_DIRECTFB
struct DynamicRender {
	IDirectFBSurface* destination;
	IDirectFBSurface* frame;
};
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {

#if HAVE_DIRECTFB
	const int Window::CAPS_NONE         = DWCAPS_NONE;
	const int Window::CAPS_NOSTRUCTURE  = DWCAPS_NODECORATION;
	const int Window::CAPS_ALPHACHANNEL = DWCAPS_ALPHACHANNEL;
	const int Window::CAPS_ALL          = DWCAPS_ALL;
#endif

	Window::Window(int x, int y, int width, int height) {
		this->win = NULL;
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
		this->visible = false;
		this->transparencyValue = 255;
		this->r = 0;
		this->g = 0;
		this->b = 0;
		this->childSurfaces = new vector<Surface*>;
		this->fit = true;
		this->stretch = true;
		this->caps = CAPS_NOSTRUCTURE;

		pthread_mutex_init(&mutex, NULL);
		pthread_mutex_init(&mutexC, NULL);
	}

	Window::~Window() {
		wclog << this << ">> Window::~Window" << endl;
		Surface* surface;
		vector<Surface*>::iterator i;

		lock();
		lockChilds();
		if (childSurfaces != NULL) {
			i = childSurfaces->begin();
			while (i != childSurfaces->end()) {
				surface = *i;
				if (surface != NULL) {
					surface->setParent(NULL);
				}
				++i;
			}
			delete childSurfaces;
			childSurfaces = NULL;
		}
		unlockChilds();

		if (win != NULL) {
			IOHandler::releaseWindow(this);
			win = NULL;
		}
		unlock();
		pthread_mutex_destroy(&mutex);
		pthread_mutex_destroy(&mutexC);
	}

	void Window::setCaps(int caps) {
		this->caps = caps;
	}

	void Window::addCaps(int capability) {
		this->caps = (this->caps | capability);
	}

	int Window::getCaps() {
		return caps;
	}

	void Window::draw() {
		if (win != NULL) {
			cout << "Window::draw Warning! Requesting redraw" << endl;

		} else {
#if HAVE_DIRECTFB
			DFBWindowDescription dsc;

			dsc.flags  = (DFBWindowDescriptionFlags)(
				    DWDESC_POSX |
				    DWDESC_POSY |
				    DWDESC_WIDTH |
				    DWDESC_HEIGHT | DWDESC_CAPS);

			dsc.posx   = x;
			dsc.posy   = y;
			dsc.width  = width;
			dsc.height = height;
			dsc.caps  = (DFBWindowCapabilities)(caps);

			win = IOHandler::createWindow(&dsc);
			if (win != NULL) {

				IDirectFBWindow* w = (IDirectFBWindow*)win;
				DFBCHECK(w->SetOpacity(w, 0x00));
			}
#endif
		}

		if (win != NULL && (caps & Window::CAPS_ALPHACHANNEL)) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			DFBCHECK(w->SetOptions(w,(DFBWindowOptions)DWOP_ALPHACHANNEL));
#endif
		}

		setBackgroundColor(r, g, b);
	}

	void Window::setBounds(int posX, int posY, int w, int h) {
		if (win == NULL) {
			this->x = posX;
			this->y = posY;
			this->width = w;
			this->height = h;
			return;
		}

		this->x = posX;
		this->y = posY;
		this->width = w;
		this->height = h;

		lock();
		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			DFBCHECK(w->SetBounds(w, x, y, width, height));
#endif
		}
		unlock();
	}

	void Window::setBackgroundColor(int r, int g, int b) {
		//wclog << this << ">> Window::setBackgroundColor" << endl;
		if (win == NULL) {
			return;
		}

#if HAVE_DIRECTFB
		IDirectFBWindow* w = (IDirectFBWindow*)win;
		IDirectFBSurface* surface;

		DFBCHECK(w->GetSurface(w, &surface));
		DFBCHECK(surface->Clear(surface, r, g, b, 0xFF));
#endif
		this->r = r;
		this->g = g;
		this->b = b;
	}

	Color* Window::getBgColor() {
		return new Color(r, g, b);
	}

	void Window::setColorKey(int r, int g, int b) {
		//cout << this << ">> Window::setColorKey" << endl;
		//lock();
		if (win == NULL) {
			return;
		}
		this->r = r;
		this->g = g;
		this->b = b;

#if HAVE_DIRECTFB
		IDirectFBWindow* w = (IDirectFBWindow*)win;
		DFBCHECK(w->SetColorKey(w, r, g, b));

		if (caps & Window::CAPS_ALPHACHANNEL) {
			DFBCHECK(w->SetOptions(w, (DFBWindowOptions)
				    (DWOP_COLORKEYING | DWOP_ALPHACHANNEL)));

		} else {
			DFBCHECK(w->SetOptions(
					w, (DFBWindowOptions)(DWOP_COLORKEYING)));
		}
#endif
		//unlock();
		setBackgroundColor(r, g, b);
	}

	void Window::moveTo(int posX, int posY) {
		//wclog << this << ">> Window::moveTo" << endl;

		if (win == NULL) {
			return;
		}

		this->x = posX;
		this->y = posY;

		lock();
		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			DFBCHECK(w->MoveTo(w, x, y));
#endif
		}
		unlock();
	}

	void Window::resize(int width, int height) {
		//wclog << this << ">> Window::resize" << endl;
		if (win == NULL) {
			return;
		}

		this->width = width;
		this->height = height;

		lock();
		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			DFBCHECK(w->Resize(w, width, height));
#endif
		}
		unlock();
	}

	void Window::raise() {
		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			DFBCHECK(w->Raise(w));
#endif
		}
	}

	void Window::lower() {
		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			DFBCHECK(w->Lower(w));
#endif
		}
	}

	void Window::raiseToTop() {
		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			DFBCHECK(w->RaiseToTop(w));
#endif
		}
	}

	void Window::lowerToBottom() {
		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			DFBCHECK(w->LowerToBottom(w));
#endif
		}
	}

	void Window::setCurrentTransparency(int alpha) {
		if (alpha != 0) {
			this->visible = true;

		} else {
			this->visible = false;
		}

		lock();
		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			w->SetOpacity(w, alpha);
#endif
		}
		unlock();
	}

	void Window::setTransparencyValue(int alpha) {
		this->transparencyValue = alpha;
	}

	int Window::getTransparencyValue() {
		return this->transparencyValue;
	}

	void Window::show() {
		this->visible = true;

		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			w->SetOpacity(w, transparencyValue);
#endif
		}
	}

	void Window::hide() {
		this->visible = false;

		lock();
		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			DFBCHECK(w->SetOpacity(w, 0x00));
#endif
		}
		unlock();
	}

	int Window::getX() {
		return this->x;
	}

	int Window::getY() {
		return this->y;
	}

	int Window::getW() {
		return this->width;
	}

	int Window::getH() {
		return this->height;
	}

	void* Window::getContent() {
		return win;
	}
/*
	IDirectFBSurface* Window::getContentSurface() {
		if (win != NULL) {
			IDirectFBSurface* s;
			DFBCHECK(win->GetSurface(win, &s));
			return s;

		} else {
			return NULL;
		}
	}
*/
	void Window::setColor(int r, int g, int b, int alpha) {
		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			IDirectFBSurface* s;

			DFBCHECK(w->GetSurface(w, &s));
			DFBCHECK(s->SetColor(s, r, g, b, alpha));
#endif
		}
	}

	void Window::setBorder(int r, int g, int b, int alpha, int bWidth) {
		//lock();
		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			IDirectFBSurface* s;

			DFBCHECK(w->GetSurface(w, &s));
			DFBCHECK(s->SetColor(s, r, g, b, alpha));

			int i;

			if (bWidth < 0) {
				bWidth = bWidth * -1;
			}

			for (i=0; i < bWidth; i++) {
				DFBCHECK(s->DrawRectangle(
					    s, i, i, width - (2*i), height - (2*i)));
			}
			DFBCHECK(s->Flip(s, NULL, (DFBSurfaceFlipFlags)0));
#endif
		}
		//unlock();
	}

	void Window::setBorder(Color* color, int bWidth) {
		setBorder(
			    color->getR(),
			    color->getG(),
			    color->getB(),
			    color->getAlpha(),
			    bWidth);
	}

	bool Window::isVisible() {
		return this->visible;
	}

	void Window::validate() {
		if (win != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* w = (IDirectFBWindow*)win;
			IDirectFBSurface* s = NULL;
			DFBCHECK(w->GetSurface(w, &s));

			if (s != NULL) {
				lockChilds();
				if (childSurfaces != NULL && !childSurfaces->empty()) {
					Surface* surface;
					surface = childSurfaces->at(0);
					if (surface != NULL) {
						renderFrom(surface);
					}

				} else {
					DFBCHECK(s->Flip(s, NULL, (DFBSurfaceFlipFlags)0));
				}
				unlockChilds();
			}
#endif
		}
		//wclog << "Window::validate " << endl;
	}

	void Window::addChildSurface(Surface* s) {
		int i;
		Surface* surface;

		lockChilds();
		for (i = 0; i < childSurfaces->size(); i++) {
			surface = childSurfaces->at(i);
			if (surface == s) {
				unlockChilds();
				return;
			}
		}
		childSurfaces->push_back(s);
		unlockChilds();
	}

	bool Window::removeChildSurface(Surface* s) {
		int i;
		vector<Surface*>::iterator j;
		Surface* surface;

		lockChilds();
		if (childSurfaces == NULL) {
			unlockChilds();
			return false;
		}

		for (i=0; i < childSurfaces->size(); i++) {
			surface = childSurfaces->at(i);
			if (surface == s) {
				j = childSurfaces->begin() + i;
				childSurfaces->erase(j);
				unlockChilds();
				return true;
			}
		}
		unlockChilds();
		return false;
	}

	void Window::setStretch(bool stretchTo) {
		this->stretch = stretchTo;
	}

	bool Window::getStretch() {
		return this->stretch;
	}

	void Window::setFit(bool fitTo) {
		this->fit = fitTo;
	}

	bool Window::getFit() {
		return this->fit;
	}

	void Window::clear() {
		setBackgroundColor(r, g, b);
	}

	void Window::renderFrom(Surface* surface) {
#if HAVE_DIRECTFB
		IDirectFBSurface* contentSurface;
		contentSurface = (IDirectFBSurface*)(surface->getContent());
		lock();
		if (win != NULL && contentSurface != NULL) {
			int w, h;

			IDirectFBWindow* winWg = (IDirectFBWindow*)win;
			DFBCHECK(contentSurface->GetSize(contentSurface, &w, &h));

			IDirectFBSurface* s;
			DFBCHECK(winWg->GetSurface(winWg, &s));

			if (s != NULL && s != contentSurface) {
				DFBCHECK(s->Clear(s, 0, 0, 0, 0));
				if ((w != width || h != height) && fit) {
					if (stretch) {
						DFBCHECK(s->StretchBlit(
								s, contentSurface, NULL, NULL));

					} else {
						Surface* sur;
						IDirectFBSurface* s2;
						sur = new Surface(width, height);
						s2 = (IDirectFBSurface*)(sur->getContent());

						DFBCHECK(s2->StretchBlit(
							    s2,
							    contentSurface,
							    NULL,
							    NULL));

						DFBCHECK(s2->Flip(
								s2, NULL, (DFBSurfaceFlipFlags)DSFLIP_BLIT));

						DFBCHECK(s->Blit(s, s2, NULL, 0, 0));
						DFBCHECK(s->Flip(
								s, NULL, (DFBSurfaceFlipFlags) DSFLIP_BLIT));

						delete sur;
					}

				} else {
					DFBCHECK(s->Blit(s, contentSurface, NULL, 0, 0));
				}
				DFBCHECK(s->Flip(s, NULL, (DFBSurfaceFlipFlags)0));
			}
		}
		unlock();
#endif
	}

	void Window::lock() {
		pthread_mutex_lock(&mutex);
	}

	void Window::unlock() {
		pthread_mutex_unlock(&mutex);
	}

	void Window::lockChilds() {
		pthread_mutex_lock(&mutexC);
	}

	void Window::unlockChilds() {
		pthread_mutex_unlock(&mutexC);
	}
}
}
}
}
}
}
