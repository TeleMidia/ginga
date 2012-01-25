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

#include "mb/interface/dx/input/DXInputEvent.h"
#include "mb/interface/CodeMap.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	DXInputEvent::DXInputEvent(void* event) {
		//clog << "DXInputEvent::DXInputEvent(void* event)" << endl;
		DXInputEvent* evt = (DXInputEvent*) event;

		this->pDidod = new DIDEVICEOBJECTDATA;
                std::memcpy(pDidod, evt->getContent(), sizeof(*pDidod));
		this->data = evt->getData();
		//this->m_mouseState = NULL;

	}

	DXInputEvent::DXInputEvent(const int keyCode) {
		//clog << "DXInputEvent::DXInputEvent(const int keyCode)" << endl;
		pDidod = new DIDEVICEOBJECTDATA();
		pDidod->dwOfs = 0xFF;
		//this->m_mouseState = NULL;
	}

	DXInputEvent::DXInputEvent(void* data, int type){
		pDidod = new DIDEVICEOBJECTDATA();
		pDidod->dwOfs = 0xFF;
		pDidod->dwData = DWORD("EVENTOUSUARIO");
		this->data = data;
		//this->m_mouseState = NULL;
	}

	DXInputEvent::DXInputEvent(int clazz, int type) {
		//clog << "DXInputEvent::DXInputEvent(int clazz, int type)" << endl;
		pDidod = new DIDEVICEOBJECTDATA();
		pDidod->dwOfs = 0xFF;
		pDidod->dwData = DWORD("EVENTOUSUARIO");
		//this->m_mouseState = NULL;
	}

	DXInputEvent::DXInputEvent(LPDIDEVICEOBJECTDATA diDevObjData){
		//clog << "DXInputEvent::DXInputEvent(LPDIDEVICEOBJECTDATA pDidod)" << endl;
		pDidod = diDevObjData;
		//this->m_mouseState = NULL;
	}

	DXInputEvent::DXInputEvent(DIMOUSESTATE2 mState){
		pDidod = NULL;
		m_mouseState = mState;
	}

	DXInputEvent::DXInputEvent(BYTE keyCode[256]) {
		//clog << "DXInputEvent::DXInputEvent(BYTE keyCode)" << endl;
	}

	DXInputEvent::~DXInputEvent() {
          delete pDidod;
		//clog << "DXInputEvent::~DXInputEvent()" << endl;
	}

	void DXInputEvent::clearContent() {
		//clog << "DXInputEvent::clearContent()" << endl;
	}

	void* DXInputEvent::getContent() {
		//clog << "DXInputEvent::getContent()" << endl;
		if(pDidod != NULL){
			return pDidod;
		}else{
			return &m_mouseState;
		}
	}

	void DXInputEvent::setKeyCode(const int keyCode) {
		//clog << "DXInputEvent::setKeyCode(const int keyCode)" << endl;
	}

	const int DXInputEvent::getKeyCode() {
		clog << "DXInputEvent::getKeyCode()" << endl;
		if(pDidod != NULL){
			return pDidod->dwOfs;
		}else{
			return DIMOFS_BUTTON0;
			//return m_mouseState.rgbButtons
		}
	}

	void DXInputEvent::setType(unsigned int type) {
		//clog << "DXInputEvent::setType(unsigned int type)" << endl;
	}

	unsigned int DXInputEvent::getType() {
		//clog << "DXInputEvent::getType()" << endl;
		
		if( isButtonPressType() || isMotionType() ||  isPressedType() || isKeyType()){
			return 0;
		}else{
			return 1;
		}

		return 0;
	}

	bool DXInputEvent::isButtonPressType() {
		//clog << "DXInputEvent::isButtonPressType()" << endl;

		if(pDidod != NULL){
			return false;
		}else{
			if( (m_mouseState.rgbButtons[0] & 0x80) || (m_mouseState.rgbButtons[1] & 0x80) || (m_mouseState.rgbButtons[2] & 0x80) ){
				return true;
			}
		}

		return false;

/*		if( (pDidod->dwOfs < DIMOFS_BUTTON7) && (pDidod->dwOfs > DIMOFS_BUTTON0) ){
			return true;
		}else{
			return false;
		}*/
	}

	bool DXInputEvent::isMotionType() {
		//clog << "DXInputEvent::isMotionType()" << endl;
		if(pDidod != NULL){
			return false;
		}else{
			if( (m_mouseState.lX != 0) || (m_mouseState.lY != 0) || (m_mouseState.lZ != 0) ){
				if( isButtonPressType()){
					return false;
				}else{
					return true;
				}
			}
		}
		return false;
		/*if( (pDidod->dwOfs == DIMOFS_X) || (pDidod->dwOfs == DIMOFS_Y) || (pDidod->dwOfs == DIMOFS_Z) ){
			return true;
		}else{
			return false;
		}*/
	}

	bool DXInputEvent::isPressedType() {
		//clog << "DXInputEvent::isPressedType()" << endl;
		if( pDidod != NULL && (pDidod->dwOfs < 0xED) && (pDidod->dwOfs > 0x01) ){
			if(pDidod->dwData & 0x80){
				return true; // "D"
			}else{
				return false; // "U"
			}
		}else{
			return false;
		}
	}

	bool DXInputEvent::isKeyType() {
		//clog << "DXInputEvent::isKeyType()" << endl;
		if( (pDidod != NULL) && (pDidod->dwOfs < 0xED) && (pDidod->dwOfs > 0x01) ){
			return true;
		}else{
			return false;
		}
	}

	bool DXInputEvent::isUserClass() {
		//clog << "DXInputEvent::isUserClass()" << endl;
		if( (pDidod != NULL) && (pDidod->dwOfs == 0xFF)){
			return true;
		}
		return false;
	}

	void DXInputEvent::setAxisValue(int x, int y, int z) {
		clog << "DXInputEvent::setAxisValue(int x, int y, int z)" << endl;
	}

	void DXInputEvent::getAxisValue(int* x, int* y, int* z) {
		//clog << "DXInputEvent::getAxisValue(int* x, int* y, int* z)" << endl;

		(*x) = m_mouseState.lX;
		(*y) = m_mouseState.lY;
		//(*z) = pMouseState->lZ;
	}

	void* DXInputEvent::getData(){
		return data;
	}
}
}
}
}
}
}

using namespace ::br::pucrio::telemidia::ginga::core::mb;

#ifdef _WIN32
extern "C" IInputEvent* createDXInputEvent(void* event, const int symbol) {
#else
extern "C" IInputEvent* createDFBInputEvent(void* event, const int symbol) {
#endif
	if (event != NULL) {
		return new DXInputEvent(event);
	}

	if (symbol >= 0) {
		return new DXInputEvent(symbol);
	}

	return NULL;
}

extern "C" IInputEvent* createDXUserEvent(int type, void* data) {

	return new DXInputEvent(data, type);
}

extern "C" void destroyDXInputEvent(IInputEvent* eb) {
	delete eb;
}
