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

#include "config.h"
#include "mb/interface/term/TermDeviceScreen.h"
#include "mb/ILocalScreenManager.h"

#include <string.h>
#include <stdlib.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	TermDeviceScreen::TermDeviceScreen(
			int numArgs, char** args, GingaWindowID parentId) {

	}

	TermDeviceScreen::~TermDeviceScreen() {

	}

	void TermDeviceScreen::releaseScreen() {
		clearWidgetPools();
	}

	void TermDeviceScreen::releaseMB() {

	}

	void TermDeviceScreen::clearWidgetPools() {
		set<string>::iterator w;
		set<string>::iterator s;

		cout << "TermDeviceScreen::clearWidgetPools ";
		cout << "windowPool size = " << windowPool.size();
		cout << ", surfacePool size = " << surfacePool.size() << endl;

		windowPool.clear();
		surfacePool.clear();
	}

	void TermDeviceScreen::setParentScreen(GingaWindowID parentId) {

	}

	void TermDeviceScreen::setBackgroundImage(string uri) {

	}

	unsigned int TermDeviceScreen::getWidthResolution() {
		return 0;
	}

	void TermDeviceScreen::setWidthResolution(unsigned int wRes) {

	}

	unsigned int TermDeviceScreen::getHeightResolution() {
		return 0;
	}

	void TermDeviceScreen::setHeightResolution(unsigned int hRes) {

	}

	void TermDeviceScreen::setColorKey(int r, int g, int b) {

	}

	void TermDeviceScreen::mergeIds(
			GingaWindowID destId, vector<GingaWindowID>* srcIds) {


	}


	/* interfacing output */

	IWindow* TermDeviceScreen::createWindow(int x, int y, int w, int h) {
		return NULL;
	}

	IWindow* TermDeviceScreen::createWindowFrom(
			GingaWindowID underlyingWindow) {

		return NULL;
	}

	void TermDeviceScreen::releaseWindow(IWindow* win) {

	}

	bool TermDeviceScreen::hasWindow(IWindow* win) {
		return false;
	}

	ISurface* TermDeviceScreen::createSurface() {
		return NULL;
	}

	ISurface* TermDeviceScreen::createSurface(int w, int h) {
		return NULL;
	}

	ISurface* TermDeviceScreen::createSurfaceFrom(void* underlyingSurface) {
		return NULL;
	}

	void TermDeviceScreen::releaseSurface(ISurface* sur) {

	}

	bool TermDeviceScreen::hasSurface(ISurface* sur) {
		return false;
	}


	/* interfacing content */
	IContinuousMediaProvider* TermDeviceScreen::createContinuousMediaProvider(
			const char* mrl, bool hasVisual, bool isRemote) {

		return NULL;
	}

	void TermDeviceScreen::releaseContinuousMediaProvider(
			IContinuousMediaProvider* provider) {


	}

	IFontProvider* TermDeviceScreen::createFontProvider(
			const char* mrl, int fontSize) {

		return NULL;
	}

	void TermDeviceScreen::releaseFontProvider(IFontProvider* provider) {

	}

	IImageProvider* TermDeviceScreen::createImageProvider(const char* mrl) {
		return NULL;
	}

	void TermDeviceScreen::releaseImageProvider(IImageProvider* provider) {

	}

	ISurface* TermDeviceScreen::createRenderedSurfaceFromImageFile(
			const char* mrl) {

		return NULL;
	}


	/* interfacing input */
	IInputManager* TermDeviceScreen::getInputManager() {
		return NULL;
	}

	IEventBuffer* TermDeviceScreen::createEventBuffer() {
		return NULL;
	}

	IInputEvent* TermDeviceScreen::createInputEvent(
			void* event, const int symbol) {

		return NULL;
	}

	IInputEvent* TermDeviceScreen::createApplicationEvent(int type, void* data) {
		return NULL;
	}

	int TermDeviceScreen::fromMBToGinga(int keyCode) {
		return 0;
	}

	int TermDeviceScreen::fromGingaToMB(int keyCode) {
		return 0;
	}


	void* TermDeviceScreen::getGfxRoot() {
		return NULL;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen*
		createTermScreen(int numArgs, char** args, GingaWindowID parentId) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::
			TermDeviceScreen(numArgs, args, parentId));
}

extern "C" void destroyTermScreen(
		::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen* ds) {

	delete ds;
}
