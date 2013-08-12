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

#include "mb/interface/CodeMap.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	const int CodeMap::KEY_QUIT              = -1;
	const int CodeMap::KEY_NULL              = 1;
	const int CodeMap::KEY_0                 = 2;
	const int CodeMap::KEY_1                 = 3;
	const int CodeMap::KEY_2                 = 4;
	const int CodeMap::KEY_3                 = 5;
	const int CodeMap::KEY_4                 = 6;
	const int CodeMap::KEY_5                 = 7;
	const int CodeMap::KEY_6                 = 8;
	const int CodeMap::KEY_7                 = 9;
	const int CodeMap::KEY_8                 = 10;
	const int CodeMap::KEY_9                 = 11;

	const int CodeMap::KEY_SMALL_A           = 12;
	const int CodeMap::KEY_SMALL_B           = 13;
	const int CodeMap::KEY_SMALL_C           = 14;
	const int CodeMap::KEY_SMALL_D           = 15;
	const int CodeMap::KEY_SMALL_E           = 16;
	const int CodeMap::KEY_SMALL_F           = 17;
	const int CodeMap::KEY_SMALL_G           = 18;
	const int CodeMap::KEY_SMALL_H           = 19;
	const int CodeMap::KEY_SMALL_I           = 20;
	const int CodeMap::KEY_SMALL_J           = 21;
	const int CodeMap::KEY_SMALL_K           = 22;
	const int CodeMap::KEY_SMALL_L           = 23;
	const int CodeMap::KEY_SMALL_M           = 24;
	const int CodeMap::KEY_SMALL_N           = 25;
	const int CodeMap::KEY_SMALL_O           = 26;
	const int CodeMap::KEY_SMALL_P           = 27;
	const int CodeMap::KEY_SMALL_Q           = 28;
	const int CodeMap::KEY_SMALL_R           = 29;
	const int CodeMap::KEY_SMALL_S           = 30;
	const int CodeMap::KEY_SMALL_T           = 31;
	const int CodeMap::KEY_SMALL_U           = 32;
	const int CodeMap::KEY_SMALL_V           = 33;
	const int CodeMap::KEY_SMALL_W           = 34;
	const int CodeMap::KEY_SMALL_X           = 35;
	const int CodeMap::KEY_SMALL_Y           = 36;
	const int CodeMap::KEY_SMALL_Z           = 37;

	const int CodeMap::KEY_CAPITAL_A         = 38;
	const int CodeMap::KEY_CAPITAL_B         = 39;
	const int CodeMap::KEY_CAPITAL_C         = 40;
	const int CodeMap::KEY_CAPITAL_D         = 41;
	const int CodeMap::KEY_CAPITAL_E         = 42;
	const int CodeMap::KEY_CAPITAL_F         = 43;
	const int CodeMap::KEY_CAPITAL_G         = 44;
	const int CodeMap::KEY_CAPITAL_H         = 45;
	const int CodeMap::KEY_CAPITAL_I         = 46;
	const int CodeMap::KEY_CAPITAL_J         = 47;
	const int CodeMap::KEY_CAPITAL_K         = 48;
	const int CodeMap::KEY_CAPITAL_L         = 49;
	const int CodeMap::KEY_CAPITAL_M         = 50;
	const int CodeMap::KEY_CAPITAL_N         = 51;
	const int CodeMap::KEY_CAPITAL_O         = 52;
	const int CodeMap::KEY_CAPITAL_P         = 53;
	const int CodeMap::KEY_CAPITAL_Q         = 54;
	const int CodeMap::KEY_CAPITAL_R         = 55;
	const int CodeMap::KEY_CAPITAL_S         = 56;
	const int CodeMap::KEY_CAPITAL_T         = 57;
	const int CodeMap::KEY_CAPITAL_U         = 58;
	const int CodeMap::KEY_CAPITAL_V         = 59;
	const int CodeMap::KEY_CAPITAL_W         = 60;
	const int CodeMap::KEY_CAPITAL_X         = 61;
	const int CodeMap::KEY_CAPITAL_Y         = 62;
	const int CodeMap::KEY_CAPITAL_Z         = 63;

	const int CodeMap::KEY_PAGE_DOWN         = 64;
	const int CodeMap::KEY_PAGE_UP           = 65;

	const int CodeMap::KEY_F1                = 66;
	const int CodeMap::KEY_F2                = 67;
	const int CodeMap::KEY_F3                = 68;
	const int CodeMap::KEY_F4                = 69;
	const int CodeMap::KEY_F5                = 70;
	const int CodeMap::KEY_F6                = 71;
	const int CodeMap::KEY_F7                = 72;
	const int CodeMap::KEY_F8                = 73;
	const int CodeMap::KEY_F9                = 74;
	const int CodeMap::KEY_F10               = 75;
	const int CodeMap::KEY_F11               = 76;
	const int CodeMap::KEY_F12               = 77;

	const int CodeMap::KEY_PLUS_SIGN         = 78;
	const int CodeMap::KEY_MINUS_SIGN        = 79;

	const int CodeMap::KEY_ASTERISK          = 80;
	const int CodeMap::KEY_NUMBER_SIGN       = 81;

	const int CodeMap::KEY_PERIOD            = 82;

	const int CodeMap::KEY_SUPER             = 83;
	const int CodeMap::KEY_PRINTSCREEN       = 84;
	const int CodeMap::KEY_MENU              = 85;
	const int CodeMap::KEY_INFO              = 86;
	const int CodeMap::KEY_EPG               = 87;

	const int CodeMap::KEY_CURSOR_DOWN       = 88;
	const int CodeMap::KEY_CURSOR_LEFT       = 89;
	const int CodeMap::KEY_CURSOR_RIGHT      = 90;
	const int CodeMap::KEY_CURSOR_UP         = 91;

	const int CodeMap::KEY_CHANNEL_DOWN      = 92;
	const int CodeMap::KEY_CHANNEL_UP        = 93;

	const int CodeMap::KEY_VOLUME_DOWN       = 94;
	const int CodeMap::KEY_VOLUME_UP         = 95;

	const int CodeMap::KEY_ENTER             = 96;

	const int CodeMap::KEY_RED               = 98;
	const int CodeMap::KEY_GREEN             = 99;
	const int CodeMap::KEY_YELLOW            = 100;
	const int CodeMap::KEY_BLUE              = 101;

	const int CodeMap::KEY_SPACE             = 102;
	const int CodeMap::KEY_BACKSPACE         = 103;
	const int CodeMap::KEY_BACK              = 104;
	const int CodeMap::KEY_ESCAPE            = 105;
	const int CodeMap::KEY_EXIT              = 106;

	const int CodeMap::KEY_POWER             = 107;
	const int CodeMap::KEY_REWIND            = 108;
	const int CodeMap::KEY_STOP              = 109;
	const int CodeMap::KEY_EJECT             = 110;
	const int CodeMap::KEY_PLAY              = 111;
	const int CodeMap::KEY_RECORD            = 112;
	const int CodeMap::KEY_PAUSE             = 113;

	const int CodeMap::KEY_GREATER_THAN_SIGN = 114;
	const int CodeMap::KEY_LESS_THAN_SIGN    = 115;

	const int CodeMap::KEY_TAB               = 116;
	const int CodeMap::KEY_TAP               = 117;

	CodeMap* CodeMap::_instance              = NULL;

	CodeMap::CodeMap() {
		map<string,int>::iterator i;

		keyMap["QUIT"]         = KEY_QUIT;
		keyMap["NO_CODE"]      = KEY_NULL;
		keyMap["0"]            = KEY_0;
		keyMap["1"]            = KEY_1;
		keyMap["2"]            = KEY_2;
		keyMap["3"]            = KEY_3;
		keyMap["4"]            = KEY_4;
		keyMap["5"]            = KEY_5;
		keyMap["6"]            = KEY_6;
		keyMap["7"]            = KEY_7;
		keyMap["8"]            = KEY_8;
		keyMap["9"]            = KEY_9;

		keyMap["a"]            = KEY_SMALL_A;
		keyMap["b"]            = KEY_SMALL_B;
		keyMap["c"]            = KEY_SMALL_C;
		keyMap["d"]            = KEY_SMALL_D;
		keyMap["e"]            = KEY_SMALL_E;
		keyMap["f"]            = KEY_SMALL_F;
		keyMap["g"]            = KEY_SMALL_G;
		keyMap["h"]            = KEY_SMALL_H;
		keyMap["i"]            = KEY_SMALL_I;
		keyMap["j"]            = KEY_SMALL_J;
		keyMap["k"]            = KEY_SMALL_K;
		keyMap["l"]            = KEY_SMALL_L;
		keyMap["m"]            = KEY_SMALL_M;
		keyMap["n"]            = KEY_SMALL_N;
		keyMap["o"]            = KEY_SMALL_O;
		keyMap["p"]            = KEY_SMALL_P;
		keyMap["q"]            = KEY_SMALL_Q;
		keyMap["r"]            = KEY_SMALL_R;
		keyMap["s"]            = KEY_SMALL_S;
		keyMap["t"]            = KEY_SMALL_T;
		keyMap["u"]            = KEY_SMALL_U;
		keyMap["v"]            = KEY_SMALL_V;
		keyMap["w"]            = KEY_SMALL_W;
		keyMap["x"]            = KEY_SMALL_X;
		keyMap["y"]            = KEY_SMALL_Y;
		keyMap["z"]            = KEY_SMALL_Z;

		keyMap["A"]            = KEY_CAPITAL_A;
		keyMap["B"]            = KEY_CAPITAL_B;
		keyMap["C"]            = KEY_CAPITAL_C;
		keyMap["D"]            = KEY_CAPITAL_D;
		keyMap["E"]            = KEY_CAPITAL_E;
		keyMap["F"]            = KEY_CAPITAL_F;
		keyMap["G"]            = KEY_CAPITAL_G;
		keyMap["H"]            = KEY_CAPITAL_H;
		keyMap["I"]            = KEY_CAPITAL_I;
		keyMap["J"]            = KEY_CAPITAL_J;
		keyMap["K"]            = KEY_CAPITAL_K;
		keyMap["L"]            = KEY_CAPITAL_L;
		keyMap["M"]            = KEY_CAPITAL_M;
		keyMap["N"]            = KEY_CAPITAL_N;
		keyMap["O"]            = KEY_CAPITAL_O;
		keyMap["P"]            = KEY_CAPITAL_P;
		keyMap["Q"]            = KEY_CAPITAL_Q;
		keyMap["R"]            = KEY_CAPITAL_R;
		keyMap["S"]            = KEY_CAPITAL_S;
		keyMap["T"]            = KEY_CAPITAL_T;
		keyMap["U"]            = KEY_CAPITAL_U;
		keyMap["V"]            = KEY_CAPITAL_V;
		keyMap["W"]            = KEY_CAPITAL_W;
		keyMap["X"]            = KEY_CAPITAL_X;
		keyMap["Y"]            = KEY_CAPITAL_Y;
		keyMap["Z"]            = KEY_CAPITAL_Z;

		keyMap["*"]            = KEY_ASTERISK;
		keyMap["#"]            = KEY_NUMBER_SIGN;

		keyMap["."]            = KEY_PERIOD;

		keyMap["MENU"]         = KEY_MENU;
		keyMap["INFO"]         = KEY_INFO;
		keyMap["GUIDE"]        = KEY_EPG;

		keyMap["CURSOR_DOWN"]  = KEY_CURSOR_DOWN;
		keyMap["CURSOR_LEFT"]  = KEY_CURSOR_LEFT;
		keyMap["CURSOR_RIGHT"] = KEY_CURSOR_RIGHT;
		keyMap["CURSOR_UP"]    = KEY_CURSOR_UP;

		keyMap["CHANNEL_DOWN"] = KEY_CHANNEL_DOWN;
		keyMap["CHANNEL_UP"]   = KEY_CHANNEL_UP;

		keyMap["VOLUME_DOWN"]  = KEY_VOLUME_DOWN;
		keyMap["VOLUME_UP"]    = KEY_VOLUME_UP;

		keyMap["ENTER"]        = KEY_ENTER;
		keyMap["TAP"]          = KEY_TAP;

		keyMap["RED"]          = KEY_RED;
		keyMap["GREEN"]        = KEY_GREEN;
		keyMap["YELLOW"]       = KEY_YELLOW;
		keyMap["BLUE"]         = KEY_BLUE;

		keyMap["	"]         = KEY_TAB;
		keyMap[" "]            = KEY_SPACE;
		keyMap["BACK"]         = KEY_BACK;
		keyMap["EXIT"]         = KEY_EXIT;

		keyMap["POWER"]        = KEY_POWER;
		keyMap["REWIND"]       = KEY_REWIND;
		keyMap["STOP"]         = KEY_STOP;
		keyMap["EJECT"]        = KEY_EJECT;
		keyMap["PLAY"]         = KEY_PLAY;
		keyMap["RECORD"]       = KEY_RECORD;
		keyMap["PAUSE"]        = KEY_PAUSE;

        for (i = keyMap.begin(); i != keyMap.end(); i++) {
		    valueMap[i->second] = i->first;
        }
	}

	CodeMap* CodeMap::getInstance() {
		if (_instance == NULL) {
			_instance = new CodeMap();
		}

		return _instance;
	}

	int CodeMap::getCode(string codeStr) {
		if (keyMap.count(codeStr) == 0) {
			return KEY_NULL;
		}

		return keyMap[codeStr];
	}

	string CodeMap::getValue(int code) {
		map<int, string>::iterator i;

		i = valueMap.find(code);
		if (i != valueMap.end()) {
			return i->second;
		}

		return "";
	}

	map<string, int>* CodeMap::cloneKeyMap() {
		return new map<string, int>(keyMap);
	}
}
}
}
}
}
}
