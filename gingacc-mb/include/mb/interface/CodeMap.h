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

#ifndef CODEMAP_H_
#define CODEMAP_H_

#include <string>
#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class CodeMap {
		public:
			static const int KEY_NULL;
			static const int KEY_0;
			static const int KEY_1;
			static const int KEY_2;
			static const int KEY_3;
			static const int KEY_4;
			static const int KEY_5;
			static const int KEY_6;
			static const int KEY_7;
			static const int KEY_8;
			static const int KEY_9;

			static const int KEY_SMALL_A;
			static const int KEY_SMALL_B;
			static const int KEY_SMALL_C;
			static const int KEY_SMALL_D;
			static const int KEY_SMALL_E;
			static const int KEY_SMALL_F;
			static const int KEY_SMALL_G;
			static const int KEY_SMALL_H;
			static const int KEY_SMALL_I;
			static const int KEY_SMALL_J;
			static const int KEY_SMALL_K;
			static const int KEY_SMALL_L;
			static const int KEY_SMALL_M;
			static const int KEY_SMALL_N;
			static const int KEY_SMALL_O;
			static const int KEY_SMALL_P;
			static const int KEY_SMALL_Q;
			static const int KEY_SMALL_R;
			static const int KEY_SMALL_S;
			static const int KEY_SMALL_T;
			static const int KEY_SMALL_U;
			static const int KEY_SMALL_V;
			static const int KEY_SMALL_W;
			static const int KEY_SMALL_X;
			static const int KEY_SMALL_Y;
			static const int KEY_SMALL_Z;

			static const int KEY_CAPITAL_A;
			static const int KEY_CAPITAL_B;
			static const int KEY_CAPITAL_C;
			static const int KEY_CAPITAL_D;
			static const int KEY_CAPITAL_E;
			static const int KEY_CAPITAL_F;
			static const int KEY_CAPITAL_G;
			static const int KEY_CAPITAL_H;
			static const int KEY_CAPITAL_I;
			static const int KEY_CAPITAL_J;
			static const int KEY_CAPITAL_K;
			static const int KEY_CAPITAL_L;
			static const int KEY_CAPITAL_M;
			static const int KEY_CAPITAL_N;
			static const int KEY_CAPITAL_O;
			static const int KEY_CAPITAL_P;
			static const int KEY_CAPITAL_Q;
			static const int KEY_CAPITAL_R;
			static const int KEY_CAPITAL_S;
			static const int KEY_CAPITAL_T;
			static const int KEY_CAPITAL_U;
			static const int KEY_CAPITAL_V;
			static const int KEY_CAPITAL_W;
			static const int KEY_CAPITAL_X;
			static const int KEY_CAPITAL_Y;
			static const int KEY_CAPITAL_Z;

			static const int KEY_PAGE_DOWN;
			static const int KEY_PAGE_UP;

			static const int KEY_F1;
			static const int KEY_F2;
			static const int KEY_F3;
			static const int KEY_F4;
			static const int KEY_F5;
			static const int KEY_F6;
			static const int KEY_F7;
			static const int KEY_F8;
			static const int KEY_F9;
			static const int KEY_F10;
			static const int KEY_F11;
			static const int KEY_F12;

			static const int KEY_PLUS_SIGN;
			static const int KEY_MINUS_SIGN;

			static const int KEY_ASTERISK;
			static const int KEY_NUMBER_SIGN;

			static const int KEY_PERIOD;

			static const int KEY_SUPER;
			static const int KEY_PRINTSCREEN;
			static const int KEY_MENU;
			static const int KEY_INFO;
			static const int KEY_EPG;

			static const int KEY_CURSOR_DOWN;
			static const int KEY_CURSOR_LEFT;
			static const int KEY_CURSOR_RIGHT;
			static const int KEY_CURSOR_UP;

			static const int KEY_CHANNEL_DOWN;
			static const int KEY_CHANNEL_UP;

			static const int KEY_VOLUME_DOWN;
			static const int KEY_VOLUME_UP;

			static const int KEY_ENTER;
			static const int KEY_OK;

			static const int KEY_RED;
			static const int KEY_GREEN;
			static const int KEY_YELLOW;
			static const int KEY_BLUE;

			static const int KEY_BACKSPACE;
			static const int KEY_BACK;
			static const int KEY_ESCAPE;
			static const int KEY_EXIT;

			static const int KEY_POWER;
			static const int KEY_REWIND;
			static const int KEY_STOP;
			static const int KEY_EJECT;
			static const int KEY_PLAY;
			static const int KEY_RECORD;
			static const int KEY_PAUSE;

			static const int KEY_GREATER_THAN_SIGN;
			static const int KEY_LESS_THAN_SIGN;

			static const int KEY_TAP;

		private:
			static CodeMap* _instance;
			map<string, int>* keyMap;
			map<int, string>* valueMap;
			CodeMap();

		public:
			int getCode(string codeStr);
			string getValue(int value);
			static CodeMap* getInstance();
			map<string, int>* cloneKeyMap();
	};
}
}
}
}
}
}

#endif /*CODEMAP_H_*/
