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

#ifndef IDATAPROCESSOR_H_
#define IDATAPROCESSOR_H_

#include "system/io/interface/content/ITimeBaseProvider.h"
using namespace br::pucrio::telemidia::ginga::core::system::io;

#include "tuner/providers/ISTCProvider.h"
using namespace br::pucrio::telemidia::ginga::core::tuning;

#include "tsparser/IDemuxer.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include "dsmcc/IStreamEventListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing;

#include "dsmcc/carousel/object/IObjectListener.h"
#include "dsmcc/carousel/IServiceDomainListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::carousel;

#include "IEPGListener.h"
#include "IFilterListener.h"

#include <map>
#include <set>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
  class IDataProcessor : public IFilterListener {
	public:
		virtual ~IDataProcessor(){};

		virtual void removeOCFilterAfterMount(bool removeIt)=0;

		virtual void setSTCProvider(ISTCProvider* stcProvider)=0;
		virtual ITimeBaseProvider* getNPTProvider()=0;
		virtual void createStreamTypeSectionFilter(
				short streamType, IDemuxer* demux)=0;

		virtual void createPidSectionFilter(int pid, IDemuxer* demux)=0;
		virtual void addEPGListener(IEPGListener* listener)=0;
		virtual void addSEListener(
				string eventType, IStreamEventListener* listener)=0;

		virtual void removeSEListener(
				string eventType, IStreamEventListener* listener)=0;

		virtual void addObjectListener(IObjectListener* listener)=0;

		virtual void setServiceDomainListener(
				IServiceDomainListener* listener)=0;

		virtual void removeObjectListener(IObjectListener* listener)=0;
		virtual void receiveSection(ITransportSection* section)=0;
  };
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::dataprocessing::IDataProcessor*
		dpCreator();

typedef void dpDestroyer(
		::br::pucrio::telemidia::ginga::core::dataprocessing::IDataProcessor*);

#endif /*IDataProcessor_H_*/
