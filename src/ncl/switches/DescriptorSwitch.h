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

#ifndef _DESCRIPTORSWITCH_H_
#define _DESCRIPTORSWITCH_H_

#include "../Entity.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "../descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "Rule.h"

extern "C" {
#include <pthread.h>
}

#include <string>
#include <iostream>
#include <vector>
using namespace std;

BR_PUCRIO_TELEMIDIA_NCL_SWITCHES_BEGIN

	class DescriptorSwitch : public GenericDescriptor {
		private:
			vector<GenericDescriptor*>* descriptorList;
			vector<Rule*>* ruleList;
			GenericDescriptor* defaultDescriptor;
			GenericDescriptor* selectedDescriptor;

			static set<DescriptorSwitch*> objects;
			static bool initMutex;
			static pthread_mutex_t _objMutex;

			static void addInstance(DescriptorSwitch* object);
			static bool removeInstance(DescriptorSwitch* object);

		public:
			DescriptorSwitch(string id);
			virtual ~DescriptorSwitch();

			static bool hasInstance(
					DescriptorSwitch* object, bool eraseFromList);

			bool addDescriptor(
				    unsigned int index,
				    GenericDescriptor* descriptor,
				    Rule* rule);

			bool addDescriptor(GenericDescriptor* descriptor, Rule* rule);
			bool containsRule(Rule* rule);
			void exchangeDescriptorsAndRules(
				    unsigned int index1,
				    unsigned int index2);

			int indexOfRule(Rule *rule);
			GenericDescriptor* getDefaultDescriptor();
			unsigned int indexOfDescriptor(GenericDescriptor* descriptor);
			GenericDescriptor* getDescriptor(unsigned int index);
			GenericDescriptor* getDescriptor(string descriptorId);
			GenericDescriptor* getDescriptor(Rule* rule);
			Rule* getRule(unsigned int index);
			unsigned int getNumDescriptors();
			unsigned int getNumRules();
			bool removeDescriptor(unsigned int index);
			bool removeDescriptor(GenericDescriptor* descriptor);
			bool removeRule(Rule *rule);
			void setDefaultDescriptor(GenericDescriptor* descriptor);
			void select(GenericDescriptor* descriptor);
			void selectDefault();
			GenericDescriptor* getSelectedDescriptor();
	};

BR_PUCRIO_TELEMIDIA_NCL_SWITCHES_END
#endif //_DESCRIPTORSWITCH_H_
