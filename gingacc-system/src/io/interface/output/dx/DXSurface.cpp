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

#ifdef _WIN32
#define _EXP_DXSURFACE_DLL		EXPORT_SYMBOLS
#define _EXP_ISURFACE_DLL		EXPORT_SYMBOLS
#define _EXP_IIOCONTAINER_DLL	EXPORT_SYMBOLS
#define _EXP_ISURFACE_DLL		EXPORT_SYMBOLS
#define _EXP_IIOCONTAINER_DLL	EXPORT_SYMBOLS
#define	_EXP_DX2DSURFACE_DLL	USE_SYMBOLS

#pragma comment(lib, "DxErr.lib")

#include <d3dx9.h>
#include <dxerr.h>

#define DXCHECK(call, location, failureMsg)			\
{													\
	HRESULT hr = call;								\
	if(FAILED(hr)){									\
		string Err(DXGetErrorDescription(hr));		\
		cout << " [ERRO] " << Err.c_str()  << endl	\
			 << " [LOCATION] " << location << endl	\
			 << " [MSG] " <<  failureMsg   << endl;	\
	}												\
}

#endif

#include "system/io/interface/output/dx/DXSurface.h"
#include "system/io/interface/output/dx/DXWindow.h"
#include "system/io/interface/content/text/IFontProvider.h"
#include "system/io/LocalDeviceManager.h"


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	DXSurface::DXSurface()
          : sur(0), parent(0), chromaColor(0)
          , caps(0)
        {
		cout << "DXSurface::DXSurface()" << endl;
	}

	DXSurface::DXSurface(void* someSurface) {
		cout << "DXSurface::DXSurface(void* someSurface)" << endl;
		this->sur = (DX2DSurface*)someSurface;
		this->parent = NULL;
		this->chromaColor = NULL;
		this->caps = 0;
	}

	DXSurface::DXSurface(int w, int h) {
		cout << "DXSurface::DXSurface(int w, int h)" << endl;

		surProp.x = 0;
		surProp.y = 0;

		surProp.width = w;
		surProp.height = h;

		this->caps = 0;
		this->sur = (DX2DSurface*)(LocalDeviceManager::getInstance()->createSurface(&surProp));

		this->parent = NULL;
		this->chromaColor = NULL;
	}

	DXSurface::~DXSurface() {
		cout << "DXSurface::~DXSurface()" << endl;
		if (chromaColor != NULL) {
			delete chromaColor;
			chromaColor = NULL;
		}

		if (sur != NULL) {
			if (parent != NULL) {
				if (parent->removeChildSurface(this)) {
					LocalDeviceManager::getInstance()->releaseSurface(sur);
					sur = NULL;
				}

			} else {
				LocalDeviceManager::getInstance()->releaseSurface(sur);
				sur = NULL;
			}
		}
	}

	void DXSurface::addCaps(int caps) {
		cout << "DXSurface::addCaps(int caps)" << endl;
		this->caps = this->caps | caps;
	}

	void DXSurface::setCaps(int caps) {
		cout << "DXSurface::setCaps(int caps)" << endl;
		this->caps = caps;
	}

	int DXSurface::getCap(string cap) {
		cout << "DXSurface::getCap(string cap)" << endl;
		return 0;
	}

	int DXSurface::getCaps() {
		cout << "DXSurface::getCaps()" << endl;
		return this->caps;
	}

	void* DXSurface::getContent() {
		cout << "DXSurface::getContent()" << endl;
		return sur;
	}

	void DXSurface::setContent(void* surface) {
		cout << "DXSurface::setContent(void* surface)" << endl;
		if (this->sur != NULL && surface != NULL) {
			if (parent == NULL || (parent)->removeChildSurface(this)) {
				LocalDeviceManager::getInstance()->releaseSurface(sur);
				sur = NULL;
			}
		}
		this->sur = (DX2DSurface*)surface;
	}

	bool DXSurface::setParent(void* parentWindow) {
		cout << "DXSurface::setParent(void* parentWindow)" << endl;
		this->parent = (IWindow*)parentWindow;
		if (parent != NULL && chromaColor != NULL) {
			parent->setColorKey(
				    chromaColor->getR(),
				    chromaColor->getG(),
				    chromaColor->getB());
		}

		if (this->sur == NULL && parent != NULL) {
			DX2DSurface* wgWin;

			wgWin = (DX2DSurface*)(parent->getContent());
			sur = wgWin;
			parent->setReleaseListener(this);
			return false;
		}

		if (parent != NULL) {
			parent->addChildSurface(this);
		}
		return true;
	}

	void* DXSurface::getParent() {
		cout << "DXSurface::getParent()" << endl;
		return this->parent;
	}

	void DXSurface::setChromaColor(IColor* color) {
		cout << "DXSurface::setChromaColor(IColor* color)" << endl;
		if (this->chromaColor != NULL) {
			delete this->chromaColor;
			chromaColor = NULL;
		}

		this->chromaColor = color;

		if (sur != NULL) {

		}
	}

	IColor* DXSurface::getChromaColor() {
		cout << "DXSurface::getChromaColor()" << endl;
		return this->chromaColor;
	}

	void DXSurface::clearContent() {
		cout << "DXSurface::clearContent()" << endl;
		if (sur == NULL) {
			return;
		}

		if (parent != NULL) {
			parent->clearContent();
		}
	}

	void DXSurface::clearSurface() {
		cout << "DXSurface::clearSurface()" << endl;
		if (sur == NULL) {
			return;
		}
	}

	ISurface* DXSurface::getSubSurface(int x, int y, int w, int h) {
		cout << "DXSurface::getSubSurface(int x, int y, int w, int h)" << endl;
		return NULL;
	}

	void DXSurface::drawLine(int x1, int y1, int x2, int y2) {
		cout << "DXSurface::drawLine(int x1, int y1, int x2, int y2)" << endl;
	}

	void DXSurface::drawRectangle(int x, int y, int w, int h) {
		cout << "DXSurface::drawRectangle(int x, int y, int w, int h)" << endl;
	}

	void DXSurface::fillRectangle(int x, int y, int w, int h) {
		cout << "DXSurface::fillRectangle(int x, int y, int w, int h)" << endl;
//		LPDIRECT3DSURFACE9 pSur;
		LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)((LocalDeviceManager::getInstance())->getGfxRoot());

		
		(this->sur)->fill(x, y, w, h);
		//((this->sur)->getTexture())->GetSurfaceLevel(0, &pSur);
		//DXCHECK(pDev->ColorFill(pSur, NULL, surProp.color),"","");

	}

	void DXSurface::drawString(int x, int y, const char* txt) {
		cout << "DXSurface::drawString(int x, int y, const char* txt)" << endl;
		sur->drawString(((x<0)?0:x) , (y<0)?0:y, txt);
	}

	void DXSurface::setBorder(IColor* borderColor) {
		cout << "DXSurface::setBorder(IColor* borderColor)" << endl;
	}

	void DXSurface::setColor(IColor* writeColor) {
		cout << "DXSurface::setColor(IColor* writeColor)" << endl;
		//surProp.color = D3DCOLOR_ARGB(writeColor->getAlpha(), writeColor->getR(), writeColor->getG(), writeColor->getB());
	}

	void DXSurface::setBgColor(IColor* bgColor) {
		cout << "DXSurface::setBgColor(IColor* bgColor)" << endl;
	}

	void DXSurface::setFont(void* font) {
		cout << "DXSurface::setFont(void* font)" << endl;
	}

	void DXSurface::flip() {
		cout << "DXSurface::flip()" << endl;
		sur->update();
	}

	void DXSurface::blit(
			int x, int y, ISurface* src,
			int srcX, int srcY, int srcW, int srcH) {
		cout << "DXSurface::blit(int x, int y, ISurface* src, int srcX, int srcY, int srcW, int srcH)" << endl;

		sur->blit(x, y, (DX2DSurface*)src->getContent(), srcX, srcY, srcW, srcH);

	}

	void DXSurface::getStringExtents(const char* text, int* w, int* h) {
		cout << "DXSurface::getStringExtents(const char* text, int* w, int* h)" << endl;
		*w = strlen(text) + 30;
		*h = 20;
	}

	void DXSurface::setClip(int x, int y, int w, int h) {
		cout << "DXSurface::setClip(int x, int y, int w, int h)" << endl;
	}

	void DXSurface::getSize(int* w, int* h) {
		cout << "DXSurface::getSize(int* w, int* h)" << endl;
		if(sur == NULL)
			return;

//		D3DSURFACE_DESC desc;
		(*w) = sur->getWidth();
		(*h) = sur->getHeight();
	}

	string DXSurface::getDumpFileUri() {
		cout << "DXSurface::getDumpFileUri()" << endl;
		return "";
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::ISurface*
		createDXSurface(void* sur, int w, int h) {

	if (sur != NULL) {
		return (new ::br::pucrio::telemidia::ginga::core::system::io::DXSurface(sur));

	} else if (w != 0 || h != 0) {
		return (new ::br::pucrio::telemidia::ginga::core::system::io::
				DXSurface(w, h));

	} else {
		return (new br::pucrio::telemidia::ginga::core::system::io::
				DXSurface());
	}
}

extern "C" void destroyDXSurface(
		::br::pucrio::telemidia::ginga::core::system::io::ISurface* s) {
	delete s;
}
