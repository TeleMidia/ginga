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

#include "mb/LocalDeviceManager.h"
#include "mb/interface/dx/content/video/DXVideoProvider.h"
#include "mb/interface/dx/output/DXSurface.h"



namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	DXVideoProvider::DXVideoProvider(const char* mrl) {
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		JCriticalSection crit;
		clog << "DXVideoProvider::DXVideoProvider(" << mrl << ")" << endl;
		pD3ddev = NULL;

		pD3ddev = (IDirect3DDevice9 *)(LocalDeviceManager::getInstance()->getGfxRoot());

		m_pVideo.reset(new VideoGraph(mrl, pD3ddev));
		
		m_pVideo->play();
		videoTex = m_pVideo->getVMRTexture();
	}

	DXVideoProvider::~DXVideoProvider() {
		clog << "DXVideoProvider::~DXVideoProvider()" << endl;
		//m_pVideo->stop();
	}

	void* DXVideoProvider::getContent() {
		clog << "DXVideoProvider::getContent()" << endl;
		return NULL;
	}

	ISurface* DXVideoProvider::getPerfectSurface() {
		clog << "DXVideoProvider::getPerfectSurface()" << endl;
		DX2DSurfaceProp surProp;

		//IDirect3DTexture9 * tex = vmr->GetTexture();
		IDirect3DTexture9 * tex = m_pVideo->getVMRTexture();
		if(tex != NULL){
			D3DSURFACE_DESC desc;
			tex->GetLevelDesc(0, &desc);
			surProp.x = 0;
			surProp.y = 0;
			surProp.z = 1.0;
			surProp.width = desc.Width;
			surProp.height = desc.Height;
		}

		DX2DSurface* sur = (DX2DSurface*)LocalDeviceManager::getInstance()->createSurface(&surProp);
		sur->replaceTex(videoTex);

		return new DXSurface(sur);
	}

	void DXVideoProvider::dynamicRenderCallBack(void* rendererContainer) {
		clog << "DXVideoProvider::dynamicRenderCallBack(void* rendererContainer)" << endl;
	}

	void DXVideoProvider::getOriginalResolution(int* height, int* width) {
		clog << "DXVideoProvider::getOriginalResolution(int* height, int* width)" << endl;
//		RECT rect =  m_pVideo->getOiginalResolution();

		(*height) = 100; //rect.bottom;
		(*width) = 100; //rect.left;
	}

	double DXVideoProvider::getTotalMediaTime() {
//		double total = vmr->getTotalMediaTime();
		double total = m_pVideo->getTotalMediaTime();
		clog << "DXVideoProvider::getTotalMediaTime -> " << total  << endl;
		return total;
	}

	double DXVideoProvider::getMediaTime() {
		double mediaTime = m_pVideo->getMediaTime();
		clog << "DXVideoProvider::getMediaTime -> " << mediaTime  << endl;
		return mediaTime;
	}

	void DXVideoProvider::setMediaTime(double pos) {
		clog << "DXVideoProvider::setMediaTime(double pos)" << endl;
		m_pVideo->seek(pos);
	}

	void DXVideoProvider::playOver(
			ISurface* surface, bool hasVisual, IProviderListener* listener) {
		clog << "DXVideoProvider::playOver(ISurface* surface, bool hasVisual, IProviderListener* listener)" << endl;
		DX2DSurface*	s = (DX2DSurface*)(surface->getContent());
		//s->setTexture(videoTex);
		
		m_pVideo->play();
	}

	void DXVideoProvider::resume(ISurface* surface, bool hasVisual) {
		clog << "DXVideoProvider::resume(ISurface* surface, bool hasVisual)" << endl;
		playOver(surface, hasVisual, 0);
	}

	void DXVideoProvider::pause() {
		clog << "DXVideoProvider::pause()" << endl;
		m_pVideo->pause();
	}

	void DXVideoProvider::stop() {
		clog << "DXVideoProvider::stop()" << endl;
		m_pVideo->stop();
	}

	void DXVideoProvider::setSoundLevel(float level) {
		clog << "DXVideoProvider::setSoundLevel(float level)" << endl;
	}

	bool DXVideoProvider::releaseAll() {
		clog << " DXVideoProvider::releaseAll()" << endl;
		return true;
	}

	void DXVideoProvider::setAVPid(int aPid, int vPid){

	}
	
	void DXVideoProvider::feedBuffers(){

	}

	bool DXVideoProvider::checkVideoResizeEvent(ISurface* frame){
		return true;
	}

	int64_t DXVideoProvider::getVPts(){
		return 0;
	}
}
}
}
}
}
}

#ifdef _WIN32
extern "C" ::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* createDXVideoProvider(const char* mrl) {
#else
extern "C" ::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* createDXVideoProvider(const char* mrl) {
#endif
	return (new ::br::pucrio::telemidia::ginga::core::mb::
			DXVideoProvider(mrl));
}

#ifdef _WIN32
extern "C" void destroyDXVideoProvider(
		::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* cmp) {
#else
extern "C" void destroyDXVideoProvider(
		::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* cmp) {
#endif

	delete cmp;
}
