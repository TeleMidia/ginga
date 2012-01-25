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
#include "mb/interface/dx/content/audio/DXAudioProvider.h"
#include "mb/interface/dx/output/DXSurface.h"



namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	DXAudioProvider::DXAudioProvider(const char* mrl) {
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		JCriticalSection crit;
		clog << "DXAudioProvider::DXAudioProvider(" << mrl << ")" << endl;
		
		IDirect3DDevice9* pD3ddev = (IDirect3DDevice9 *)(LocalDeviceManager::getInstance()->getGfxRoot());
		
		m_pAudio = new AudioGraph(mrl, pD3ddev);
		
		if(m_pAudio){
			m_pAudio->play();
		}
		//pD3ddev = NULL;

		//pD3ddev = (IDirect3DDevice9 *)(LocalDeviceManager::getInstance()->getGfxRoot());
		//vmr = new eVMR3(pD3ddev, false, 10, 10, 0.1);
	
		//if(vmr->RenderVideo( LPSTR(mrl)))
		//	vmr->Start();
		
		//videoTex = vmr->GetTexture();
	}

	DXAudioProvider::~DXAudioProvider() {
		clog << "DXAudioProvider::~DXAudioProvider()" << endl;
//		vmr->Stop();
	}

	void* DXAudioProvider::getContent() {
		clog << "DXAudioProvider::getContent()" << endl;
		return NULL;
	}

	ISurface* DXAudioProvider::getPerfectSurface() {
		clog << "DXAudioProvider::getPerfectSurface()" << endl;
		DX2DSurfaceProp surProp;

//		IDirect3DTexture9 * tex = vmr->GetTexture();
		IDirect3DTexture9 * tex = NULL;

		if(tex != NULL){
			D3DSURFACE_DESC desc;
			tex->GetLevelDesc(0, &desc);
			surProp.x = 0;
			surProp.y = 0;
			surProp.width = 1;
			surProp.height = 1;
		}else{
			surProp.x = 0;
			surProp.y = 0;
			surProp.width = 1;
			surProp.height = 1;
		}

		DX2DSurface* sur = (DX2DSurface*)LocalDeviceManager::getInstance()->createSurface(&surProp);
		sur->setTexture(tex);

		return new DXSurface(sur);
	}

	void DXAudioProvider::dynamicRenderCallBack(void* rendererContainer) {
		clog << "DXAudioProvider::dynamicRenderCallBack(void* rendererContainer)" << endl;
	}

	void DXAudioProvider::getOriginalResolution(int* height, int* width) {
		clog << "DXAudioProvider::getOriginalResolution(int* height, int* width)" << endl;
		(*height) = 200;
		(*width) = 200;
	}

	double DXAudioProvider::getTotalMediaTime() {
		clog << "DXAudioProvider::getTotalMediaTime()" << endl;
		double totalMediaTime = m_pAudio->getTotalMediaTime();
		return totalMediaTime;
	}

	double DXAudioProvider::getMediaTime() {
		double mediaTime = m_pAudio->getMediaTime();
		clog << "DXAudioProvider::getMediaTime" << mediaTime  << endl;
		return mediaTime;
	}

	void DXAudioProvider::setMediaTime(double pos) {
		clog << "DXAudioProvider::setMediaTime(double pos)" << endl;
	}

	void DXAudioProvider::playOver(
			ISurface* surface, bool hasVisual, IProviderListener* listener) {
		clog << "DXAudioProvider::playOver(ISurface* surface, bool hasVisual, IProviderListener* listener)" << endl;
		DX2DSurface*	s = (DX2DSurface*)(surface->getContent());
		// s->setTexture(videoTex);
		
		
	}

	void DXAudioProvider::resume(ISurface* surface, bool hasVisual) {
		clog << "DXAudioProvider::resume(ISurface* surface, bool hasVisual)" << endl;
		playOver(surface, hasVisual, 0);
	}

	void DXAudioProvider::pause() {
		clog << "DXAudioProvider::pause()" << endl;
		stop();
	}

	void DXAudioProvider::stop() {
		clog << "DXAudioProvider::stop()" << endl;
		//vmr->Stop();
	}

	void DXAudioProvider::setSoundLevel(float level) {
		clog << "DXAudioProvider::setSoundLevel(float level)" << endl;
	}

	bool DXAudioProvider::releaseAll() {
		clog << " DXAudioProvider::releaseAll()" << endl;
		return true;
	}

	void DXAudioProvider::setAVPid(int aPid, int vPid){

	}
	
	void DXAudioProvider::feedBuffers(){

	}

	bool DXAudioProvider::checkVideoResizeEvent(ISurface* frame){
		return true;
	}

	int64_t DXAudioProvider::getVPts(){
		return 0;
	}

}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* createDXAudioProvider(const char* mrl) {
	return (new ::br::pucrio::telemidia::ginga::core::mb::
			DXAudioProvider(mrl));
}

extern "C" void destroyDXAudioProvider(
		::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* cmp) {
	delete cmp;
}
