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

#ifndef _ISocketService_H_
#define _ISocketService_H_

#include "NetworkUtil.h"

GINGA_MULTIDEVICE_BEGIN

class ISocketService
{
public:
  virtual ~ISocketService (){};
  virtual unsigned int getInterfaceIPAddress () = 0;
  virtual int getServicePort () = 0;
  virtual void dataRequest (char *data, int taskSize, bool repeat = true)
      = 0;
  virtual bool checkOutputBuffer () = 0;
  virtual bool checkInputBuffer (char *data, int *size) = 0;
};

GINGA_MULTIDEVICE_END

#endif /*_ISocketService_H_*/
