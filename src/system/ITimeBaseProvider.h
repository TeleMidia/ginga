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

#ifndef ITimeBaseProvider_H_
#define ITimeBaseProvider_H_

#include "ITimeBaseListener.h"

GINGA_SYSTEM_BEGIN

	class ITimeBaseProvider {
		public:
			virtual ~ITimeBaseProvider(){};

			virtual void setNptPrinter(bool nptPrinter)=0;

			virtual bool addLoopListener(
					unsigned char timeBaseId,
					ITimeBaseListener* ltn)=0;

			virtual bool removeLoopListener(
					unsigned char cid,
					ITimeBaseListener* ltn)=0;

			virtual bool addTimeListener(
					unsigned char timeBaseId,
					double timeBaseValue,
					ITimeBaseListener* ltn)=0;

			virtual bool removeTimeListener(
					unsigned char timeBaseId,
					ITimeBaseListener* ltn)=0;

			virtual bool addIdListener(ITimeBaseListener* ltn)=0;
			virtual bool removeIdListener(ITimeBaseListener* ltn)=0;

			virtual unsigned char getOccurringTimeBaseId()=0;
			virtual double getCurrentTimeValue(unsigned char timeBaseId)=0;
	};

GINGA_SYSTEM_END

#endif /*ITimeBaseProvider_H_*/
