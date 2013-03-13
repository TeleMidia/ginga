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

#include "tsparser/ParentalRatingDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
	ParentalRatingDescriptor::ParentalRatingDescriptor() {
		descriptorTag = 0x55;
		descriptorLength = 0;
		countryRatings = NULL;
	}

	ParentalRatingDescriptor::~ParentalRatingDescriptor() {
		vector<Parental*>::iterator i;

		if (countryRatings != NULL) {
			for (i = countryRatings->begin(); i!= countryRatings->end(); ++i) {
				if ((*i) != NULL) {
					delete (*i);
				}
			}
			delete countryRatings;
			countryRatings = NULL;
		}
	}

	unsigned int ParentalRatingDescriptor::getDescriptorLength() {
		return (unsigned int)descriptorLength;
	}

	unsigned char ParentalRatingDescriptor::getDescriptorTag() {
		return descriptorTag;
	}

	string ParentalRatingDescriptor::getCountryCode(Parental* parental) {
		string str;

		str.append(parental->countryCode, 3);
		return str;
	}

	/*
	 * Recomended age according with binary:
	 * 0000: reserved
	 * 0001: L
	 * 0010: 10
	 * 0011: 12
	 * 0100: 14
	 * 0101: 16
	 * 0110: 18
	 */
	unsigned int ParentalRatingDescriptor::getAge(Parental* parental) {
		return (unsigned int)parental->age;
	}

	unsigned int ParentalRatingDescriptor::getContentDescription(
			Parental* parental) {

		return (unsigned int)parental->contentDescription;
	}

	vector<Parental*>* ParentalRatingDescriptor::getCountryRatings() {
		return countryRatings;
	}

	void ParentalRatingDescriptor::print() {
		clog << "ParentalRatingDescriptor::print printing..." << endl;
		clog << " -descriptorLength = " << getDescriptorLength() << endl;

		if (countryRatings != NULL){
			vector<Parental*>::iterator i;
			Parental* parental;

			for(i = countryRatings->begin(); i != countryRatings->end();++i){
				parental = ((Parental*)(*i));

				/*clog << " -country = "     << getCountryCode(parental);
				clog << " -age = "         << getAge(parental);
				clog << " -description = ";
				clog << getContentDescription(parental);
				clog << endl;*/
			}
		}
	}

	size_t ParentalRatingDescriptor::process (char* data, size_t pos){
		size_t remainingBytes;
		Parental* parental;

		//clog << "ParentalRatingDescriptor::process with pos =  " << pos ;
		descriptorLength = data[pos+1];
		//clog << " and descriptorLenght = " << (descriptorLength & 0xFF) <<endl;

		pos ++;
		if (descriptorLength > 0) {
			countryRatings = new vector<Parental*>;
			remainingBytes = descriptorLength;
		}

		while (remainingBytes){
			pos++;

			parental = new Parental;
			//memset(parental->countryCode, 0 , 3);
			memcpy(parental->countryCode, data+pos, 3);

			pos+=3;
			parental->contentDescription = ((data[pos] & 0xF0) >> 4);
			parental->age = (data[pos] & 0x0F);

			//clog << "contentDescription = ";
			//clog << (parental->contentDescription & 0xFF);
			//clog << " and age = " << (parental->age & 0XFF) << endl;

			countryRatings->push_back(parental);
			remainingBytes = remainingBytes - 4 ;
		}

		return pos;
	}
}
}
}
}
}
}
}
}
