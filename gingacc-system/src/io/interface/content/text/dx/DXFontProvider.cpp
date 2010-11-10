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
#ifndef DXCHECK

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


#endif

#include "system/io/LocalDeviceManager.h"
#include "system/io/interface/content/text/dx/DXFontProvider.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	const short DXFontProvider::A_LEFT = 0x00;
	const short DXFontProvider::A_CENTER = 0x00;
	const short DXFontProvider::A_RIGHT = 0x00;

	const short DXFontProvider::A_TOP = 0x00;
	const short DXFontProvider::A_TOP_CENTER = 0x00;
	const short DXFontProvider::A_TOP_LEFT = 0x00;
	const short DXFontProvider::A_TOP_RIGHT = 0x00;

	const short DXFontProvider::A_BOTTOM = 0x00;
	const short DXFontProvider::A_BOTTOM_CENTER = 0x00;
	const short DXFontProvider::A_BOTTOM_LEFT = 0x00;
	const short DXFontProvider::A_BOTTOM_RIGHT = 0x00;

	set<LPD3DXFONT >* DXFontProvider::pool = new set<LPD3DXFONT>;

	DXFontProvider::DXFontProvider(const char* fontUri, int heightInPixel) {
		cout << "DXFontProvider::DXFontProvider" << endl;
		pD3ddev = NULL;
		fontTex = NULL;
		pD3ddev = (IDirect3DDevice9 *)(LocalDeviceManager::getInstance()->getGfxRoot());
		// Get Font uri ?
		// Create Font
		DXCHECK(D3DXCreateFont( pD3ddev, heightInPixel + 8, 10, 10, 1, FW_DONTCARE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &font ), "", "");
		pool->insert(font);
	}

	DXFontProvider::~DXFontProvider() {
		cout << "DXFontProvider::~DXFontProvider" << endl;
		set<LPD3DXFONT>::iterator i;
		i = pool->find(font);
		if (i != pool->end()) {
			pool->erase(i);
		}
		font->Release();
		font = NULL;
	}

	void* DXFontProvider::getContent() {
		cout << "DXFontProvider::getContent()" << endl;
		return font;
	}

	int DXFontProvider::getMaxAdvance() {
		cout << "DXFontProvider::getMaxAdvance()" << endl;
		int maxAdvance = 0;
		TEXTMETRICA mtc;

		font->GetTextMetricsA(&mtc);

		return mtc.tmWeight;
	}

	int DXFontProvider::getStringWidth(const char* text, int textLength) {
		cout << "DXFontProvider::getStringWidth()" << endl;
		TEXTMETRICA mtc;


		font->GetTextMetricsA(&mtc);

		return  (textLength * 6);//mtc.tmMaxCharWidth;
	}

	int DXFontProvider::getHeight() {
		cout << "DXFontProvider::getHeight()" << endl;
		TEXTMETRICA mtc;

		font->GetTextMetricsA(&mtc);
		return mtc.tmHeight;
	}

	void DXFontProvider::playOver( void* surface, const char* text, int x, int y, short align) {
		cout << "DXFontProvider::playOver()" << endl;
		DX2DSurface*	givenSurface = NULL;
		IDirect3DSurface9*	texSur = NULL; // Texture Surface

		givenSurface = (DX2DSurface*)(((ISurface*)surface)->getContent());
		
		//givenSurface->fill();
	
		if(fontTex == NULL){
			pD3ddev->CreateTexture(givenSurface->getWidth(), givenSurface->getHeight(), 1, 0, D3DFMT_X8R8G8B8 , D3DPOOL_MANAGED, &fontTex, NULL);
		}

		if( (fontTex != NULL ) && SUCCEEDED( fontTex->GetSurfaceLevel(0, &texSur))){
			
			HDC hdc = 0;

            if (SUCCEEDED(texSur->GetDC(&hdc))){
				RECT rect = {x, y, givenSurface->getWidth(), givenSurface->getHeight()};
				SetTextColor(hdc, D3DCOLOR_ARGB(0, 255, 255, 255));
				SetBkColor(hdc, D3DCOLOR_ARGB(0, 0, 0, 0)); // fg color
				SetBkMode(hdc, TRANSPARENT);
				int txtSize = strlen(text);
				rect.right += txtSize + givenSurface->getWidth(); 
				WCHAR *wBuffer = new WCHAR [txtSize];
				mbstowcs(wBuffer, text, txtSize);

				DrawTextW(hdc, wBuffer, txtSize, &rect, DT_LEFT|DT_TOP);
				texSur->ReleaseDC(hdc);
			}
			DX2DSurfaceProp prop;
			prop.color = D3DCOLOR_ARGB(255, 100, 100, 0);
			prop.width  = givenSurface->getWidth();
			prop.height = givenSurface->getHeight();
			givenSurface->setProperties(&prop);
			givenSurface->setTexture(fontTex);

		}

/*		if(texture == NULL)
			cout << "" << endl;

		// Get Level 0 Texture Surface
		DXCHECK(texture->GetSurfaceLevel(0, &texSur),"DXFontProvider::playOver()", "Can´t Get Surface Level");

		pD3ddev->ColorFill(texSur, NULL, D3DCOLOR_ARGB(100,100,0,0) );

		// Time to setup renderTarget
		DXCHECK(pD3ddev->GetRenderTarget(0, &pBackBuffer),"DXFontProvider::playOver()", "Can´t Get Surface Level");
		DXCHECK(pD3ddev->BeginScene(), "","");
		DXCHECK(pD3ddev->SetRenderTarget(0, texSur),"DXFontProvider::playOver()", "Can´t Get Surface Level");
		DXCHECK(font->DrawTextA(NULL, LPCSTR(text), strlen(text), NULL, DT_LEFT, D3DCOLOR_ARGB(255, 255, 255, 255)),"DXFontProvider::playOver()", "Can´t Get Surface Level");
		DXCHECK(pD3ddev->EndScene(), "","");
		// Restore BackBuffer
		DXCHECK(pD3ddev->SetRenderTarget(0, pBackBuffer),"DXFontProvider::playOver()", "Can´t SetRenderTarget");
		givenSurface->setTexture(texture);
		givenSurface->show();*/
		//}
	/*	IDirect3DDevice9*	gfxDev = NULL;
		IDirect3DSurface9*	givenSurface = NULL;
		IDirect3DSurface9*	texSur = NULL; // Texture Surface
		IDirect3DSurface9*	pBackBuffer = NULL;
		LPDIRECT3DTEXTURE9	texture = NULL;
		D3DSURFACE_DESC		destSurDesc ;
		int					gSurWidth, gSurHeight;

		gfxDev = (IDirect3DDevice9 *)(LocalDeviceManager::getInstance()->getGfxRoot());
		givenSurface = (IDirect3DSurface9*)(((ISurface*)surface)->getContent());

		DXCHECK(givenSurface->GetDesc(&destSurDesc), "DXFontProvider::playOver()", "Can´t Get Descriptor");

		((ISurface*)surface)->getSize(&gSurWidth, &gSurHeight);
		DXCHECK(gfxDev->CreateTexture(gSurWidth, gSurHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8,
										D3DPOOL_DEFAULT, &texture, NULL),"DXFontProvider::playOver()", "Can´t Get Size" );

		// Get Level 0 Texture Surface
		DXCHECK(texture->GetSurfaceLevel(0, &texSur),"DXFontProvider::playOver()", "Can´t Get Surface Level");

		// Surface cloning. Why don´t you do it after texture drawing ?
		// DXCHECK(D3DXLoadSurfaceFromSurface(givenSurface, NULL, NULL, texSur, NULL, NULL, D3DX_FILTER_LINEAR, D3DCOLOR_RGBA(0, 0, 0, 255)));

		// Time to setup renderTarget
		DXCHECK(gfxDev->GetRenderTarget(0, &pBackBuffer),"DXFontProvider::playOver()", "Can´t Get Surface Level");
		DXCHECK(gfxDev->SetRenderTarget(0, texSur),"DXFontProvider::playOver()", "Can´t Get Surface Level");
		
		// Clear Texture Surface with Background Color 
		DXCHECK(gfxDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 10 , 0, 0, 0), 1.0f, 0),
				"DXFontProvider::playOver()", "Can´t Get Surface Level");

		DXCHECK(gfxDev->BeginScene(),"DXFontProvider::playOver()", "Can´t Get Surface Level");
		DXCHECK(font->DrawTextA(NULL, LPCSTR(text), strlen(text), NULL, DT_LEFT, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f )),"DXFontProvider::playOver()", "Can´t Get Surface Level");
		DXCHECK(gfxDev->EndScene(),"DXFontProvider::playOver()", "Can´t End Scene");

		// Restore BackBuffer
		DXCHECK(gfxDev->SetRenderTarget(0, pBackBuffer),"DXFontProvider::playOver()", "Can´t SetRenderTarget");

		// Surface cloning. Why don´t you do it after texture drawing ?
		DXCHECK(D3DXLoadSurfaceFromSurface(givenSurface, NULL, NULL, texSur, NULL, NULL, D3DX_FILTER_LINEAR, D3DCOLOR_RGBA(0, 0, 0, 10)),
			"DXFontProvider::playOver()", "Can´t D3DXLoadSurfaceFromSurface");

		while(1){
			POINT pt;
			pt.x = 200; //rand() % 100 + 1;
			pt.y = 200; //rand() % 100 + 1;

			Sleep(50);
			gfxDev->UpdateSurface(givenSurface, NULL, pBackBuffer, &pt);

			gfxDev->Present(NULL, NULL, NULL, NULL);

		}
		pBackBuffer->Release();
		texSur->Release();
		texture->Release();

/*
		IDirectFBSurface* s;
		s = (IDirectFBSurface*)(((ISurface*)surface)->getContent());
		DFBCHECK(s->DrawString(
				s, text, -1, x, y, (DFBSurfaceTextFlags)(align))); */
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::IFontProvider*
		createFontProvider(const char* fontUri, int heightInPixel) {
	return (new ::br::pucrio::telemidia::ginga::core::system::io::
			DXFontProvider(fontUri, heightInPixel));
}

extern "C" void destroyFontProvider(
		::br::pucrio::telemidia::ginga::core::system::io::IFontProvider* fp) {
	delete fp;
}
