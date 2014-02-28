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

#include "ncl/transition/Transition.h"
#include "ncl/transition/TransitionUtil.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace transition {
	Transition::Transition(string id, int type) : Entity(id) {
		setType(type);

		this->dur           = 1000; /* 1 second */
		this->startProgress = 0.0;
		this->endProgress   = 1.0;
		this->direction     = Transition::DIRECTION_FORWARD;
		this->horzRepeat    = 1;
		this->vertRepeat    = 1;
		this->fadeColor     = new Color("black");
		this->borderColor   = new Color("black");
		this->borderWidth   = 0;
	}

	Transition::~Transition() {
		if (fadeColor != NULL) {
			delete fadeColor;
			fadeColor = NULL;
		}

		if (borderColor != NULL) {
			delete borderColor;
			borderColor = NULL;
		}
	}

	Color* Transition::getBorderColor() {
		return borderColor;
	}

	int Transition::getBorderWidth() {
		return borderWidth;
	}

	short Transition::getDirection() {
		return direction;
	}

	double Transition::getDur() {
		return dur;
	}

	double Transition::getEndProgress() {
		return endProgress;
	}

	Color* Transition::getFadeColor() {
		return fadeColor;
	}

	int Transition::getHorzRepeat() {
		return horzRepeat;
	}

	double Transition::getStartProgress() {
		return startProgress;
	}

	int Transition::getSubtype() {
		return subtype;
	}

	int Transition::getType() {
		return type;
	}

	int Transition::getVertRepeat() {
		return vertRepeat;
	}

	void Transition::setBorderColor(Color* color) {
		if (color != NULL) {
			this->borderColor = color;
		}
	}

	void Transition::setBorderWidth(int width) {
		if (width >= 0) {
			this->borderWidth = width;
		}
	}

	void Transition::setDirection(short dir) {
		if (dir >= Transition::DIRECTION_FORWARD
				&& dir <= Transition::DIRECTION_REVERSE) {

			this->direction = dir;
		}
	}

	void Transition::setDur(double dur) {
		if (dur >= 0) {
			this->dur = dur;
		}
	}

	void Transition::setEndProgress(double ep) {
		if (ep >= 0 && ep <= 1 && ep >= startProgress) {
			this->endProgress = ep;
		}
	}

	void Transition::setFadeColor(Color* color) {
		if (color != NULL) {
			this->fadeColor = color;
		}
	}

	void Transition::setHorzRepeat(int num) {
		if (num > 0) {
			this->horzRepeat = num;
		}
	}

	void Transition::setStartProgress(double sp) {
		if (sp >= 0 && sp <= 1 && sp <= endProgress) {
			this->startProgress = sp;
		}
	}

	void Transition::setSubtype(int subtype) {
		switch (type) {
			case Transition::TYPE_BARWIPE:
				if (subtype >= Transition::SUBTYPE_BARWIPE_LEFTTORIGHT &&
						subtype <= Transition::SUBTYPE_BARWIPE_TOPTOBOTTOM) {

					this->subtype = subtype;
				}

				break;

			case Transition::TYPE_IRISWIPE:
				if (subtype >= Transition::SUBTYPE_IRISWIPE_RECTANGLE &&
						subtype <= Transition::SUBTYPE_IRISWIPE_DIAMOND) {

					this->subtype = subtype;
				}

				break;

			case Transition::TYPE_CLOCKWIPE:
				if (subtype >= Transition::SUBTYPE_CLOCKWIPE_CLOCKWISETWELVE &&
					  subtype <= Transition::SUBTYPE_CLOCKWIPE_CLOCKWISENINE) {

					this->subtype = subtype;
				}

				break;

			case Transition::TYPE_SNAKEWIPE:
				if (subtype >= Transition::SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL
					    && subtype <=
					    Transition::SUBTYPE_SNAKEWIPE_BOTTOMLEFTDIAGONAL) {

					this->subtype = subtype;
				}

				break;

			case Transition::TYPE_FADE:
			default:
				if (subtype >= Transition::SUBTYPE_FADE_CROSSFADE &&
						subtype <= Transition::SUBTYPE_FADE_FADEFROMCOLOR) {

					this->subtype = subtype;
				}
				break;
			}
	}

	void Transition::setType(int type) {
		if (type >= Transition::TYPE_BARWIPE &&
			    type <= Transition::TYPE_FADE) {

			this->type = type;
			subtype = TransitionUtil::getDefaultSubtype(type);
		}
	}

	void Transition::setVertRepeat(int num) {
		if (num > 0) {
			this->vertRepeat = num;
		}
	}
}
}
}
}
}
