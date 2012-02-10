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

#ifndef DXDEVICESCREEN_H_
#define DXDEVICESCREEN_H_

#include <d3d9.h>
#include <d3dx9tex.h>
#include <D3D9Types.h>

#include "mb/interface/IDeviceScreen.h"
#include "mb/interface/dx/output/DX2DSurface.h"

#include <pthread.h>
#include <set>
#include <iostream>
using namespace std;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

//#include <windows.h>

#include <iostream>
using namespace std;

#include <boost/thread.hpp>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class DXDeviceScreen : public IDeviceScreen, public Thread {
		
		protected:
			HWND dxHwnd;
		private:
			HDC	 dxHdc;

			IDirect3D9			*pD3d;
			IDirect3DDevice9	*pD3dDev;

			int width;
			int height;

			set<DX2DSurface*>* windowPool;
			set<DX2DSurface*>* surfacePool;

			pthread_mutex_t winMutex;
			pthread_mutex_t surMutex;
			pthread_mutex_t init_lock;

			pthread_cond_t init_cond;
			bool				m_running;
			int init;

		public:
			DXDeviceScreen(int numArgs=0, char* args[]=NULL);
			~DXDeviceScreen();

			static LRESULT CALLBACK wndProc(HWND hWnd, UINT message,
											WPARAM wParam, LPARAM lParam);

			void releaseScreen();
			void releaseMB();

			void refreshScreen(){};

			unsigned int getWidthResolution();
			void setWidthResolution(unsigned int wRes);
			unsigned int getHeightResolution();
			void setHeightResolution(unsigned int hRes);
			void setColorKey(int r, int g, int b);
			void* createWindow(void* desc);
			void releaseWindow(void* win);
			void* createSurface(void* desc);
			void releaseSurface(void* sur);
			void* getGfxRoot();
			void setBackgroundImage(string uri);
	
			void mergeIds(int destId, vector<int>* srcIds){};

			HWND getHwnd();
		private:
			void run();
			bool d3dInit();
			void render();

                        boost::mutex release_run_mutex;
                        bool release_run;

                        boost::mutex released_mutex;
                        boost::condition_variable released_cond;
                        bool released;
	};
}
}
}
}
}
}

#endif /*DXDEVICESCREEN_H_*/
