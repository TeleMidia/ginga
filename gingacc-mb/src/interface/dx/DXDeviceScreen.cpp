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


#include "mb/interface/dx/DXDeviceScreen.h"

#include <boost/thread.hpp>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	LRESULT CALLBACK  DXDeviceScreen::wndProc(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam){

		switch (message){
			case WM_DESTROY:
				PostQuitMessage(0);
			break;
		}

		return (DefWindowProc(hWnd, message, wParam, lParam));
	}

	DXDeviceScreen::DXDeviceScreen(int numArgs, char* args[]) 
          : Thread(), release_run(false), released(false)
        {
		clog << "DXDeviceScreen::DXDeviceScreen(int numArgs, char* args[])" << endl;
		this->dxHdc = NULL;
		this->pD3d = NULL;
		this->pD3dDev = NULL;
		this->m_running = true;
		if((numArgs > 0) && (args != NULL)){
			int tmp = atoi(args[0]);

			this->dxHwnd = reinterpret_cast<HWND>(tmp);
			this->width  = atoi(args[1]);
			this->height = atoi(args[2]);

		}else{
			this->dxHwnd = NULL;
			this->width  = 800;
			this->height = 600;
		}

		init = 0;

		windowPool = new set<DX2DSurface*>;
		surfacePool = new set<DX2DSurface*>;

		pthread_mutex_init(&winMutex, NULL);
		pthread_mutex_init(&surMutex, NULL);
		pthread_mutex_init(&init_lock, NULL);

		pthread_cond_init(&init_cond, NULL);

		Thread::start();

		pthread_mutex_lock(&init_lock);
		while (init != 1) {
			pthread_cond_wait(&init_cond, &init_lock);
		}
		pthread_mutex_unlock(&init_lock);

	}

	DXDeviceScreen::~DXDeviceScreen() {
		clog << "DXDeviceScreen::~DXDeviceScreen()" << endl;
		m_running = false;
		Sleep(1000);
                {
                  boost::unique_lock<boost::mutex> lock(release_run_mutex);
                  release_run = true;
                }

                {
                  boost::unique_lock<boost::mutex> lock(released_mutex);
                  
                  while(!released)
                    released_cond.wait(lock);
                }

		set<DX2DSurface*>::iterator w;
		for (w = windowPool->begin(); w != windowPool->end(); ++w) {
			if ((*w) != NULL) {
				//(*w)->Release();
			}
		}
		windowPool->clear();
		delete windowPool;
		windowPool = NULL;
		pthread_mutex_unlock(&winMutex);
		pthread_mutex_destroy(&winMutex);

		//Releasing still Surface objects in Surface Pool
		set<DX2DSurface*>::iterator s;
		for (s = surfacePool->begin(); s != surfacePool->end(); ++s) {
			if ((*s) != NULL) {
				//(*s)->Release();
			}
		}
		surfacePool->clear();
		delete surfacePool;
		surfacePool = NULL;
		pthread_mutex_unlock(&surMutex);
		pthread_mutex_destroy(&surMutex);

	}

	unsigned int DXDeviceScreen::getWidthResolution() {
		clog << "DXDeviceScreen::getWidthResolution()" << endl;
		return width;
	}

	void DXDeviceScreen::setWidthResolution(unsigned int wRes) {
		clog << "DXDeviceScreen::setWidthResolution(unsigned int wRes)" << endl;
	}

	unsigned int DXDeviceScreen::getHeightResolution() {
		clog << "DXDeviceScreen::getHeightResolution()" << endl;
		return height;
	}

	void DXDeviceScreen::setHeightResolution(unsigned int hRes) {
		clog << "DXDeviceScreen::setHeightResolution(unsigned int hRes)" << endl;
	}

	void DXDeviceScreen::setColorKey(int r, int g, int b) {
		clog << "DXDeviceScreen::setColorKey(int r, int g, int b)" << endl;
	}

	void* DXDeviceScreen::createWindow(void* desc) {
		clog << "DXDeviceScreen::createWindow(void* desc)" << endl;
		DX2DSurface* window = NULL;
		DX2DSurfaceProp *surProp = (DX2DSurfaceProp*) desc;


		if (pD3dDev != NULL) {
			
			window = new DX2DSurface(surProp->x, surProp->y, surProp->width, surProp->height, pD3dDev);

			pthread_mutex_lock(&winMutex);
			windowPool->insert(window);
			pthread_mutex_unlock(&winMutex);
		}

		return (void*)window;
	}

	void DXDeviceScreen::releaseWindow(void* win) {
		clog << "DXDeviceScreen::releaseWindow(void* win)" << endl;
		set<DX2DSurface*>::iterator i;
		DX2DSurface* w;
		w = (DX2DSurface*)win;

		pthread_mutex_lock(&winMutex);
		i = windowPool->find(w);
		if (i != windowPool->end()) {
			windowPool->erase(i);
			pthread_mutex_unlock(&winMutex);

		} else {
			pthread_mutex_unlock(&winMutex);
			return;
		}

		win = NULL;
		w = NULL;
	}

	void* DXDeviceScreen::createSurface(void* desc) {
		clog << "DXDeviceScreen::createSurface(void* desc)" << endl;
		DX2DSurface* surface = NULL;
		DX2DSurfaceProp *surProp = (DX2DSurfaceProp*) desc;

		if(pD3dDev != NULL){

			surface = new DX2DSurface(surProp->x, surProp->y, surProp->width, surProp->height, pD3dDev);

			pthread_mutex_lock(&surMutex);
			surfacePool->insert(surface);
			pthread_mutex_unlock(&surMutex);
		}
		return (void*)surface;
	}

	void DXDeviceScreen::releaseSurface(void* sur) {
		clog << "DXDeviceScreen::releaseSurface(void* sur)" << endl;
		set<DX2DSurface*>::iterator i;
		DX2DSurface* s;
		s = (DX2DSurface*)sur;

		pthread_mutex_lock(&surMutex);
		i = surfacePool->find(s);
		if (i != surfacePool->end()) {
			surfacePool->erase(i);
			pthread_mutex_unlock(&surMutex);

		} else {
			pthread_mutex_unlock(&surMutex);
			return;
		}

		s = NULL;
		sur = NULL;
	}

	void* DXDeviceScreen::getGfxRoot() {
		clog << "DXDeviceScreen::getGfxRoot()" << endl;
		return (this->pD3dDev);
	}

	void DXDeviceScreen::run(){
		clog << "DXDeviceScreen::run()" << endl;

		HINSTANCE dxHinstance;
		WNDCLASSEX wndclass;

		MSG msg;

		dxHinstance = GetModuleHandle(NULL);

		wndclass.cbSize = sizeof(WNDCLASSEX);
		wndclass.style  = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = (WNDPROC)wndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = dxHinstance;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = TEXT("GingaWindowClass");
		wndclass.hIconSm = 0;

		RegisterClassEx(&wndclass);

		if(dxHwnd == NULL){
			dxHwnd = CreateWindow(TEXT("GingaWindowClass"),TEXT("GingaWindows"),
										WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
										width, height, NULL, NULL, dxHinstance, NULL);
		}

		if( this->pD3d == NULL && this->pD3dDev == NULL){
			d3dInit();
		}


		ShowWindowAsync(dxHwnd, SW_SHOWNORMAL);
		UpdateWindow(dxHwnd);

		clog << "DXDeviceScreen::run() -> " << dxHwnd << endl;

		pthread_mutex_lock(&init_lock);
		init = 1;
		pthread_mutex_unlock(&init_lock);

		pthread_cond_broadcast(&init_cond);

		ZeroMemory( &msg, sizeof( msg ) );
                {
                  boost::unique_lock<boost::mutex> release_lock(release_run_mutex);
                  while( !release_run && msg.message != WM_QUIT )
                  {
                    release_lock.unlock();
                    if(PeekMessage(&msg, dxHwnd, 0, 0, PM_REMOVE))
                    {
                      TranslateMessage( &msg );
                      DispatchMessage( &msg );
                    }
                    else
                    {
                      Sleep(20);
					  if(m_running){
						  render();
					  }
                    }
                    if( msg.message == WM_MOVING){  
                      //render();
                    }
                    release_lock.lock();
                  }
                }

                {
                  boost::unique_lock<boost::mutex> lock(released_mutex);
                  released = true;
                  released_cond.notify_one();
                }

/*		if( this->pD3d == NULL && this->pD3dDev == NULL){
			d3dInit();
		}

		clog << "DXDeviceScreen::run() -> " << dxHwnd << endl;

		init = 1;
		pthread_cond_broadcast(&init_cond);

		while(1){
			Sleep(100);
			render();
		}
*/
	}

	void DXDeviceScreen::render(){

		if(this->pD3dDev == NULL || this->pD3d == NULL)
			return;

		set<DX2DSurface*>::iterator w;
		set<DX2DSurface*>::iterator k;

		if(SUCCEEDED((this->pD3dDev)->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0 ))){
		//	clog << "clearok" << endl;
		}else{
			clog << "falhou" << endl;
		}

		srand ( time(NULL) );
		
		if(SUCCEEDED((this->pD3dDev)->BeginScene())){
			pthread_mutex_lock(&winMutex);
			if( (windowPool != NULL) && !windowPool->empty()){
				for (k = windowPool->begin(); k != windowPool->end(); ++k) {
					
					if ((*k) != NULL) {
						(*k)->draw2DSurface();
					}
					
				}
			}
			pthread_mutex_unlock(&winMutex);
			
			pthread_mutex_lock(&surMutex);
			if(!surfacePool->empty()){
				for (w = surfacePool->begin(); w != surfacePool->end(); ++w) {
					
					if ((*w) != NULL) {
						(*w)->draw2DSurface();
					}
					
				}
			}
			pthread_mutex_unlock(&surMutex);
			
			(this->pD3dDev)->EndScene();
		}

		if(!surfacePool->empty() || !windowPool->empty()){
			(this->pD3dDev)->Present(NULL, NULL, NULL, NULL); 
		}
	}


	bool DXDeviceScreen::d3dInit(){
		clog << "DXDeviceScreen::d3dInit()" << endl;

		D3DDISPLAYMODE dspMode;

		if( (pD3d = Direct3DCreate9(D3D_SDK_VERSION)) == NULL){
			clog << "nao consegui iniciar o dx v: " << D3D9b_SDK_VERSION << endl;		
			return false;
		}

		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		
		d3dpp.Windowed = true;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3dpp.BackBufferCount = 1;
		d3dpp.BackBufferHeight = 0;
		d3dpp.BackBufferWidth = 0;
		d3dpp.hDeviceWindow = dxHwnd;
		
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		//d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;


		if(FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, dxHwnd,
									 D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED, &d3dpp,&pD3dDev))){
			clog << " NAO CONSEGUI CRIAR O Direct3dDev D3DDEVTYPE_HAL " << endl;
			return false;
		}

		// Alpha Bleding and Z-Index Capabilities
		pD3dDev->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
		//pD3dDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);    // both sides of the triangles
		pD3dDev->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
		//pD3dDev->SetRenderState ( D3DRS_ZFUNC, D3DCMP_GREATEREQUAL ) ;

        pD3dDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
        pD3dDev->SetRenderState(D3DRS_ALPHAREF, (DWORD)0);
        pD3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 

		// keep the alpha blending in
		pD3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);    // turn on the color blending
		pD3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);    // set source factor
		pD3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);    // set dest factor
		pD3dDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);    // set the operation

		
		/*pD3dDev->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
		pD3dDev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
		pD3dDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		pD3dDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);*/

		pD3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dspMode);
		//width = this->dspMode.Width;
		//height = dspMode.Height;

		return true;
	}

	HWND DXDeviceScreen::getHwnd(){
		return dxHwnd;
	}

	void DXDeviceScreen::setBackgroundImage(string uri){

	}
}
}
}
}
}
}

extern "C"  ::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen*
		createDXScreen(int numArgs, char* args[]) {
	return (new ::br::pucrio::telemidia::ginga::core::mb::
			DXDeviceScreen(numArgs, args));
}
extern "C" void destroyDXScreen(
		::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen* ds) {
	delete ds;
}
