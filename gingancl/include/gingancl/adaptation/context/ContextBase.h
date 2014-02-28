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

#ifndef _CONTEXTBASE_H_
#define _CONTEXTBASE_H_

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adaptation {
namespace context {
	const string SYSTEM_LANGUAGE("system.language");
	const string SYSTEM_CAPTION("system.caption");
	const string SYSTEM_SUBTITLE("system.subtitle");
	const string SYSTEM_RETURN_BIT_RATE("system.returnBitRate");
	const string SYSTEM_SCREEN_SIZE("system.screenSize");
	const string SYSTEM_SCREEN_GRAPHIC_SIZE(
			"system.screenGraphicSize");

	const string SYSTEM_AUDIO_TYPE("system.audioType");
	const string SYSTEM_CPU("system.CPU");
	const string SYSTEM_MEMORY("system.memory");
	const string SYSTEM_OPERATING_SYSTEM("system.operatingSystem");
	const string SYSTEM_JAVA_CONFIG("system.javaConfiguration");
	const string SYSTEM_JAVA_PROFILE("system.javaProfile");
	const string SYSTEM_LUA_VERSION("system.luaVersion");

	const string SYSTEM_DEVNUMBER("system.devNumber");
	const string SYSTEM_CLASSTYPE("system.classType");
	const string SYSTEM_INFO("system.info");
	const string SYSTEM_CLASS_NUMBER("system.classNumber");

	const string USER_AGE("user.age");
	const string USER_LOCATION("user.location");
	const string USER_GENRE("user.genre");

	const string DEFAULT_FOCUS_BORDER_COLOR(
			"default.focusBorderColor");

	const string DEFAULT_SEL_BORDER_COLOR(
			"default.selBorderColor");

	const string DEFAULT_FOCUS_BORDER_WIDTH(
			"default.focusBorderWidth");

	const string DEFAULT_FOCUS_BORDER_TRANSPARENCY(
			"default.focusBorderTransparency");
}
}
}
}
}
}
}

#endif //_CONTEXTBASE_H_
