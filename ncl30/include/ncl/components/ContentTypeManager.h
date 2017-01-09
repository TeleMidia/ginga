/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef CONTENTTYPEMANAGER_H_
#define CONTENTTYPEMANAGER_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <pthread.h>

#include <string>
#include <map>
#include <set>
using namespace std;

/**
 * Mantem um registro das ferramentas disponiveis para exibicao dos objetos de
 * representacao. Esse registro, na realidade, mantem duas tabelas.
 * A primeira tabela guarda os visualizadores default para cada tipo MIME
 * registrado. A segunda tabela associa visualizadores a classes de
 * controladores implementadas.
 */

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {
	class ContentTypeManager {
		private:
			//static const string mimeFile = "nclConfig/mimedefs.ini";
			static ContentTypeManager *_instance;

			// tabela com o exibidor default
			//Properties *mimeDefaultTable;
			map<string, string> mimeDefaultTable;

			static string absUrl;

			pthread_mutex_t mutex;

			ContentTypeManager();

		public:
			static ContentTypeManager *getInstance();

			void setMimeFile(string mimeFile);
			void readMimeDefinitions();
			string getMimeType(string fileExtension);
	};
}
}
}
}
}

#endif /*CONTENTTYPEMANAGER_H_*/
