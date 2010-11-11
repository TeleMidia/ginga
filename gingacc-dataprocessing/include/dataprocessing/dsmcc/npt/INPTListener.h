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

#ifndef INPTLISTENER_H_
#define INPTLISTENER_H_

#include "system/io/interface/content/ITimeBaseListener.h"
using namespace br::pucrio::telemidia::ginga::core::system::io;

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace dsmcc {
namespace npt {
	class INPTListener : public ITimeBaseListener {
		public:
#ifndef _WIN32
			static const double MAX_NPT_VALUE      = 47722;
#else
			static const int MAX_NPT_VALUE			= 47722;
#endif
			static const unsigned char INVALID_CID = 255;
			static const short NPT_ST_OCCURRING    = 0;
			static const short NPT_ST_PAUSED       = 1;

			//TODO: to implement NPT stream_mode (table 8.6 ISO/IEC 13818-6)

			virtual ~INPTListener(){};

			virtual void updateTimeBaseId(
					unsigned char oldContentId,
					unsigned char newContentId)=0;

			virtual void valueReached(unsigned char cid, int64_t nptValue)=0;
			virtual void loopDetected()=0;
	};
}
}
}
}
}
}
}
}

#endif /*INPTLISTENER_H_*/
