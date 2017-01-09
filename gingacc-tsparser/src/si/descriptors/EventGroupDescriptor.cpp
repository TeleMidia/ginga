/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

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
		//TODO: WORSE CODE EVER MADE: FIX IT
		/*vector<struct MultipleEvent*>::iterator i;
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
			multiEvents = NULL;
		}*/
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
//		events = new struct Event[eventCount];
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
				remainingBytes -= 8;
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
