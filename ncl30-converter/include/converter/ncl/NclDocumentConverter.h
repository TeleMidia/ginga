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

#ifndef NclDocumentConverter_H
#define NclDocumentConverter_H

#include "ncl/components/Node.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "../framework/DocumentParser.h"
#include "../framework/ModuleParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "../framework/ncl/NclDocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework::ncl;

#include "ncl/NclDocument.h"
#include "ncl/IPrivateBaseContext.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "../IDocumentConverter.h"

#include <string>
using namespace std;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
  class NclDocumentConverter : public IDocumentConverter,
		  public NclDocumentParser {

	private:
		IPrivateBaseContext* privateBaseContext;
		bool ownManager;

		bool parseEntityVar;
		void* parentObject;

	public:
		NclDocumentConverter();
		virtual ~NclDocumentConverter();

		void setConverterInfo(
				IPrivateBaseContext* pbc, IDeviceLayout* deviceLayout);

	private:
		virtual void initializeInstance(std::string& data, short scenario);
		virtual void testInstance(std::string& data, short scenario);

	protected:
		void checkManager();
		virtual void initialize();
		virtual void* parseRootElement(DOMElement *rootElement);

	public:
		string getAttribute(void* element, string attribute);
		Node *getNode(string id);
		bool removeNode(Node *node);
		IPrivateBaseContext* getPrivateBaseContext();
		NclDocument* importDocument(string* docLocation);
		void* parseEntity(string entityLocation,
			    NclDocument* document,
			    void* parent);

		void* getObject(string tableName, string key) {
			return DocumentParser::getObject(tableName, key);
		}

		void* parse(string uri, string iUriD, string fUriD) {
			return DocumentParser::parse(uri, iUriD, fUriD);
		}
  };
}
}
}
}
}

#endif
