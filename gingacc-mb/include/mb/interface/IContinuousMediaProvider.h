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

#ifndef ICONTINUOUSMEDIAPROVIDER_H_
#define ICONTINUOUSMEDIAPROVIDER_H_

#include "ISurface.h"

#include "IProviderListener.h"

extern "C" {
#include <stdint.h>
}

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class IContinuousMediaProvider {
		public:
			virtual ~IContinuousMediaProvider(){};
			virtual void setLoadSymbol(string symbol)=0;
			virtual string getLoadSymbol()=0;
			virtual void setAVPid(int aPid, int vPid)=0;
			virtual void feedBuffers()=0;
			virtual bool checkVideoResizeEvent(ISurface* frame)=0;
			virtual double getTotalMediaTime()=0;
			virtual int64_t getVPts()=0;
			virtual double getMediaTime()=0;
			virtual void setMediaTime(double pos)=0;
			virtual void playOver(
					ISurface* surface,
					bool hasVisual=true, IProviderListener* listener=NULL)=0;

			virtual void resume(ISurface* surface, bool hasVisual)=0;
			virtual void pause()=0;
			virtual void stop()=0;
			virtual void setSoundLevel(float level)=0;
			virtual void* getContent()=0;
			virtual void getOriginalResolution(int* height, int* width)=0;
			virtual bool releaseAll()=0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::mb::IContinuousMediaProvider*
		CMPCreator(GingaScreenID screenId, const char* mrl);

typedef void CMPDestroyer(::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* cmp);

#endif /*ICONTINUOUSMEDIAPROVIDER_H_*/
