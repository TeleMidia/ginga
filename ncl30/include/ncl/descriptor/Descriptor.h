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

#ifndef _DESCRIPTOR_H_
#define _DESCRIPTOR_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "../Entity.h"
#include "../Parameter.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "../layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "../navigation/FocusDecoration.h"
#include "../navigation/KeyNavigation.h"
using namespace ::br::pucrio::telemidia::ncl::navigation;

#include "../transition/Transition.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "GenericDescriptor.h"

#include <string>
#include <map>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace descriptor {
	class Descriptor : public GenericDescriptor {
		public:
			static const short FIT_FILL = 0;
			static const short FIT_HIDDEN = 1;
			static const short FIT_MEET = 2;
			static const short FIT_MEETBEST = 3;
			static const short FIT_SLICE = 4;

			static const short SCROLL_NONE = 0;
			static const short SCROLL_HORIZONTAL = 1;
			static const short SCROLL_VERTICAL = 2;
			static const short SCROLL_BOTH = 3;
			static const short SCROLL_AUTOMATIC = 4;

		protected:
			double explicitDuration;
			string presentationTool;
			bool freeze;
			long repetitions;
			LayoutRegion* region;
			map<string, Parameter*> parameters;

			KeyNavigation* keyNavigation;
			FocusDecoration* focusDecoration;
			vector<Transition*> inputTransitions;
			vector<Transition*> outputTransitions;

		public:
			Descriptor(string id);
			virtual ~Descriptor();
			double getExplicitDuration();
			string getPlayerName();

			LayoutRegion* getRegion();
			long getRepetitions();
			bool isFreeze();
			void setFreeze(bool freeze);
			void setExplicitDuration(double d);
			void setPlayerName(string name);
			void setRegion(LayoutRegion* region);
			void setRepetitions(long r);
			void addParameter(Parameter *parameter);
			vector<Parameter*> *getParameters();
			Parameter *getParameter(string paramName);
			void removeParameter(Parameter *parameter);
			KeyNavigation* getKeyNavigation();
			void setKeyNavigation(KeyNavigation* keyNav);
			FocusDecoration* getFocusDecoration();
			void setFocusDecoration(FocusDecoration* focusDec);
			vector<Transition*>* getInputTransitions();
			bool addInputTransition(Transition* transition, int pos);
			void removeInputTransition(Transition* transition);
			void removeAllInputTransitions();
			vector<Transition*>* getOutputTransitions();
			bool addOutputTransition(Transition* transition, int pos);
			void removeOutputTransition(Transition* transition);
			void removeAllOutputTransitions();
	};
}
}
}
}
}

#endif //_DESCRIPTOR_H_
