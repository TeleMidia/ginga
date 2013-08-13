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

#include "tuner/providers/Channels.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
Channels::Channels() {
	defaultFreq = -1;
	channelList = new(map<long, string>);
	virtualChannelList = new(map<long, unsigned char>);
}

Channels::~Channels() {
	if (channelList) delete channelList;
	if (virtualChannelList) delete virtualChannelList;
}

void Channels::insertFreq(string name, long freq, unsigned char virtualChannel) {
	(*channelList)[freq] = name;
	(*virtualChannelList)[freq] = virtualChannel;
}

void Channels::removeFreq(long freq) {
	channelList->erase (freq);
	virtualChannelList->erase (freq);
}

string Channels::getName(long freq) {
	for (map<long,string>::iterator it=channelList->begin(); it!=channelList->end(); ++it)
		if (it->first == freq) {
			return it->second;
		}
	return "";
}

long Channels::getFreqByName(string name) {
	for (map<long,string>::iterator it=channelList->begin(); it!=channelList->end(); ++it)
		if (it->second.compare(name) == 0) {
			return it->first;
		}
	return -1;
}

unsigned char Channels::getVirtualChannel(long freq) {
	if (virtualChannelList->count(freq)) {
		return (*virtualChannelList)[freq];
	}
	return 0;
}

long Channels::getPreviousFreq(long currentFreq) {
	for (map<long,string>::reverse_iterator it=channelList->rbegin(); it!=channelList->rend(); ++it)
		if (it->first < currentFreq) {
			return it->first;
		}
	return -1;
}

long Channels::getNextFreq(long currentFreq) {
	for (map<long,string>::iterator it=channelList->begin(); it!=channelList->end(); ++it)
		if (it->first > currentFreq) {
			return it->first;
		}
	return -1;
}

void Channels::cleanList() {
	channelList->clear();
	virtualChannelList->clear();
}

int Channels::getListSize() {
	return channelList->size();
}

int Channels::loadFromFile(string filename) {
	string line;
	char error = 0;
	ifstream myfile (filename);
	if (myfile.is_open()) {
		unsigned found;
		long freq, vch, isDefault;
		string name;
		defaultFreq = -1;
		while (myfile.good()) {
			getline (myfile,line);
			for (int i = 0; i < 3; i++) {
				found=line.find(';');
				if (found!=std::string::npos) {
					string str = line.substr (0, found);
					line = line.substr(found+1, 255);
					switch (i) {
					case 0:
						isDefault = atol(str.c_str());
						break;
					case 1:
						freq = atol(str.c_str());
						break;
					case 2:
						vch = atol(str.c_str());
						insertFreq(line, freq, (unsigned char)vch);
						if (isDefault) defaultFreq = freq;
						break;
					}
				} else {
					error = 1;
					break;
				}
			}
			if (error) break;
		}
		myfile.close();
		if (error) return -2;
		return 0;
	} else clog << "Channels::loadFromFile - Unable to open file"; 
	return -1;
}

int Channels::saveToFile(string filename) {
	map<long, string>::iterator it;
	map<long, unsigned char>::iterator itv;
	string value;
	ofstream myfile (filename);
	if (myfile.is_open()) {
		it=channelList->begin();
		itv=virtualChannelList->begin();
		while ((it!=channelList->end()) && (itv!=virtualChannelList->end())) {
			char buffer[7];
			if (it->first == defaultFreq) {
				myfile << "1;";
			} else {
				myfile << "0;";
			}
			_itoa_s(it->first,buffer,10);
			value.assign(buffer);
			myfile << buffer << ";";
			_itoa_s(itv->second,buffer,10);
			_itoa_s(itv->second,buffer,10);
			myfile << buffer << ";" << it->second << endl;
			++it;
			++itv;
		}
		myfile.close();
		return 0;
	}
	else clog << "Channels::saveToFile - Unable to open file";
	return -1;
}

void Channels::setDefaultFreq(long freq) {
	if (virtualChannelList->count(freq)) {
		defaultFreq = freq;
	}
}

long Channels::getDefaultFreq() {
	return defaultFreq;
}

map<long, string>* Channels::getChannelsListName() {
	return channelList;
}

map<long, unsigned char>* Channels::getVirtualChannelsList() {
	return virtualChannelList;
}
}
}
}
}
}
}
