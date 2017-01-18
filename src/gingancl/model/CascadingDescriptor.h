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

#ifndef _CASCADINGDESCRIPTOR_H_
#define _CASCADINGDESCRIPTOR_H_

#include "util/functions.h"
#include "util/Color.h"

#include "ncl/Parameter.h"
using namespace ::ginga::ncl;

#include "ncl/Descriptor.h"
#include "ncl/GenericDescriptor.h"
using namespace ::ginga::ncl;

#include "ncl/LayoutRegion.h"
using namespace ::ginga::ncl;

#include "ncl/DescriptorSwitch.h"
using namespace ::ginga::ncl;

#include "ncl/FocusDecoration.h"
#include "ncl/KeyNavigation.h"
using namespace ::ginga::ncl;

#include "ncl/Transition.h"
using namespace ::ginga::ncl;

#include "FormatterRegion.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include <string>
#include <map>
#include <vector>
#include <set>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_PRESENTATION_BEGIN

  class CascadingDescriptor {
	protected:
		set<string> typeSet;

	private:
		static int dummyCount;
		string id;
		vector<GenericDescriptor*> descriptors;
		vector<GenericDescriptor*> unsolvedDescriptors;
		map<string, string> parameters;
		double explicitDuration;
		string playerName;
		long repetitions;
		bool freeze;
		LayoutRegion* region;
		FormatterRegion* formatterRegion;

		string focusIndex, moveUp, moveDown, moveLeft, moveRight;
		string focusSrc, selectionSrc;
		Color* focusBorderColor;
		Color* selBorderColor;
		int focusBorderWidth;
		int selBorderWidth;
		double focusBorderTransparency;
		vector<Transition*>* inputTransitions;
		vector<Transition*>* outputTransitions;

		void cascadeDescriptor(Descriptor* descriptor);

	public:
		CascadingDescriptor(GenericDescriptor* firstDescriptor);
		CascadingDescriptor(CascadingDescriptor* descriptor);
		~CascadingDescriptor();

	protected:
		void initializeCascadingDescriptor();

	public:
		bool instanceOf(string s);
		string getId();
		bool isLastDescriptor(GenericDescriptor* descriptor);
		void cascade(GenericDescriptor* preferredDescriptor);
		GenericDescriptor* getUnsolvedDescriptor(int i);
		vector<GenericDescriptor*>* getUnsolvedDescriptors();
		void cascadeUnsolvedDescriptor();
		double getExplicitDuration();
		bool getFreeze();
		string getPlayerName();
		LayoutRegion* getRegion();
		LayoutRegion* getRegion(void* formatterLayout, void* object);
		FormatterRegion* getFormatterRegion();

	private:
		void updateRegion(void* formatterLayout, string name, string value);

		void createDummyRegion(void* formatterLayout, void* executionObject);
		void createDummyRegion(void* formatterLayout);

	public:
		void setFormatterLayout(void* formatterLayout);
		long getRepetitions();
		vector<Parameter*>* getParameters();
		string getParameterValue(string paramName);
		vector<GenericDescriptor*>* getNcmDescriptors();
		Color* getFocusBorderColor();
		double getFocusBorderTransparency();
		int getFocusBorderWidth();
		string getFocusIndex();
		string getFocusSrc();
		string getSelectionSrc();
		string getMoveDown();
		string getMoveLeft();
		string getMoveRight();
		string getMoveUp();
		Color* getSelBorderColor();
		int getSelBorderWidth();
		vector<Transition*>* getInputTransitions();
		vector<Transition*>* getOutputTransitions();
  };

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_PRESENTATION_END
#endif //_CASCADINGDESCRIPTOR_H_
