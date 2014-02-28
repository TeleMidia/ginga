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

#ifndef _COLOR_H_
#define _COLOR_H_

#include "util/functions.h"
#include "IColor.h"

#include <string>
#include <iostream>
using namespace std;

TELEMIDIA_UTIL_BEGIN_DECLS

class Color : public IColor {
	private:
		int r, g, b, alpha;

	protected:
		void setColorToI(string color);

	public:
		static const int white = 0XFFFFFF;
		static const int yellow = 0XFFFF00;
		static const int red = 0XFF0000;
		static const int pink = 0XFFC0CB;
		static const int orange = 0XFFA500;
		static const int magenta = 0XFF00FF;
		static const int green = 0X008000;
		static const int cyan = 0X00FFFF;
		static const int blue = 0X0000FF;
		static const int lightGray = 0XD3D3D3;
		static const int gray = 0X808080;
		static const int darkGray = 0XA9A9A9;
		static const int black = 0X000000;
		static const int silver = 0XC0C0C0;
		static const int maroon = 0X800000;
		static const int fuchsia = 0XFF00FF;
		static const int purple = 0X800080;
		static const int lime = 0X00FF00;
		static const int olive = 0X808000;
		static const int navy = 0X000080;
		static const int aqua = 0X00FFFF;
		static const int teal = 0X008080;

		static const string swhite;
		static const string syellow;
		static const string sred;
		static const string spink;
		static const string sorange;
		static const string smagenta;
		static const string sgreen;
		static const string scyan;
		static const string sblue;
		static const string slightGray;
		static const string sgray;
		static const string sdarkGray;
		static const string sblack;
		static const string ssilver;
		static const string smaroon;
		static const string sfuchsia;
		static const string spurple;
		static const string slime;
		static const string solive;
		static const string snavy;
		static const string saqua;
		static const string steal;

		static int colortoi(string color);
		Color();
		Color(int r, int g, int b, int alpha=255);
		Color(string color, int alpha=255);
		void setColor(string color);
		void setColor(int red, int green, int blue);
		int getR();
		int getG();
		int getB();
		int getAlpha();
		uint32_t getRGBA();
		uint32_t getARGB();
};

TELEMIDIA_UTIL_END_DECLS

#endif //_COLOR_H_
