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

#ifndef ISDBTFRONTEND_H_
#define ISDBTFRONTEND_H_

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <stdio.h>
#include <error.h>
#include <stdint.h>

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "../frontend_parameter.h"

#include "../IFrontendFilter.h"
#include "../../IChannel.h"

#include <map>
#include <vector>
#include <iostream>
using namespace std;


typedef struct lockedFiltersAction {
	::br::pucrio::telemidia::ginga::core::tuning::IFrontendFilter* ff;
	bool isAdd;
} ActionsToFilters;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	class ISDBTFrontend : public Thread {
		private:
			static const int IFE_FORCED_MIN_FREQ  = 450143000;
			static const int IFE_FORCED_STEP_SIZE = 1000000;

			static const short IFE_MAX_FILTERS    = 20;

			static struct pollfd pollFds[ISDBTFrontend::IFE_MAX_FILTERS];

			struct dvb_frontend_parameters params;
			struct dvb_frontend_info info;

			int feFd;
			int dmFd;
			fe_status_t feStatus;
			unsigned int currentFreq;
			static bool firstFilter;
			static vector<IFrontendFilter*>* runningFilters;
			static vector<ActionsToFilters*>* actsToRunningFilters;

		public:
			static const string IFE_FE_DEV_NAME;
			static const string IFE_DVR_DEV_NAME;
			static const string IFE_DEMUX_DEV_NAME;

			int dvrFd;
			ISDBTFrontend(int feFd);
			virtual ~ISDBTFrontend();

			bool hasFrontend();

		private:
			void initIsdbtParameters();
			void dumpFrontendInfo();
			void updateIsdbtFrontendParameters();
			bool isTuned();

		public:
			bool getSTCValue(uint64_t* stc, int* valueType);
			bool changeFrequency(unsigned int frequency);
			void scanFrequencies(vector<IChannel*>* channels);

			void attachFilter(IFrontendFilter* filter);
			int createPesFilter(int pid, int pesType, bool compositeFiler);
			void removeFilter(IFrontendFilter* filter);

		private:
			void updatePool();
			void readFilters();

			void run();
	};
}
}
}
}
}
}

#endif /*ISDBTFRONTEND_H_*/
