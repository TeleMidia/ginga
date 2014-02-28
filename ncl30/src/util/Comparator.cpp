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

#include "ncl/util/Comparator.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace util {
	bool Comparator::evaluate(
		    string first,
		    string second,
		    short comparator) { //2 object -> comparable

		if (first == "" || second == "") {
			return false;
		}

		int ret;
		ret = first.compare(second);
		switch (comparator) {
			case CMP_EQ:
				if (ret == 0)
					return true;
				else
					return false;

			case CMP_NE:
				if (ret == 0)
					return false;
				else
					return true;

			case CMP_LT:
				if (ret < 0)
					return true;
				else
					return false;

			case CMP_LTE:
				if (ret <= 0)
					return true;
				else
					return false;

			case CMP_GT:
				if (ret > 0)
					return true;
				else
					return false;

			case CMP_GTE:
				if (ret >= 0)
					return true;
				else
					return false;

			default:
				return false;
		}
	}

	bool Comparator::evaluate(
		    float first,
		    float second,
		    short comparator) { //2 object -> comparable

		int ret;
		if (first == second)
			ret = 0;
		else if (first < second)
			ret = -1;
		else
			ret = 1;

		switch (comparator) {
			case CMP_EQ:
				if (ret == 0)
					return true;
				else
					return false;

			case CMP_NE:
				if (ret == 0)
					return false;
				else
					return true;

			case CMP_LT:
				if (ret < 0)
					return true;
				else
					return false;

			case CMP_LTE:
				if (ret <= 0)
					return true;
				else
					return false;

			case CMP_GT:
				if (ret > 0)
					return true;
				else
					return false;

			case CMP_GTE:
				if (ret >= 0)
					return true;
				else
					return false;

			default:
				return false;
		}
	}

	string Comparator::toString(short comparator) {

		switch (comparator) {
			case Comparator::CMP_EQ:
				return "eq";

			case Comparator::CMP_NE:
				return "ne";

			case Comparator::CMP_GT:
				return "gt";

			case Comparator::CMP_GTE:
				return "gte";

			case Comparator::CMP_LT:
				return "lt";

			case Comparator::CMP_LTE:
				return "lte";

			default :
				return "eq";
		}
	}

	short Comparator::fromString(string comp) {
		string comparator = comp;
		for(unsigned i =0; i<comparator.length(); i++) {
			if(comparator[i]>='A' && comparator[i]<='Z') {
				comparator[i] = comparator[i] - 'A' + 'a';
			}
		}

		if (comparator=="eq") {
  			return Comparator::CMP_EQ;
		}
  		else if (comparator=="ne") {
  			return Comparator::CMP_NE;
	  	}
	  	else if (comparator=="gt") {
	  		return Comparator::CMP_GT;
	  	}
	  	else if (comparator=="lt") {
	  		return Comparator::CMP_LT;
	  	}
	  	else if (comparator=="gte") {
	  		return Comparator::CMP_GTE;
	  	}
	  	else if (comparator=="lte") {
	  		return Comparator::CMP_LTE;
	  	}
	  	else {
	  		return Comparator::CMP_EQ;
	  	}
	}
}
}
}
}
}
