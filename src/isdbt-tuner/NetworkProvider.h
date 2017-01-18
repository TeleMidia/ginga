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

#ifndef NETWORKPROVIDER_H_
#define NETWORKPROVIDER_H_

#include "system/SystemCompat.h"
#include "system/PracticalSocket.h"
using namespace ::ginga::system;


#include "IDataProvider.h"

BR_PUCRIO_TELEMIDIA_GINGA_CORE_TUNING_BEGIN

	class NetworkProvider : public IDataProvider {
		protected:
			string addr;
			string protocol;
			int portNumber;
			short capabilities;
			UDPSocket *udpSocket;

		public:
			NetworkProvider(string address, int port, string protocol);
			~NetworkProvider();

			virtual void setListener(ITProviderListener* listener){};
			virtual void attachFilter(IFrontendFilter* filter){};
			virtual void removeFilter(IFrontendFilter* filter){};

			virtual short getCaps() {
				return capabilities;
			};

			virtual bool tune() {
				if (callServer() > 0) {
					return true;
				}

				return false;
			};

			virtual Channel* getCurrentChannel() {
				return NULL;
			}

			virtual bool getSTCValue(uint64_t* stc, int* valueType) {
				return false;
			}

			virtual bool changeChannel(int factor) {
				return false;
			}

			bool setChannel(string channelValue) {
				return false;
			}

			virtual int createPesFilter(
					int pid, int pesType, bool compositeFiler) {

				return -1;
			}

			virtual string getPesFilterOutput() {
				return "";
			}

			virtual void close() {};

			virtual int callServer();

			virtual char* receiveData(int* len);
	};

BR_PUCRIO_TELEMIDIA_GINGA_CORE_TUNING_END
#endif /*NETWORKPROVIDER_H_*/
