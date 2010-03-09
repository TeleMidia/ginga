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

#include "cm/IComponentManager.h"
#include "cm/assembler/IComponentDescription.h"
#include "cm/parser/IComponentParser.h"
#include "cm/component/IComponent.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;

#include "../include/IUpdate.h"
using namespace ::br::pucrio::telemidia::ginga::core::um;

#include <sys/stat.h>
#include <sys/types.h>

int main() {
	/*cout << "MAIN instancing CM" << endl;
	IComponentManager* cm = IComponentManager::getCMInstance();
	IComponentDescription* as;
	IUpdate* up;
	IComponent* cp;
	map<string, IComponent*>* comps;

	cout << "MAIN cm instance ok" << endl;

	string remoteUri = "http://www.telemidia.puc-rio.br/~marcio/gingaUpdate/";
	string localUri = "/tmp/gingaUpdate/";

	cp = ((ComponentCreator*)(cm->getObject("Component")))(
			"test.xml", "0.12.1", "xml");

	cp->addUri(remoteUri);

	up = ((UpdateCreator*)(cm->getObject("Update")))(cp);

	up->setLocation(localUri);
	up->start();

	comps = cm->getComponentDescription();

	as = ((ComponentDescriptionCreator*)(
			cm->getObject("ComponentDescription")))(comps);

	as->setDescriptionName("xmlGerado.xml");
	as->setLocation(localUri);
	as->describe();

	delete up;
	delete as;
	delete cp;

	cm->releaseComponentFromObject("Update");
	cm->releaseComponentFromObject("Component");
	cm->releaseComponentFromObject("ComponentDescription");*/

	return 0;
}
