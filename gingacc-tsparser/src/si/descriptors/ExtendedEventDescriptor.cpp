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

#include "../../../include/ExtendedEventDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
	ExtendedEventDescriptor::ExtendedEventDescriptor() {
		descriptorTag    = 0x4E;
		descriptorLength = 0;
		descriptorNumber = 0;
		lengthOfItems    = 0;
		textChar         = NULL;
		textLength       = 0;
		items            = NULL;

	}

	ExtendedEventDescriptor::~ExtendedEventDescriptor() {
		if (textChar != NULL) {
			delete textChar;
			textChar = NULL;
		}
		if (items != NULL) {
			vector<Item*>::iterator i;
			for (i = items->begin(); i!= items->end(); ++i){
				if ((*i)!= NULL){
					if((*i)->itemChar != NULL){
						delete ((*i)->itemChar);
					}

					if((*i)->itemDescriptionChar != NULL){
						delete ((*i)->itemDescriptionChar);
					}
					delete (*i);
				}
			}
			delete items;
			items = NULL;
		}
	}
	unsigned char ExtendedEventDescriptor::getDescriptorTag(){
		return descriptorTag;
	}

	unsigned int ExtendedEventDescriptor::getDescriptorLength() {
		return (unsigned int)descriptorLength;
	}

	unsigned int ExtendedEventDescriptor::getDescriptorNumber() {
		return (unsigned int)descriptorNumber;
	}

	unsigned int ExtendedEventDescriptor::getLastDescriptorNumber(){
		return (unsigned int)lastDescriptorNumber;
	}
	string ExtendedEventDescriptor::getLanguageCode() {
		string str;
		str.append(languageCode, 3);
		return str;
	}

	string ExtendedEventDescriptor::getTextChar(){
		string str;

		if(textChar == NULL){
			return "";
		}
		str.append(textChar, textLength);
		return str;
	}

	unsigned int ExtendedEventDescriptor::getTextLength() {
		return (unsigned int)textLength;
	}
	vector<Item*>* ExtendedEventDescriptor::getItems() {
		return items;
	}
	string ExtendedEventDescriptor::getItemDescriptionChar(struct Item* item) {
		string str;

		if(item->itemDescriptionChar == NULL){
			return "";
		}
		str.append(item->itemDescriptionChar, item->itemDescriptionLength);
		return str;
	}
	string ExtendedEventDescriptor::getItemChar(struct Item* item) {
		string str;
		if(item->itemChar == NULL){
			return "";
		}
		str.append(item->itemChar, item->itemLength);
		return str;
	}
	void ExtendedEventDescriptor::print() {
		cout << "ExtendedEventDescriptor::print" << endl;
		cout << " -descriptorNumber = "     << getDescriptorNumber()    << endl;
		cout << " -lastDescriptorNumber = " << getLastDescriptorNumber()<< endl;
		cout << " -languageCode = "         << getLanguageCode()        << endl;
		cout << " -textChar = "             << getTextChar()            << endl;

		if (lengthOfItems > 0) {
			vector<Item*>::iterator i;
			struct Item* item;

			for (i = items->begin(); i!= items->end(); ++i){
				item = ((Item*)(*i));
				cout << " -Item: ";
				cout << "itemDescriptionChar = " <<
						getItemDescriptionChar(item);
				cout << " -itemChar = " << getItemChar(item);
			}
			cout << endl;
		}
	}
	size_t ExtendedEventDescriptor::process(char* data, size_t pos) {
		unsigned char remainingBytesItems;
		unsigned char value;
		struct Item* item;
		size_t itempos;

		//cout << "ExtendedEventDescriptor::process with pos = " << pos;

		descriptorLength = data[pos+1];//pos = 19
		pos += 2; //pos = 20
		//cout << " and  lenght = " << (descriptorLength&0xFF) << endl;

		descriptorNumber = ((data[pos] & 0xF0) >> 4);//4 bits
		lastDescriptorNumber = ((data[pos] & 0x0F));//4 bits
		//cout << " Extended descriptorNumber = " << (descriptorNumber & 0xFF);
		//cout << " and lastDescriptorNumber = " << (lastDescriptorNumber & 0xFF) << endl;
		pos += 1;

		memcpy(languageCode, data+pos, 3);
		//cout << "Extended languageCode = " << languageCode <<endl;
		pos += 3;

		lengthOfItems = data[pos];
		itempos = pos;
		pos ++; //item_descriptor_lenght
		//cout << "Extended Lenght Of Items = " << (lengthOfItems & 0xFF);
		//cout << " and pos is = " << pos << endl;

		remainingBytesItems = lengthOfItems;
		items = new vector<Item*>;
		while (remainingBytesItems){

			item = new struct Item;

			item->itemDescriptionLength = data[pos];
			//cout << " itemDescriptionLenght = "<< (item->itemDescriptionLength & 0xFF);
			if (item->itemDescriptionLength != 0){

				item->itemDescriptionChar =
						new char[item->itemDescriptionLength];

				memset(item->itemDescriptionChar, 0,
						item->itemDescriptionLength);

				memcpy(item->itemDescriptionChar, data+pos+1,
						item->itemDescriptionLength);

			}
			pos += data[pos] + 1;
			item->itemLength = data[pos];

			if (item->itemLength != 0){
				item->itemChar = new char[item->itemLength];
				memset(item->itemChar, 0, item->itemLength);
				memcpy(item->itemChar, data+pos+1, item->itemLength);
				//cout << "itemLenght = " << (item->itemLength&0xFF);
				/*
				cout << "itemChar = ";
				for (int i = 0; i < item->itemLength; i++){
					cout << (item->itemChar[i]);
				}
				cout << endl;
				*/
			}
			value = item->itemDescriptionLength + item->itemLength + 2;
			items->push_back(item);
			pos += data[pos] + 1;
			//cout << "ExtendedEvent:: insertItem finishing with pos = ";
			//cout << pos << endl;
			remainingBytesItems -= value;
		}

		itempos += lengthOfItems + 1;
		if (pos != itempos){
			//cout << "Extended error in pos after all items " << endl;
			pos = itempos;
		}

		textLength = data[pos];

		if(textLength > 0){
			//cout << "ExtendedEventDescriptor::process textLength = ";
			//cout << (unsigned int) textLength << endl;
			textChar = new char[textLength];
			if(textChar == NULL){
				cout << "ExtendedEventDescriptor::process error allocating memory" << endl;
				return -1;
			}
			memset(textChar, 0, textLength);
			memcpy(textChar, data+pos+1, textLength);
			/*
			cout << "ExtendedEventDescriptor::process textChar = ";
			for (int i = 0; i < textLength; i++){
				cout << textChar[i];
			}
			cout << endl;
			*/
		}
		pos += textLength;
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
