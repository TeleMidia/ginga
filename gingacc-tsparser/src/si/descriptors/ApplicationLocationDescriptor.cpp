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

#include "tsparser/ApplicationLocationDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	ApplicationLocationDescriptor::ApplicationLocationDescriptor() {
		baseDirectoryByte        = NULL;
		baseDirectoryLength      = 0;
		classPathExtensionByte   = NULL;
		classPathExtensionLength = 0;
		descriptorTag            = 0;
		initialClassByte         = NULL;
		initialClassLentgh       = 0;
	}

	ApplicationLocationDescriptor::~ApplicationLocationDescriptor() {
		if (baseDirectoryByte != NULL) {
			delete baseDirectoryByte;
		}
		if (classPathExtensionByte != NULL) {
			delete classPathExtensionByte;
		}
		if (initialClassByte != NULL) {
			delete initialClassByte;
		}
	}

	unsigned char ApplicationLocationDescriptor::getDescriptorTag() {
		return descriptorTag;
	}

	unsigned int ApplicationLocationDescriptor::getDescriptorLength() {
		return descriptorLength;
	}

	string ApplicationLocationDescriptor::getBaseDirectory() {
		string baseDir;
		baseDir.assign(baseDirectoryByte, (baseDirectoryLength & 0xFF));

		return baseDir;
	}

	unsigned int ApplicationLocationDescriptor::getBaseDirectoryLength() {
		return baseDirectoryLength;
	}

	unsigned int ApplicationLocationDescriptor::getClassPathExtensionLength(){
		return classPathExtensionLength;
	}

	string ApplicationLocationDescriptor::getClassPathExtension() {
		string classPath;
		classPath.assign(
				classPathExtensionByte, (classPathExtensionLength & 0xFF));

		return classPath;
	}

	unsigned int ApplicationLocationDescriptor::getInitialClassLength(){
		return initialClassLentgh;
	}

	string ApplicationLocationDescriptor::getInitialClass() {
		string initialClass;
		initialClass.assign(initialClassByte, (initialClassLentgh & 0xFF));

		return initialClass;
	}

	void ApplicationLocationDescriptor::print() {

	}

	size_t ApplicationLocationDescriptor::process(char* data, size_t pos) {
		descriptorTag    = data[pos];
		descriptorLength = data[pos+1];
		pos += 2;

		baseDirectoryLength = data[pos];

		baseDirectoryByte = new char[baseDirectoryLength];
		memcpy(baseDirectoryByte, data+pos+1, baseDirectoryLength);
		pos += baseDirectoryLength + 1;

		classPathExtensionLength = data[pos];

		classPathExtensionByte = new char[classPathExtensionLength];
		memcpy(classPathExtensionByte, data+pos+1, classPathExtensionLength);
		pos += classPathExtensionLength + 1;

		initialClassLentgh = descriptorLength - baseDirectoryLength -
				classPathExtensionLength - 2;

		initialClassByte = new char[initialClassLentgh];
		memcpy(initialClassByte, data+pos, initialClassLentgh);

		pos += initialClassLentgh;

		string baseDir, iniClass;
		baseDir.assign(baseDirectoryByte, (baseDirectoryLength & 0xFF));
		iniClass.assign(initialClassByte, (initialClassLentgh & 0xFF));

		return pos;
	}
}
}
}
}
}
}
}
