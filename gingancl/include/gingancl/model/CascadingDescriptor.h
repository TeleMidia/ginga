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

#ifndef _CASCADINGDESCRIPTOR_H_
#define _CASCADINGDESCRIPTOR_H_

#include "util/functions.h"
#include "util/Color.h"

#include "ncl/Parameter.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl/descriptor/Descriptor.h"
#include "ncl/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl/switches/DescriptorSwitch.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl/navigation/FocusDecoration.h"
#include "ncl/navigation/KeyNavigation.h"
using namespace ::br::pucrio::telemidia::ncl::navigation;

#include "ncl/transition/Transition.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "FormatterRegion.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include <string>
#include <map>
#include <vector>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace presentation {
  class CascadingDescriptor {
	protected:
		set<string> typeSet;

	private:
		static int dummyCount;
		string id;
		vector<GenericDescriptor*> descriptors;
		vector<GenericDescriptor*> unsolvedDescriptors;
		map<string, string> parameters;
		double explicitDuration;
		string playerName;
		long repetitions;
		bool freeze;
		LayoutRegion* region;
		FormatterRegion* formatterRegion;

		string focusIndex, moveUp, moveDown, moveLeft, moveRight;
		string focusSrc, selectionSrc;
		Color* focusBorderColor;
		Color* selBorderColor;
		int focusBorderWidth;
		int selBorderWidth;
		double focusBorderTransparency;
		vector<Transition*>* inputTransitions;
		vector<Transition*>* outputTransitions;

		void cascadeDescriptor(Descriptor* descriptor);

	public:
		CascadingDescriptor(GenericDescriptor* firstDescriptor);
		CascadingDescriptor(CascadingDescriptor* descriptor);
		~CascadingDescriptor();

	protected:
		void initializeCascadingDescriptor();

	public:
		bool instanceOf(string s);
		string getId();
		bool isLastDescriptor(GenericDescriptor* descriptor);
		void cascade(GenericDescriptor* preferredDescriptor);
		GenericDescriptor* getUnsolvedDescriptor(int i);
		vector<GenericDescriptor*>* getUnsolvedDescriptors();
		void cascadeUnsolvedDescriptor();
		double getExplicitDuration();
		bool getFreeze();
		string getPlayerName();
		LayoutRegion* getRegion();
		LayoutRegion* getRegion(void* formatterLayout, void* object);
		FormatterRegion* getFormatterRegion();

	private:
		void updateRegion(void* formatterLayout, string name, string value);

		void createDummyRegion(void* formatterLayout, void* executionObject);
		void createDummyRegion(void* formatterLayout);

	public:
		void setFormatterLayout(void* formatterLayout);
		long getRepetitions();
		vector<Parameter*>* getParameters();
		string getParameterValue(string paramName);
		vector<GenericDescriptor*>* getNcmDescriptors();
		Color* getFocusBorderColor();
		double getFocusBorderTransparency();
		int getFocusBorderWidth();
		string getFocusIndex();
		string getFocusSrc();
		string getSelectionSrc();
		string getMoveDown();
		string getMoveLeft();
		string getMoveRight();
		string getMoveUp();
		Color* getSelBorderColor();
		int getSelBorderWidth();
		vector<Transition*>* getInputTransitions();
		vector<Transition*>* getOutputTransitions();
  };
}
}
}
}
}
}
}

#endif //_CASCADINGDESCRIPTOR_H_
