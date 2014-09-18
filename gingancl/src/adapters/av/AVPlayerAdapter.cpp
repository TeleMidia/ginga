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

#include "gingancl/adapters/av/AVPlayerAdapter.h"

#include "gingancl/adapters/AdaptersComponentSupport.h"

#include <string.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace av {
	AVPlayerAdapter::AVPlayerAdapter() : FormatterPlayerAdapter() {
		typeSet.insert("AVPlayerAdapter");
	}

	void AVPlayerAdapter::createPlayer() {
		CascadingDescriptor* descriptor;
		string soundLevel;

		clog << "AVPlayerAdapter::createPlayer for '" << mrl << "'" << endl;

		if (mrl != "") {
#if HAVE_MULTIPROCESS
			playerCompName = "PlayerProcess";
			player = ((PlayerCreator*)(cm->getObject(playerCompName)))(
					myScreen, "AVPlayer");

			player->setMrl(mrl);

#elif HAVE_COMPONENTS
			playerCompName = "AVPlayer";
			player = ((PlayerCreator*)(cm->getObject(playerCompName)))(
					myScreen, mrl.c_str());
#else
			player = new AVPlayer(myScreen, mrl.c_str());
#endif
		}

		FormatterPlayerAdapter::createPlayer();

		clog << "AVPlayerAdapter::createPlayer for '" << mrl << "'" << endl;
	}

	bool AVPlayerAdapter::setPropertyValue(
		    AttributionEvent* event,
		    string value) {

		return FormatterPlayerAdapter::setPropertyValue(
				event, value);

#ifdef STx7100
#ifdef GEODE
		if (value == "") {
	  		return;
	  	}
		string propName;
		AVPlayer* vp;
		CascadingDescriptor* descriptor;
		FormatterRegion* region;
		LayoutRegion* ncmRegion;

		propName = event->getAnchor()->getPropertyName();
		propName = event->getAnchor()->getPropertyName();
				if (propName == "size" || propName == "location" ||
					propName == "bounds" ||
					propName == "top" || propName == "left" ||
					propName == "bottom" || propName == "right" ||
					propName == "width" || propName == "height") {

			if (playerObj->instanceOf("AVPlayerObject")) {
				descriptor = object->getDescriptor();
				region = descriptor->getFormatterRegion();
				ncmRegion = region->getLayoutRegion();

				vp = ((AVPlayerObject*)playerObj)->getPlayer();
				vp->setVoutWindow(ncmRegion->getAbsoluteLeft(),
					ncmRegion->getAbsoluteTop(),
					ncmRegion->getWidthInPixels(),
					ncmRegion->getHeightInPixels());

				vp->setAlphaBlend(ncmRegion->getAbsoluteLeft(),
					ncmRegion->getAbsoluteTop(),
					ncmRegion->getWidthInPixels(),
					ncmRegion->getHeightInPixels());
			}
		}
#endif /*GEODE*/
#endif /*STx7100*/
	}
}
}
}
}
}
}
}

extern "C" IPlayerAdapter* createAaVAdapter() {
	return new ::br::pucrio::telemidia::ginga::ncl::adapters::av::AVPlayerAdapter();
}
