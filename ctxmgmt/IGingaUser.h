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

#ifndef _IGingaUser_H_
#define _IGingaUser_H_

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace contextmanager {
  class IGingaUser {
	public:
		virtual ~IGingaUser(){};

		virtual int getUserId()=0;
		virtual string getUserName()=0;
		virtual int getUserAge()=0;
		virtual char getUserGenre()=0;
		virtual string getUserLocation()=0;
		virtual bool isValidPassword(string passwd)=0;

		virtual bool setPassword(string oldPasswd, string newPasswd)=0;
		virtual void setUserName(string passwd, string userName)=0;
		virtual void setUserAge(string passwd, int userAge)=0;
		virtual void setUserLocation(string passwd, string userLocation)=0;
		virtual void setUserGenre(string passwd, char userGenre)=0;

		virtual void saveTo(FILE* fd)=0;
  };
}
}
}
}
}
}

#endif /*_IGingaUser_H_*/
