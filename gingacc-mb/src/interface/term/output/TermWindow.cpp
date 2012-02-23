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

#include "util/Color.h"

#include "mb/LocalScreenManager.h"
#include "mb/interface/term/output/TermWindow.h"
#include "mb/interface/term/output/TermSurface.h"

#include <stdlib.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	TermWindow::TermWindow(
			GingaWindowID underlyingWindowID,
			GingaWindowID parentWindowID,
			GingaScreenID screenId,
			int x, int y, int width, int height) {

	}

	TermWindow::~TermWindow() {

	}

	void TermWindow::initialize(
			GingaWindowID underlyingWindowID,
			GingaWindowID parentWindowID,
			GingaScreenID screenId,
			int x, int y, int w, int h) {


	}

	GingaScreenID TermWindow::getScreen() {
		return myScreen;
	}

	void TermWindow::revertContent() {

	}

	void TermWindow::setReleaseListener(ISurface* listener) {

	}

	int TermWindow::getCap(string cap) {
		return 0;
	}

	void TermWindow::setCaps(int caps) {

	}

	void TermWindow::addCaps(int capability) {

	}

	int TermWindow::getCaps() {
		return 0;
	}

	void TermWindow::draw() {

	}

	void TermWindow::setBounds(int posX, int posY, int w, int h) {

	}

	void TermWindow::moveTo(int posX, int posY) {

	}

	void TermWindow::resize(int width, int height) {

	}

	void TermWindow::raiseToTop() {

	}

	void TermWindow::lowerToBottom() {

	}

	void TermWindow::setCurrentTransparency(int alpha) {

	}

	int TermWindow::getTransparencyValue() {
		return 0;
	}

	GingaWindowID TermWindow::getId() {
		return NULL;
	}

	void TermWindow::show() {

	}

	void TermWindow::hide() {

	}

	int TermWindow::getX() {
		return this->x;
	}

	int TermWindow::getY() {
		return this->y;
	}

	int TermWindow::getW() {
		return this->width;
	}

	int TermWindow::getH() {
		return this->height;
	}

	void TermWindow::setX(int x) {

	}

	void TermWindow::setY(int y) {

	}

	void TermWindow::setW(int w) {

	}

	void TermWindow::setH(int h) {

	}

	void* TermWindow::getContent() {
		return NULL;
	}

	void TermWindow::setGhostWindow(bool ghost) {

	}

	bool TermWindow::isVisible() {
		return this->visible;
	}

	void TermWindow::validate() {

	}

	void TermWindow::unprotectedValidate() {

	}

	void TermWindow::addChildSurface(ISurface* s) {

	}

	bool TermWindow::removeChildSurface(ISurface* s) {
		return false;
	}

	void TermWindow::setStretch(bool stretchTo) {

	}

	bool TermWindow::getStretch() {
		return this->stretch;
	}

	void TermWindow::setFit(bool fitTo) {

	}

	bool TermWindow::getFit() {
		return this->fit;
	}

	void TermWindow::clearContent() {

	}

	bool TermWindow::isMine(ISurface* surface) {
		return false;
	}

	void TermWindow::renderImgFile(string serializedImageUrl) {

	}

	void TermWindow::renderFrom(ISurface* surface) {

	}

	void TermWindow::blit(IWindow* src) {

	}

	void TermWindow::stretchBlit(IWindow* src) {

	}

	string TermWindow::getDumpFileUri(int quality, int dumpW, int dumpH) {
		return "";
	}

	void TermWindow::lock() {
		pthread_mutex_lock(&mutex);
	}

	void TermWindow::unlock() {
		pthread_mutex_unlock(&mutex);
	}

	void TermWindow::lockChilds() {
		pthread_mutex_lock(&mutexC);
	}

	void TermWindow::unlockChilds() {
		pthread_mutex_unlock(&mutexC);
	}
}
}
}
}
}
}
