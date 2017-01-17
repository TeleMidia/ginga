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

#ifndef DESCRIPTORBASE_H_
#define DESCRIPTORBASE_H_

#include "../Base.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "GenericDescriptor.h"

#include <string>
#include <vector>
using namespace std;

BR_PUCRIO_TELEMIDIA_NCL_DESCRIPTOR_BEGIN

	class DescriptorBase : public Base {
		private:
			vector<GenericDescriptor*>* descriptorSet;

		public:
			DescriptorBase(string id);
			virtual ~DescriptorBase();

			bool addDescriptor(GenericDescriptor* descriptor);
			bool addBase(
				    Base* base,
				    string alias,
				    string location);

			void clear();

		private:
			GenericDescriptor* getDescriptorLocally(string descriptorId);

		public:
			GenericDescriptor* getDescriptor(string descriptorId);
			vector<GenericDescriptor*>* getDescriptors();
			bool removeDescriptor(GenericDescriptor* descriptor);
	};

BR_PUCRIO_TELEMIDIA_NCL_DESCRIPTOR_END
#endif /*DESCRIPTORBASE_H_*/
