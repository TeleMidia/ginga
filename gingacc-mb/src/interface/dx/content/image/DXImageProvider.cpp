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


#ifndef DXCHECK

#pragma comment(lib, "DxErr.lib")

#include <d3dx9.h>
#include <dxerr.h>
#include <FreeImage.h>

#define DXCHECK(call, location, failureMsg)			\
{													\
	HRESULT hr = call;								\
	if(FAILED(hr)){									\
		string Err(DXGetErrorDescription(hr));		\
		clog << " [ERRO] " << Err.c_str()  << endl	\
			 << " [LOCATION] " << location << endl	\
			 << " [MSG] " <<  failureMsg   << endl;	\
	}												\
}
#endif 

#include "util/Color.h"

#include "mb/interface/dx/content/image/DXImageProvider.h"
#include "mb/interface/dx/output/DXSurface.h"
#include "mb/LocalScreenManager.h"



namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	DXImageProvider::DXImageProvider(const char* pmrl)
	{
		clog << "DXImageProvider::DXImageProvider( " << pmrl << " ) " << endl;
		FIBITMAP *pDibPicture;
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		UINT width, heigth;

		mrl = string(pmrl);
		pD3ddev = NULL;
		pD3ddev = (IDirect3DDevice9 *)(ScreenManagerFactory::getInstance()->getGfxRoot());
		
		if(pD3ddev == NULL)
			clog << "DXImageProvider - No Direct3D device found" << endl;

		fif = FreeImage_GetFileType(pmrl, 0);
		if(fif == FIF_UNKNOWN) {
			fif = FreeImage_GetFIFFromFilename(pmrl);
		}
		
		if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
			pDibPicture = FreeImage_Load(fif, pmrl, 0);

			pDibPicture = FreeImage_Clone(pDibPicture);
			pDibPicture = FreeImage_ConvertTo32Bits(pDibPicture);
			
			width = FreeImage_GetWidth(pDibPicture);
			heigth = FreeImage_GetHeight(pDibPicture);

			info.Width = width;
			info.Height = heigth;

			if(FAILED(pD3ddev->CreateTexture( width, heigth, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &(imageTex), NULL))){
				clog << "Erro" << endl;
			}else{

				LPDIRECT3DSURFACE9 surfaceLevel;

				BYTE* textura = new BYTE[4*width*heigth];
				BYTE* pixeles = (BYTE*)FreeImage_GetBits(pDibPicture);


				unsigned char* curbuffer = textura;

				int rowSpan = width * 4;

				// invert ;)
				for(int row = heigth - 1; row >= 0; row--){
					memcpy(curbuffer, (pixeles + row * rowSpan), rowSpan);
					curbuffer +=rowSpan;
				}

				if(SUCCEEDED(imageTex->GetSurfaceLevel(0, &surfaceLevel)))
				{
					RECT srcRect = {0, 0, width, heigth};
					if(SUCCEEDED(D3DXLoadSurfaceFromMemory(surfaceLevel, NULL, NULL, textura,
						D3DFMT_A8R8G8B8, FreeImage_GetPitch(pDibPicture),NULL, &srcRect, D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER, 0))){
						surfaceLevel->Release();
					}else{
						surfaceLevel->Release();
					}

				}else{
					clog << "Erro" << endl;
				}

				delete[] textura;
			}

		}

		FreeImage_Unload(pDibPicture);
	}

	DXImageProvider::~DXImageProvider()
	{
		clog << "DXImageProvider::~DXImageProvider( " << mrl.c_str() << " ) " << endl;
		imageTex->Release();
	}

	void* DXImageProvider::getContent()
	{
		clog << "DXImageProvider::getContent()" << endl;
		return NULL;
	}

	void DXImageProvider::playOver(ISurface* surface)
	{
		clog << "DXImageProvider::playOver(ISurface* surface)" << endl;
		DX2DSurface*	s = (DX2DSurface*)(surface->getSurfaceContent());
	}

	ISurface* DXImageProvider::prepare(bool isGif)
	{
		clog << "DXImageProvider::prepare(bool isGif)" << endl;
		ISurface* renderedSurface = NULL;
		DX2DSurface* destination = NULL;
		DX2DSurfaceProp	surProp;

		surProp.width = info.Width;
		surProp.height = info.Height;
		surProp.x = 0;
		surProp.y = 0;
		surProp.z = 1.0;

		destination = (DX2DSurface*)(ScreenManagerFactory::getInstance()->createSurface(&surProp));
		renderedSurface = new DXSurface(destination);

		if(destination != NULL && renderedSurface != NULL){
			((DX2DSurface*)renderedSurface->getSurfaceContent())->setTexture(imageTex);
		}
		return renderedSurface;
	}

	bool DXImageProvider::releaseAll()
	{
		clog << "DXImageProvider::releaseAll()" << endl;
		return false;
	}

}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::IImageProvider*
		createImageProvider(const char* mrl) {
	return (new ::br::pucrio::telemidia::ginga::core::mb::
			DXImageProvider(mrl));
}

extern "C" void destroyImageProvider(
		::br::pucrio::telemidia::ginga::core::mb::IImageProvider* ip) {
	delete ip;
}
