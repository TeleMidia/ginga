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

#ifndef DESCRIPTORUTIL_H_
#define DESCRIPTORUTIL_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "Descriptor.h"

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace descriptor {
  class DescriptorUtil {
	public:
		static short getFitCode(string fitName) {
			if (upperCase(fitName) == upperCase("fill")) {
  				return Descriptor::FIT_FILL;

  			} else if (upperCase(fitName) == upperCase("hidden")) {
		  		return Descriptor::FIT_HIDDEN;

		  	} else if (upperCase(fitName) == upperCase("meet")) {
		  		return Descriptor::FIT_MEET;

		  	} else if (upperCase(fitName) == upperCase("meetBest")) {
		  		return Descriptor::FIT_MEETBEST;

		  	} else if (upperCase(fitName) == upperCase("slice")) {
		  		return Descriptor::FIT_SLICE;

		  	} else {
		  		return -1;
		  	}
		}

		static string getFitName(short fit) {
			switch (fit) {
				case Descriptor::FIT_FILL:
					return "fill";

				case Descriptor::FIT_HIDDEN:
					return "hidden";

				case Descriptor::FIT_MEET:
					return "meet";

				case Descriptor::FIT_MEETBEST:
					return "meetBest";

				case Descriptor::FIT_SLICE:
					return "slice";

				default: 
					return "";
			}
		}

		static short getScrollCode(string scrollName) {
			if (upperCase(scrollName) == upperCase("none")) {
		  		return Descriptor::SCROLL_NONE;

		  	} else if (upperCase(scrollName) == upperCase("horizontal")) {
		  		return Descriptor::SCROLL_HORIZONTAL;

		  	} else if (upperCase(scrollName) == upperCase("vertical")) {
		  		return Descriptor::SCROLL_VERTICAL;

		  	} else if (upperCase(scrollName) == upperCase("both")) {
		  		return Descriptor::SCROLL_BOTH;

		  	} else if (upperCase(scrollName) == upperCase("automatic")) {
		  		return Descriptor::SCROLL_AUTOMATIC;

		  	} else {
		  		return -1;
		  	}
		}

		static string getScrollName(short scroll) {
			switch (scroll) {
				case Descriptor::SCROLL_NONE:
					return "none";

				case Descriptor::SCROLL_HORIZONTAL:
					return "horizontal";

				case Descriptor::SCROLL_VERTICAL:
					return "vertical";

				case Descriptor::SCROLL_BOTH:
					return "both";

				case Descriptor::SCROLL_AUTOMATIC:
					return "automatic";

				default: 
					return "";
			}
		}
  };
}
}
}
}
}

#endif /*DESCRIPTORUTIL_H_*/
