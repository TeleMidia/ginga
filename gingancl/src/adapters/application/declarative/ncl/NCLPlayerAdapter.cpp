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

#include "player/INCLPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "gingancl/adapters/application/declarative/ncl/NCLPlayerAdapter.h"

#include "gingancl/adapters/AdaptersComponentSupport.h"

#include "mb/ILocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace application {
namespace ncl {
	NCLPlayerAdapter::NCLPlayerAdapter(IPlayerAdapterManager* manager) :
			ApplicationPlayerAdapter(manager)  {

		typeSet.insert("NCLPlayerAdapter");
	}

	void NCLPlayerAdapter::createPlayer() {
		FormatterRegion* region = NULL;
		LayoutRegion* ncmRegion;
		CascadingDescriptor* descriptor = NULL;
		string value;
		bool isPercent;
		ISurface* s;
		NclPlayerData* childData, *playerData;
		PropertyAnchor* property;

		if (getObjectDevice() == 2) {
			clog << "NCLPlayerAdapter::createPlayer ";
			clog << " remote handler" << endl;
			return;
		}

		player = NULL;
		if (fileExists(mrl)) {
			playerCompName                = "Formatter";
			playerData                    = manager->getNclPlayerData();

			childData                     = new NclPlayerData;
			childData->screenId           = myScreen;
			childData->x                  = 0;
			childData->y                  = 0;
			childData->w                  = 0;
			childData->h                  = 0;
			childData->devClass           = playerData->devClass;
			childData->transparency       = playerData->transparency;
			childData->baseId             = playerData->baseId;
			childData->privateBaseManager = playerData->privateBaseManager;
			childData->playerId           = object->getId();
			childData->enableGfx          = false;
			childData->parentDocId        = playerData->docId;
			childData->nodeId             = ((NodeEntity*)(
					object->getDataObject()->getDataEntity()))->getId();

			childData->docId              = "";
			childData->focusManager       = playerData->focusManager;
			childData->editListener       = playerData->editListener;

			descriptor = object->getDescriptor();
			if (descriptor != NULL) {
				region = descriptor->getFormatterRegion();
			}

			if (region != NULL) {
				ncmRegion           = region->getLayoutRegion();
				childData->x        = (int)(ncmRegion->getAbsoluteLeft());
				childData->y        = (int)(ncmRegion->getAbsoluteTop());
				childData->w        = (int)(ncmRegion->getWidthInPixels());
				childData->h        = (int)(ncmRegion->getHeightInPixels());
				childData->devClass = ncmRegion->getDeviceClass();

				property = object->getNCMProperty("transparency");
				if (property != NULL) {
					value = property->getPropertyValue();

				} else {
					value = descriptor->getParameterValue("transparency");
				}

				if (value != "") {
					float transpValue;
					float parentOpacity = (1 - playerData->transparency);

					value = cvtPercentual(value, &isPercent);
					transpValue = util::stof(value);
					if (isPercent) {
						transpValue = transpValue / 100;
					}

					transpValue = (1 - (parentOpacity -
							(parentOpacity * transpValue)));

					childData->transparency = transpValue;
				}
			}

			ILocalScreenManager* dm;

#if HAVE_COMPONENTS
			dm = ((LocalScreenManagerCreator*)(
					cm->getObject("LocalScreenManager")))();

			player = ((NCLPlayerCreator*)(cm->getObject(playerCompName)))(
					childData);
#else
			dm = LocalScreenManager::getInstance();
			player = (INCLPlayer*)(new FormatterMediator(childData));
#endif

			s = dm->createSurface(myScreen);
			s->setCaps(s->getCap("ALPHACHANNEL"));

			((INCLPlayer*)player)->setSurface(s);
			if (((INCLPlayer*)player)->setCurrentDocument(mrl) == NULL) {
				clog << "NCLPlayerAdapter::createPlayer Warning! ";
				clog << "can't set '" << mrl << "' as document";
				clog << endl;
			}

			if (region != NULL) {
				((INCLPlayer*)player)->setParentLayout(
						region->getLayoutManager());
			}

		} else {
			clog << "NCLPlayerAdapter::createPlayer Warning! ";
			clog << "file not found: '" << mrl << "'" << endl;
		}

		ApplicationPlayerAdapter::createPlayer();
	}

	bool NCLPlayerAdapter::setAndLockCurrentEvent(FormatterEvent* event) {
		string interfaceId;

		lockEvent();
		if (preparedEvents.count(event->getId()) != 0 &&
				!event->instanceOf("SelectionEvent") &&
				event->instanceOf("AnchorEvent")) {

			interfaceId = ((AnchorEvent*)event)->getAnchor()->getId();

			if ((((AnchorEvent*)event)->getAnchor())->instanceOf(
					"LabeledAnchor")) {

				interfaceId = ((LabeledAnchor*)((AnchorEvent*)event)->
						getAnchor())->getLabel();

			} else if ((((AnchorEvent*)event)->getAnchor())->instanceOf(
					"LambdaAnchor")) {

				interfaceId = "";
			}

			currentEvent = event;
			((ApplicationExecutionObject*)object)->setCurrentEvent(
					currentEvent);

			if (player != NULL) {
				player->setCurrentScope(interfaceId);
			}

		} else if (event->instanceOf("AttributionEvent")) {
			interfaceId = ((AttributionEvent*)
					event)->getAnchor()->getPropertyName();

			if (player != NULL) {
				player->setScope(interfaceId, IPlayer::TYPE_ATTRIBUTION);
			}

			currentEvent = event;
			((ApplicationExecutionObject*)object)->setCurrentEvent(
					currentEvent);

			if (player != NULL) {
				player->setCurrentScope(interfaceId);
			}

		} else {
			unlockEvent();
			return false;
		}

		return true;
	}

	void NCLPlayerAdapter::unlockCurrentEvent(FormatterEvent* event) {
		if (event != currentEvent) {
			clog << "NCLPlayerAdapter::unlockCurrentEvent ";
			clog << "Handling events warning!" << endl;
		}
		unlockEvent();
	}

	void NCLPlayerAdapter::flip() {
		if (player != NULL) {
			player->flip();
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

using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

extern "C" IPlayerAdapter* createNCLAdapter(
		IPlayerAdapterManager* manager, void* param) {

	return new application::ncl::NCLPlayerAdapter(manager);
}

extern "C" void destroyNCLAdapter(IPlayerAdapter* player) {
	delete player;
}
