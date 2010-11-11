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

#include "system/io/interface/input/CodeMap.h"

#ifdef _WIN32
#include <dinput.h>
#else

#ifdef __cplusplus
extern "C" {
#endif
#include <directfb.h>
#ifdef __cplusplus
}
#endif
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
#ifndef _WIN32
	const int CodeMap::KEY_NULL              = DIKS_NULL;
	const int CodeMap::KEY_0                 = DIKS_0;
	const int CodeMap::KEY_1                 = DIKS_1;
	const int CodeMap::KEY_2                 = DIKS_2;
	const int CodeMap::KEY_3                 = DIKS_3;
	const int CodeMap::KEY_4                 = DIKS_4;
	const int CodeMap::KEY_5                 = DIKS_5;
	const int CodeMap::KEY_6                 = DIKS_6;
	const int CodeMap::KEY_7                 = DIKS_7;
	const int CodeMap::KEY_8                 = DIKS_8;
	const int CodeMap::KEY_9                 = DIKS_9;

	const int CodeMap::KEY_SMALL_A           = DIKS_SMALL_A;
	const int CodeMap::KEY_SMALL_B           = DIKS_SMALL_B;
	const int CodeMap::KEY_SMALL_C           = DIKS_SMALL_C;
	const int CodeMap::KEY_SMALL_D           = DIKS_SMALL_D;
	const int CodeMap::KEY_SMALL_E           = DIKS_SMALL_E;
	const int CodeMap::KEY_SMALL_F           = DIKS_SMALL_F;
	const int CodeMap::KEY_SMALL_G           = DIKS_SMALL_G;
	const int CodeMap::KEY_SMALL_H           = DIKS_SMALL_H;
	const int CodeMap::KEY_SMALL_I           = DIKS_SMALL_I;
	const int CodeMap::KEY_SMALL_J           = DIKS_SMALL_J;
	const int CodeMap::KEY_SMALL_K           = DIKS_SMALL_K;
	const int CodeMap::KEY_SMALL_L           = DIKS_SMALL_L;
	const int CodeMap::KEY_SMALL_M           = DIKS_SMALL_M;
	const int CodeMap::KEY_SMALL_N           = DIKS_SMALL_N;
	const int CodeMap::KEY_SMALL_O           = DIKS_SMALL_O;
	const int CodeMap::KEY_SMALL_P           = DIKS_SMALL_P;
	const int CodeMap::KEY_SMALL_Q           = DIKS_SMALL_Q;
	const int CodeMap::KEY_SMALL_R           = DIKS_SMALL_R;
	const int CodeMap::KEY_SMALL_S           = DIKS_SMALL_S;
	const int CodeMap::KEY_SMALL_T           = DIKS_SMALL_T;
	const int CodeMap::KEY_SMALL_U           = DIKS_SMALL_U;
	const int CodeMap::KEY_SMALL_V           = DIKS_SMALL_V;
	const int CodeMap::KEY_SMALL_W           = DIKS_SMALL_W;
	const int CodeMap::KEY_SMALL_X           = DIKS_SMALL_X;
	const int CodeMap::KEY_SMALL_Y           = DIKS_SMALL_Y;
	const int CodeMap::KEY_SMALL_Z           = DIKS_SMALL_Z;

	const int CodeMap::KEY_CAPITAL_A         = DIKS_CAPITAL_A;
	const int CodeMap::KEY_CAPITAL_B         = DIKS_CAPITAL_B;
	const int CodeMap::KEY_CAPITAL_C         = DIKS_CAPITAL_C;
	const int CodeMap::KEY_CAPITAL_D         = DIKS_CAPITAL_D;
	const int CodeMap::KEY_CAPITAL_E         = DIKS_CAPITAL_E;
	const int CodeMap::KEY_CAPITAL_F         = DIKS_CAPITAL_F;
	const int CodeMap::KEY_CAPITAL_G         = DIKS_CAPITAL_G;
	const int CodeMap::KEY_CAPITAL_H         = DIKS_CAPITAL_H;
	const int CodeMap::KEY_CAPITAL_I         = DIKS_CAPITAL_I;
	const int CodeMap::KEY_CAPITAL_J         = DIKS_CAPITAL_J;
	const int CodeMap::KEY_CAPITAL_K         = DIKS_CAPITAL_K;
	const int CodeMap::KEY_CAPITAL_L         = DIKS_CAPITAL_L;
	const int CodeMap::KEY_CAPITAL_M         = DIKS_CAPITAL_M;
	const int CodeMap::KEY_CAPITAL_N         = DIKS_CAPITAL_N;
	const int CodeMap::KEY_CAPITAL_O         = DIKS_CAPITAL_O;
	const int CodeMap::KEY_CAPITAL_P         = DIKS_CAPITAL_P;
	const int CodeMap::KEY_CAPITAL_Q         = DIKS_CAPITAL_Q;
	const int CodeMap::KEY_CAPITAL_R         = DIKS_CAPITAL_R;
	const int CodeMap::KEY_CAPITAL_S         = DIKS_CAPITAL_S;
	const int CodeMap::KEY_CAPITAL_T         = DIKS_CAPITAL_T;
	const int CodeMap::KEY_CAPITAL_U         = DIKS_CAPITAL_U;
	const int CodeMap::KEY_CAPITAL_V         = DIKS_CAPITAL_V;
	const int CodeMap::KEY_CAPITAL_W         = DIKS_CAPITAL_W;
	const int CodeMap::KEY_CAPITAL_X         = DIKS_CAPITAL_X;
	const int CodeMap::KEY_CAPITAL_Y         = DIKS_CAPITAL_Y;
	const int CodeMap::KEY_CAPITAL_Z         = DIKS_CAPITAL_Z;

	const int CodeMap::KEY_PAGE_DOWN         = DIKS_PAGE_DOWN;
	const int CodeMap::KEY_PAGE_UP           = DIKS_PAGE_UP;

	const int CodeMap::KEY_F1                = DIKS_F1;
	const int CodeMap::KEY_F2                = DIKS_F2;
	const int CodeMap::KEY_F3                = DIKS_F3;
	const int CodeMap::KEY_F4                = DIKS_F4;
	const int CodeMap::KEY_F5                = DIKS_F5;
	const int CodeMap::KEY_F6                = DIKS_F6;
	const int CodeMap::KEY_F7                = DIKS_F7;
	const int CodeMap::KEY_F8                = DIKS_F8;
	const int CodeMap::KEY_F9                = DIKS_F9;
	const int CodeMap::KEY_F10               = DIKS_F10;
	const int CodeMap::KEY_F11               = DIKS_F11;
	const int CodeMap::KEY_F12               = DIKS_F12;

	const int CodeMap::KEY_PLUS_SIGN         = DIKS_PLUS_SIGN;
	const int CodeMap::KEY_MINUS_SIGN        = DIKS_MINUS_SIGN;

	const int CodeMap::KEY_ASTERISK          = DIKS_ASTERISK;
	const int CodeMap::KEY_NUMBER_SIGN       = DIKS_NUMBER_SIGN;

	const int CodeMap::KEY_PERIOD            = DIKS_PERIOD;

	const int CodeMap::KEY_SUPER             = DIKS_SUPER;
	const int CodeMap::KEY_PRINTSCREEN       = DIKS_PRINT;
	const int CodeMap::KEY_MENU              = DIKS_MENU;
	const int CodeMap::KEY_INFO              = DIKS_INFO;
	const int CodeMap::KEY_EPG               = DIKS_EPG;

	const int CodeMap::KEY_CURSOR_DOWN       = DIKS_CURSOR_DOWN;
	const int CodeMap::KEY_CURSOR_LEFT       = DIKS_CURSOR_LEFT;
	const int CodeMap::KEY_CURSOR_RIGHT      = DIKS_CURSOR_RIGHT;
	const int CodeMap::KEY_CURSOR_UP         = DIKS_CURSOR_UP;

	const int CodeMap::KEY_CHANNEL_DOWN      = DIKS_CHANNEL_DOWN;
	const int CodeMap::KEY_CHANNEL_UP        = DIKS_CHANNEL_UP;

	const int CodeMap::KEY_VOLUME_DOWN       = DIKS_VOLUME_DOWN;
	const int CodeMap::KEY_VOLUME_UP         = DIKS_VOLUME_UP;

	const int CodeMap::KEY_ENTER             = DIKS_ENTER;
	const int CodeMap::KEY_OK                = DIKS_OK;

	const int CodeMap::KEY_RED               = DIKS_RED;
	const int CodeMap::KEY_GREEN             = DIKS_GREEN;
	const int CodeMap::KEY_YELLOW            = DIKS_YELLOW;
	const int CodeMap::KEY_BLUE              = DIKS_BLUE;

	const int CodeMap::KEY_BACKSPACE         = DIKS_BACKSPACE;
	const int CodeMap::KEY_BACK              = DIKS_BACK;
	const int CodeMap::KEY_ESCAPE            = DIKS_ESCAPE;
	const int CodeMap::KEY_EXIT              = DIKS_EXIT;

	const int CodeMap::KEY_POWER             = DIKS_POWER;
	const int CodeMap::KEY_REWIND            = DIKS_REWIND;
	const int CodeMap::KEY_STOP              = DIKS_STOP;
	const int CodeMap::KEY_EJECT             = DIKS_EJECT;
	const int CodeMap::KEY_PLAY              = DIKS_PLAY;
	const int CodeMap::KEY_RECORD            = DIKS_RECORD;
	const int CodeMap::KEY_PAUSE             = DIKS_PAUSE;

	const int CodeMap::KEY_GREATER_THAN_SIGN = DIKS_GREATER_THAN_SIGN;
	const int CodeMap::KEY_LESS_THAN_SIGN    = DIKS_LESS_THAN_SIGN;

	const int CodeMap::KEY_TAP = DIKS_CUSTOM0;
#else
	const int CodeMap::KEY_NULL = 0x00; // ?
	const int CodeMap::KEY_0 = DIK_0;
	const int CodeMap::KEY_1 = DIK_1;
	const int CodeMap::KEY_2 = DIK_2;
	const int CodeMap::KEY_3 = DIK_3;
	const int CodeMap::KEY_4 = DIK_4;
	const int CodeMap::KEY_5 = DIK_5;
	const int CodeMap::KEY_6 = DIK_6;
	const int CodeMap::KEY_7 = DIK_7;
	const int CodeMap::KEY_8 = DIK_8;
	const int CodeMap::KEY_9 = DIK_9;

	const int CodeMap::KEY_GREATER_THAN_SIGN = 0x00;
	const int CodeMap::KEY_LESS_THAN_SIGN = 0x00;
	const int CodeMap::KEY_SUPER = 0x00;

	const int CodeMap::KEY_SMALL_A = DIK_A;
	const int CodeMap::KEY_SMALL_B = DIK_B;
	const int CodeMap::KEY_SMALL_C = DIK_C;
	const int CodeMap::KEY_SMALL_D = DIK_D;
	const int CodeMap::KEY_SMALL_E = DIK_E;
	const int CodeMap::KEY_SMALL_F = DIK_F;
	const int CodeMap::KEY_SMALL_G = DIK_G;
	const int CodeMap::KEY_SMALL_H = DIK_H;
	const int CodeMap::KEY_SMALL_I = DIK_I;
	const int CodeMap::KEY_SMALL_J = DIK_J;
	const int CodeMap::KEY_SMALL_K = DIK_K;
	const int CodeMap::KEY_SMALL_L = DIK_L;
	const int CodeMap::KEY_SMALL_M = DIK_M;
	const int CodeMap::KEY_SMALL_N = DIK_N;
	const int CodeMap::KEY_SMALL_O = DIK_O;
	const int CodeMap::KEY_SMALL_P = DIK_P;
	const int CodeMap::KEY_SMALL_Q = DIK_Q;
	const int CodeMap::KEY_SMALL_R = DIK_R;
	const int CodeMap::KEY_SMALL_S = DIK_S;
	const int CodeMap::KEY_SMALL_T = DIK_T;
	const int CodeMap::KEY_SMALL_U = DIK_U;
	const int CodeMap::KEY_SMALL_V = DIK_V;
	const int CodeMap::KEY_SMALL_W = DIK_W;
	const int CodeMap::KEY_SMALL_X = DIK_X;
	const int CodeMap::KEY_SMALL_Y = DIK_Y;
	const int CodeMap::KEY_SMALL_Z = DIK_Z;

	const int CodeMap::KEY_CAPITAL_A = DIK_A;
	const int CodeMap::KEY_CAPITAL_B = DIK_B;
	const int CodeMap::KEY_CAPITAL_C = DIK_C;
	const int CodeMap::KEY_CAPITAL_D = DIK_D;
	const int CodeMap::KEY_CAPITAL_E = DIK_E;
	const int CodeMap::KEY_CAPITAL_F = DIK_F;
	const int CodeMap::KEY_CAPITAL_G = DIK_G;
	const int CodeMap::KEY_CAPITAL_H = DIK_H;
	const int CodeMap::KEY_CAPITAL_I = DIK_I;
	const int CodeMap::KEY_CAPITAL_J = DIK_J;
	const int CodeMap::KEY_CAPITAL_K = DIK_K;
	const int CodeMap::KEY_CAPITAL_L = DIK_L;
	const int CodeMap::KEY_CAPITAL_M = DIK_M;
	const int CodeMap::KEY_CAPITAL_N = DIK_N;
	const int CodeMap::KEY_CAPITAL_O = DIK_O;
	const int CodeMap::KEY_CAPITAL_P = DIK_P;
	const int CodeMap::KEY_CAPITAL_Q = DIK_Q;
	const int CodeMap::KEY_CAPITAL_R = DIK_R;
	const int CodeMap::KEY_CAPITAL_S = DIK_S;
	const int CodeMap::KEY_CAPITAL_T = DIK_T;
	const int CodeMap::KEY_CAPITAL_U = DIK_U;
	const int CodeMap::KEY_CAPITAL_V = DIK_V;
	const int CodeMap::KEY_CAPITAL_W = DIK_W;
	const int CodeMap::KEY_CAPITAL_X = DIK_X;
	const int CodeMap::KEY_CAPITAL_Y = DIK_Y;
	const int CodeMap::KEY_CAPITAL_Z = DIK_Z;

	const int CodeMap::KEY_PAGE_DOWN = DIK_PGDN;
	const int CodeMap::KEY_PAGE_UP	 = DIK_PGUP;

	const int CodeMap::KEY_F1 = DIK_F1;
	const int CodeMap::KEY_F2 = DIK_F2;
	const int CodeMap::KEY_F3 = DIK_F3;
	const int CodeMap::KEY_F4 = DIK_F4;
	const int CodeMap::KEY_F5 = DIK_F5;
	const int CodeMap::KEY_F6 = DIK_F6;
	const int CodeMap::KEY_F7 = DIK_F7;
	const int CodeMap::KEY_F8 = DIK_F8;
	const int CodeMap::KEY_F9 = DIK_F9;
	const int CodeMap::KEY_F10 = DIK_F10;
	const int CodeMap::KEY_F11 = DIK_F11;
	const int CodeMap::KEY_F12 = DIK_F12;

	const int CodeMap::KEY_PLUS_SIGN = DIK_NUMPADPLUS;
	const int CodeMap::KEY_MINUS_SIGN = DIK_NUMPADPERIOD;

	const int CodeMap::KEY_ASTERISK = DIK_MULTIPLY;
	const int CodeMap::KEY_NUMBER_SIGN = 0x23; // ?
	const int CodeMap::KEY_PERIOD = 0x00; // ?
	const int CodeMap::KEY_PRINTSCREEN = 0x00;

	const int CodeMap::KEY_MENU = DIK_APPS; 
	const int CodeMap::KEY_INFO = 0x14; // ?
	const int CodeMap::KEY_EPG = 0x1B; // ?

	const int CodeMap::KEY_CURSOR_DOWN = DIK_DOWN;
	const int CodeMap::KEY_CURSOR_LEFT = DIK_LEFT;
	const int CodeMap::KEY_CURSOR_RIGHT = DIK_RIGHT;
	const int CodeMap::KEY_CURSOR_UP = DIK_UP;

	const int CodeMap::KEY_CHANNEL_DOWN = 0x47; // ?
	const int CodeMap::KEY_CHANNEL_UP = 0x46; // ?

	const int CodeMap::KEY_VOLUME_DOWN = DIK_VOLUMEDOWN;
	const int CodeMap::KEY_VOLUME_UP = DIK_VOLUMEUP;

	const int CodeMap::KEY_ENTER = DIK_RETURN;
	const int CodeMap::KEY_OK = 0x0B; // ?

	const int CodeMap::KEY_RED = 0x42; // ?
	const int CodeMap::KEY_GREEN = 0x43; // ?
	const int CodeMap::KEY_YELLOW = 0x44; // ?
	const int CodeMap::KEY_BLUE = 0x45; // ?

	const int CodeMap::KEY_BACKSPACE = DIK_BACKSPACE;
	const int CodeMap::KEY_BACK = DIK_BACK;
	const int CodeMap::KEY_ESCAPE = DIK_ESCAPE;
	const int CodeMap::KEY_EXIT = 0x62; // ?

	const int CodeMap::KEY_POWER = DIK_POWER;
	const int CodeMap::KEY_REWIND = 0x59; // ?
	const int CodeMap::KEY_STOP = DIK_STOP;
	const int CodeMap::KEY_EJECT = 0x57; // ?
	const int CodeMap::KEY_PLAY = DIK_PLAYPAUSE;
	const int CodeMap::KEY_RECORD = 0x56;
	const int CodeMap::KEY_PAUSE = DIK_PAUSE;

	const int CodeMap::KEY_TAP = DIMOFS_BUTTON0;

#endif
	CodeMap::CodeMap() {
		keyMap = new map<string, int>;
		(*keyMap)["NO_CODE"] = KEY_NULL;
		(*keyMap)["0"] = KEY_0;
		(*keyMap)["1"] = KEY_1;
		(*keyMap)["2"] = KEY_2;
		(*keyMap)["3"] = KEY_3;
		(*keyMap)["4"] = KEY_4;
		(*keyMap)["5"] = KEY_5;
		(*keyMap)["6"] = KEY_6;
		(*keyMap)["7"] = KEY_7;
		(*keyMap)["8"] = KEY_8;
		(*keyMap)["9"] = KEY_9;

		(*keyMap)["a"] = KEY_SMALL_A;
		(*keyMap)["b"] = KEY_SMALL_B;
		(*keyMap)["c"] = KEY_SMALL_C;
		(*keyMap)["d"] = KEY_SMALL_D;
		(*keyMap)["e"] = KEY_SMALL_E;
		(*keyMap)["f"] = KEY_SMALL_F;
		(*keyMap)["g"] = KEY_SMALL_G;
		(*keyMap)["h"] = KEY_SMALL_H;
		(*keyMap)["i"] = KEY_SMALL_I;
		(*keyMap)["j"] = KEY_SMALL_J;
		(*keyMap)["k"] = KEY_SMALL_K;
		(*keyMap)["l"] = KEY_SMALL_L;
		(*keyMap)["m"] = KEY_SMALL_M;
		(*keyMap)["n"] = KEY_SMALL_N;
		(*keyMap)["o"] = KEY_SMALL_O;
		(*keyMap)["p"] = KEY_SMALL_P;
		(*keyMap)["q"] = KEY_SMALL_Q;
		(*keyMap)["r"] = KEY_SMALL_R;
		(*keyMap)["s"] = KEY_SMALL_S;
		(*keyMap)["t"] = KEY_SMALL_T;
		(*keyMap)["u"] = KEY_SMALL_U;
		(*keyMap)["v"] = KEY_SMALL_V;
		(*keyMap)["w"] = KEY_SMALL_W;
		(*keyMap)["x"] = KEY_SMALL_X;
		(*keyMap)["y"] = KEY_SMALL_Y;
		(*keyMap)["z"] = KEY_SMALL_Z;

		(*keyMap)["A"] = KEY_CAPITAL_A;
		(*keyMap)["B"] = KEY_CAPITAL_B;
		(*keyMap)["C"] = KEY_CAPITAL_C;
		(*keyMap)["D"] = KEY_CAPITAL_D;
		(*keyMap)["E"] = KEY_CAPITAL_E;
		(*keyMap)["F"] = KEY_CAPITAL_F;
		(*keyMap)["G"] = KEY_CAPITAL_G;
		(*keyMap)["H"] = KEY_CAPITAL_H;
		(*keyMap)["I"] = KEY_CAPITAL_I;
		(*keyMap)["J"] = KEY_CAPITAL_J;
		(*keyMap)["K"] = KEY_CAPITAL_K;
		(*keyMap)["L"] = KEY_CAPITAL_L;
		(*keyMap)["M"] = KEY_CAPITAL_M;
		(*keyMap)["N"] = KEY_CAPITAL_N;
		(*keyMap)["O"] = KEY_CAPITAL_O;
		(*keyMap)["P"] = KEY_CAPITAL_P;
		(*keyMap)["Q"] = KEY_CAPITAL_Q;
		(*keyMap)["R"] = KEY_CAPITAL_R;
		(*keyMap)["S"] = KEY_CAPITAL_S;
		(*keyMap)["T"] = KEY_CAPITAL_T;
		(*keyMap)["U"] = KEY_CAPITAL_U;
		(*keyMap)["V"] = KEY_CAPITAL_V;
		(*keyMap)["W"] = KEY_CAPITAL_W;
		(*keyMap)["X"] = KEY_CAPITAL_X;
		(*keyMap)["Y"] = KEY_CAPITAL_Y;
		(*keyMap)["Z"] = KEY_CAPITAL_Z;

		(*keyMap)["*"] = KEY_ASTERISK;
		(*keyMap)["#"] = KEY_NUMBER_SIGN;

		(*keyMap)["."] = KEY_PERIOD;

		(*keyMap)["MENU"]  = KEY_MENU;
		(*keyMap)["INFO"]  = KEY_INFO;
		(*keyMap)["GUIDE"] = KEY_EPG;

		(*keyMap)["CURSOR_DOWN"]  = KEY_CURSOR_DOWN;
		(*keyMap)["CURSOR_LEFT"]  = KEY_CURSOR_LEFT;
		(*keyMap)["CURSOR_RIGHT"] = KEY_CURSOR_RIGHT;
		(*keyMap)["CURSOR_UP"]    = KEY_CURSOR_UP;

		(*keyMap)["CHANNEL_DOWN"] = KEY_CHANNEL_DOWN;
		(*keyMap)["CHANNEL_UP"]   = KEY_CHANNEL_UP;

		(*keyMap)["VOLUME_DOWN"]  = KEY_VOLUME_DOWN;
		(*keyMap)["VOLUME_UP"]    = KEY_VOLUME_UP;

		(*keyMap)["ENTER"]  = KEY_ENTER;
		(*keyMap)["OK"]     = KEY_OK;
		(*keyMap)["TAP"]    = KEY_TAP;

		(*keyMap)["RED"]    = KEY_RED;
		(*keyMap)["GREEN"]  = KEY_GREEN;
		(*keyMap)["YELLOW"] = KEY_YELLOW;
		(*keyMap)["BLUE"]   = KEY_BLUE;

		(*keyMap)["BACK"]   = KEY_BACK;
		(*keyMap)["EXIT"]   = KEY_EXIT;

		(*keyMap)["POWER"]  = KEY_POWER;
		(*keyMap)["REWIND"] = KEY_REWIND;
		(*keyMap)["STOP"]   = KEY_STOP;
		(*keyMap)["EJECT"]  = KEY_EJECT;
		(*keyMap)["PLAY"]   = KEY_PLAY;
		(*keyMap)["RECORD"] = KEY_RECORD;
		(*keyMap)["PAUSE"]  = KEY_PAUSE;

		valueMap = new map<int, string>;

        map<string,int>::iterator it;
        for (it=keyMap->begin(); it!=keyMap->end(); it++)
		    (*valueMap)[it->second] = it->first;

/*
		(*valueMap)[KEY_NULL] = "";
		(*valueMap)[KEY_0] = "0";
		(*valueMap)[KEY_1] = "1";
		(*valueMap)[KEY_2] = "2";
		(*valueMap)[KEY_3] = "3";
		(*valueMap)[KEY_4] = "4";
		(*valueMap)[KEY_5] = "5";
		(*valueMap)[KEY_6] = "6";
		(*valueMap)[KEY_7] = "7";
		(*valueMap)[KEY_8] = "8";
		(*valueMap)[KEY_9] = "9";

		(*valueMap)[KEY_SMALL_A] = "a";
		(*valueMap)[KEY_SMALL_B] = "b";
		(*valueMap)[KEY_SMALL_C] = "c";
		(*valueMap)[KEY_SMALL_D] = "d";
		(*valueMap)[KEY_SMALL_E] = "e";
		(*valueMap)[KEY_SMALL_F] = "f";
		(*valueMap)[KEY_SMALL_G] = "g";
		(*valueMap)[KEY_SMALL_H] = "h";
		(*valueMap)[KEY_SMALL_I] = "i";
		(*valueMap)[KEY_SMALL_J] = "j";
		(*valueMap)[KEY_SMALL_K] = "k";
		(*valueMap)[KEY_SMALL_L] = "l";
		(*valueMap)[KEY_SMALL_M] = "m";
		(*valueMap)[KEY_SMALL_N] = "n";
		(*valueMap)[KEY_SMALL_O] = "o";
		(*valueMap)[KEY_SMALL_P] = "p";
		(*valueMap)[KEY_SMALL_Q] = "q";
		(*valueMap)[KEY_SMALL_R] = "r";
		(*valueMap)[KEY_SMALL_S] = "s";
		(*valueMap)[KEY_SMALL_T] = "t";
		(*valueMap)[KEY_SMALL_U] = "u";
		(*valueMap)[KEY_SMALL_V] = "v";
		(*valueMap)[KEY_SMALL_W] = "w";
		(*valueMap)[KEY_SMALL_X] = "x";
		(*valueMap)[KEY_SMALL_Y] = "y";
		(*valueMap)[KEY_SMALL_Z] = "z";

		(*valueMap)[KEY_CAPITAL_A] = "A";
		(*valueMap)[KEY_CAPITAL_B] = "B";
		(*valueMap)[KEY_CAPITAL_C] = "C";
		(*valueMap)[KEY_CAPITAL_D] = "D";
		(*valueMap)[KEY_CAPITAL_E] = "E";
		(*valueMap)[KEY_CAPITAL_F] = "F";
		(*valueMap)[KEY_CAPITAL_G] = "G";
		(*valueMap)[KEY_CAPITAL_H] = "H";
		(*valueMap)[KEY_CAPITAL_I] = "I";
		(*valueMap)[KEY_CAPITAL_J] = "J";
		(*valueMap)[KEY_CAPITAL_K] = "K";
		(*valueMap)[KEY_CAPITAL_L] = "L";
		(*valueMap)[KEY_CAPITAL_M] = "M";
		(*valueMap)[KEY_CAPITAL_N] = "N";
		(*valueMap)[KEY_CAPITAL_O] = "O";
		(*valueMap)[KEY_CAPITAL_P] = "P";
		(*valueMap)[KEY_CAPITAL_Q] = "Q";
		(*valueMap)[KEY_CAPITAL_R] = "R";
		(*valueMap)[KEY_CAPITAL_S] = "S";
		(*valueMap)[KEY_CAPITAL_T] = "T";
		(*valueMap)[KEY_CAPITAL_U] = "U";
		(*valueMap)[KEY_CAPITAL_V] = "V";
		(*valueMap)[KEY_CAPITAL_W] = "W";
		(*valueMap)[KEY_CAPITAL_X] = "X";
		(*valueMap)[KEY_CAPITAL_Y] = "Y";
		(*valueMap)[KEY_CAPITAL_Z] = "Z";

		(*valueMap)[KEY_ASTERISK] = "*";
		(*valueMap)[KEY_NUMBER_SIGN] = "#";

		(*valueMap)[KEY_MENU] = "MENU";
		(*valueMap)[KEY_INFO] = "INFO";
		(*valueMap)[KEY_EPG] = "GUIDE";

		(*valueMap)[KEY_CURSOR_DOWN] = "CURSOR_DOWN";
		(*valueMap)[KEY_CURSOR_LEFT] = "CURSOR_LEFT";
		(*valueMap)[KEY_CURSOR_RIGHT] = "CURSOR_RIGHT";
		(*valueMap)[KEY_CURSOR_UP] = "CURSOR_UP";

		(*valueMap)[KEY_ENTER] = "ENTER";

		(*valueMap)[KEY_RED] = "RED";
		(*valueMap)[KEY_GREEN] = "GREEN";
		(*valueMap)[KEY_YELLOW] = "YELLOW";
		(*valueMap)[KEY_BLUE] = "BLUE";
*/
	}

	int CodeMap::getCode(string codeStr) {
		if (keyMap->count(codeStr) == 0) {
			return KEY_NULL;
		}

		return (*keyMap)[codeStr];
	}

	string CodeMap::getValue(int code) {
		map<int, string>::iterator i;

		i = valueMap->find(code);
		if (i != valueMap->end()) {
			return i->second;
		}

		return "";
	}

	CodeMap* CodeMap::_instance = 0;

	CodeMap* CodeMap::getInstance() {
		if (CodeMap::_instance == NULL) {
			CodeMap::_instance = new CodeMap();
		}
		return CodeMap::_instance;
	}

	map<string, int>* CodeMap::cloneKeyMap() {
		return new map<string, int>(*keyMap);
	}
}
}
}
}
}
}
}
