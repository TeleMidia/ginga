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

#include "gingancl/adapters/mirror/MirrorPlayerAdapter.h"
#include "gingancl/adapters/AdaptersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace mirror {
	MirrorPlayerAdapter::MirrorPlayerAdapter(IPlayerAdapterManager* manager) :
			FormatterPlayerAdapter(manager) {
		
	}

	MirrorPlayerAdapter::~MirrorPlayerAdapter() {
		player = NULL;
	}

	void MirrorPlayerAdapter::createPlayer() {
		FormatterRegion* fRegion;
		CascadingDescriptor* descriptor;
		LayoutRegion* ncmRegion = NULL;
		GingaSurfaceID mirrorSur;

		clog << "MirrorPlayerAdapter::createPlayer '" << mrl << "'" << endl;

		bool hasVisual = true;
		string prefix = "ncl-mirror://";
		FormatterPlayerAdapter* sourceAdapter = NULL;
		ExecutionObject* execObjSrc;
		size_t pos;

		FormatterPlayerAdapter::createPlayer();

		pos = mrl.find(prefix);
		assert(pos != std::string::npos);
		if (object != NULL && object->getMirrorSrc() != NULL && player != NULL) {
			execObjSrc = object->getMirrorSrc();
			sourceAdapter = (FormatterPlayerAdapter*)manager->getObjectPlayer(execObjSrc);
			mirrorSrc = sourceAdapter->getPlayer();

			descriptor = object->getDescriptor();
			if (descriptor != NULL) {
				fRegion = descriptor->getFormatterRegion();
				if (fRegion != NULL) {
					ncmRegion = fRegion->getLayoutRegion();
					mirrorSur = dm->createSurface(
							myScreen, 
							ncmRegion->getWidthInPixels(), 
							ncmRegion->getHeightInPixels());

					player->setSurface(mirrorSur);
				}
			}
		}

		clog << "MirrorPlayerAdapter::createPlayer '";
		clog << mrl << "' ALL DONE" << endl;
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::ncl::adapters::IPlayerAdapter*
		createMirAdapter(IPlayerAdapterManager* manager, void* param) {

	return new ::br::pucrio::telemidia::ginga::ncl::adapters::mirror::MirrorPlayerAdapter(manager);
}

extern "C" void destroyMirAdapter(
		::br::pucrio::telemidia::ginga::ncl::adapters::IPlayerAdapter* player) {

	delete player;
}
