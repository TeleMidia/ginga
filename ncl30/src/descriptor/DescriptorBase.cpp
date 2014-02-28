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

#include "ncl/descriptor/DescriptorBase.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace descriptor {
	DescriptorBase::DescriptorBase(string id) : Base(id) {
		descriptorSet = new vector<GenericDescriptor*>;
		typeSet.insert("DescriptorBase");
	}

	DescriptorBase::~DescriptorBase() {
		vector<GenericDescriptor*>::iterator i;

		if (descriptorSet != NULL) {
			i = descriptorSet->begin();
			while (i != descriptorSet->end()) {
				delete *i;
				++i;
			}
			delete descriptorSet;
			descriptorSet = NULL;
		}
	}

	bool DescriptorBase::addDescriptor(GenericDescriptor* descriptor) {
		if (descriptor == NULL)
			return false;

		vector<GenericDescriptor*>::iterator i;
		for (i=descriptorSet->begin(); i!= descriptorSet->end(); ++i) {
			if (*i == descriptor) {
				return false;
			}
		}
		descriptorSet->push_back(descriptor);
		return true;
	}

	bool DescriptorBase::addBase(Base* base, string alias, string location)
		     throw(IllegalBaseTypeException*) {

		if (base->instanceOf("DescriptorBase")) {
			return Base::addBase(base, alias, location);
		}
		return false;
	}

	void DescriptorBase::clear() {
		descriptorSet->clear();
		Base::clear();
	}

	GenericDescriptor* DescriptorBase::getDescriptorLocally(string descriptorId) {
		vector<GenericDescriptor*>::iterator descriptors;

		descriptors = descriptorSet->begin();
		while (descriptors != descriptorSet->end()) {
			if ((*descriptors)->getId() == descriptorId) {
				return (*descriptors);
			}
			++descriptors;
		}
		return NULL;
	}

	GenericDescriptor* DescriptorBase::getDescriptor(string descriptorId) {
		string::size_type index;
		string prefix, suffix;
		DescriptorBase* base;

		index = descriptorId.find_first_of("#");
		if (index == string::npos) {
			return getDescriptorLocally(descriptorId);
		}
		prefix = descriptorId.substr(0, index);
		index++;
		suffix = descriptorId.substr(index, descriptorId.length() - index);
		if (baseAliases.find(prefix) != baseAliases.end()) {
			base = (DescriptorBase*)(baseAliases[prefix]);
			return base->getDescriptor(suffix);

		} else if (baseLocations.find(prefix) != baseLocations.end()) {
			base = (DescriptorBase*)(baseLocations[prefix]);
			return base->getDescriptor(suffix);

		} else {
			return NULL;
		}
	}

	vector<GenericDescriptor*>* DescriptorBase::getDescriptors() {
		return descriptorSet;
	}

	bool DescriptorBase::removeDescriptor(GenericDescriptor* descriptor) {
		vector<GenericDescriptor*>::iterator i;
		for (i=descriptorSet->begin(); i!=descriptorSet->end(); ++i) {
			if (*i == descriptor) {
				descriptorSet->erase(i);
				return true;
			}
		}
		return false;
	}
}
}
}
}
}
