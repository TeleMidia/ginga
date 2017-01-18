/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef Channels_H_
#define Channels_H_


GINGA_TUNER_BEGIN

class Channels {
	private:
		map<long, string> *channelList;
		map<long, unsigned char> *virtualChannelList;
		long defaultFreq;

	public:
		Channels();
		~Channels();

		void insertFreq(string name, long freq, unsigned char virtualChannel);
		void removeFreq(long freq);
		string getName(long freq);
		long getFreqByName(string name);
		unsigned char getVirtualChannel(long freq);
		long getPreviousFreq(long currentFreq);
		long getNextFreq(long currentFreq);
		void cleanList();
		int loadFromFile(string filename);
		int saveToFile(string filename);
		int getListSize();
		void setDefaultFreq(long freq);
		long getDefaultFreq();
		map<long, string>* getChannelsListName();
		map<long, unsigned char>* getVirtualChannelsList();
};

GINGA_TUNER_END
#endif /*Channels_H_*/
