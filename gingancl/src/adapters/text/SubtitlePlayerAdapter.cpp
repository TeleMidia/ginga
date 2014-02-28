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

#include "gingancl/adapters/text/SubtitlePlayerAdapter.h"

#include "gingancl/adapters/AdaptersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace text {
	SubtitlePlayerAdapter::SubtitlePlayerAdapter(
			IPlayerAdapterManager* manager) :
				FormatterPlayerAdapter(manager) {

		typeSet.insert("SubtitlePlayerAdapter");
	}

	void SubtitlePlayerAdapter::createPlayer() {
		string paramValue;
		CascadingDescriptor* descriptor;
		bool hasVisual = true;

#if HAVE_MULTIPROCESS
		playerCompName = "PlayerProcess";
		player = ((PlayerCreator*)(cm->getObject(playerCompName)))(
				myScreen, "SrtPlayer", &hasVisual);

		player->setMrl(mrl, true);

#elif HAVE_COMPONENTS
		playerCompName = "SrtPlayer";
		player = ((PlayerCreator*)(cm->getObject(playerCompName)))(
				myScreen, mrl.c_str(), &hasVisual);
#else
		player = new SrtPlayer(myScreen, mrl.c_str());
#endif

		descriptor = object->getDescriptor();
		if (descriptor != NULL) {
			paramValue = descriptor->getParameterValue("x-setFontUri");
			if (paramValue == "") {
				paramValue = SystemCompat::appendGingaFilesPrefix("font/decker.ttf");
			}
			player->setPropertyValue("x-setFontUri", paramValue);

			paramValue = descriptor->getParameterValue("x-setFontSize");
			if (paramValue == "") {
				paramValue = "30";
			}
			player->setPropertyValue("x-setFontSize", paramValue);

			paramValue = descriptor->getParameterValue("x-controlVisibility");
			if (paramValue == "") {
				paramValue = "FALSE";
			}
			player->setPropertyValue("x-controlVisibility", paramValue);

			paramValue = descriptor->getParameterValue("x-setFontColor");
			if (paramValue == "") {
				paramValue = descriptor->getParameterValue("x-setRGBFontColor");
				if (paramValue == "") {
					paramValue = "191,191,0";
				}
				player->setPropertyValue("x-setRGBFontColor", paramValue);

			} else {
				player->setPropertyValue("x-setFontColor", paramValue);
			}
		}

		player->setPropertyValue("loadFile", mrl);
		FormatterPlayerAdapter::createPlayer();
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::ncl::adapters::IPlayerAdapter*
		createSubAdapter(IPlayerAdapterManager* manager, void* param) {

	return new ::br::pucrio::telemidia::ginga::ncl::adapters::text::
			SubtitlePlayerAdapter(manager);
}

extern "C" void destroySubAdapter(
		::br::pucrio::telemidia::ginga::ncl::adapters::IPlayerAdapter* player) {

	delete player;
}
