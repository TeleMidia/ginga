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

#ifndef NclComponentsConverter_H
#define NclComponentsConverter_H

#include "ncl/reuse/ReferNode.h"
#include "ncl/reuse/ReferredNode.h"
using namespace ::br::pucrio::telemidia::ncl::reuse;

#include "ncl/switches/SwitchNode.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/interfaces/Anchor.h"
#include "ncl/interfaces/Port.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/components/NodeEntity.h"
#include "ncl/components/ContextNode.h"
#include "ncl/components/ContentNode.h"
#include "ncl/components/AbsoluteReferenceContent.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/link/Link.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "ncl/connectors/Role.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include <map>
#include <vector>
#include <string>
using namespace std;

#include "../framework/DocumentParser.h"
#include "../framework/ModuleParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "../framework/ncl/NclComponentsParser.h"
using namespace ::br::pucrio::telemidia::converter::framework::ncl;

#include "NclPresentationSpecConverter.h"
#include "NclLinkingConverter.h"
#include "NclInterfacesConverter.h"
#include "NclPresentationControlConverter.h"

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
  class NclComponentsConverter : public NclComponentsParser {
	/*tabela com a lista dos elementos q devem ser compilados ao fim do
	 * processamento da composicao, ja' que referenciam outros elementos
	 * TODO deve ser feito o mesmo para linkBase, mas como esse elemento costuma
	 * vir ao final da composicao, foi ignorado pelo momento
	 * para reaproveitar:
	 * 1) copiar e inicializar as vars. abaixo
	 * 2) criar addbinddescriptor...2 para representar o add original
	 * 3) criar addcompositeinterface...2 para representar o add original
	 * 4) copiar o addbinddescriptor... addcompositeinterface...
	 *    posCreateCompositionContentGroup e posCompileCompositionContentGroup
	 *
	 * 5)chamar o posCreateCompositionContentGroup do createcomposition
	 * 6) chamar o posCreateCompositionContentGroup na criacao do body
	 * 7) chamar o posCompileCompositionContentGroup no poscompile do body
	 * 8) criar createBindDescriptorGroup2 e fazer o antigo retornar o elemento
	 * 9) criar createCompositeInterfaceElementGroup2 e fazer o antigo retornar
	 *    o elemento
	 */

	public:
		NclComponentsConverter(DocumentParser *documentParser);
		void addPortToContext(void *parentObject, void *childObject);
		void addPropertyToContext(void *parentObject, void *childObject);
		void addContextToContext(void *parentObject, void *childObject);
		void addMediaToContext(void *parentObject, void *childObject);
		void addLinkToContext(void *parentObject, void *childObject);

	private:
		void addNodeToContext(ContextNode *contextNode, NodeEntity *node);
		void addAnchorToMedia(ContentNode *contentNode, Anchor *anchor);

	public:
		void addAreaToMedia(void *parentObject, void *childObject);
		void addPropertyToMedia(void *parentObject, void *childObject);
		void addSwitchToContext(void *parentObject, void *childObject);
		void *createContext(DOMElement *parentElement, void *objGrandParent);
		void *posCompileContext(DOMElement *parentElement, void *parentObject);
		void *createMedia(DOMElement *parentElement, void *objGrandParent);
  };
}
}
}
}
}

#endif //NclComponentsConverter
