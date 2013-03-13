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

#include "dataprocessing/dsmcc/carousel/Binding.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	Binding::Binding() {
		ior = NULL;
	}

	Binding::~Binding() {
		releaseIor();
	}

	void Binding::releaseIor() {
		if (ior != NULL) {
			delete ior;
			ior = NULL;
		}
	}

	void Binding::setId(string id) {
		this->id = id;
	}

	string Binding::getId() {
		return id;
	}

	void Binding::setKind(string kind) {
		this->kind = kind;
	}

	string Binding::getKind() {
		return kind;
	}

	void Binding::setType(unsigned int type) {
		this->type = type;
	}

	unsigned int Binding::getType() {
		return type;
	}

	void Binding::setIor(Ior* ior) {
		releaseIor();

		this->ior = ior;
	}

	Ior* Binding::getIor() {
		return ior;
	}

	void Binding::setObjectInfo(string objectInfo) {
		this->objectInfo = objectInfo;
	}

	string Binding::getObjectInfo() {
		return this->objectInfo;
	}

	void Binding::print() {
		clog << "BINDING" << endl;
		clog << "id = " << id.c_str() << endl;
		clog << "kind = " << kind.c_str() << endl;
		clog << "type = " << type << endl;
		clog << " info = " << objectInfo.c_str() << endl;
	}
}
}
}
}
}
}
}
