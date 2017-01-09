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

#include "gingancl/FormatterMediator.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;

#include "gingancl/adapters/image/ImagePlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::image;

#include "config.h"

#include "mb/LocalScreenManager.h"
#include "mb/ILocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#if HAVE_LINKS
#include "gingancl/adapters/application/declarative/xhtml/links/LinksPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::application::xhtml;
#endif

int main(int argc, char** argv, char** envp) {
	FormatterMediator* formatter;
	IScreenManager* dm;
	NclPlayerData* data = new NclPlayerData;
	IPlayerAdapter* player;
	IWindow* w;
	ISurface* s;
	GingaScreenID screenId;

	dm = ScreenManagerFactory::getInstance();
	screenId = dm->createScreen(0, NULL);

	if (argc > 1 && strcmp(argv[1], "links") == 0) {
#if HAVE_LINKS

		w = dm->createWindow(screenId, 120, 120, 400, 400, 0.0);

		w->setCaps(w->getCap("ALPHACHANNEL"));
		w->draw();
		w->show();

		string mrl = "www.google.com";
#endif

	} else {
		clog << "Test::main() initializing formatter" << endl;
		data->screenId          = screenId;
		data->baseId            = "test";
		data->playerId          = "test";
		data->devClass          = 0;
		data->x                 = 0;
		data->y                 = 0;
		data->w                 = 0;
		data->h                 = 0;
		data->enableGfx         = false;
		data->parentDocId       = "";
		data->nodeId            = "";
		data->docId             = "";
		data->focusManager      = NULL;
		data->editListener      = NULL;

		formatter = new FormatterMediator(data);
	}

	dm->clearWidgetPools(screenId);

	//TODO: tests
	return 0;
}
