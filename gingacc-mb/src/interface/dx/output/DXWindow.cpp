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

#include "util/Color.h"

#include "mb/LocalScreenManager.h"
#include "mb/interface/dx/output/DXWindow.h"
#include "mb/interface/dx/output/DXSurface.h"

#include <stdlib.h>


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

	float DXWindow::lastBiggestZindex = 1.0000f;
	float DXWindow::lastLowestZindex = 0.0000f;

	map<float, list<DXWindow*>>* DXWindow::depthMap = new map<float, list<DXWindow*>>();

	DXWindow::DXWindow(int x, int y, int width, int height) {
		clog << "DXWindow::DXWindow(int x, int y, int width, int height)" << endl;
		this->win = NULL;
		this->winSur = NULL;

		winProp.x = x;
		winProp.y = y;
		winProp.z = 1.0000f;
		winProp.width = width;
		winProp.height = height ;
		winProp.color = D3DCOLOR_ARGB(255, 255, 255, 255);

		this->ghost = false;
		this->visible = false;
		this->transparencyValue = 0;

		this->childSurfaces = new vector<ISurface*>;
		this->releaseListener = NULL;
		this->fit = true;
		this->stretch = true;
		this->caps = 0;

		((*depthMap)[1.0000f]).push_back(this);

		pthread_mutex_init(&mutex, NULL);
		pthread_mutex_init(&mutexC, NULL);
	}

	DXWindow::~DXWindow() {
		clog << "DXWindow::~DXWindow" << endl;
		ISurface* surface;
		vector<ISurface*>::iterator i;

		lock();
		lockChilds();
		if (releaseListener != NULL) {
			releaseListener->setParentWindow(NULL);
		}

		if (childSurfaces != NULL) {
			i = childSurfaces->begin();
			while (i != childSurfaces->end()) {
				surface = *i;
				if (surface != NULL) {
					surface->setParentWindow(NULL);
				}
				++i;
			}
			delete childSurfaces;
			childSurfaces = NULL;
		}
		unlockChilds();

		if (winSur != NULL) {
			LocalScreenManager::getInstance()->releaseSurface(winSur);
			winSur = NULL;
		}

		if (win != NULL) {
			LocalScreenManager::getInstance()->releaseWindow(win);
			win = NULL;
		}
		unlock();

		pthread_mutex_unlock(&mutex);
		pthread_mutex_unlock(&mutexC);

		pthread_mutex_destroy(&mutex);
		pthread_mutex_destroy(&mutexC);
	}

	void DXWindow::setChildSurface(ISurface* listener) {
		clog << "XWindow::setReleaseListener(ISurface* listener)" << endl;
		this->releaseListener = listener;
	}

	int DXWindow::getCap(string cap) {
		clog << "DXWindow::getCap(string cap)" << endl;
		return 0;
	}

	void DXWindow::setCaps(int caps) {
		clog << "DXWindow::setCaps(int caps)" << endl;
		this->caps = caps;
	}

	void DXWindow::addCaps(int capability) {
		clog << "DXWindow::addCaps(int capability)" << endl;
		this->caps = (this->caps | capability);
	}

	int DXWindow::getCaps() {
		clog << " DXWindow::getCaps()" << endl;
		return caps;
	}

	void DXWindow::draw() {
		clog << "DXWindow::draw()" << endl;

		if (win != NULL) {
			clog << "DXWindow::draw Warning! Requesting redraw" << endl;
		} else {
			win = (DX2DSurface *)(LocalScreenManager::getInstance()->createWindow(&winProp));
			if (win != NULL) {
				this->winSur = this->win;
			}
		}

	}

	void DXWindow::setBounds(int posX, int posY, int w, int h) {
		clog << "DXWindow::setBounds(" << posY << ", " << posY << ", " << w << ", " << h << ")" << endl;
		if (win == NULL) {
			clog << "DXWindow::setBounds Warning! window is null " << endl;
			winProp.x = posX;
			winProp.y = posY;
			winProp.width = w;
			winProp.height = h;
			return;
		}

		winProp.x = posX;
		winProp.y = posY;
		winProp.width = w;
		winProp.height = h;
		lock();
		win->setProperties(&winProp);
		unlock();
	}

	void DXWindow::setBackgroundColor(int r, int g, int b, int alpha) {
		clog << "DXWindow::setBackgroundColor(r = " <<  r << ", g = " << g << ", b = " <<  b << ", alpha = " << alpha << " )" << endl;
		winProp.color = D3DCOLOR_ARGB(alpha, r, g, b);
	}

	void DXWindow::setColorKey(int r, int g, int b) {
		clog << "DXWindow::setColorKey(r = " <<  r << ", g = " << g << ", b = " <<  b << ")" << endl;
	}

	void DXWindow::moveTo(int posX, int posY) {
		clog << "DXWindow::moveTo(int posX, int posY)" << endl;

		winProp.x = posX;
		winProp.y = posY;

		lock();
		win->moveTo(winProp.x, winProp.y);
		unlock();
	}

	void DXWindow::resize(int width, int height) {
		clog << "DXWindow::resize(int width, int height)" << endl;
		
		winProp.width = width;
		winProp.height = height;

		lock();
		win->resize(winProp.width, winProp.height);
		unlock();
	}

	void DXWindow::raiseToTop() {
		clog << "DXWindow::raiseToTop()" << endl;
		lock();
		((*depthMap)[winProp.z]).remove(this);
		winProp.z = lastBiggestZindex = (lastBiggestZindex - 0.002f) ;
		clog << "lastBiggestZindex = " << lastBiggestZindex << endl;
		((*depthMap)[winProp.z]).push_back(this);
		win->setProperties(&winProp);
		unlock();
	}

	void DXWindow::lowerToBottom() {
		clog << "DXWindow::lowerToBottom()" << endl;
		lock();
		((*depthMap)[winProp.z]).remove(this);
		winProp.z = lastLowestZindex = (lastLowestZindex + 0.002f) ;
		((*depthMap)[winProp.z]).push_back(this);
		win->setProperties(&winProp);
		unlock();
	}

	void DXWindow::setCurrentTransparency(int alpha) {
		clog << "DXWindow::setCurrentTransparency(alpha = " << alpha << ")" << endl;
		winProp.color =  (winProp.color & 0x00FFFFFF) | (alpha << 24);
	}

	int DXWindow::getTransparencyValue() {
		clog << "DXWindow::getTransparencyValue()" << endl;
		return 0;
	}

	void DXWindow::show() {
		clog << "DXWindow::show()" << endl;
		this->visible = true;
		win->show();
	}

	void DXWindow::hide() {
		clog << "DXWindow::hide()" << endl;
		this->visible = false;
		win->hide();
	}

	int DXWindow::getX() {
		clog << "DXWindow::getX()" << endl;
		return winProp.x;
	}

	int DXWindow::getY() {
		clog << "DXWindow::getY()" << endl;
		return winProp.y;
	}

	int DXWindow::getW() {
		clog << "DXWindow::getW()" << endl;
		return winProp.width;
	}

	int DXWindow::getH() {
		clog << "DXWindow::getH()" << endl;
		return winProp.height;
	}

	void* DXWindow::getContent() {
		clog << "DXWindow::getContent()" << endl;
		return win;
	}

	void DXWindow::setColor(int r, int g, int b, int alpha) {
		clog << "DXWindow::setColor(int r, int g, int b, int alpha)" << endl;
		winProp.color = D3DCOLOR_ARGB(alpha, r, g, b);
	}

	void DXWindow::setBorder(int r, int g, int b, int alpha, int bWidth) {
		clog << "DXWindow::setBorder(int r, int g, int b, int alpha, int bWidth)" << endl;
		clog << "setBorder: Not implemented yet" << endl; 
	}

	void DXWindow::setBorder(IColor* color, int bWidth) {
		clog << "DXWindow::setBorder(IColor* color, int bWidth)" << endl;
		clog << "setBorder: Not implemented yet" << endl; 
	}

	void DXWindow::setGhostWindow(bool ghost) {
		clog << "DXWindow::setGhostWindow(bool ghost)" << endl;
		clog << "setGhostWindow: Not implemented yet" << endl; 
	}

	bool DXWindow::isVisible() {
		clog << "DXWindow::isVisible()" << endl;
		return visible;
	}

	void DXWindow::validate() {
		clog << "DXWindow::validate()" << endl;
		if (win != NULL && winSur != NULL) {
			if (winSur != NULL) {
				lockChilds();
				if (childSurfaces != NULL && !childSurfaces->empty()) {
					ISurface* surface;
					surface = childSurfaces->at(0);
					if (surface != NULL) {
						renderFrom(surface);
					}

				} else {
						clog << "No child surfaces" << endl;
				}
				unlockChilds();
			}
		}
	}

	void DXWindow::setStretch(bool stretchTo) {
		clog << "DXWindow::setStretch(bool stretchTo)" << endl;
		clog << "setStretch: Not implemented yet" << endl; 
	}

	bool DXWindow::getStretch() {
		clog << "DXWindow::getStretch()" << endl;
		clog << "getStretch: Not implemented yet" << endl; 
		return NULL;
	}

	void DXWindow::setFit(bool fitTo) {
		clog << "DXWindow::setFit(bool fitTo)" << endl;
		clog << "setFit: Not implemented yet" << endl; 
	}

	bool DXWindow::getFit() {
		clog << "DXWindow::getFit()" << endl;
		clog << "getFit: Not implemented yet" << endl; 
		return false;
	}

	void DXWindow::clearContent() {
		clog << "DXWindow::clearContent()" << endl;
		clog << "clearContent: Not implemented yet" << endl; 
	}

	bool DXWindow::isMine(ISurface* surface) {
		clog << "DXWindow::isMine(ISurface* surface)" << endl;
		DX2DSurface* contentSurface;

		if (win == NULL || winSur == NULL || surface == NULL) {
			return false;
		}

		contentSurface = (DX2DSurface*)(surface->getContent());

		if (contentSurface == winSur) {
			return true;
		}
		return false;
	}

	void DXWindow::renderFrom(ISurface* surface) {
		clog << "DXWindow::renderFrom(ISurface* surface)" << endl;
		DX2DSurface* contentSurface, *s;
		//D3DSURFACE_DESC desc;
		int w, h;

		if (win != NULL && !isMine(surface)){

			contentSurface = (DX2DSurface*)(surface->getContent());

			if(contentSurface == NULL){
				clog << "contentSurface is NULL" << endl;
			}

			w = contentSurface->getWidth();
			h = contentSurface->getHeight();
			
			if( winSur != NULL && winSur != contentSurface){
				if( (w != winProp.width || h != winProp.height) && fit ){
					if(stretch){
						//winSur->setTexture(contentSurface->getTexture());
						winSur->replaceTex(contentSurface->getTexture());
					}else{
						
					}
				}else{
					//winSur->setTexture(contentSurface->getTexture());
					winSur->replaceTex(contentSurface->getTexture());
				}
			}
		}
	}

	void DXWindow::blit(IWindow* src) {
		clog << "DXWindow::blit(IWindow* src)" << endl;
		clog << "blit: Not implemented yet" << endl; 
	}

	void DXWindow::stretchBlit(IWindow* src) {
		clog << "DXWindow::stretchBlit(IWindow* src)" << endl;
		clog << "stretchBlit: Not implemented yet" << endl; 
	}

	string DXWindow::getDumpFileUri() {
		clog << "DXWindow::getDumpFileUri()" << endl;
		clog << "getDumpFileUri: Not implemented yet" << endl; 
		return "";
	}

	void DXWindow::lock() {
		clog << "DXWindow::lock()" << endl;
		pthread_mutex_lock(&mutex);
	}

	void DXWindow::unlock() {
		clog << "DXWindow::unlock()" << endl;
		pthread_mutex_unlock(&mutex);
	}

	void DXWindow::lockChilds() {
		clog << "DXWindow::lockChilds()" << endl;
		pthread_mutex_lock(&mutexC);
	}

	void DXWindow::unlockChilds() {
		clog << "DXWindow::unlockChilds()" << endl;
		pthread_mutex_unlock(&mutexC);
	}

	void DXWindow::setMaxTransparencyValue(int maxValue){
		clog << "DXWindow::setMaxTransparencyValue()" << endl;
		clog << "setMaxTransparencyValue: Not implemented yet" << endl; 
	}

	string DXWindow::getDumpFileUri(int quality, int dumpW, int dumpH){
		clog << "DXWindow::getDumpFileUri()" << endl;
		clog << "getDumpFileUri: Not implemented yet" << endl; 
		return "";
	}

}
}
}
}
}
}
