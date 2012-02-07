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


#include "mb/interface/dx/input/DXEventBuffer.h"
#include "mb/interface/dx/input/DXInputEvent.h"
#include "mb/interface/dx/DXDeviceScreen.h"


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	set<IInputEvent*>* DXEventBuffer::userEventsPool = new set<IInputEvent*>();

	DXEventBuffer::DXEventBuffer() {
		clog << "DXEventBuffer::DXEventBuffer()" << endl;

		m_dInput			= NULL;
		m_dInputKeyBoard	= NULL;
		m_dInputMouse		= NULL;
		m_mouseHolding	= false;

		nEvt		= 0;

		m_cursorPos.x = 0;
		m_cursorPos.y = 0;
		
		m_scrWidth	= 0;
		m_scrHeight = 0;

		pthread_mutex_init(&m_mtxInput, NULL);
		pthread_mutex_init(&evt_lock, NULL);

		//pthread_cond_init(&evt_cond, NULL);

		initDirectInput();
	}

	DXEventBuffer::~DXEventBuffer() {
		clog << "DXEventBuffer::~DXEventBuffer()" << endl;

		pthread_mutex_lock(&m_mtxInput);
		if(m_dInput){

			m_dInputKeyBoard->Unacquire();
			m_dInputKeyBoard->Release();
			m_dInputKeyBoard = NULL;

			m_dInputMouse->Unacquire();
			m_dInputMouse->Release();
			m_dInputMouse = NULL;

			m_dInput->Release();
			m_dInput = NULL;
		}
		pthread_mutex_unlock(&m_mtxInput);
		pthread_mutex_destroy(&m_mtxInput);

		pthread_mutex_unlock(&evt_lock);
		pthread_mutex_destroy(&evt_lock);
	}

	void DXEventBuffer::wakeUp() {
		clog << "DXEventBuffer::wakeUp()" << endl;
		pthread_mutex_lock(&evt_lock);
	}

	void DXEventBuffer::postInputEvent(IInputEvent* evt) {
		clog << "DXEventBuffer::postInputEvent(IInputEvent* event)" << endl;

		if(evt->isKeyType()){

		}else if(evt->isMotionType()){

		}else if(evt->isApplicationType()){
			//DIDEVICEOBJECTDATA* didod = ((DIDEVICEOBJECTDATA*)evt->getContent());
			userEventsPool->insert(evt);
		}

	}

	void DXEventBuffer::waitEvent() {
		//clog << "DXEventBuffer::waitEvent()" << endl;
		pthread_mutex_lock(&evt_lock);
		Sleep(80);
		pthread_mutex_unlock(&evt_lock);
	}

	IInputEvent* DXEventBuffer::getNextEvent() {
		//clog << "DXEventBuffer::getNextEvent()" << endl;
		DIMOUSESTATE2 tmp_mouse;
		DWORD dwElements = 1;
		HRESULT hr = 0;

		pthread_mutex_lock(&m_mtxInput);

		ZeroMemory( &tmp_mouse, sizeof(tmp_mouse) );
		std::auto_ptr<DIDEVICEOBJECTDATA> pDidod (new DIDEVICEOBJECTDATA());

		if (!userEventsPool->empty()){
			IInputEvent* evt;
			set<IInputEvent*>::iterator it;
			it = userEventsPool->begin();
			evt = ((IInputEvent*)(*it));
			userEventsPool->erase(userEventsPool->begin());
			pthread_mutex_unlock(&m_mtxInput);
			return (evt);
		}

		if( m_dInputKeyBoard != NULL){

            hr = m_dInputKeyBoard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), pDidod.get(), &dwElements, 0 );

			if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)){
				hr = m_dInputKeyBoard->Acquire();

				while(hr == DIERR_INPUTLOST){
					hr = m_dInputKeyBoard->Acquire();
				}

			}

			if( hr == DI_OK ){

				if(pDidod->dwOfs != NULL){
					printf( "---> 0x%02x [%s] \n", pDidod->dwOfs, (pDidod->dwData & 0x80) ? "D" : "U");
					pthread_mutex_unlock(&m_mtxInput);
					return new DXInputEvent(pDidod.release());
				}
			}
			pthread_mutex_unlock(&m_mtxInput);
		}

		if( m_dInputMouse != NULL){
			POINT curPt;

			POINT leftTopPt;
			POINT rightBotPt;

			leftTopPt.x = 0;
			leftTopPt.y = 0;

			rightBotPt.x = m_scrWidth;
			rightBotPt.y = m_scrHeight;

			hr = m_dInputMouse->GetDeviceState(sizeof(tmp_mouse),&tmp_mouse);

			if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)){
				m_dInputMouse->Acquire();
				
				while(hr == DIERR_INPUTLOST){
					hr = m_dInputMouse->Acquire();
				}
			}
			
			GetCursorPos(&curPt);
			ScreenToClient(m_hwndC, &curPt);
			ClientToScreen(m_hwndC, &rightBotPt);

			m_cursorPos.x = curPt.x;
			m_cursorPos.y = curPt.y;

			if(m_cursorPos.x < 0){
				m_dInputMouse->Unacquire();
				pthread_mutex_unlock(&m_mtxInput);
				return NULL;
			}else{
				if(m_cursorPos.x > m_scrWidth){
					m_dInputMouse->Unacquire();
					pthread_mutex_unlock(&m_mtxInput);
					return NULL;
				}
			}

			if(m_cursorPos.y < 0){
				m_dInputMouse->Unacquire();
				pthread_mutex_unlock(&m_mtxInput);
				return NULL;
			}else{
				if(m_cursorPos.y > m_scrHeight){
					m_dInputMouse->Unacquire();
					pthread_mutex_unlock(&m_mtxInput);
					return NULL;
				}
			}

			tmp_mouse.lX = curPt.x;
			tmp_mouse.lY = curPt.y;

			if(((tmp_mouse.rgbButtons[0] & 0x80) || (tmp_mouse.rgbButtons[1] & 0x80)) ){
				if(!m_mouseHolding){
					m_mouseHolding = true;
					pthread_mutex_unlock(&m_mtxInput);
					return new DXInputEvent(tmp_mouse); // button (left/right) pressed
				}
			}else{
				m_mouseHolding = false;
				pthread_mutex_unlock(&m_mtxInput);
				return new DXInputEvent(tmp_mouse); // just xy axis moving
			}
			pthread_mutex_unlock(&m_mtxInput);
		}

		pthread_mutex_unlock(&m_mtxInput);

		return NULL;
	}

	void* DXEventBuffer::getContent() {
		clog << "DXEventBuffer::getContent()" << endl;
		return NULL;
	}
	void DXEventBuffer::initDirectInput(){

		LPDIRECT3DDEVICE9		pD3dDev		= NULL;
		LPDIRECT3DSWAPCHAIN9	pSwapChain	= NULL;
		D3DPRESENT_PARAMETERS	dPresentParam;

		HINSTANCE hInstance;
		HRESULT keybHr = 0;
		HRESULT mouseHr = 0;

		Sleep(1000);
		memset(&dPresentParam, 0, sizeof(D3DPRESENT_PARAMETERS));
		pD3dDev = (LPDIRECT3DDEVICE9)(LocalScreenManager::getInstance())->getGfxRoot(0);

		if(pD3dDev){
			pD3dDev->GetSwapChain(0, &pSwapChain); // Default SwapChain
			if(pSwapChain){
				pSwapChain->GetPresentParameters(&dPresentParam);
			}else{
				clog << "DXEventBuffer: error - pSwapChain is NULL" << endl;
			}
		}else{
			clog << "DXEventBuffer: error - pD3dDev is NULL" << endl;
		}

		m_scrWidth	= (LocalScreenManager::getInstance())->getDeviceWidth();	
		m_scrHeight = (LocalScreenManager::getInstance())->getDeviceHeight(); 
		m_hwndC		= dPresentParam.hDeviceWindow;
 		hInstance	= (HINSTANCE)GetModuleHandle(NULL);

		DIPROPDWORD dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = 8;
		
		keybHr = DirectInput8Create( hInstance,DIRECTINPUT_VERSION ,IID_IDirectInput8, (void**)&m_dInput, NULL);

		if(FAILED(keybHr)){
			clog << "DXEventBuffer: error - m_dInput is NULL" << endl;
		}
		
		keybHr = m_dInput->CreateDevice( GUID_SysKeyboard, &m_dInputKeyBoard, NULL);

		if(FAILED(keybHr)){
			clog << "DXEventBuffer: error - can not create  m_dInputKeyBoard device" << endl;
		}

		keybHr = m_dInputKeyBoard->SetDataFormat(&c_dfDIKeyboard);

		if(FAILED(keybHr)){
			clog << "DXEventBuffer: error - can not set dInputKeyBoard data format" << endl;
		}

		keybHr = m_dInputKeyBoard->SetCooperativeLevel(m_hwndC, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		// DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
		if(FAILED(keybHr)){
			clog << "DXEventBuffer: error - can not set dInputKeyBoard Cooperative Level" << endl;
		}

		keybHr = m_dInputKeyBoard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

		if(FAILED(keybHr)){
			clog << "error - DirectInput8Create" << endl;
		}

		m_dInputKeyBoard->Acquire();

		if(FAILED(keybHr)){
			clog << "error - DirectInput8Create" << endl;
		}

		mouseHr = m_dInput->CreateDevice( GUID_SysMouse, &m_dInputMouse, NULL);

		if(FAILED(mouseHr)){
			clog << "error - DirectInput8Create" << endl;
		}

		mouseHr = m_dInputMouse->SetDataFormat(&c_dfDIMouse2);

		if(FAILED(mouseHr)){
			clog << "error - DirectInput8Create" << endl;
		}

		mouseHr = m_dInputMouse->SetCooperativeLevel(m_hwndC, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );

		if(FAILED(mouseHr)){
			clog << "error - DirectInput8Create" << endl;
		}
		
		m_dInputMouse->Acquire();
		if(FAILED(mouseHr)){
			clog << "error - DirectInput8Create" << endl;
		}
	
		/*
		POINT curPt;
		GetCursorPos(&curPt);
		m_cursorX = curPt.x;
		m_cursorY = curPt.y;
		*/
	}
}
}
}
}
}
}
