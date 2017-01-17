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

#ifndef _IDataFile_H_
#define _IDataFile_H_

#include "INCLStructure.h"

#include <string>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_NCL_BEGIN

	class IDataFile : public INCLStructure {
		public:
			virtual ~IDataFile(){};
			int getType(){return ST_DATAFILE;};
			virtual int getId()=0;
			virtual void setComponentTag(string componentTag)=0;
			virtual string getCopmonentTag()=0;
			virtual void setUri(string uri)=0;
			virtual string getUri()=0;
			virtual void setSize(double size)=0;
			virtual double getSize()=0;
	};

BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_NCL_END
#endif //_IDataFile_H_
