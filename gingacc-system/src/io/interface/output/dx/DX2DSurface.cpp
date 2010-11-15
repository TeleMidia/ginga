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


#pragma comment(lib, "DxErr.lib")
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


#include "system/io/interface/output/dx/DX2DSurface.h"
#include "system/io/LocalDeviceManager.h"
#include <dxerr.h>
namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	DX2DSurface::DX2DSurface(int x, int y, int w, int h, IDirect3DDevice9 *dev)
	{
		pD3ddev = NULL;
		pD3ddev = dev;
	
		this->x =  x ;
		this->y =  y ; 
		this->z = 1.0; // as far we can get
		this->width =  (w < 0)? 1:w ;
		this->height = (h < 0)? 1:h ;
		this->alpha = 0;
		this->color = D3DCOLOR_ARGB(255, 255, 255, 255);

		this->visible = false;
		this->blitEnable = false;

		this->pTex = NULL;
		this->pTexCanvas = NULL;
		this->pTexFont = NULL;

		pthread_mutex_init(&visible_lock, NULL);
		pthread_mutex_init(&tex_lock, NULL);
		
		if(FAILED(pD3ddev->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &(pTex), NULL))){
			cout << "erro" << endl;
		}else{
			LPDIRECT3DSURFACE9 pTexSur;
			DXCHECK((pTex)->GetSurfaceLevel(0, &pTexSur),"","") ;
			DXCHECK(pD3ddev->ColorFill(pTexSur, NULL, color ),"","");
			pTexSur->Release();
		}


		if(FAILED(pD3ddev->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
											D3DPOOL_DEFAULT, &(pTexCanvas), NULL))){
			cout << "erro" << endl;
		}else{
			LPDIRECT3DSURFACE9 pCanvasSur;
			DXCHECK((pTexCanvas)->GetSurfaceLevel(0, &pCanvasSur),"","") ;
			DXCHECK(pD3ddev->ColorFill(pCanvasSur, NULL, color ),"","");
			pCanvasSur->Release();
		}

		initGraphics();
	}

	DX2DSurface::~DX2DSurface() 
	{

		pTex->Release();
		pTex = NULL;
		pthread_mutex_destroy(&tex_lock);
		pthread_mutex_destroy(&visible_lock);
	}

	void DX2DSurface::initGraphics()
	{
		VOID* pVoid = NULL;
		VOID* IndexData = NULL;
		short Indices[6] ={0, 1, 2, 3, 1, 0};

		if(pD3ddev != NULL){

			buildVertex(); 
			pD3ddev->CreateVertexBuffer(4*sizeof(SURFACEVERTEX), 0, SURFACEFVF,
										D3DPOOL_MANAGED, &pVbuffer, NULL);

			pVbuffer->Lock(0, 0, (void**)&pVoid, 0);   
			memcpy(pVoid, vertex, sizeof(vertex)); 
			pVbuffer->Unlock();

			if(FAILED(pD3ddev->CreateIndexBuffer(sizeof(short)*6, 0, D3DFMT_INDEX16,
												D3DPOOL_MANAGED, &pIndexBuffer, NULL))){
				cout << "Falhou" << endl;
			}

			if(SUCCEEDED(pIndexBuffer->Lock(0, 0, &IndexData, 0))){
				memcpy(IndexData, (void*) &Indices, sizeof(Indices));
				pIndexBuffer->Unlock();
			}
		}
	}

	void DX2DSurface::buildVertex()
	{
		// v0 (0.0, 0.0) bottom left
		vertex[0].x = x;
		vertex[0].y = y;
		vertex[0].z = z;
		vertex[0].rhw = 1.0f;
		vertex[0].color = this->color;
		vertex[0].tu = 0.0;
		vertex[0].tv = 0.0;

		// v1 (1.0, 1.0) top right
		vertex[1].x = x + width;
		vertex[1].y = y + height;
		vertex[1].z = z; //z;
		vertex[1].rhw = 1.0f;
		vertex[1].color = this->color;
		vertex[1].tu = 1.0;
		vertex[1].tv = 1.0;

		// v2 (0.0 , 1.0) top left 
		vertex[2].x = x;
		vertex[2].y = y + height;
		vertex[2].z = z;
		vertex[2].rhw = 1.0f;
		vertex[2].color = this->color;
		vertex[2].tu = 0.0;
		vertex[2].tv = 1.0;

		// v3 (1.0, 0.0) bottom right
		vertex[3].x = x + width;
		vertex[3].y = y;
		vertex[3].z = z;
		vertex[3].rhw = 1.0f;
		vertex[3].color = this->color;
		vertex[3].tu = 1.0;
		vertex[3].tv = 0.0;
	}

	void DX2DSurface::setProperties(DX2DSurfaceProp* surProp){
		x = surProp->x;
		y = surProp->y;
		z = surProp->z;

		width = surProp->width;
		height = surProp->height;

		//color = surProp->color;
		initGraphics();
	}

	int DX2DSurface::getX(){
		return x;
	}

	int DX2DSurface::getY(){
		return y;
	}

	int DX2DSurface::getWidth(){
		return width;
	}

	int DX2DSurface::getHeight(){
		return height;
	}

	void DX2DSurface::resize(int width, int height){
		cout << "resize" << endl;
	}

	void DX2DSurface::moveTo(int posX, int posY){

	}

	void DX2DSurface::show(){
		pthread_mutex_lock(&visible_lock);
		this->visible = true;
		pthread_mutex_unlock(&visible_lock);
	}

	void DX2DSurface::hide(){
		pthread_mutex_lock(&visible_lock);
		this->visible = false;
		pthread_mutex_unlock(&visible_lock);
	}

	LPDIRECT3DTEXTURE9 DX2DSurface::getTexture(){
		if(pTex == NULL)
			cout << "DX2DSurface::getTexture() : pTex is NULL" << endl;

		return pTex;
	}

	void DX2DSurface::setTexture(LPDIRECT3DTEXTURE9 tex){
		
		LPDIRECT3DSURFACE9 pSSur, pDSur;

		if (tex == NULL){
				cout << "DX2DSurface::setTexture WARNING! Texture is NULL! Returning..." << endl;
				return;
		}

		if(FAILED(tex->GetSurfaceLevel(0, &pSSur))){
			cout << "Can't get tex surface level 0" << endl;
		}

		if(FAILED(pTex->GetSurfaceLevel(0, &pDSur))){
			cout << "Can't get pTex surface level 0" << endl; 
		}


		pthread_mutex_lock(&tex_lock);

		DXCHECK( pD3ddev->StretchRect(pSSur, NULL, pDSur, NULL, D3DTEXF_NONE), "DX2DSurface::setTexture", "erro" );


		//pTex = tex;
		
		pthread_mutex_unlock(&tex_lock);
		pSSur->Release();
		pDSur->Release();

	}

	void DX2DSurface::replaceTex(LPDIRECT3DTEXTURE9 newTex){
		pthread_mutex_lock(&tex_lock);
		pTex->Release();
		pTex = NULL;
		pTex = newTex;
		pthread_mutex_unlock(&tex_lock);
	}

	void DX2DSurface::draw2DSurface(){
		HRESULT hr;

		pthread_mutex_lock(&tex_lock);
		pthread_mutex_lock(&visible_lock);
		if(pTex != NULL && (visible == true )){
			hr = pD3ddev->SetFVF(SURFACEFVF);
			hr = pD3ddev->SetIndices(pIndexBuffer);
			hr = pD3ddev->SetTexture(0, pTex);

			hr = pD3ddev->SetStreamSource(0, pVbuffer, 0, sizeof(SURFACEVERTEX));
			hr = pD3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
			hr = 0;
		}
		pthread_mutex_unlock(&visible_lock);
		pthread_mutex_unlock(&tex_lock);
	}

	void DX2DSurface::blit(int x, int y, DX2DSurface* src,
			int srcX, int srcY, int srcW, int srcH){
			
			LPDIRECT3DTEXTURE9 tmpTex;
			D3DSURFACE_DESC srcSurDesc;
			D3DSURFACE_DESC tmpSurDesc;
			LPDIRECT3DSURFACE9 pSrcSur;
			LPDIRECT3DSURFACE9 pTmpSur;
			LPDIRECT3DSURFACE9 pCanvasSur;
			RECT *srcRect = NULL;
			POINT dstPoint;
			HRESULT hr;

			// --- DEFAULT to SYSTEM

			LPDIRECT3DTEXTURE9 srcTex = src->getTexture();
			DXCHECK(srcTex->GetLevelDesc(0, &srcSurDesc), "srcTex->GetLevelDesc" , "error");
			
			if(FAILED(pD3ddev->CreateTexture( srcSurDesc.Width, srcSurDesc.Height, 1, D3DUSAGE_DYNAMIC, srcSurDesc.Format,
											D3DPOOL_SYSTEMMEM, &(tmpTex), NULL))){
				cout << "erro" << endl;
			}
	
			tmpTex->GetLevelDesc(0, &tmpSurDesc);
			
			srcTex->GetSurfaceLevel(0, &pSrcSur);
			tmpTex->GetSurfaceLevel(0, &pTmpSur);
			
			DXCHECK(pD3ddev->GetRenderTargetData(pSrcSur, pTmpSur), "DXSurface::blit" , "error");

			// SYSTEM to DEFAULT
			hr = (pTexCanvas)->GetSurfaceLevel(0, &pCanvasSur) ;
			
			srcX = (srcX >= 0)? srcX : 0;
			srcY = (srcY >= 0)? srcY: 0;
			srcH = (srcH >= 0)? srcH: srcSurDesc.Height;
			srcW = (srcW >= 0)? srcW: srcSurDesc.Width;

			srcRect = new RECT();
			srcRect->bottom = srcY + srcH;
			srcRect->left = srcX;
			srcRect->right = srcX + srcW;
			srcRect->top = srcY;

			/*if( (srcX >= 0) && (srcY >= 0) && (srcH >= 0) && (srcW >= 0) ){
				srcRect->bottom = srcY + srcH;
				srcRect->left = srcX;
				srcRect->right = srcX + srcW;
				srcRect->top = srcY;
			}*/

			dstPoint.x = x;
			dstPoint.y = y;

			if(FAILED(pD3ddev->UpdateSurface(pTmpSur, srcRect, pCanvasSur, &dstPoint))){
				cout << "DX2DSurface::blit(...) : " << endl;
			}

			pCanvasSur->Release();
			pSrcSur->Release();
			pTmpSur->Release();
			tmpTex->Release();
			//srcTex->Release();
			delete srcRect;
	}

	void DX2DSurface::fill(int x, int y, int w, int h){
		LPDIRECT3DSURFACE9 pCanvasSur;
		RECT* rect = new RECT();
		rect->bottom = y + h;
		rect->left = x;
		rect->right = x + w;
		rect->top = y;

		if( (pTexCanvas != NULL) && (pTex != NULL)){
			DXCHECK((pTexCanvas)->GetSurfaceLevel(0, &pCanvasSur),"","") ;
			DXCHECK(pD3ddev->ColorFill(pCanvasSur, rect, color ),"","");
			pCanvasSur->Release();
		}
	}

	void DX2DSurface::update(){
		LPDIRECT3DTEXTURE9 oldPtex;

		//oldPtex = this->getTexture();
		this->setTexture(pTexCanvas);
		//oldPtex->Release();
		pTexCanvas->Release();
		pTexCanvas = NULL;

		if(pTexCanvas != NULL){
			LPDIRECT3DSURFACE9 pCanvasSur;

			DXCHECK((pTexCanvas)->GetSurfaceLevel(0, &pCanvasSur),"","") ;
			DXCHECK(pD3ddev->ColorFill(pCanvasSur, NULL, color ),"","");
			pCanvasSur->Release();
		}else{

			DXCHECK(pD3ddev->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &(pTexCanvas), NULL),"","");
		}

	}

	void DX2DSurface::drawString(int x, int y, const char* txt){
		IDirect3DSurface9*	texSur = NULL; // Texture Surface
		IDirect3DSurface9*	pCanvasSur = NULL;
		IDirect3DSurface9*	pSwpSur = NULL;
		LPDIRECT3DTEXTURE9	pTexSwp;
		POINT pt;
		int w = this->width;
		int h = this->height;

		pt.x = 0;
		pt.y = 0;
		if( (pTexFont == NULL)){
			pD3ddev->CreateTexture(w, h, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8 , D3DPOOL_SYSTEMMEM, &pTexFont, NULL);
		}

		if(FAILED(pD3ddev->CreateTexture(w, h, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8 , D3DPOOL_DEFAULT, &pTexSwp, NULL))){
			cout << "asdas" << endl;
		}

		if( (pTexFont != NULL ) && SUCCEEDED( pTexFont->GetSurfaceLevel(0, &texSur))){
			
			HDC hdc = 0;

            if (SUCCEEDED(texSur->GetDC(&hdc))){
				RECT rect = {x, y, w, h};
				SetTextColor(hdc, D3DCOLOR_ARGB(100, 255, 255, 255));
				SetBkColor(hdc, D3DCOLOR_ARGB(100, 0, 0, 0)); // fg color
				
				int txtSize = strlen(txt);
				WCHAR *wBuffer = new WCHAR [txtSize];
				mbstowcs(wBuffer, txt, txtSize);
				DrawTextW(hdc, wBuffer, txtSize, &rect, DT_LEFT|DT_TOP);
				texSur->ReleaseDC(hdc);

				
				if(SUCCEEDED( pTexSwp->GetSurfaceLevel(0, &pSwpSur) )){
				//	DXCHECK( pD3ddev->UpdateSurface(texSur, NULL, pCanvasSur,&pt), "", "");
					if( SUCCEEDED( pD3ddev->UpdateSurface(texSur, NULL, pSwpSur, NULL) )){
						if(SUCCEEDED(pTexCanvas->GetSurfaceLevel(0, &pCanvasSur))){
							RECT rectTwo = {0, 0, w, h};
							if( FAILED(pD3ddev->StretchRect(pSwpSur, NULL, pCanvasSur, &rectTwo, D3DTEXF_NONE)) ){
								cout << "asdas" << endl;
							}
							pCanvasSur->Release();
						}else{
							cout << "asdas" << endl;
						}
					}
					pSwpSur->Release();
					pTexSwp->Release();
				}
				
				texSur->Release();

			}else{
				cout << "" << endl;
			}
			
		}
	}
}
}
}
}
}
}
}
