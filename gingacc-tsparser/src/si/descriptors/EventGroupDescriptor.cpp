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

#include "tsparser/EventGroupDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
//TODO: test this class - no use of this descriptor on TS files
//events is an array and multiEvents is as vector.
	EventGroupDescriptor::EventGroupDescriptor() {
		this->descriptorTag    = 0;
		this->descriptorLength = 0;
		this->eventCount       = 0;
		this->events           = NULL;
		this->groupType        = 0;
		this->multiEvents      = NULL;
	}

	EventGroupDescriptor::~EventGroupDescriptor() {
		vector<Event*>::iterator i;
		if(events != NULL){
			for(i = events->begin(); i != events->end(); ++i){
				delete (*i);
			}
			delete events;
			events == NULL;
		}
		if(multiEvents != NULL){
			for(i = multiEvents->begin(); i != multiEvents->end(); ++i){
				delete (*i);
			}
			delete multiEvents;
			multiEvents == NULL;
		}
	}
	unsigned char EventGroupDescriptor::getDescriptorTag(){
		return descriptorTag;
	}
	unsigned int EventGroupDescriptor::getDescriptorLength(){
		return (unsigned int)descriptorLength;
	}
	void EventGroupDescriptor::print(){
		clog << "EventGroupDescriptor::print printing..."<< endl;
	}
	size_t EventGroupDescriptor::process(char* data, size_t pos){
		struct Event* event;
		struct MultipleEvent* multiEvent;
		unsigned char remainingBytes;

		descriptorLength =  data[pos+1];

		pos += 2;
		groupType = ((data[pos] >> 4) & 0x0F);
		eventCount = (data[pos] & 0x0F);

		pos++;
		events = new struct Event[eventCount];
		for(int i = 0; i < eventCount; i++){

			//pos ++;
			event = new struct Event;
			event->serviceId = (((data[pos] << 8) & 0xFF00) |
					(data[pos+1] & 0xFF));
			pos += 2;

			event->eventId = (((data[pos] << 8) & 0xFF00) |
					(data[pos+1] & 0xFF));
			pos += 2;

			events->push_back(event);
		}
		remainingBytes = descriptorLength - (eventCount * 4);
		/*eventCount is the number of events, not the bytes belongs to events.
		* So to have bytes belongs to event, multiply the number of events by
		* the number of bytes per event (which is 4, 2 bytes to each field).
		*/
		if(groupType == 4 || groupType == 5){
			multiEvents = new vector<MultipleEvent*>;

			while(remainingBytes){
				multiEvent = new struct MultipleEvent;

				multiEvent->originalNetworkId = (((data[pos] << 8) & 0xFF00) |
						(data[pos+1] & 0xFF));
				pos += 2;

				multiEvent->transportStreamId = (((data[pos] << 8) & 0xFF00) |
						(data[pos+1] & 0xFF));
				pos += 2;

				multiEvent->event = new struct Event;
				multiEvent->event->serviceId = (((data[pos] << 8) & 0xFF00) |
						(data[pos+1] & 0xFF));
				pos += 2;

				multiEvent->event->eventId = (((data[pos] << 8) & 0xFF00) |
						(data[pos+1] & 0xFF));
				pos += 2;

				multiEvents->push_back(multiEvent);
				remainingBytes - 8;
			}
		}
		else{
			pos += remainingBytes;
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
