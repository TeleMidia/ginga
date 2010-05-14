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
#include "../../../include/SeriesDescriptor.h"


namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si{
namespace descriptors{
	SeriesDescriptor::SeriesDescriptor() {
		descriptorTag       = 0xD5;
		descriptorLength    = 0;
		episodeNumber       = 0;
		seriesNameChar      = NULL;
		expireDate[0]       = 0;
		expireDate[1]       = 0;
		expireDateValidFlag = false;
		lastEpisodeNumber   = 0;
		programPattern      = 0;
		repeatLabel         = 0;
		seriesId            = 0;
		seriesNameChar      = NULL;
		seriesNameLength    = 0;
	}

	SeriesDescriptor::~SeriesDescriptor() {
		if(seriesNameChar != 0){
			delete seriesNameChar;
			seriesNameChar = NULL;
		}
	}

	unsigned char SeriesDescriptor::getDescriptorTag() {
		return descriptorTag;
	}

	unsigned int SeriesDescriptor::getDescriptorLength() {
		return (unsigned int)descriptorLength;
	}

	unsigned short SeriesDescriptor::getSeriesId() {
		return seriesId;
	}

	unsigned char SeriesDescriptor::getRepeatLabel() {
		return repeatLabel;
	}

	unsigned char SeriesDescriptor::getProgramPattern() {
		return programPattern;
	}

	unsigned short SeriesDescriptor::getEpisodeNumber() {
		return episodeNumber;
	}

	unsigned short SeriesDescriptor::getLastEpisodeNumber() {
		return lastEpisodeNumber;
	}

	string SeriesDescriptor::getSeriesNameChar() {
		string str;

		if(seriesNameChar == NULL){
			return "";
		}
		str.append(seriesNameChar, seriesNameLength);
		return str;
	}
	/*
	map<string, string> SeriesDescriptor::mapGenerator() {
		map<string, string> series;
		stringstream ss;

		ss << seriesId;
		t["id"] = ss.str();
		ss = "";

		ss << repeatLabel;
		t["repeatLabel"] = ss.str();
		ss = "";

		ss << programPattern;
		t["programPattern"] =  ss.str();
		ss = "";

		ss << lastEpisodeNumber;
		t["lastEpisodeNumber"] = ss.str();
		ss = "";

		t["name"] = getSeriesNameChar();

		return series;

	}
	*/
	void SeriesDescriptor::print(){
		cout << "SeriesDescriptor::print printing...";
		cout << " -seriesId = "            << seriesId              << endl;
		cout << " -descriptorLength = "    << getDescriptorLength() << endl;
		cout << " -episodeNumber = "       << episodeNumber         << endl;
		cout << " -lastEpisodeNumber = "   << lastEpisodeNumber     << endl;
		cout << " -seriesNameChar = "      << getSeriesNameChar()   << endl;
	}
	size_t SeriesDescriptor::process(char* data, size_t pos){
		//cout << "SeriesDescriptor::process with pos = " << pos;

		descriptorLength = data[pos+1];
		pos += 2;

		//cout << "and descriptorlength = ";
		//cout<< (descriptorLength & 0xFF) << endl;

		seriesId = (((data[pos] << 8) & 0xFF00) | (data[pos+1] & 0xFF));
		//cout << "Series seriedId = "<< (seriesId & 0xFF) << endl;
		pos += 2; //pos = 22

		repeatLabel = ((data[pos] & 0xF0) >> 4);
		programPattern = ((data[pos] & 0x0E) >> 1);
		expireDateValidFlag =  (data[pos] & 0x01);
		pos ++;//pos = 23

		//cout << "Series repeatLabel" << repeatLabel;
		//cout << " and programPattern = " << programPattern;
		//cout << " and expireDateValidFlag = " << expireDateValidFlag << endl;

		expireDate[0] = data[pos];
		//cout << "Series expireDate = " << (expireDate[0]&0xFF);
		expireDate[1] = data[pos+1];
		//cout <<"."<< (expireDate[1] & 0xFF) << endl;
		pos += 2;//pos = 25

		episodeNumber = (((data[pos] << 4) & 0xFF00) |
				(data[pos+1] & 0xF0) >> 4);

		lastEpisodeNumber =  (((data[pos+1] & 0x0F) << 8) |
				(data[pos+2] & 0xFF) >> 8);

		pos+=3;//pos = 28
		//cout << "Series episodeNumber = " << (episodeNumber & 0xFF);
		//cout << " and lastEpisodeNUmber = " ;
		//cout << (lastEpisodeNumber & 0xFF) << endl;

		seriesNameLength = descriptorLength - 8;
		if (seriesNameLength > 0){
			seriesNameChar = new char[seriesNameLength];
			memset(seriesNameChar, 0, seriesNameLength);
			memcpy(seriesNameChar, data+pos, seriesNameLength);
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
