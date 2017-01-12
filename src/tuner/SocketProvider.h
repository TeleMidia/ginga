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

#ifndef SOCKETPROVIDER_H_
#define SOCKETPROVIDER_H_

#include "IDataProvider.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	class SocketProvider : public IDataProvider {
		protected:
			string addr;
			int portNumber;
			int socketDescriptor;
			short capabilities;

		public:
			virtual ~SocketProvider(){};

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

			virtual IChannel* getCurrentChannel() {
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

			virtual void close() {
				::close(socketDescriptor);
			};

			virtual int callServer()=0;
			virtual char* receiveData(int* len)=0;
	};
}
}
}
}
}
}

#endif /*SOCKETPROVIDER_H_*/
