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

#ifndef TRANSITION_H_
#define TRANSITION_H_

#include "util/Color.h"
using namespace ::br::pucrio::telemidia::util;

#include "../Entity.h"
using namespace ::br::pucrio::telemidia::ncl;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace transition {
	class Transition : public Entity {
		public:
			static const int TYPE_BARWIPE = 0;
			static const int TYPE_IRISWIPE = 1;
			static const int TYPE_CLOCKWIPE = 2;
			static const int TYPE_SNAKEWIPE = 3;
			static const int TYPE_FADE = 4;

			static const int SUBTYPE_BARWIPE_LEFTTORIGHT = 0;
			static const int SUBTYPE_BARWIPE_TOPTOBOTTOM = 1;

			static const int SUBTYPE_IRISWIPE_RECTANGLE = 20;
			static const int SUBTYPE_IRISWIPE_DIAMOND = 21;

			static const int SUBTYPE_CLOCKWIPE_CLOCKWISETWELVE = 40;
			static const int SUBTYPE_CLOCKWIPE_CLOCKWISETHREE = 41;
			static const int SUBTYPE_CLOCKWIPE_CLOCKWISESIX = 42;
			static const int SUBTYPE_CLOCKWIPE_CLOCKWISENINE = 43;

			static const int SUBTYPE_SNAKEWIPE_TOPLEFTHORIZONTAL = 60;
			static const int SUBTYPE_SNAKEWIPE_TOPLEFTVERTICAL = 61;
			static const int SUBTYPE_SNAKEWIPE_TOPLEFTDIAGONAL = 62;
			static const int SUBTYPE_SNAKEWIPE_TOPRIGHTDIAGONAL = 63;
			static const int SUBTYPE_SNAKEWIPE_BOTTOMRIGHTDIAGONAL = 64;
			static const int SUBTYPE_SNAKEWIPE_BOTTOMLEFTDIAGONAL = 65;

			static const int SUBTYPE_FADE_CROSSFADE = 80;
			static const int SUBTYPE_FADE_FADETOCOLOR = 81;
			static const int SUBTYPE_FADE_FADEFROMCOLOR = 82;

			static const short DIRECTION_FORWARD = 0;
			static const short DIRECTION_REVERSE = 1;

		private:
			int type;
			int subtype;
			double dur;
			double startProgress;
			double endProgress;
			short direction;
			Color* fadeColor;
			int horzRepeat;
			int vertRepeat;
			Color* borderColor;
			int borderWidth;

		public:
			Transition(string id, int type);
			virtual ~Transition();
			Color* getBorderColor();
			int getBorderWidth();
			short getDirection();
			double getDur();
			double getEndProgress();
			Color* getFadeColor();
			int getHorzRepeat();
			double getStartProgress();
			int getSubtype();
			int getType();
			int getVertRepeat();
			void setBorderColor(Color* color);
			void setBorderWidth(int width);
			void setDirection(short dir);
			void setDur(double dur);
			void setEndProgress(double ep);
			void setFadeColor(Color* color);
			void setHorzRepeat(int num);
			void setStartProgress(double sp);
			void setSubtype(int subtype);
			void setType(int type);
			void setVertRepeat(int num);
	};
}
}
}
}
}

#endif /*TRANSITION_H_*/
