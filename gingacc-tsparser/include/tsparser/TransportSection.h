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

#ifndef TRANSPORTSECTION_H_
#define TRANSPORTSECTION_H_

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "ITransportSection.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof (a) / sizeof ((a)[0]))
#endif


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
	class TransportSection : public ITransportSection {
		private:
			// Original TS packet id.
			unsigned int pid;

		protected:
			// ISO/IEC 13818-1 TS SECTION HEADER
			unsigned int tableId;
			bool sectionSyntaxIndicator;

			/*
			 * Number of bytes of the section, starting immediately following
			 * the sectionLength field, and including CRC32. Thus, 9 bytes
			 * of overhead.
			 */
			unsigned int sectionLength;

			/*
			 * Transport_stream_id for PAT
			 * program_number for PMT
			 * table_id_extension for DSM_CC Section
			 */
			unsigned int idExtention;

			unsigned int versionNumber;
			bool currentNextIndicator;
			unsigned int sectionNumber;
			unsigned int lastSectionNumber;

			// Section data.
			char section[4096];

			unsigned int currentSize;

			string sectionName;

		public:
			TransportSection();
			TransportSection(char* sectionBytes, unsigned int size);

			virtual ~TransportSection();

		private:
			void initialize();

		public:
			void setESId(unsigned int id);
			unsigned int getESId();
			void addData(char* bytes, unsigned int size);

		private:
			void setSectionName();
			bool create(char *sectionBytes, unsigned int size);
			bool constructionFailed;

		public:
			string getSectionName();
			unsigned int getTableId();
			bool getSectionSyntaxIndicator();
			unsigned int getSectionLength();
			unsigned int getCurrentSize();
			unsigned int getExtensionId();
			unsigned int getVersionNumber();
			bool getCurrentNextIndicator();
			unsigned int getSectionNumber();
			unsigned int getLastSectionNumber();
			void* getPayload();
			unsigned int getPayloadSize();
			bool isConsolidated();
			bool isConstructionFailed();
			static unsigned int crc32(char *data, unsigned int len);
			void print();
	};
}
}
}
}
}
}

#endif /*TRANSPORTSECTION_H_*/
