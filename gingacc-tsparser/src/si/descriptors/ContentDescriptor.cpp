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
//jumping reserved_future_use (first 4 bits of data[pos])
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

#include "../../../include/ContentDescriptor.h"
namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si{
namespace descriptors{
//TODO: get methods?
	ContentDescriptor::ContentDescriptor() {
		descriptorTag       = 0x54;
		//contentNibbleLevel1 = 0;
		//contentNibbleLevel2 = 0;
		//userNibble1         = 0;
		//userNibble2         = 0;
		contents              = NULL;
	}

	ContentDescriptor::~ContentDescriptor() {
		if (contents != NULL) {
			vector<Content*>::iterator i;
			for(i = contents->begin(); i != contents->end(); ++i){
				delete(*i);
			}
			delete contents;
			contents = NULL;
		}
	}

	unsigned char ContentDescriptor::getDescriptorTag() {
		return descriptorTag;
	}

	unsigned int ContentDescriptor::getDescriptorLength() {
		return (unsigned int)descriptorLength;
	}

	unsigned short ContentDescriptor::getContentNibble1(struct Content* content){
		return content->contentNibbleLevel1;
	}

	unsigned short ContentDescriptor::getContentNibble2(struct Content* content){
		return content->contentNibbleLevel2;
	}

	unsigned short ContentDescriptor::getUserNibble1(struct Content* content) {
		return content->userNibble1;
	}

	unsigned short ContentDescriptor::getUserNibble2(struct Content* content) {
		return content->userNibble2;
	}
	vector<Content*>* ContentDescriptor::getContents( ){
		return contents;
	}

	void ContentDescriptor::print() {
		cout << "ContentDescriptor::print printing..." << endl;
		cout << " -descriptorLength = " << getDescriptorLength() << endl;
		if(contents != NULL){
			vector<Content*>::iterator i;
			struct Content* content;

			for(i = contents->begin(); i != contents->end(); ++i) {
				content = ((struct Content *)(*i));
				cout << " -contentNibble1 = " << getContentNibble1(content);
				cout << " -contentNibble2 = " << getContentNibble2(content);
				cout << " -userNibble1 = "    << getUserNibble1(content);
				cout << " -userNibble2 = "    << getUserNibble2(content);
			}
		}
		cout << endl;

	}

	size_t ContentDescriptor::process(char* data, size_t pos){
		size_t remainingBytes = 0;
		struct Content* content;

		//cout << "ContentDescriptor process with pos = " << pos;

		descriptorLength = data[pos+1];

		//cout << " and length = " << (descriptorLength & 0xFF) << endl;
		//pos += 2;
		pos++;
		remainingBytes = descriptorLength;
		contents = new vector<Content*>;

		while (remainingBytes > 0){

			pos++;
			content = new struct Content;
			content->contentNibbleLevel1 = ((data[pos] & 0xF0) >> 4);
			content->contentNibbleLevel2 = (data[pos] & 0x0F);

			//cout << "Content contentNibble 1 = " << ((unsigned int)contentNibbleLevel1 ) ;
		    //cout << " and 2 = " << ((unsigned int)contentNibbleLevel2) << endl;
			pos ++;

			content->userNibble1 = ((data[pos] & 0xF0) >> 4);
			content->userNibble2 = (data[pos] & 0x0F);

			//cout << "userNibble 1 = " << ((unsigned int)userNibble1) ;
			//cout << " and 2 = " << ((unsigned int)userNibble2&0xFF) << endl;
			contents->push_back(content);
			remainingBytes = remainingBytes - 2;
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
