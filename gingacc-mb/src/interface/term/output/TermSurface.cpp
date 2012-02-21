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

#include <stdlib.h>
#include "mb/interface/term/output/TermSurface.h"
#include "mb/interface/Matrix.h"
#include "mb/interface/term/output/TermWindow.h"
#include "mb/interface/IFontProvider.h"
#include "mb/LocalScreenManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	TermSurface::TermSurface(GingaScreenID screenId) {
		initialize(screenId);
	}

	TermSurface::TermSurface(GingaScreenID screenId, void* underlyingSurface) {
		initialize(screenId);

		this->sur = underlyingSurface;
	}

	TermSurface::TermSurface(GingaScreenID screenId, int w, int h) {
		this->sur = NULL;
	}

	TermSurface::~TermSurface() {

	}

	void TermSurface::initialize(GingaScreenID screenId) {
		this->myScreen      = screenId;
		this->sur           = NULL;
		this->parent        = NULL;
		this->chromaColor   = NULL;
		this->caps          = 0;
		this->hasExtHandler = false;
	}

	void TermSurface::write(int x, int y, int w, int h, int pitch, char* buff) {

	}

	void TermSurface::setExternalHandler(bool extHandler) {
		this->hasExtHandler = extHandler;
	}

	bool TermSurface::hasExternalHandler() {
		return this->hasExtHandler;
	}

	void TermSurface::addCaps(int caps) {
		this->caps = this->caps | caps;
	}

	void TermSurface::setCaps(int caps) {
		this->caps = caps;
	}

	int TermSurface::getCap(string cap) {
		return 0;
	}

	int TermSurface::getCaps() {
		return this->caps;
	}

	void* TermSurface::getSurfaceContent() {
		return sur;
	}

	void TermSurface::setSurfaceContent(void* surface) {
		this->sur = surface;
	}

	bool TermSurface::setParent(void* parentWindow) {
		return true;
	}

	void* TermSurface::getParent() {
		return this->parent;
	}

	void TermSurface::clearContent() {
		if (sur == NULL) {
			clog << "TermSurface::clearContent Warning! ";
			clog << "Can't clear content: ";
			clog << "internal surface is NULL" << endl;
			return;
		}

		if (parent != NULL) {
			parent->clearContent();
		}
	}

	void TermSurface::clearSurface() {
		if (sur == NULL) {
			clog << "TermSurface::clearSurface Warning! ";
			clog << "Can't clear surface: ";
			clog << "internal surface is NULL" << endl;
			return;
		}
	}

	void TermSurface::drawLine(int x1, int y1, int x2, int y2) {

	}

	void TermSurface::drawRectangle(int x, int y, int w, int h) {

	}

	void TermSurface::fillRectangle(int x, int y, int w, int h) {

	}

	void TermSurface::drawString(int x, int y, const char* txt) {

	}

	void TermSurface::setChromaColor(int r, int g, int b, int alpha) {

	}

	IColor* TermSurface::getChromaColor() {
		return NULL;
	}

	void TermSurface::setBorderColor(int r, int g, int b, int alpha) {

	}

	IColor* TermSurface::getBorderColor() {
		return NULL;
	}

	void TermSurface::setBgColor(int r, int g, int b, int alpha) {

	}

	IColor* TermSurface::getBgColor() {
		return NULL;
	}

	void TermSurface::setColor(int r, int g, int b, int alpha) {

	}

	IColor* TermSurface::getColor() {
		return NULL;
	}

	void TermSurface::setFont(void* font) {

	}

	void TermSurface::flip() {

	}

	void TermSurface::scale(double x, double y) {

	}

	void TermSurface::blit(
			int x, int y, ISurface* src,
			int srcX, int srcY, int srcW, int srcH) {

	}

	void TermSurface::getStringExtents(const char* text, int* w, int* h) {

	}

	void TermSurface::setClip(int x, int y, int w, int h) {

	}

	void TermSurface::getSize(int* w, int* h) {

	}

	string TermSurface::getDumpFileUri() {
		return "";
	}

	void TermSurface::setMatrix(void* matrix) {

	}
}
}
}
}
}
}
