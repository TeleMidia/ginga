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

#ifndef IFrontendFilter_H_
#define IFrontendFilter_H_

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	class IFrontendFilter {
		public:
			virtual ~IFrontendFilter(){};

			virtual void setPid(int pid)=0;
			virtual int getPid()=0;

			virtual void setTid(int tid)=0;
			virtual int getTid()=0;

			virtual void setTidExt(int tidExt)=0;
			virtual int getTidExt()=0;

			virtual void setDescriptor(int feDescripor)=0;
			virtual int getDescriptor()=0;
			virtual void receivePes(char* pes, int pesLen)=0;
			virtual void receiveSection(char* section, int secLen)=0;
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::tuning::IFrontendFilter*
		feFilterCreator(int id);

typedef void feFilterDestroyer(
		::br::pucrio::telemidia::ginga::core::tuning::IFrontendFilter*);

#endif /*IFrontendFilter_H_*/
