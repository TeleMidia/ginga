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

#include "gingancl/adapters/av/tv/ProgramAVPlayerAdapter.h"

#include "gingancl/adapters/AdaptersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace av {
namespace tv {
	ProgramAVPlayerAdapter::ProgramAVPlayerAdapter() : FormatterPlayerAdapter() {
		typeSet.insert("ProgramAVPlayerAdapter");
		lastValue = "";
	}

	ProgramAVPlayerAdapter* ProgramAVPlayerAdapter::_instance = 0;

	ProgramAVPlayerAdapter* ProgramAVPlayerAdapter::getInstance() {
		if (ProgramAVPlayerAdapter::_instance == NULL) {
			ProgramAVPlayerAdapter::_instance = new ProgramAVPlayerAdapter();
		}

		return ProgramAVPlayerAdapter::_instance;
	}

	bool ProgramAVPlayerAdapter::hasPrepared() {
		return object != NULL;
	}

	bool ProgramAVPlayerAdapter::start() {
		return FormatterPlayerAdapter::start();
	}
	
	bool ProgramAVPlayerAdapter::stop() {
		player->setPropertyValue("bounds", "");     // full screen

		//TODO: create a map to handle each default value (before AIT app start cmd)
		player->setPropertyValue("soundLevel", "1.0"); // full sound

		return FormatterPlayerAdapter::stop();
	}

	bool ProgramAVPlayerAdapter::resume() {
		updateAVBounds();
		return FormatterPlayerAdapter::resume();
	}

	void ProgramAVPlayerAdapter::createPlayer() {
		CascadingDescriptor* descriptor;
		string soundLevel;

		player = ProgramAV::getInstance(myScreen);

		FormatterPlayerAdapter::createPlayer();
		updateAVBounds();
	}

	bool ProgramAVPlayerAdapter::setPropertyValue(
		    AttributionEvent* event, string value) {

		string propName;
		bool attribution;

		attribution = FormatterPlayerAdapter::setPropertyValue(
				event, value);

		propName = event->getAnchor()->getPropertyName();
		clog << "ProgramAVPlayerAdapter::setPropertyValue '";
		clog << propName << "' ";
		clog << value << "'" << endl;

		if (propName == "size" || propName == "location" ||
				propName == "bounds" ||
				propName == "top" || propName == "left" ||
				propName == "bottom" || propName == "right" ||
				propName == "width" || propName == "height") {

			updateAVBounds();

		} else if (propName == "standby") {
			if (value == "true") {
				player->setPropertyValue("bounds", ""); // fullscreen
				lastValue = player->getPropertyValue("soundLevel");
				player->setPropertyValue("soundLevel", ""); // audio

			} else {
				updateAVBounds();
				player->setPropertyValue("soundLevel", lastValue); // audio
			}

		} else {
			player->setPropertyValue(propName, value);
		}

		return attribution;
	}

	void ProgramAVPlayerAdapter::updateAVBounds() {
		CascadingDescriptor* descriptor;
		FormatterRegion* region;
		LayoutRegion* ncmRegion;

		if (object != NULL && object->getDescriptor() != NULL) {
			descriptor = object->getDescriptor();
			region     = descriptor->getFormatterRegion();
			ncmRegion  = region->getLayoutRegion();

			player->setPropertyValue(
					"bounds",
					itos(ncmRegion->getAbsoluteLeft()) + "," +
					itos(ncmRegion->getAbsoluteTop()) + "," +
					itos(ncmRegion->getWidthInPixels()) + "," +
					itos(ncmRegion->getHeightInPixels()));



		} else {
			clog << "ProgramAVPlayerAdapter::updateAVBounds Warning!";
			clog << "NULL object or descriptor" << endl;
		}
	}
}
}
}
}
}
}
}
}

extern "C" IPlayerAdapter* createPAVAdapter() {
	return ::br::pucrio::telemidia::ginga::ncl::adapters::av::tv::
			ProgramAVPlayerAdapter::getInstance();
}
