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

#ifndef _NCLSectionProcessor_H_
#define _NCLSectionProcessor_H_

#include "INCLSectionProcessor.h"

#include <sys/stat.h>
#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace ncl {
	class NCLSectionProcessor : public INCLSectionProcessor {
		private:
			IMetadata* metadata;
			vector<StreamData*>* dataToProcess;

		public:
			NCLSectionProcessor();
			~NCLSectionProcessor();

			string getRootUri();

		private:
			void clear();
			void clearMetadata();
			void clearDataToProcess();

			void createDirectory(string newDir);
			void addDataToProcess(char* stream, int streamSize);
			void processDataFile(char* stream, int streamSize);

		public:
			//receiving stream
			void process(char* stream, int streamSize);
			void mount();
			bool isConsolidated();

			IMetadata* getMetadata();

			//creating stream
			vector<StreamData*>* createNCLSections(
					string componentTag,
					string name,
					string baseUri,
					vector<string>* files,
					map<int, string>* eventMap);
	};
}
}
}
}
}
}
}

#endif //_NCLSectionProcessor_H_
