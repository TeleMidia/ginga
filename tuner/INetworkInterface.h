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

#ifndef INETWORKINTERFACE_H_
#define INETWORKINTERFACE_H_

#include "IFrontendFilter.h"
#include "IDataProvider.h"
#include "ISTCProvider.h"
#include "IChannel.h"

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	class INetworkInterface : public ISTCProvider {
		public:
			virtual ~INetworkInterface(){};
			virtual short getCaps()=0;
			virtual int getId()=0;
			virtual string getName()=0;
			virtual string getProtocol()=0;
			virtual string getAddress()=0;
			virtual void attachFilter(IFrontendFilter* filter)=0;
			virtual void removeFilter(IFrontendFilter* filter)=0;
			virtual void setDataProvider(IDataProvider* provider)=0;
			virtual bool hasSignal()=0;
			virtual IDataProvider* tune()=0;
			virtual bool changeChannel(int factor)=0;
			virtual bool setChannel(string channelValue)=0;
			virtual bool getSTCValue(uint64_t* stc, int* valueType)=0;
			virtual IChannel* getCurrentChannel()=0;
			virtual int createPesFilter(
					int pid, int pesType, bool compositeFiler)=0;

			virtual string getPesFilterOutput()=0;

			virtual char* receiveData(int* len)=0;

			virtual void close()=0;
	};
}
}
}
}
}
}

#endif /*NETWORKINTERFACE_H_*/
